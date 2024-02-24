#include "ClientProtocol.h"
#include "IPCProtocol.h"
#include "Notification.h"
#include "Server.h"

Void BroadcastMessage(
    ServerContextRef Context,
    CString Message
) {
    /*
    Int32 MessageLength = MIN((Int32)strlen(Message), sizeof(S2C_DATA_NFY_MESSAGE_BROADCAST_BODY) - 1);

    S2C_DATA_NFY_MESSAGE_BROADCAST* Notification = PacketInit(S2C_DATA_NFY_MESSAGE_BROADCAST);
    Notification->Command = S2C_NFY_MESSAGE_BROADCAST;
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
    ServerContextRef Context
) {
    IPC_DATA_WORLD_NFYUSERLIST* Notification = PacketInit(IPC_DATA_WORLD_NFYUSERLIST);
    Notification->Command = IPC_WORLD_NFYUSERLIST;
    Notification->PlayerCount = SocketGetConnectionCount(Context->ClientSocket);
    Notification->MaxPlayerCount = Context->Config.WorldSvr.MaxConnectionCount;
    SocketSendAll(Context->MasterSocket, Notification);
}

struct _AppendMobToMobSpawnIndexArguments {
    RTWorldContextRef WorldContext;
    S2C_DATA_MOBS_SPAWN* Notification;
};

Void AppendMobToMobSpawnIndex(
    RTEntityID Entity,
    Void* Userdata
) {
    struct _AppendMobToMobSpawnIndexArguments* Arguments = (struct _AppendMobToMobSpawnIndexArguments*)Userdata;
    RTMobRef Mob = RTWorldContextGetMob(Arguments->WorldContext, Entity);
    if (Mob->IsDead) return;

    Arguments->Notification->Count += 1;

    S2C_DATA_MOBS_SPAWN_INDEX* Spawn = PacketAppendStruct(S2C_DATA_MOBS_SPAWN_INDEX);
    Spawn->Entity = Mob->ID;
    Spawn->PositionBegin.X = Mob->Movement.PositionBegin.X;
    Spawn->PositionBegin.Y = Mob->Movement.PositionBegin.Y;
    Spawn->PositionEnd.X = Mob->Movement.PositionEnd.X;
    Spawn->PositionEnd.Y = Mob->Movement.PositionEnd.Y;
    Spawn->MobSpeciesIndex = Mob->Spawn.MobSpeciesIndex;
    Spawn->MaxHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    Spawn->CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    Spawn->Level = Mob->SpeciesData->Level;
}

Void ServerBroadcastMobListToClient(
    ServerContextRef Context,
    ClientContextRef Client
) {
    RTRuntimeRef Runtime = Context->Runtime;
    RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);
    RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
    assert(WorldContext);

    S2C_DATA_MOBS_SPAWN* Notification = PacketInit(S2C_DATA_MOBS_SPAWN);
    Notification->Command = S2C_MOBS_SPAWN; 
    Notification->Count = 0;

    struct _AppendMobToMobSpawnIndexArguments Arguments = { 0 };
    Arguments.WorldContext = WorldContext;
    Arguments.Notification = Notification;
    RTWorldContextEnumerateBroadcastTargets(
        WorldContext,
        RUNTIME_ENTITY_TYPE_MOB,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y,
        &AppendMobToMobSpawnIndex,
        &Arguments
    );

    if (Notification->Count > 0) {
        SocketSend(Context->ClientSocket, Client->Connection, Notification);
    }
}

struct _AppendCharacterToCharacterSpawnIndexArguments {
    ServerContextRef Context;
    RTWorldContextRef WorldContext;
    S2C_DATA_CHARACTERS_SPAWN* Notification;
};

Void AppendCharacterToCharacterSpawnIndex(
    RTEntityID Entity,
    Void* Userdata
) {
    struct _AppendCharacterToCharacterSpawnIndexArguments* Arguments = (struct _AppendCharacterToCharacterSpawnIndexArguments*)Userdata;
    RTCharacterRef Character = RTWorldManagerGetCharacter(Arguments->WorldContext->WorldManager, Entity);

    Arguments->Notification->Count += 1;

    S2C_DATA_CHARACTERS_SPAWN_INDEX* Spawn = PacketAppendStruct(S2C_DATA_CHARACTERS_SPAWN_INDEX);
    Spawn->CharacterIndex = Character->CharacterIndex;
    Spawn->Entity = Character->ID;
    Spawn->Level = Character->Info.Basic.Level;
    Spawn->OverlordLevel = Character->Info.Overlord.Level;
    Spawn->ForceWingRank = 0;
    Spawn->ForceWingLevel = Character->Info.ForceWing.Level;
    Spawn->MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    Spawn->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    Spawn->MovementSpeed = (UInt32)(Character->Movement.Speed * RUNTIME_MOVEMENT_SPEED_SCALE);
    Spawn->PositionBegin.X = Character->Movement.PositionBegin.X;
    Spawn->PositionBegin.Y = Character->Movement.PositionBegin.Y;
    Spawn->PositionEnd.X = Character->Movement.PositionEnd.X;
    Spawn->PositionEnd.Y = Character->Movement.PositionEnd.Y;
    Spawn->PKLevel = 0;
    Spawn->Nation = Character->Info.Profile.Nation;
    Spawn->CharacterStyle = SwapUInt32(Character->Info.Style.RawValue);
    Spawn->CharacterLiveStyle = 0;
    Spawn->SkillEffectIndex = 0;
    Spawn->BattleMode = 0;
    Spawn->IsDead = RTCharacterIsAlive(Arguments->WorldContext->WorldManager->Runtime, Character) ? 0 : 1;
    Spawn->EquipmentSlotCount = Character->EquipmentInfo.Count;
    Spawn->IsPersonalShop = 0;
    Spawn->GuildIndex = 0;
    Spawn->PetFlags = 0;
    Spawn->ActiveBuffCount = 0;
    Spawn->UnknownCount = 0;
    Spawn->SpecialBuffCount = 0;
    Spawn->PassiveBuffCount = 0;
    Spawn->DisplayTitle = 0;
    Spawn->EventTitle = 0;
    Spawn->GuildTitle = 0;
    Spawn->WarTitle = 0;

    ClientContextRef Client = ServerGetClientByEntity(Arguments->Context, Character->ID);
    assert(Client);

    Spawn->NameLength = strlen(Client->CharacterName) + 1;
    CString Name = (CString)PacketAppendMemory(strlen(Client->CharacterName));
    memcpy(Name, Client->CharacterName, strlen(Client->CharacterName));

    PacketAppendValue(UInt8, 0); // GuildNameLength

    for (Index Index = 0; Index < Character->EquipmentInfo.Count; Index += 1) {
        RTItemSlotRef ItemSlot = &Character->EquipmentInfo.Slots[Index];

        S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* Slot = PacketAppendStruct(S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
        Slot->EquipmentSlotIndex = ItemSlot->SlotIndex;
        Slot->ItemID = ItemSlot->Item.Serial;
        Slot->ItemOptions = ItemSlot->ItemOptions;
        Slot->ItemAppearance = 0;
    }
}

Void ServerBroadcastCharacterListToClient(
    ServerContextRef Context,
    ClientContextRef Client
) {
    RTRuntimeRef Runtime = Context->Runtime;
    RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);
    RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
    assert(WorldContext);

    S2C_DATA_CHARACTERS_SPAWN* Notification = PacketInit(S2C_DATA_CHARACTERS_SPAWN);
    Notification->Command = S2C_CHARACTERS_SPAWN;
    Notification->Count = 0;
    Notification->SpawnType = S2C_DATA_ENTITY_SPAWN_TYPE_LIST;

    struct _AppendCharacterToCharacterSpawnIndexArguments Arguments = { 0 };
    Arguments.Context = Context;
    Arguments.WorldContext = WorldContext;
    Arguments.Notification = Notification;
    RTWorldContextEnumerateBroadcastTargets(
        WorldContext,
        RUNTIME_ENTITY_TYPE_CHARACTER,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y,
        &AppendCharacterToCharacterSpawnIndex,
        &Arguments
    );

    if (Notification->Count > 0) {
        SocketSend(Context->ClientSocket, Client->Connection, Notification);
    }
}

Void BroadcastToParty(
    ServerContextRef Context,
    RTEntityID PartyID,
    Void *Notification
) {
    RTPartyRef Party = RTRuntimeGetParty(Context->Runtime, PartyID);
    if (!Party) return;

    for (Int32 Index = 0; Index < Party->MemberCount; Index += 1) {
        RTPartySlotRef Slot = &Party->Members[Index];
        if (RTEntityIsNull(Slot->MemberID)) continue;

        RTCharacterRef Character = RTWorldManagerGetCharacter(Context->Runtime->WorldManager, Slot->MemberID);
        if (!Character) continue;

        ClientContextRef Client = ServerGetClientByEntity(Context, Character->ID);
        if (!Client) continue;

        SocketSend(Context->ClientSocket, Client->Connection, Notification);
    }
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

    /* TODO: Add a correct chunking system into RTWorld
    
    Int32 ChunkX = (X >= 0) ? X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT : -1;
    Int32 ChunkY = (Y >= 0) ? Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT : -1;
    if (ChunkX < 0 || ChunkY < 0) return;

    for (Int32 Index = 0; Index < World->CharacterCount; Index++) {
        RTEntity CharacterEntity = World->Characters[Index];
        if (!RTEntityIsNull(Entity) && RTEntityIsEqual(Entity, CharacterEntity)) continue;

        RTCharacterRef Character = RTWorldManagerGetCharacter(Context->Runtime->WorldManager, CharacterEntity);
        RTMovementUpdateDeadReckoning(Context->Runtime, &Character->Movement);

        Int32 CharacterChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
        Int32 CharacterChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
        Int32 MaxDistance = 2;
        Int32 DeltaX = ABS(ChunkX - CharacterChunkX);
        Int32 DeltaY = ABS(ChunkY - CharacterChunkY);

        if (DeltaX <= MaxDistance && DeltaY <= MaxDistance) {
            ClientContextRef Client = ServerGetClientByEntity(Server, CharacterEntity);
            if (Client) {
                SocketSend(Server->ClientSocket, Client->Connection, Packet);
            }
        }
    }
    */
}
// TODO: The characters and mobs shouldn't be respawned when the client already has received a spawn!
Void ServerRuntimeOnEvent(
    RTRuntimeRef Runtime,
    RTEventRef Event,
    Void* UserData
) {
    ServerContextRef Context = (ServerContextRef)UserData;

    if (Event->Type == RUNTIME_EVENT_CHARACTER_SPAWN) {
        S2C_DATA_CHARACTERS_SPAWN* Notification = PacketInit(S2C_DATA_CHARACTERS_SPAWN);
        Notification->Command = S2C_CHARACTERS_SPAWN;
        Notification->Count = 1;
        Notification->SpawnType = S2C_DATA_ENTITY_SPAWN_TYPE_INIT; // TODO: Add all spawn types!

        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(
            Runtime->WorldManager, 
            Event->Data.CharacterSpawn.CharacterIndex
        );

        S2C_DATA_CHARACTERS_SPAWN_INDEX* Spawn = PacketAppendStruct(S2C_DATA_CHARACTERS_SPAWN_INDEX);
        Spawn->CharacterIndex = Character->CharacterIndex;
        Spawn->Entity = Character->ID;
        Spawn->Level = Character->Info.Basic.Level;
        Spawn->OverlordLevel = Character->Info.Overlord.Level;
        Spawn->ForceWingRank = 0;
        Spawn->ForceWingLevel = Character->Info.ForceWing.Level;
        Spawn->MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        Spawn->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
        Spawn->MovementSpeed = (UInt32)(Character->Movement.Speed * RUNTIME_MOVEMENT_SPEED_SCALE);
        Spawn->PositionBegin.X = Character->Movement.PositionBegin.X;
        Spawn->PositionBegin.Y = Character->Movement.PositionBegin.Y;
        Spawn->PositionEnd.X = Character->Movement.PositionEnd.X;
        Spawn->PositionEnd.Y = Character->Movement.PositionEnd.Y;
        Spawn->PKLevel = 0;
        Spawn->Nation = Character->Info.Profile.Nation;
        Spawn->CharacterStyle = SwapUInt32(Character->Info.Style.RawValue);
        Spawn->CharacterLiveStyle = 0;
        Spawn->SkillEffectIndex = 0;
        Spawn->BattleMode = 0;
        Spawn->IsDead = RTCharacterIsAlive(Runtime, Character) ? 0 : 1;
        Spawn->EquipmentSlotCount = Character->EquipmentInfo.Count;
        Spawn->IsPersonalShop = 0;
        Spawn->GuildIndex = 0;
        Spawn->PetFlags = 0;
        Spawn->ActiveBuffCount = 0;
        Spawn->UnknownCount = 0;
        Spawn->SpecialBuffCount = 0;
        Spawn->PassiveBuffCount = 0;
        Spawn->DisplayTitle = 0;
        Spawn->EventTitle = 0;
        Spawn->GuildTitle = 0;
        Spawn->WarTitle = 0;

        ClientContextRef Client = ServerGetClientByEntity(Context, Character->ID);
        assert(Client);

        Spawn->NameLength = strlen(Client->CharacterName) + 1;
        CString Name = (CString)PacketAppendMemory(strlen(Client->CharacterName));
        memcpy(Name, Client->CharacterName, strlen(Client->CharacterName));

        PacketAppendValue(UInt8, 0); // GuildNameLength

        for (Index Index = 0; Index < Character->EquipmentInfo.Count; Index += 1) {
            RTItemSlotRef ItemSlot = &Character->EquipmentInfo.Slots[Index];

            S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* Slot = PacketAppendStruct(S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
            Slot->EquipmentSlotIndex = ItemSlot->SlotIndex;
            Slot->ItemID = ItemSlot->Item.Serial;
            Slot->ItemOptions = ItemSlot->ItemOptions;
            Slot->ItemAppearance = 0;
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

    if (Event->Type == RUNTIME_EVENT_CHARACTER_DESPAWN) {
        S2C_DATA_CHARACTERS_DESPAWN* Notification = PacketInit(S2C_DATA_CHARACTERS_DESPAWN);
        Notification->Command = S2C_CHARACTERS_DESPAWN;
        Notification->CharacterIndex = Event->Data.CharacterSpawn.CharacterIndex;
        Notification->DespawnType = S2C_DATA_ENTITY_DESPAWN_TYPE_DISAPPEAR; // TODO: Add all spawn types!

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_CHUNK_UPDATE) {
        ClientContextRef Client = ServerGetClientByEntity(Context, Event->TargetID);
        if (Client) {
            ServerBroadcastMobListToClient(Context, Client);
            ServerBroadcastCharacterListToClient(Context, Client);
        }
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_LEVEL_UP) {
        ClientContextRef Client = ServerGetClientByEntity(Context, Event->TargetID);
        if (!Client) return;

        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);
        {
            S2C_DATA_NFY_CHARACTER_DATA* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_DATA);
            Notification->Command = S2C_NFY_CHARACTER_DATA;
            Notification->Type = S2C_DATA_CHARACTER_UPDATE_TYPE_LEVEL;
            Notification->Level = Character->Info.Basic.Level;
            SocketSend(Context->ClientSocket, Client->Connection, Notification);
        }

        S2C_DATA_NFY_CHARACTER_EVENT* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_EVENT);
        Notification->Command = S2C_NFY_CHARACTER_EVENT;
        Notification->Type = S2C_DATA_CHARACTER_EVENT_TYPE_LEVELUP;
        Notification->CharacterIndex = Client->CharacterIndex;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_OVERLORD_LEVEL_UP) {
        ClientContextRef Client = ServerGetClientByEntity(Context, Event->TargetID);
        if (!Client) return;

        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);
        {
            S2C_DATA_NFY_CHARACTER_DATA* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_DATA);
            Notification->Command = S2C_NFY_CHARACTER_DATA;
            Notification->Type = S2C_DATA_CHARACTER_UPDATE_TYPE_OVERLORD_LEVEL;
            Notification->Level = Character->Info.Overlord.Level;
            SocketSend(Context->ClientSocket, Client->Connection, Notification);
        }

        S2C_DATA_NFY_CHARACTER_EVENT* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_EVENT);
        Notification->Command = S2C_NFY_CHARACTER_EVENT;
        Notification->Type = S2C_DATA_CHARACTER_EVENT_TYPE_OVERLORD_LEVELUP;
        Notification->CharacterIndex = Client->CharacterIndex;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_DATA) {
        ClientContextRef Client = ServerGetClientByEntity(Context, Event->TargetID);
        if (Client) {
            RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);
            S2C_DATA_NFY_CHARACTER_DATA* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_DATA);
            Notification->Command = S2C_NFY_CHARACTER_DATA;
            Notification->Type = Event->Data.CharacterData.Type;
            
            if (Notification->Type == S2C_DATA_CHARACTER_UPDATE_TYPE_HPPOTION) {
                Notification->HPAfterPotion = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
            }

            if (Notification->Type == S2C_DATA_CHARACTER_UPDATE_TYPE_MPPOTION) {
                // TODO: Check if this is also padded like HPAfterPotion
                Notification->MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
            }

            if (Notification->Type == S2C_DATA_CHARACTER_UPDATE_TYPE_HP) {
                Notification->HP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
            }

            if (Notification->Type == S2C_DATA_CHARACTER_UPDATE_TYPE_MP) {
                Notification->MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
            }

            if (Notification->Type == S2C_DATA_CHARACTER_UPDATE_TYPE_SP) {
                Notification->MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
            }

            if (Notification->Type == S2C_DATA_CHARACTER_UPDATE_TYPE_RAGE) {
                Notification->Rage = Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
            }

            // TODO: Add missing types of updates...
            // TODO: This can cause the client to close... but the packet length is correct!
            SocketSend(Context->ClientSocket, Client->Connection, Notification);
        }

        return;
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_UPDATE_SKILL_STATUS) {
        ClientContextRef Client = ServerGetClientByEntity(Context, Event->TargetID);
        if (Client) {
            RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);

            Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
            Int32 SkillLevelMax = RTRuntimeDataCharacterRankUpConditionGet(
                Runtime->Context,
                Character->Info.Skill.Rank,
                BattleStyleIndex
            );

            S2C_DATA_UPDATE_SKILL_STATUS* Notification = PacketInit(S2C_DATA_UPDATE_SKILL_STATUS);
            Notification->Command = S2C_UPDATE_SKILL_STATUS;
            Notification->SkillRank = Character->Info.Skill.Rank;
            Notification->SkillLevel = Character->Info.Skill.Level;
            Notification->SkillLevelMax = SkillLevelMax;
            Notification->SkillExp = (UInt32)Character->Info.Skill.Exp;
            Notification->SkillPoint = Character->Info.Skill.Point;
            SocketSend(Context->ClientSocket, Client->Connection, Notification);
        }

        return;
    }

    if (Event->Type == RUNTIME_EVENT_CHARACTER_BATTLE_RANK_UP) {
        ClientContextRef Client = ServerGetClientByEntity(Context, Event->TargetID);
        if (!Client) return;

        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Client->CharacterIndex);
        {
            S2C_DATA_BATTLE_RANK_UP* Notification = PacketInit(S2C_DATA_BATTLE_RANK_UP);
            Notification->Command = S2C_BATTLE_RANK_UP;
            Notification->Level = Character->Info.Style.BattleRank;
            SocketSend(Context->ClientSocket, Client->Connection, Notification);
        }

        S2C_DATA_NFY_CHARACTER_EVENT* Notification = PacketInit(S2C_DATA_NFY_CHARACTER_EVENT);
        Notification->Command = S2C_NFY_CHARACTER_EVENT;
        Notification->Type = S2C_DATA_CHARACTER_EVENT_TYPE_RANKUP;
        Notification->CharacterIndex = Client->CharacterIndex;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_SPAWN || Event->Type == RUNTIME_EVENT_MOB_UPDATE) {
        S2C_DATA_MOBS_SPAWN* Notification = PacketInit(S2C_DATA_MOBS_SPAWN);
        Notification->Command = S2C_MOBS_SPAWN;
        Notification->Count = 1;

        S2C_DATA_MOBS_SPAWN_INDEX* Spawn = PacketAppendStruct(S2C_DATA_MOBS_SPAWN_INDEX);
        Spawn->Entity = Event->TargetID;
        Spawn->PositionBegin.X = Event->Data.MobSpawnOrUpdate.PositionBeginX;
        Spawn->PositionBegin.Y = Event->Data.MobSpawnOrUpdate.PositionBeginY;
        Spawn->PositionEnd.X = Event->Data.MobSpawnOrUpdate.PositionEndX;
        Spawn->PositionEnd.Y = Event->Data.MobSpawnOrUpdate.PositionEndY;
        Spawn->MobSpeciesIndex = Event->Data.MobSpawnOrUpdate.MobSpeciesIndex;
        Spawn->MaxHP = Event->Data.MobSpawnOrUpdate.MaxHP;
        Spawn->CurrentHP = Event->Data.MobSpawnOrUpdate.CurrentHP;
        Spawn->Level = Event->Data.MobSpawnOrUpdate.Level;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_DESPAWN) {
        S2C_DATA_MOBS_DESPAWN* Notification = PacketInit(S2C_DATA_MOBS_DESPAWN);
        Notification->Command = S2C_MOBS_DESPAWN;
        Notification->Entity = Event->TargetID;
        Notification->DespawnType = S2C_DATA_ENTITY_DESPAWN_TYPE_DEAD;

        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_MOB_MOVEMENT_BEGIN) {
        S2C_DATA_MOB_MOVEMENT_BEGIN* Notification = PacketInit(S2C_DATA_MOB_MOVEMENT_BEGIN);
        Notification->Command = S2C_MOB_MOVEMENT_BEGIN;
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
        S2C_DATA_MOB_MOVEMENT_END* Notification = PacketInit(S2C_DATA_MOB_MOVEMENT_END);
        Notification->Command = S2C_MOB_MOVEMENT_END;
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
        S2C_DATA_MOB_CHASE_BEGIN* Notification = PacketInit(S2C_DATA_MOB_CHASE_BEGIN);
        Notification->Command = S2C_MOB_CHASE_BEGIN;
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
        S2C_DATA_MOB_CHASE_END* Notification = PacketInit(S2C_DATA_MOB_CHASE_END);
        Notification->Command = S2C_MOB_CHASE_END;
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
        S2C_DATA_NFY_MOB_ATTACK_AOE* Notification = PacketInit(S2C_DATA_NFY_MOB_ATTACK_AOE);
        Notification->Command = S2C_NFY_MOB_ATTACK_AOE;
        Notification->Entity = Event->SourceID;
        Notification->IsDefaultSkill = Event->Data.MobAttack.IsDefaultSkill;
        Notification->MobHP = Event->Data.MobAttack.MobHP;
        Notification->TargetCount = Event->Data.MobAttack.ResultCount;

        for (Int32 Index = 0; Index < Event->Data.MobAttack.ResultCount; Index += 1) {
            RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Event->Data.MobAttack.Results[Index].Entity);

            S2C_DATA_MOB_ATTACK_TARGET* Target = PacketAppendStruct(S2C_DATA_MOB_ATTACK_TARGET);
            Target->CharacterIndex = Character->CharacterIndex;
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

    if (Event->Type == RUNTIME_EVENT_ITEM_SPAWN) {
        S2C_DATA_NFY_SPAWN_ITEM* Notification = PacketInit(S2C_DATA_NFY_SPAWN_ITEM);
        Notification->Command = S2C_NFY_SPAWN_ITEM;
        Notification->Count = 1;

        UInt32 SourceIndex = *(UInt32 *)&Event->SourceID;
        if (Event->SourceID.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
            RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Event->SourceID);
            SourceIndex = Character->CharacterIndex;
        }

        S2C_DATA_NFY_SPAWN_ITEM_INDEX* Spawn = PacketAppendStruct(S2C_DATA_NFY_SPAWN_ITEM_INDEX);
        Spawn->Entity = Event->Data.ItemSpawn.Entity;
        Spawn->ItemOptions = Event->Data.ItemSpawn.ItemOptions;
        Spawn->SourceIndex = SourceIndex;
        Spawn->ItemID = Event->Data.ItemSpawn.ItemID;
        Spawn->X = Event->Data.ItemSpawn.X;
        Spawn->Y = Event->Data.ItemSpawn.Y;
        Spawn->UniqueKey = Event->Data.ItemSpawn.UniqueKey;
        Spawn->ContextType = S2C_DATA_NFY_SPAWN_ITEM_CONTEXT_TYPE_MOBS;
        Spawn->ItemProperty = Event->Data.ItemSpawn.ItemProperty;
        
        return BroadcastToWorld(
            Context,
            Event->World,
            kEntityIDNull,
            Event->X,
            Event->Y,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_ITEM_DESPAWN) {
        S2C_DATA_NFY_DESPAWN_ITEM* Notification = PacketInit(S2C_DATA_NFY_DESPAWN_ITEM);
        Notification->Command = S2C_NFY_DESPAWN_ITEM;
        Notification->Entity = Event->TargetID;
        Notification->DespawnType = S2C_DATA_ENTITY_DESPAWN_TYPE_DISAPPEAR;

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
        S2C_DATA_NFY_QUEST_DUNGEON_PATTERN_PART_COMPLETED* Notification = PacketInit(S2C_DATA_NFY_QUEST_DUNGEON_PATTERN_PART_COMPLETED);
        Notification->Command = S2C_NFY_QUEST_DUNGEON_PATTERN_PART_COMPLETED;
        Notification->PatternPartIndex = Event->World->PatternPartIndex;

        return BroadcastToParty(
            Context,
            Event->TargetID,
            Notification
        );
    }

    if (Event->Type == RUNTIME_EVENT_PARTY_QUEST_MISSION_MOB_KILL) {
        S2C_DATA_NFY_PARTY_QUEST_MISSION_MOB_KILL* Notification = PacketInit(S2C_DATA_NFY_PARTY_QUEST_MISSION_MOB_KILL);
        Notification->Command = S2C_NFY_PARTY_QUEST_MISSION_MOB_KILL;
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

        S2C_DATA_NFY_ATTACK_TO_MOB* Notification = PacketInit(S2C_DATA_NFY_ATTACK_TO_MOB);
        Notification->Command = S2C_NFY_ATTACK_TO_MOB;
        Notification->CharacterIndex = Character->CharacterIndex,
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
