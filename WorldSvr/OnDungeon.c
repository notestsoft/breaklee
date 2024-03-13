#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CHECK_DUNGEON_PLAYTIME) {
	if (!Character) goto error;
	
	// TODO: Add xml data

	S2C_DATA_CHECK_DUNGEON_PLAYTIME* Response = PacketInit(S2C_DATA_CHECK_DUNGEON_PLAYTIME);
	Response->Command = S2C_CHECK_DUNGEON_PLAYTIME;
	Response->DungeonID = Packet->DungeonID;
	Response->MaxInstanceCount = 10;
	Response->InstanceCount = 0;
	Response->Unknown1[0] = 0x01;
	Response->RemainingPlayTimeInSeconds = 21600;
	Response->MaxEntryCount = 99;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_DUNGEON_REWARD_LIST) {
	if (!Character) goto error;
	
	// TODO: Add reward list data

	S2C_DATA_GET_DUNGEON_REWARD_LIST* Response = PacketInit(S2C_DATA_GET_DUNGEON_REWARD_LIST);
	Response->Command = S2C_GET_DUNGEON_REWARD_LIST;
	Response->DungeonID = Packet->DungeonID;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ENTER_DUNGEON_GATE) {
    if (!Character) goto error;

    S2C_DATA_ENTER_DUNGEON_GATE* Response = PacketInit(S2C_DATA_ENTER_DUNGEON_GATE);
    Response->Command = S2C_ENTER_DUNGEON_GATE;
    Response->Result = 0;
    Response->DungeonID = Packet->DungeonID;
    Response->Unknown1 = Packet->Unknown1;
    Response->NpcID = Packet->NpcID;
    Response->Unknown3 = Packet->Unknown3;
    Response->Unknown4 = Packet->Unknown4;
    Response->WorldID = Packet->WorldID;
    Response->Unknown5 = Packet->Unknown5;
    Response->DungeonBoostLevel = Packet->DungeonBoostLevel;

    RTWorldContextRef World = RTRuntimeGetWorldByID(Runtime, Packet->WorldID);
    if (!World) goto error;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, Packet->DungeonID);
    if (DungeonData && DungeonData->WorldID == Packet->WorldID) {
        Response->Result = 1;
    }

    if (RTCharacterHasQuestDungeon(Runtime, Character, Packet->DungeonID) && DungeonData) {

        // TODO: Verify NpcID, WorldID
        Response->Result = 1;
    }

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_START) {
    if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    S2C_DATA_QUEST_DUNGEON_START* Response = PacketInit(S2C_DATA_QUEST_DUNGEON_START);
    Response->Command = S2C_QUEST_DUNGEON_START;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_SPAWN) {
    if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    // TODO: Not all quest dungeons have a dungeon index like skalids room?
    //       So how is the npc spawned at all? >> DungeonID = 4162 why do we have 0 here?

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
    if (!DungeonData) goto error;

    // TODO: Character is joining the dungeon which doesn't mean that it is a start command,
    //       because a party member could have had already started it!

    if (!World->Active) {
        RTDungeonStart(World);
        // RTWorldContextUpdate(Runtime, World);

        S2C_DATA_NFY_QUEST_DUNGEON_SPAWN* Response = PacketInit(S2C_DATA_NFY_QUEST_DUNGEON_SPAWN);
        Response->Command = S2C_NFY_QUEST_DUNGEON_SPAWN;
        Response->DungeonTimeout1 = (World->DungeonTimeout - GetTimestamp()) * 1000;
        Response->DungeonTimeout2 = DungeonData->MissionTimeout * 1000;
        SocketSend(Socket, Connection, Response);
    }

    return;
    
error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_END) {
    if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    Bool Success = RTDungeonEnd(World);
    if (Success) {
        RTCharacterDungeonQuestFlagSet(Character, World->DungeonIndex);

        S2C_DATA_NFY_DUNGEON_COMPLETE_INFO* Notification = PacketInit(S2C_DATA_NFY_DUNGEON_COMPLETE_INFO);
        Notification->Command = S2C_NFY_DUNGEON_COMPLETE_INFO;
        BroadcastToParty(Context, World->Party, Notification);
    }

    S2C_DATA_QUEST_DUNGEON_END* Response = PacketInit(S2C_DATA_QUEST_DUNGEON_END);
    Response->Command = S2C_QUEST_DUNGEON_END;
    Response->Result = Success ? 1 : 0;
    SocketSend(Socket, Connection, Response);

    S2C_DATA_NFY_QUEST_DUNGEON_END* Notification = PacketInit(S2C_DATA_NFY_QUEST_DUNGEON_END);
    Notification->Command = S2C_NFY_QUEST_DUNGEON_END;
    Notification->CharacterIndex = Client->CharacterIndex;
    Notification->Result = Success ? 1 : 0;
    Notification->Unknown1 = 28;
    BroadcastToParty(Context, World->Party, Notification);

    return;

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ATTACK_BOSS_MOB) {
    if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    assert(World);

    RTMobRef Mob = RTWorldContextGetMob(World, Packet->Entity);
    if (!Mob) goto error;

    // TODO: Find out what this field means!
    if (Mob->SpeciesData->Boss == 2) {
        RTWorldDespawnMob(Runtime, World, Mob);
    }
    else {
        goto error;
    }

    // TODO: Implementation missing!
    // NOTE: It can be that when an npc is progressed but the dungeon is reopened it calls to repeat the event trigger of the npc and despawn it...

    S2C_DATA_ATTACK_BOSS_MOB* Response = PacketInit(S2C_DATA_ATTACK_BOSS_MOB);
    Response->Command = S2C_ATTACK_BOSS_MOB;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_GATE_OPEN) {
    if (!Character) goto error;
    // TODO: Implementation missing!
    
error:
	return SocketDisconnect(Socket, Connection);
}
