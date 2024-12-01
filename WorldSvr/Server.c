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
    AllocatorRef Allocator = AllocatorGetSystemDefault();
    ArchiveRef Archive = ArchiveCreateEmpty(Allocator);    
    ArchiveRef TempArchive = ArchiveCreateEmpty(Allocator);
    Bool Loaded = true;

    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(Archive, PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "quest.enc"), true);
    Loaded &= ServerLoadQuestData(Context->Runtime, Archive);
    if (!Loaded) Fatal("Failed to load runtime quest data!");
    ArchiveClear(Archive, true);

    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(Archive, PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "rank.enc"), true);
    Loaded &= ServerLoadBattleStyleFormulaData(Context->Runtime, Archive);
    if (!Loaded) Fatal("Failed to load runtime battle style formula data!");
    Loaded &= ServerLoadCharacterTemplateData(Context, Archive);
    if (!Loaded) Fatal("Failed to load character init data!");
    ArchiveClear(Archive, true);

    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(Archive, PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "Terrain.enc"), true);
    Loaded &= ServerLoadTerrainData(Context->Runtime, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Config.WorldSvr.ScriptDataPath, Archive);
    if (!Loaded) Fatal("Failed to load terrain data!");
    ArchiveClear(Archive, true);

    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(Archive, PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "cabal.enc"), true);
    Loaded &= ServerLoadWorldData(Context->Runtime, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Config.WorldSvr.ScriptDataPath, Archive, TempArchive);
    if (!Loaded) Fatal("Failed to load world data!");
    ArchiveClear(Archive, true);

    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(Archive, PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "skill.enc"), true);
    Loaded &= ServerLoadSkillData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Archive);
    if (!Loaded) Fatal("Failed to load skill data!");
    ArchiveClear(Archive, true);

    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(Archive, PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "cont.enc"), true);
    Loaded &= ServerLoadQuestDungeonData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Archive);
    if (!Loaded) Fatal("Failed to load quest dungeon data!");
    ArchiveClear(Archive, true);

    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(Archive, PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "cont2.enc"), true);
    Loaded &= ServerLoadMissionDungeonData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Archive);
    if (!Loaded) Fatal("Failed to load mission dungeon data!");
    ArchiveClear(Archive, true);

    Loaded &= ArchiveLoadFromFileEncryptedNoAlloc(Archive, PathCombineNoAlloc(Config.WorldSvr.RuntimeDataPath, "cont3.enc"), true);
    Loaded &= ServerLoadMissionDungeonData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Archive);
    if (!Loaded) Fatal("Failed to load mission dungeon data!");
    ArchiveClear(Archive, true);

    Loaded &= ServerLoadItemData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    if (!Loaded) Fatal("Failed to load runtime item data!");

    Loaded &= ServerLoadMobData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    if (!Loaded) Fatal("Failed to load runtime mob data!");

    Loaded &= ServerLoadMobPatrolData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    if (!Loaded) Fatal("Failed to load mob patrol data!"); 

    Loaded &= ServerLoadMobPatternData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, Config.WorldSvr.ScriptDataPath);
    if (!Loaded) Fatal("Failed to load mob pattern data!");

    Loaded &= ServerLoadOptionPoolData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath);
    if (!Loaded) Fatal("Failed to load option pool data!");

    Loaded &= ServerLoadWorldDropData(Context, Config.WorldSvr.RuntimeDataPath, Config.WorldSvr.ServerDataPath, TempArchive);
    if (!Loaded) Fatal("Failed to load world drop data!");

    /*
    IndexSetRef IndexSet = IndexSetCreate(AllocatorGetDefault(), 256);

    for (Int Index = 0; Index < Context->Runtime->ItemDataCount; Index++) {
        RTItemDataRef ItemData = &Context->Runtime->ItemData[Index];
        if (ItemData.) {
            IndexSetInsert(IndexSet, ItemData->ItemType);
        }
    }

    IndexSetIteratorRef Iterator = IndexSetGetIterator(IndexSet);
    while (Iterator) {
        Trace("StackableItem(%d)", Iterator->Value);
        Iterator = IndexSetIteratorNext(IndexSet, Iterator);
    }
    IndexSetDestroy(IndexSet);
    */
    ArchiveDestroy(Archive);

    for (Int WorldIndex = 0; WorldIndex < Context->Runtime->WorldManager->MaxWorldDataCount; WorldIndex += 1) {
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
    Char Argument1[256] = { 0 };
    Char Argument2[256] = { 0 };

    while (fscanf(File, "%31s %255s", ScriptDirective, Argument1) == 2) {
        if (strcmp(ScriptDirective, "AddNetworkScript") == 0) {
            CString ScriptFilePath = PathCombineNoAlloc(Config.WorldSvr.ScriptDataPath, Argument1);
            if (!FileExists(ScriptFilePath)) {
                Error("Error loading ScriptRegistry");
                break;
            }

            ServerSocketLoadScript(Context->Server, Context->ClientSocket, ScriptFilePath);
        }
        else if (strcmp(ScriptDirective, "AddItemScript") == 0) {
            fscanf(File, " %255s", Argument2);
            UInt64 ItemID = 0;
            ParseUInt64(Argument1, &ItemID);

            CString ScriptFilePath = PathCombineNoAlloc(Config.WorldSvr.ScriptDataPath, Argument2);
            if (!FileExists(ScriptFilePath)) {
                Error("Error loading ScriptRegistry");
                break;
            }

            RTScriptRef Script = RTScriptManagerLoadScript(Context->Runtime->ScriptManager, ScriptFilePath);
            DictionaryInsert(Context->ItemScriptRegistry, &ItemID, &Script, sizeof(RTScriptRef));
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
    UInt32 CharacterIndex,
    CString CharacterName
) {
    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
    while (Iterator) {
        SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
        Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

        ClientContextRef Client = (ClientContextRef)Connection->Userdata;
        if (Client && Client->CharacterIndex == CharacterIndex) {
            RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, CharacterIndex);
            if (Character && CharacterName && !CStringIsEqual(CharacterName, Character->Name)) continue;
            
            return Client;
        }
    }

    return NULL;
}
