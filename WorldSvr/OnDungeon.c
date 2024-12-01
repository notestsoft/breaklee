#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CHECK_DUNGEON_PLAYTIME) {
	if (!Character) goto error;
	
    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, Packet->DungeonIndex);
    if (!DungeonData) goto error;

    RTDataDungeonGroupRef DungeonGroupData = RTRuntimeDataDungeonGroupGet(Runtime->Context, Packet->DungeonIndex);
    if (!DungeonGroupData) goto error;

    S2C_DATA_CHECK_DUNGEON_PLAYTIME* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHECK_DUNGEON_PLAYTIME);
    Response->DungeonIndex = Packet->DungeonIndex;
    Response->MaxInstanceCount = Runtime->WorldManager->MaxPartyWorldContextCount;
    Response->InstanceCount = RTWorldContextGetPartyInstanceCount(Runtime->WorldManager);
    Response->IsAccessible = Response->InstanceCount < Response->MaxInstanceCount;

    if (DungeonData->IsElite > 0) {
        Response->MaxEliteEntryCount = 1;
        Response->EliteEntryCount = 0;
    }

    RTDataDungeonTimeRef DungeonTimeData = RTRuntimeDataDungeonTimeGet(Runtime->Context, DungeonGroupData->DungeonGroup);
    if (DungeonTimeData) {
        for (Int Index = 0; Index < DungeonTimeData->DungeonPlayTimeCount; Index += 1) {
            RTDataDungeonPlayTimeRef DungeonPlayTimeData = &DungeonTimeData->DungeonPlayTimeList[Index];
            Response->WeekdayPlaytimeInSeconds[DungeonPlayTimeData->DayOfWeek] = DungeonPlayTimeData->PlayTime;
        }
    }

    Response->RemainingPlaytimeInSeconds = 3600;
    Response->MaxEntryCount = 1;
    Response->EntryCount = 0;
	SocketSend(Socket, Connection, Response);
    return;

error:
    {
        S2C_DATA_CHECK_DUNGEON_PLAYTIME* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHECK_DUNGEON_PLAYTIME);
        SocketSend(Socket, Connection, Response);
    }
}

CLIENT_PROCEDURE_BINDING(GET_DUNGEON_REWARD_LIST) {
	if (!Character) goto error;

    Trace("GetDungeonRewardList");
	// TODO: Add reward list data

	S2C_DATA_GET_DUNGEON_REWARD_LIST* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_DUNGEON_REWARD_LIST);
	Response->DungeonID = Packet->DungeonID;
	SocketSend(Socket, Connection, Response);
    return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ENTER_DUNGEON_GATE) {
    if (!Character) goto error;

    Trace("EnterDungeonGate");

    S2C_DATA_ENTER_DUNGEON_GATE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ENTER_DUNGEON_GATE);
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
    if (DungeonData && DungeonData->WorldIndex == Packet->WorldID) {
        Response->Result = 1;
    }

    if (RTCharacterHasQuestDungeon(Runtime, Character, Packet->DungeonID) && DungeonData) {

        // TODO: Verify NpcID, WorldID
        Response->Result = 1;
    }

	SocketSend(Socket, Connection, Response);
    return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_START) {
    if (!Character) goto error;

    Trace("QuestDungeonStart");

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    S2C_DATA_QUEST_DUNGEON_START* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, QUEST_DUNGEON_START);
    Response->Active = World->Active;
    SocketSend(Socket, Connection, Response);

    if (World->Active) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
        if (!DungeonData) goto error;

        S2C_DATA_NFY_QUEST_DUNGEON_SPAWN* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_QUEST_DUNGEON_SPAWN);
        Response->DungeonTimeout1 = (UInt32)(World->DungeonTimeout - GetTimestampMs());
        Response->DungeonTimeout2 = (UInt32)(World->DungeonTimeout - GetTimestampMs());
        SocketSend(Socket, Connection, Response);
    }

    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_SPAWN) {
    if (!Character) goto error;

    Trace("QuestDungeonSpawn");

    if (Packet->IsActive) {
        RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
        if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
            World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

        // TODO: Not all quest dungeons have a dungeon index like skalids room?
        //       So how is the npc spawned at all? >> DungeonID = 4162 why do we have 0 here?

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
        if (!DungeonData) goto error;

        // TODO: Character is joining the dungeon which doesn't mean that it is a start command,
        //       because a party member could have had already started it!

        // TODO: Find a way to check if the character was already spawned into the dungeon before
        if (DungeonData->RemoveItem > 0) {
            // TODO: The inventory slot should also be checked inside the warp command
            RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Character->DungeonEntryItemSlotIndex);
            if (!ItemSlot) goto error;
            if ((ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX) != DungeonData->EntryItemID.Serial ||
                ItemSlot->ItemOptions != DungeonData->EntryItemOption) goto error;

            RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
            Character->SyncMask.InventoryInfo = true;
        }

        if (RTDungeonStart(World)) {
            S2C_DATA_NFY_QUEST_DUNGEON_SPAWN* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_QUEST_DUNGEON_SPAWN);
            Response->DungeonTimeout1 = (UInt32)(World->DungeonTimeout - GetTimestampMs());
            Response->DungeonTimeout2 = DungeonData->MissionTimeout * 1000;
            SocketSend(Socket, Connection, Response);
        }
    }

    return;
    
error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_END) {
    if (!Character) goto error;

    Trace("QuestDungeonEnd");

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    Bool Success = RTDungeonEnd(World);
    if (Success) {
        RTCharacterDungeonQuestFlagSet(Character, World->DungeonIndex);

        S2C_DATA_NFY_DUNGEON_COMPLETE_INFO* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_DUNGEON_COMPLETE_INFO);
        BroadcastToParty(Context, World->Party, Notification);
    }
    else {
        // TODO: Check how this packet is behaving in a party dungeon!
        Success = RTDungeonFail(World);
    }

    S2C_DATA_QUEST_DUNGEON_END* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, QUEST_DUNGEON_END);
    Response->Result = Success ? 1 : 0;
    SocketSend(Socket, Connection, Response);

    S2C_DATA_NFY_QUEST_DUNGEON_END* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_QUEST_DUNGEON_END);
    Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
    Notification->Result = Success ? 1 : 0;
    Notification->Unknown1 = 28;
    BroadcastToParty(Context, World->Party, Notification);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ATTACK_BOSS_MOB) {
    if (!Character) goto error;

    Trace("AttackBossMob");

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    assert(World);

    RTMobRef Mob = RTWorldContextGetMob(World, Packet->Entity);
    if (!Mob) goto error;

    // TODO: Find out what this field means and map it in new data format!
    //if (Mob->SpeciesData->Boss == 2) {
    Mob->IsPermanentDeath = true;
    RTWorldDespawnMob(Runtime, World, Mob);
    //}
    //else {
    //    goto error;
    //}

    // TODO: Implementation missing!
    // NOTE: It can be that when an npc is progressed but the dungeon is reopened it calls to repeat the event trigger of the npc and despawn it...

    S2C_DATA_ATTACK_BOSS_MOB* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ATTACK_BOSS_MOB);
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(QUEST_DUNGEON_GATE_OPEN) {
    if (!Character) goto error;

    Trace("QuestDungeonGateOpen");
    // TODO: Implementation missing!
    return;
    
error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(DUNGEON_PAUSE) {
    if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    S2C_DATA_DUNGEON_PAUSE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DUNGEON_PAUSE);
    Response->Result = RTDungeonPause(World) ? 0 : 1;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(DUNGEON_RESUME) {
    if (!Character) goto error;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) goto error;

    S2C_DATA_DUNGEON_RESUME* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DUNGEON_RESUME);
    Response->Result = RTDungeonResume(World) ? 0 : 1;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}
