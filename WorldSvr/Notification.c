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
        "CharactersSpawn -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_DESPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharactersDespawn -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(MOBS_SPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("MobSpawn -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(MOBS_DESPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("MobsDespawn -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(ITEMS_SPAWN) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("ItemsSpawn -> Character(%d)",
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

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_BATTLE_RANK_UP) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterBattleRankUp -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_DATA) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterData -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_EVENT) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterEvent -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_SKILL_MASTERY_UPDATE) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterSkillMasteryUpdate -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_FORCE_WING_GRADE) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterForceWingGrade -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_FORCE_WING_UPDATE) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterForceWingUpdate -> Character(%d)",
        Character->CharacterIndex
    );
}

NOTIFICATION_PROCEDURE_BINDING(CHARACTER_FORCE_WING_EXP) {
    SendRuntimeNotification(Socket, Connection, (RTNotificationRef)Notification);
    Trace("CharacterForceWingExp -> Character(%d)",
        Character->CharacterIndex
    );
}

Void BroadcastPartyData(
    ServerContextRef Context,
    RTCharacterRef Character
) {
    IPC_W2P_DATA_PARTY_DATA* Request = IPCPacketBufferInit(Context->IPCSocket->PacketBuffer, W2P, PARTY_DATA);
    Request->Header.Source = Context->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_PARTY;
    Request->MemberInfo.CharacterIndex = Character->CharacterIndex;
    Request->MemberInfo.Level = Character->Info.Basic.Level;
    Request->MemberInfo.OverlordLevel = Character->Info.Overlord.Level;
    Request->MemberInfo.MythRebirth = 0;
    Request->MemberInfo.MythHolyPower = 0;
    Request->MemberInfo.MythLevel = 0;
    Request->MemberInfo.ForceWingGrade = Character->ForceWingInfo.Grade;
    Request->MemberInfo.ForceWingLevel = Character->ForceWingInfo.Level;
    IPCSocketUnicast(Context->IPCSocket, Request);
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
    Notification->Type = Context->Config.WorldSvr.WorldType;
    IPCSocketUnicast(Server->IPCSocket, Notification);
}

Void BroadcastToParty(
    ServerContextRef Context,
    RTEntityID PartyID,
    Void *Notification
) {
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

Void ServerRuntimeOnEvent(
    RTRuntimeRef Runtime,
    RTEventRef Event,
    Void* UserData
) {
    ServerContextRef Context = (ServerContextRef)UserData;
    PacketBufferRef PacketBuffer = Context->ClientSocket->PacketBuffer;

    if (Event->Type == RUNTIME_EVENT_MOB_MOVEMENT_BEGIN) {
        S2C_DATA_MOB_MOVEMENT_BEGIN* Notification = PacketBufferInit(PacketBuffer, S2C, MOB_MOVEMENT_BEGIN);
        Notification->Entity = Event->TargetID;
        Notification->TickCount = Event->Data.MobMovementBegin.TickCount;
        Notification->PositionBegin.X = Event->Data.MobMovementBegin.PositionBeginX;
        Notification->PositionBegin.Y = Event->Data.MobMovementBegin.PositionBeginY;
        Notification->PositionEnd.X = Event->Data.MobMovementBegin.PositionEndX;
        Notification->PositionEnd.Y = Event->Data.MobMovementBegin.PositionEndY;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_MOVEMENT_END) {
        S2C_DATA_MOB_MOVEMENT_END* Notification = PacketBufferInit(PacketBuffer, S2C, MOB_MOVEMENT_END);
        Notification->Entity = Event->TargetID;
        Notification->Position.X = Event->Data.MobMovementEnd.PositionCurrentX;
        Notification->Position.Y = Event->Data.MobMovementEnd.PositionCurrentY;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_CHASE_BEGIN) {
        S2C_DATA_MOB_CHASE_BEGIN* Notification = PacketBufferInit(PacketBuffer, S2C, MOB_CHASE_BEGIN);
        Notification->Entity = Event->TargetID;
        Notification->TickCount = Event->Data.MobMovementBegin.TickCount;
        Notification->PositionBegin.X = Event->Data.MobMovementBegin.PositionBeginX;
        Notification->PositionBegin.Y = Event->Data.MobMovementBegin.PositionBeginY;
        Notification->PositionEnd.X = Event->Data.MobMovementBegin.PositionEndX;
        Notification->PositionEnd.Y = Event->Data.MobMovementBegin.PositionEndY;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_CHASE_END) {
        S2C_DATA_MOB_CHASE_END* Notification = PacketBufferInit(PacketBuffer, S2C, MOB_CHASE_END);
        Notification->Entity = Event->TargetID;
        Notification->Position.X = Event->Data.MobMovementEnd.PositionCurrentX;
        Notification->Position.Y = Event->Data.MobMovementEnd.PositionCurrentY;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_ATTACK) {
        S2C_DATA_NFY_MOB_ATTACK_AOE* Notification = PacketBufferInit(PacketBuffer, S2C, NFY_MOB_ATTACK_AOE);
        Notification->Entity = Event->SourceID;
        Notification->IsDefaultSkill = Event->Data.MobAttack.IsDefaultSkill;
        Notification->MobHP = Event->Data.MobAttack.MobHP;
        Notification->TargetCount = Event->Data.MobAttack.ResultCount;

        for (Int32 Index = 0; Index < Event->Data.MobAttack.ResultCount; Index += 1) {
            RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Event->Data.MobAttack.Results[Index].Entity);

            S2C_DATA_MOB_ATTACK_TARGET* Target = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_MOB_ATTACK_TARGET);
            Target->CharacterIndex = (UInt32)Character->CharacterIndex;
            Target->IsDead = Event->Data.MobAttack.Results[Index].IsDead;
            Target->Result = Event->Data.MobAttack.Results[Index].Result;
            Target->AppliedDamage = Event->Data.MobAttack.Results[Index].AppliedDamage;
            Target->TargetHP = Event->Data.MobAttack.Results[Index].TargetHP;
        }

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_QUEST_DUNGEON_PATTERN_PART_COMPLETED) {
        S2C_DATA_NFY_QUEST_DUNGEON_PATTERN_PART_COMPLETED* Notification = PacketBufferInit(PacketBuffer, S2C, NFY_QUEST_DUNGEON_PATTERN_PART_COMPLETED);
        Notification->PatternPartIndex = Event->World->PatternPartIndex;

        return BroadcastToParty(
            Context,
            Event->TargetID,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_PARTY_QUEST_MISSION_MOB_KILL) {
        S2C_DATA_NFY_PARTY_QUEST_MISSION_MOB_KILL* Notification = PacketBufferInit(PacketBuffer, S2C, NFY_PARTY_QUEST_MISSION_MOB_KILL);
        Notification->QuestID = Event->Data.PartyQuestMissionMobKill.QuestID;
        Notification->MobSpeciesIndex = Event->Data.PartyQuestMissionMobKill.MobSpeciesIndex;

        return BroadcastToParty(
            Context,
            Event->TargetID,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_ATTACK_TO_MOB) {
        RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Event->Data.AttackToMob.Character);

        S2C_DATA_NFY_ATTACK_TO_MOB* Notification = PacketBufferInit(PacketBuffer, S2C, NFY_ATTACK_TO_MOB);
        Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
        Notification->Mob = Event->Data.AttackToMob.Mob;
        Notification->MobIDType = RUNTIME_ENTITY_TYPE_MOB;
        Notification->AttackType = Event->Data.AttackToMob.AttackType;
        Notification->MobHP = Event->Data.AttackToMob.MobHP;
        Notification->CharacterHP = Event->Data.AttackToMob.CharacterHP;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }
}
