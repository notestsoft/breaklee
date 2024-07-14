#include "ClientProtocol.h"
#include "IPCProtocol.h"
#include "Notification.h"
#include "Server.h"
#include "NotificationProcedures.h"

Void SendRuntimeNotification(
    SocketRef Socket,
    SocketConnectionRef Connection,
    RTNotificationRef Notification
) {
    S2C_DATA_RUNTIME_NOTIFICATION* Packet = PacketBufferInit(Socket->PacketBuffer, S2C, RUNTIME_NOTIFICATION);
    Packet->Command = (UInt16)Notification->Command;

    UInt8* Data = ((UInt8*)Notification) + sizeof(struct _RTNotification);
    Int32 DataLength = Notification->Length - sizeof(struct _RTNotification);
    UInt8* Payload = PacketBufferAppend(Socket->PacketBuffer, DataLength);
    memcpy(Payload, Data, DataLength);
    SocketSend(Socket, Connection, Packet);
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTERS_SPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace(
        "CharactersSpawn -> Character(%lld)",
        Character->CharacterIndex
    );

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        if (Party) SendPartyData(Context, Context->ClientSocket, Party);
    }
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_DESPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharactersDespawn -> Character(%lld)",
        Character->CharacterIndex
    );

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        if (Party) SendPartyData(Context, Context->ClientSocket, Party);
    }
}

NOTIFICATION_PROCEDURE_BINDING(MOBS_SPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("MobSpawn -> Character(%lld)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(MOBS_DESPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("MobsDespawn -> Character(%lld)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(ITEMS_SPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("ItemsSpawn -> Character(%lld)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(ITEMS_DESPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("ItemsDespawn");
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_ITEM_EQUIP) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterItemEquip");
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_ITEM_UNEQUIP) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterItemUnequip");
}


NOTIFICATION_PROCEDURE_BINDING(MOB_MOVE_BEGIN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("MobMoveBegin(%d, %d, %d, %d, %d, %d, %d, %d)",
        Notification->Entity.EntityIndex,
        Notification->Entity.WorldIndex,
        Notification->Entity.EntityType,
        Notification->TickCount,
        Notification->PositionBeginX,
        Notification->PositionBeginY,
        Notification->PositionEndX,
        Notification->PositionEndY
    );
}

NOTIFICATION_PROCEDURE_BINDING(MOB_MOVE_END) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("MobMoveEnd(%d, %d, %d, %d, %d)",
        Notification->Entity.EntityIndex,
        Notification->Entity.WorldIndex,
        Notification->Entity.EntityType,
        Notification->PositionCurrentX,
        Notification->PositionCurrentY
    );
}

NOTIFICATION_PROCEDURE_BINDING(MOB_CHASE_BEGIN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("MobChaseBegin(%d, %d, %d, %d, %d, %d, %d, %d)",
        Notification->Entity.EntityIndex,
        Notification->Entity.WorldIndex,
        Notification->Entity.EntityType,
        Notification->TickCount,
        Notification->PositionBeginX,
        Notification->PositionBeginY,
        Notification->PositionEndX,
        Notification->PositionEndY
    );
}

NOTIFICATION_PROCEDURE_BINDING(MOB_CHASE_END) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("MobChaseEnd(%d, %d, %d, %d, %d)",
        Notification->Entity.EntityIndex,
        Notification->Entity.WorldIndex,
        Notification->Entity.EntityType,
        Notification->PositionCurrentX,
        Notification->PositionCurrentY
    );
}

NOTIFICATION_PROCEDURE_BINDING(SKILL_TO_CHARACTER) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
}

NOTIFICATION_PROCEDURE_BINDING(ATTACK_TO_MOB) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_BATTLE_RANK_UP) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterBattleRankUp -> Character(%lld)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_DATA) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterData -> Character(%lld)",
        Character->CharacterIndex
    );

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        SendPartyData(Context, Context->ClientSocket, Party);
    }
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_EVENT) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterEvent -> Character(%lld)",
        Character->CharacterIndex
    );

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        SendPartyData(Context, Context->ClientSocket, Party);
    }
}

NOTIFICATION_PROCEDURE_BINDING(DUNGEON_PATTERN_PART_COMPLETED) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
}

NOTIFICATION_PROCEDURE_BINDING(PARTY_QUEST_ACTION) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
}

NOTIFICATION_PROCEDURE_BINDING(PARTY_QUEST_LOOT_ITEM) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
}
 
NOTIFICATION_PROCEDURE_BINDING(PARTY_QUEST_MISSION_MOB_KILL) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
}

NOTIFICATION_PROCEDURE_BINDING(CREATE_ITEM) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
}

NOTIFICATION_PROCEDURE_BINDING(MOB_ATTACK_AOE) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
}

NOTIFICATION_PROCEDURE_BINDING(CHANGE_GENDER) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        SendPartyData(Context, Context->ClientSocket, Party);
    }
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_SKILL_MASTERY_UPDATE) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterSkillMasteryUpdate -> Character(%lld)",
        Character->CharacterIndex
    );

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        SendPartyData(Context, Context->ClientSocket, Party);
    }
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_FORCE_WING_GRADE) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterForceWingGrade -> Character(%lld)",
        Character->CharacterIndex
    );

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        SendPartyData(Context, Context->ClientSocket, Party);
    }
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_FORCE_WING_UPDATE) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterForceWingUpdate -> Character(%lld)",
        Character->CharacterIndex
    );

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        SendPartyData(Context, Context->ClientSocket, Party);
    }
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_FORCE_WING_EXP) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterForceWingExp -> Character(%lld)",
        Character->CharacterIndex
    );

    if (!RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
        SendPartyData(Context, Context->ClientSocket, Party);
    }
}

Void BroadcastMessage(
    ServerContextRef Context,
    CString Message
) {
    /*
    Int32 MessageLength = MIN((Int32)strlen(Message), sizeof(S2C_DATA_NFY_MESSAGE_BROADCAST_BODY) - 1);

    S2C_DATA_NFY_MESSAGE_BROADCAST* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_MESSAGE_BROADCAST);
    // Notification->Payload.MessagePayloadHeader = ...;
    Notification->Payload.MessagePayloadLength = MAX(0, MessageLength - (
        sizeof(S2C_DATA_NFY_MESSAGE_BROADCAST_PAYLOAD) -
        sizeof(Notification->Payload.MessagePayloadLength)
    ));
    memcpy(&Notification->Payload.Body.Message[0], Message, MessageLength);

    for (Int32 Index = 0; Index < Context->Runtime->CharacterCount; Index += 1) {
        RTCharacterRef Character = &Context->Runtime->Characters[Index];
        ClientContextRef Client = ServerGetClientByEntity(Context, Character->ID);
        if (Client) {
            SocketSend(Context->ClientSocket, Client->Connection, Notification);
        }
    }

    SocketSendAll(Context->MasterSocket, Notification);
    */
}

Void BroadcastUserList(
    ServerRef Server,
    ServerContextRef Context
) { 
    IPC_W2M_DATA_NFY_WORLD_INFO* Notification = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2M, NFY_WORLD_INFO);
    Notification->Header.Source = Server->IPCSocket->NodeID;
    Notification->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Notification->Header.Target.Type = IPC_TYPE_MASTER;
    Notification->PlayerCount = SocketGetConnectionCount(Context->ClientSocket);
    Notification->MaxPlayerCount = Context->Config.WorldSvr.MaxConnectionCount;
    CStringCopySafe(Notification->Host, 64 + 1, Context->Config.WorldSvr.Host);
    Notification->Port = Context->Config.WorldSvr.Port;
    Notification->Type = (UInt32)Context->Runtime->Environment.RawValue;
    IPCSocketUnicast(Server->IPCSocket, Notification);
}

Void BroadcastToParty(
    ServerContextRef Context,
    RTEntityID PartyID,
    Void *Notification
) {
    if (RTPartyIsSoloDungeon(PartyID)) {
        RTPartyRef Party = RTPartyManagerGetParty(Context->Runtime->PartyManager, PartyID);
        if (!Party) return;

        for (Index Index = 0; Index < Party->MemberCount; Index += 1) {
            RTPartySlotRef PartySlot = &Party->Members[Index];
            ClientContextRef Client = ServerGetClientByIndex(Context, PartySlot->Info.CharacterIndex, NULL);
            if (!Client) return;

            SocketSend(Context->ClientSocket, Client->Connection, Notification);
        }

        return;
    }

    IPC_W2P_DATA_BROADCAST_TO_PARTY* Request = IPCPacketBufferInit(Context->IPCSocket->PacketBuffer, W2P, BROADCAST_TO_PARTY);
    Request->Header.Source = Context->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_PARTY;
    Request->PartyID = PartyID;
    Request->Length = PacketGetLength(
        Context->ClientSocket->ProtocolIdentifier,
        Context->ClientSocket->ProtocolVersion,
        Context->ClientSocket->ProtocolExtension,
        Notification
    );
    IPCPacketBufferAppendCopy(Context->IPCSocket->PacketBuffer, Notification, Request->Length);
    IPCSocketUnicast(Context->IPCSocket, Request);
}

struct _BroadcastToWorldArguments {
    ServerContextRef Context;
    RTRuntimeRef Runtime;
    RTEntityID Source;
    Void* Packet;
};

Void _BroadcastToWorldProc(
    RTEntityID Entity,
    Void* Userdata
) {
    struct _BroadcastToWorldArguments* Arguments = (struct _BroadcastToWorldArguments*)Userdata;

    if (RTEntityIsEqual(Entity, Arguments->Source)) return;

    ClientContextRef Client = ServerGetClientByEntity(Arguments->Context, Entity);
    if (!Client) return;

    SocketSend(Arguments->Context->ClientSocket, Client->Connection, Arguments->Packet);
}

Void BroadcastToWorld(
    ServerContextRef Context,
    RTWorldContextRef WorldContext,
    RTEntityID Entity,
    Int32 X,
    Int32 Y,
    Void *Packet
) {
    struct _BroadcastToWorldArguments Arguments = { 0 };
    Arguments.Context = Context;
    Arguments.Runtime = Context->Runtime;
    Arguments.Source = Entity;
    Arguments.Packet = Packet;
    RTWorldContextEnumerateBroadcastTargets(
        WorldContext,
        RUNTIME_ENTITY_TYPE_CHARACTER,
        X,
        Y,
        &_BroadcastToWorldProc,
        &Arguments
    );
}
