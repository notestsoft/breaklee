#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"
#include "ServerDataLoader.h"
#include "IPCCommands.h"
#include "IPCProtocol.h"

Void ServerLoadRuntimeData(
    ServerConfig Config,
    ServerContextRef Context
) {
    memset(Context->RuntimeData, 0, sizeof(struct _RuntimeData));

    AllocatorRef Allocator = AllocatorGetSystemDefault();
    ArchiveRef MainArchive = ArchiveCreateEmpty(Allocator);
    ArchiveRef Cont1Archive = ArchiveCreateEmpty(Allocator);
    ArchiveRef Cont2Archive = ArchiveCreateEmpty(Allocator);
    ArchiveRef Cont3Archive = ArchiveCreateEmpty(Allocator);
    ArchiveRef QuestArchive = ArchiveCreateEmpty(Allocator);
    ArchiveRef RankArchive = ArchiveCreateEmpty(Allocator);
    ArchiveRef SkillArchive = ArchiveCreateEmpty(Allocator);
    ArchiveRef TerrainArchive = ArchiveCreateEmpty(Allocator);
    ArchiveRef CharacterInitArchive = ArchiveCreateEmpty(Allocator);

    Bool Loaded = true;
    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(
        MainArchive,
        PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "cabal.enc"),
        true
    );
    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(
        Cont1Archive,
        PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "cont.enc"),
        true
    );
    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(
        Cont2Archive,
        PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "cont2.enc"),
        true
    );
    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(
        Cont3Archive,
        PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "cont3.enc"),
        true
    );
    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(
        QuestArchive,
        PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "quest.enc"),
        true
    );
    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(
        RankArchive,
        PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "rank.enc"),
        true
    );
    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(
        SkillArchive,
        PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "skill.enc"),
        true
    );
    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(
        TerrainArchive,
        PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "Terrain.enc"),
        true
    );
    Loaded &= ArchiveLoadFromFile(
        CharacterInitArchive,
        PathCombineNoAlloc(Config.WorldSvr.ServerDataPath, "server_character_init.xml"),
        true
    );
    Loaded &= ServerLoadQuestData(Context->Runtime, QuestArchive);
    Loaded &= ServerLoadBattleStyleFormulaData(Context->Runtime, RankArchive);
    Loaded &= ServerLoadItemData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    Loaded &= ServerLoadMobData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    Loaded &= ServerLoadShopData(Context->Runtime, Config.WorldSvr.ServerDataPath, Config.WorldSvr.ServerDataPath);
    Loaded &= ServerLoadWorldData(Context->Runtime, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Config.WorldSvr.ScriptDataPath, TerrainArchive, MainArchive, true);
    Loaded &= ServerLoadCharacterTemplateData(Context, RankArchive, CharacterInitArchive);
    Loaded &= ServerLoadSkillData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, SkillArchive);
    Loaded &= ServerLoadDungeonData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Cont1Archive, Cont2Archive, Cont3Archive);
    Loaded &= ServerLoadWorldDropData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    // TODO: Enable file loading check after finishing migration to new data structure!
    if (!Loaded) Fatal("Runtime data loading failed!");

    ArchiveDestroy(MainArchive);
    ArchiveDestroy(Cont1Archive);
    ArchiveDestroy(Cont2Archive);
    ArchiveDestroy(Cont3Archive);
    ArchiveDestroy(QuestArchive);
    ArchiveDestroy(RankArchive);
    ArchiveDestroy(SkillArchive);
    ArchiveDestroy(CharacterInitArchive);

    for (Index WorldIndex = 0; WorldIndex < Context->Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!RTWorldDataExists(Context->Runtime->WorldManager, WorldIndex)) continue;

        RTWorldContextRef WorldContext = RTWorldContextCreateGlobal(Context->Runtime->WorldManager, WorldIndex);
        RTWorldSetMobTable(
            WorldContext->WorldManager->Runtime,
            WorldContext,
            WorldContext->WorldData->MobTable
        );
    }
}

Void ServerLoadScriptData(
    ServerConfig Config,
    ServerContextRef Context
) {
    CString FilePath = PathCombineNoAlloc(Config.WorldSvr.ScriptDataPath, "ScriptRegistry.txt");
    FileRef File = fopen(FilePath, "r");
    if (!File) return;

    Char ScriptDirective[32] = { 0 };
    Char ScriptFileName[256] = { 0 };

    while (fscanf(File, "%31s %255s", ScriptDirective, ScriptFileName) == 2) {
        CString ScriptFilePath = PathCombineNoAlloc(Config.WorldSvr.ScriptDataPath, ScriptFileName);
        if (!FileExists(ScriptFilePath)) {
            Error("Error loading ScriptRegistry");
            break;
        }

        if (strcmp(ScriptDirective, "AddNetworkScript") == 0) {
            ServerSocketLoadScript(Context->Server, Context->ClientSocket, ScriptFilePath);
        }
        else if (strcmp(ScriptDirective, "AddRuntimeScript") == 0) {
            Error("AddRuntimeScript is not supported yet!");
        }
        else {
            Error("Invalid directive: %s\n", ScriptDirective);
        }
    }

    fclose(File);
}

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Context,
    UInt32 AuthKey,
    UInt16 EntityID
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if ((Client->Flags & CLIENT_FLAGS_CONNECTED) &&
            Client->AuthKey == AuthKey &&
            Connection->ID == EntityID) {
            return Client;
        }
    }

    return NULL;
}

ClientContextRef ServerGetClientByEntity(
    ServerContextRef Context,
    RTEntityID Entity
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);

    RTCharacterRef Character = RTWorldManagerGetCharacter(Context->Runtime->WorldManager, Entity);

    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client && Client->CharacterIndex == Character->CharacterIndex) {
            return Client;
        }
    }

    return NULL;
}

ClientContextRef ServerGetClientByIndex(
    ServerContextRef Context,
    Index CharacterIndex,
    CString CharacterName
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client && Client->CharacterIndex == CharacterIndex) {
            if (CharacterName && !CStringIsEqual(Client->CharacterName, CharacterName)) continue;
            
            return Client;
        }
    }

    return NULL;
}
