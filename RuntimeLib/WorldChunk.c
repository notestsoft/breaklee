#include "Movement.h"
#include "PartyManager.h"
#include "Runtime.h"
#include "WorldChunk.h"
#include "WorldManager.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Void RTNotificationAppendCharacterSpawnIndex(
    RTRuntimeRef Runtime,
    NOTIFICATION_DATA_CHARACTERS_SPAWN* Notification,
    RTCharacterRef Character
) {
    NOTIFICATION_DATA_CHARACTERS_SPAWN_INDEX* NotificationCharacter = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_INDEX);
    NotificationCharacter->CharacterIndex = (UInt32)Character->CharacterIndex;
    NotificationCharacter->Entity = Character->ID;
    NotificationCharacter->Level = Character->Data.Info.Level;
    NotificationCharacter->OverlordLevel = Character->Data.OverlordMasteryInfo.Info.Level;
    NotificationCharacter->HolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
    NotificationCharacter->Rebirth = Character->Data.MythMasteryInfo.Info.Rebirth;
    NotificationCharacter->MythLevel = Character->Data.MythMasteryInfo.Info.Level;
    NotificationCharacter->ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
    NotificationCharacter->ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
    NotificationCharacter->MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    NotificationCharacter->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    NotificationCharacter->CurrentShield = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
    NotificationCharacter->MaxRage = Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX];
    NotificationCharacter->CurrentRage = Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
    NotificationCharacter->MovementSpeed = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED];
    NotificationCharacter->PositionBeginX = Character->Movement.PositionBegin.X;
    NotificationCharacter->PositionBeginY = Character->Movement.PositionBegin.Y;
    NotificationCharacter->PositionEndX = Character->Movement.PositionEnd.X;
    NotificationCharacter->PositionEndY = Character->Movement.PositionEnd.Y;
    NotificationCharacter->PKState = Character->Data.Info.PKState.RawValue;
    NotificationCharacter->Nation = Character->Data.StyleInfo.Nation;
    NotificationCharacter->Unknown3 = Character->Data.StyleInfo.Unknown2;
    NotificationCharacter->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
    NotificationCharacter->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
    NotificationCharacter->ActiveAuraSkillIndex = RTCharacterGetAuraModeSkillIndex(Runtime, Character, Character->Data.BattleModeInfo.Info.AuraModeIndex);
    NotificationCharacter->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
    NotificationCharacter->IsDead = RTCharacterIsAlive(Runtime, Character) ? 0 : 1;
    NotificationCharacter->EquipmentSlotCount = Character->Data.EquipmentInfo.Info.EquipmentSlotCount;
    NotificationCharacter->CostumeSlotCount = RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT;
    NotificationCharacter->IsInvisible = false;
    NotificationCharacter->IsPersonalShop = false;
    NotificationCharacter->GuildIndex = 0;
    NotificationCharacter->GuildColor = 0;
    NotificationCharacter->Unknown8 = 0;
    NotificationCharacter->Unknown9 = 0;
    NotificationCharacter->Unknown10 = 0;
    NotificationCharacter->HasPetInfo = 0;
    NotificationCharacter->PetCount = 0;
    NotificationCharacter->HasBlessingBeadBaloon = 0;
    NotificationCharacter->HasItemBallon = 0;
    NotificationCharacter->SkillBuffCount = Character->Data.BuffInfo.Info.SkillBuffCount;
    NotificationCharacter->GmBuffCount = Character->Data.BuffInfo.Info.GmBuffCount;
    NotificationCharacter->ForceCaliburBuffCount = Character->Data.BuffInfo.Info.ForceCaliburBuffCount;
    NotificationCharacter->FirePlaceBuffCount = Character->Data.BuffInfo.Info.FirePlaceBuffCount;
    NotificationCharacter->IsBringer = 0;
    NotificationCharacter->DisplayTitle = Character->Data.AchievementInfo.Info.DisplayTitle;
    NotificationCharacter->EventTitle = Character->Data.AchievementInfo.Info.EventTitle;
    //NotificationCharacter->WarTitle = Character->Data.AchievementInfo.Info.WarTitle;
    //NotificationCharacter->AbilityTitle = Character->Data.AchievementInfo.Info.GuildTitle;

    //NotificationCharacter->BfxCount = 1;
    //NotificationCharacter->EventNameColor = 0xC0000000;
    //NotificationCharacter->EventTitle = 11063;

    NotificationCharacter->NameLength = strlen(Character->Name) + 1;
    RTNotificationAppendCString(Notification, Character->Name);

    NOTIFICATION_DATA_CHARACTERS_SPAWN_GUILD* Guild = (NOTIFICATION_DATA_CHARACTERS_SPAWN_GUILD*)&NotificationCharacter->Name[NotificationCharacter->NameLength];
    Guild->GuildNameLength = 0;

    for (Int EquipmentIndex = 0; EquipmentIndex < Character->Data.EquipmentInfo.Info.EquipmentSlotCount; EquipmentIndex += 1) {
        RTItemSlotRef ItemSlot = &Character->Data.EquipmentInfo.EquipmentSlots[EquipmentIndex];

        NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* NotificationSlot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
        NotificationSlot->EquipmentSlotIndex = ItemSlot->SlotIndex;
        NotificationSlot->ItemID = ItemSlot->Item.Serial;
        NotificationSlot->ItemOptions = ItemSlot->ItemOptions;
        NotificationSlot->ItemDuration = ItemSlot->ItemDuration.Serial;
    }

    for (Int Index = 0; Index < RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT; Index += 1) {
        NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* Slot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
        Slot->EquipmentSlotIndex = Index;
    }

    // S2C_DATA_CHARACTERS_SPAWN_PERSONAL_SHOP_MESSAGE PersonalShopMessage[IsPersonalShop];
    // S2C_DATA_CHARACTERS_SPAWN_PERSONAL_SHOP_INFO PersonalShopInfo[IsPersonalShop];
    // S2C_DATA_CHARACTERS_SPAWN_BUFF_SLOT Buffs[ActiveBuffCount + DebuffCount + GmBuffCount + PassiveBuffCount];
    for (Int Index = 0; Index < Character->Data.BuffInfo.Info.SkillBuffCount; Index += 1) {
        RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];
    
        NOTIFICATION_DATA_CHARACTERS_SPAWN_BUFF_SLOT* NotificationBuff = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_BUFF_SLOT);
        NotificationBuff->SkillIndex = BuffSlot->SkillIndex;
        NotificationBuff->SkillLevel = BuffSlot->SkillLevel;
    }
    
    Int8 BuffSlotCounts[] = {
         Character->Data.BuffInfo.Info.SkillBuffCount,
         Character->Data.BuffInfo.Info.PotionBuffCount,
         Character->Data.BuffInfo.Info.GmBuffCount,
         Character->Data.BuffInfo.Info.ForceCaliburBuffCount,
         Character->Data.BuffInfo.Info.UnknownBuffCount2,
         Character->Data.BuffInfo.Info.ForceWingBuffCount,
         Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };
    Int BuffSlotOffset = 0;
    for (Int BuffType = 0; BuffType < RUNTIME_BUFF_SLOT_TYPE_COUNT; BuffType += 1) {
        if (BuffType == RUNTIME_BUFF_SLOT_TYPE_POTION ||
            BuffType == RUNTIME_BUFF_SLOT_TYPE_UNKNOWN_2 ||
            BuffType == RUNTIME_BUFF_SLOT_TYPE_FORCE_WING) {
            BuffSlotOffset += BuffSlotCounts[BuffType];
            continue;
        }

        for (Int Index = BuffSlotOffset; Index < BuffSlotOffset + BuffSlotCounts[BuffType]; Index += 1) {
            RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];

            NOTIFICATION_DATA_CHARACTERS_SPAWN_BUFF_SLOT* NotificationBuff = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_BUFF_SLOT);
            NotificationBuff->SkillIndex = BuffSlot->SkillIndex;
            NotificationBuff->SkillLevel = BuffSlot->SkillLevel;
        }

        BuffSlotOffset += BuffSlotCounts[BuffType];
    }
    
    //NOTIFICATION_DATA_CHARACTERS_SPAWN_BFX_SLOT* BfxSlot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_BFX_SLOT);
    //BfxSlot->BfxIndex = 8;
    //BfxSlot->Duration = 10000;

    // S2C_DATA_CHARACTERS_SPAWN_PET_SLOT Pets[PetCount];
    // S2C_DATA_CHARACTERS_SPAWN_BALOON_SLOT ItemBaloon[HasItemBaloon];
}


Void RTWorldChunkBroadcastCharactersToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
);

Void RTWorldChunkBroadcastMobsToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
);

Void RTWorldChunkBroadcastItemsToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
);

Void RTWorldChunkInitialize(
	RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
	RTWorldChunkRef Chunk,
    Int WorldIndex,
    Int WorldInstanceIndex,
    Int ChunkX,
    Int ChunkY
) {
    Chunk->Runtime = Runtime;
    Chunk->WorldContext = WorldContext;
    Chunk->WorldIndex = WorldIndex;
    Chunk->WorldInstanceIndex = WorldInstanceIndex;
	Chunk->ReferenceCount = 0;
    Chunk->ChunkX = ChunkX;
    Chunk->ChunkY = ChunkY;
	Chunk->Characters = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
	Chunk->Mobs = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
	Chunk->Items = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
    Chunk->Objects = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
}

Void RTWorldChunkDeinitialize(
	RTWorldChunkRef Chunk
) {
	ArrayDestroy(Chunk->Characters);
	ArrayDestroy(Chunk->Mobs);
	ArrayDestroy(Chunk->Items);
    ArrayDestroy(Chunk->Objects);
}

ArrayRef RTWorldChunkGetContainer(
	RTWorldChunkRef Chunk,
	RTEntityID Entity
) {
	switch (Entity.EntityType) {
	case RUNTIME_ENTITY_TYPE_CHARACTER:
		return Chunk->Characters;

	case RUNTIME_ENTITY_TYPE_MOB:
		return Chunk->Mobs;

	case RUNTIME_ENTITY_TYPE_ITEM:
		return Chunk->Items;

    case RUNTIME_ENTITY_TYPE_OBJECT:
        return Chunk->Objects;

	default:
		UNREACHABLE("Invalid entity type given for world chunk!");
	}
}

Void RTWorldChunkInsert(
	RTWorldChunkRef Chunk,
	RTEntityID Entity,
    Int32 Reason
) {
	ArrayRef Container = RTWorldChunkGetContainer(Chunk, Entity);
	assert(!ArrayContainsElement(Container, &Entity));
	ArrayAppendElement(Container, &Entity);
    RTWorldChunkNotify(Chunk, Entity, Reason, true);

    if (Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
        Chunk->WorldContext->ReferenceCount += 1;
    }
}

Void RTWorldChunkUpdate(
    RTWorldChunkRef WorldChunk,
	RTEntityID Entity
) {
    RTRuntimeRef Runtime = WorldChunk->Runtime;
    assert(Runtime);

    RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Entity);
    assert(Character);
    
    RTMovementRef Movement = &Character->Movement;
    RTWorldChunkRef NewChunk = RTWorldContextGetChunk(Movement->WorldContext, Movement->PositionCurrent.X, Movement->PositionCurrent.Y);
    if (WorldChunk == NewChunk) return;
    
    RTWorldChunkRemove(Movement->WorldChunk, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE);
    RTWorldChunkInsert(NewChunk, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE);
    
    Trace("ServerSetChunkPos(%d, %d)", NewChunk->ChunkX, NewChunk->ChunkY);
    Int32 OldBeginX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 OldBeginY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 OldEndX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 OldEndY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 NewBeginX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, NewChunk->ChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 NewBeginY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, NewChunk->ChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 NewEndX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, NewChunk->ChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 NewEndY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, NewChunk->ChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    Movement->WorldChunk = NewChunk;

    NOTIFICATION_DATA_CHARACTERS_SPAWN* Notification = RTNotificationInit(CHARACTERS_SPAWN);
    Notification->SpawnType = NOTIFICATION_SPAWN_TYPE_MOVE;
    Notification->Count = 1;
    RTNotificationAppendCharacterSpawnIndex(Runtime, Notification, Character);

    for (Int DeltaChunkX = NewBeginX; DeltaChunkX <= NewEndX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = NewBeginY; DeltaChunkY <= NewEndY; DeltaChunkY += 1) {
            if (DeltaChunkX >= OldBeginX && DeltaChunkX <= OldEndX && DeltaChunkY >= OldBeginY && DeltaChunkY <= OldEndY) continue;

            Int WorldChunkIndex = DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(WorldChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT);
        
            RTWorldChunkRef NearbyWorldChunk = &WorldChunk->WorldContext->Chunks[WorldChunkIndex];
            RTNotificationDispatchToChunk(Notification, NearbyWorldChunk);
            RTWorldChunkBroadcastCharactersToCharacter(NearbyWorldChunk, Entity);
            RTWorldChunkBroadcastMobsToCharacter(NearbyWorldChunk, Entity);
            RTWorldChunkBroadcastItemsToCharacter(NearbyWorldChunk, Entity);
        }
    }
}

Void RTWorldChunkRemove(
	RTWorldChunkRef Chunk,
	RTEntityID Entity,
    Int32 Reason
) {
    RTWorldChunkNotify(Chunk, Entity, Reason, false);
    ArrayRef Container = RTWorldChunkGetContainer(Chunk, Entity);
	assert(ArrayContainsElement(Container, &Entity));
	ArrayRemoveElement(Container, &Entity);

    if (Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
        Chunk->WorldContext->ReferenceCount -= 1;
    }
}

Void RTWorldChunkBroadcastNearbyCharactersToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
) {
    Int32 StartChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 StartChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    for (Int DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Int WorldChunkIndex = (Int)DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(WorldChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT);

            RTWorldChunkRef NearbyWorldChunk = &WorldChunk->WorldContext->Chunks[WorldChunkIndex];
            RTWorldChunkBroadcastCharactersToCharacter(NearbyWorldChunk, Entity);
        }
    }
}

Void RTWorldChunkBroadcastCharactersToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
) {
    RTRuntimeRef Runtime = WorldChunk->Runtime;
    assert(Runtime);
    
    RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Entity);
    assert(Character);
    
    NOTIFICATION_DATA_CHARACTERS_SPAWN* Notification = RTNotificationInit(CHARACTERS_SPAWN);
    Notification->SpawnType = NOTIFICATION_SPAWN_TYPE_LIST;
    Notification->Count = 0;
    
    for (Int EntityIndex = 0; EntityIndex < ArrayGetElementCount(WorldChunk->Characters); EntityIndex += 1) {
        RTEntityID CharacterEntity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Characters, EntityIndex);
        if (RTEntityIsEqual(Entity, CharacterEntity)) continue;
        
        RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, CharacterEntity);
        RTNotificationAppendCharacterSpawnIndex(Runtime, Notification, Character);
        Notification->Count += 1;
    }

    if (Notification->Count > 0) {
        RTNotificationDispatchToCharacter(Notification, Character);
    }
}

Void RTWorldChunkBroadcastNearbyMobsToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
) {
    Int32 StartChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 StartChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    for (Int DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Int WorldChunkIndex = DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(WorldChunkIndex < RUNTIME_WORLD_CHUNK_COUNT* RUNTIME_WORLD_CHUNK_COUNT);

            RTWorldChunkRef NearbyWorldChunk = &WorldChunk->WorldContext->Chunks[WorldChunkIndex];
            RTWorldChunkBroadcastMobsToCharacter(NearbyWorldChunk, Entity);
        }
    }
}

Void RTWorldChunkBroadcastMobsToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
) {
    RTRuntimeRef Runtime = WorldChunk->Runtime;
    assert(Runtime);
    
    RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Entity);
    assert(Character);
    
    NOTIFICATION_DATA_MOBS_SPAWN* Notification = RTNotificationInit(MOBS_SPAWN);
    Notification->Count = ArrayGetElementCount(WorldChunk->Mobs);
    
    for (Int Index = 0; Index < ArrayGetElementCount(WorldChunk->Mobs); Index += 1) {
        RTEntityID MobEntity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Mobs, Index);
        RTMobRef Mob = RTWorldContextGetMob(WorldChunk->WorldContext, MobEntity);
        assert(Mob);

        NOTIFICATION_DATA_MOBS_SPAWN_INDEX* NotificationMob = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOBS_SPAWN_INDEX);
        NotificationMob->Entity = MobEntity;
        NotificationMob->PositionBeginX = Mob->Movement.PositionCurrent.X;
        NotificationMob->PositionBeginY = Mob->Movement.PositionCurrent.Y;
        NotificationMob->PositionEndX = Mob->Movement.PositionEnd.X;
        NotificationMob->PositionEndY = Mob->Movement.PositionEnd.Y;
        NotificationMob->MobSpeciesIndex = Mob->SpeciesData->MobSpeciesIndex;
        NotificationMob->MaxHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        NotificationMob->CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
        NotificationMob->IsChasing = Mob->IsChasing;
        NotificationMob->Level = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_LEVEL];
        NotificationMob->Nation = 0;
    }
    
    if (Notification->Count > 0) {
        RTNotificationDispatchToCharacter(Notification, Character);
    }
}

Void RTWorldChunkBroadcastNearbyItemsToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
) {
    Int32 StartChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 StartChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    for (Int DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Int WorldChunkIndex = DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(WorldChunkIndex < RUNTIME_WORLD_CHUNK_COUNT* RUNTIME_WORLD_CHUNK_COUNT);

            RTWorldChunkRef NearbyWorldChunk = &WorldChunk->WorldContext->Chunks[WorldChunkIndex];
            RTWorldChunkBroadcastItemsToCharacter(NearbyWorldChunk, Entity);
        }
    }
}

Void RTWorldChunkBroadcastItemsToCharacter(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity
) {
    RTRuntimeRef Runtime = WorldChunk->Runtime;
    assert(Runtime);
    
    RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Entity);
    assert(Character);
    
    NOTIFICATION_DATA_ITEMS_SPAWN* Notification = RTNotificationInit(ITEMS_SPAWN);
    Notification->Count = ArrayGetElementCount(WorldChunk->Items);
    
    for (Int Index = 0; Index < ArrayGetElementCount(WorldChunk->Items); Index += 1) {
        RTEntityID ItemEntity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Items, Index);
        RTWorldItemRef Item = RTWorldContextGetItem(WorldChunk->WorldContext, ItemEntity);
        assert(Item);

        NOTIFICATION_DATA_ITEMS_SPAWN_INDEX* NotificationItem = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_ITEMS_SPAWN_INDEX);
        NotificationItem->Entity = ItemEntity;
        NotificationItem->ItemOptions = Item->ItemOptions;
        NotificationItem->SourceIndex = Item->ItemSourceIndex;
        NotificationItem->ItemID = Item->Item.Serial;
        NotificationItem->X = Item->X;
        NotificationItem->Y = Item->Y;
        NotificationItem->UniqueKey = Item->ItemUniqueKey;
        NotificationItem->ContextType = Item->ContextType;
        NotificationItem->ItemProperty = Item->ItemProperty;
    }
    
    if (Notification->Count > 0) {
        RTNotificationDispatchToCharacter(Notification, Character);
    }
}

Void RTWorldChunkNotify(
    RTWorldChunkRef WorldChunk,
    RTEntityID Entity,
    Int32 Reason,
    Bool IsInsertion
) {
    if (Reason == RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE) return;

    RTRuntimeRef Runtime = WorldChunk->Runtime;

    if (IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_MOB/* && Reason == RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT*/) {
        NOTIFICATION_DATA_MOBS_SPAWN* Notification = RTNotificationInit(MOBS_SPAWN);
        Notification->Count = 1;
        
        RTMobRef Mob = RTWorldContextGetMob(WorldChunk->WorldContext, Entity);
        assert(Mob);

        NOTIFICATION_DATA_MOBS_SPAWN_INDEX* NotificationMob = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOBS_SPAWN_INDEX);
        NotificationMob->Entity = Entity;
        NotificationMob->PositionBeginX = Mob->Movement.PositionBegin.X;
        NotificationMob->PositionBeginY = Mob->Movement.PositionBegin.Y;
        NotificationMob->PositionEndX = Mob->Movement.PositionEnd.X;
        NotificationMob->PositionEndY = Mob->Movement.PositionEnd.Y;
        NotificationMob->MobSpeciesIndex = Mob->SpeciesData->MobSpeciesIndex;
        NotificationMob->MaxHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        NotificationMob->CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
        NotificationMob->IsChasing = Mob->IsChasing;
        NotificationMob->Level = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_LEVEL];
        NotificationMob->Nation = 0;
        RTNotificationDispatchToNearby(Notification, WorldChunk);
    }

    if (!IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_MOB/* && Reason == RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT*/) {
        NOTIFICATION_DATA_MOBS_DESPAWN* Notification = RTNotificationInit(MOBS_DESPAWN);
        Notification->Entity = Entity;
        Notification->DespawnType = Reason;
        
        switch (Reason) {
            case RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT:
                Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_INIT;
                break;
                
            case RUNTIME_WORLD_CHUNK_UPDATE_REASON_MOVE:
                Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_DISAPPEAR;
                break;
                
            case RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP:
                Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_WARP;
                break;
                
            default:
                Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_DEAD;
                break;
        }

        RTNotificationDispatchToNearby(Notification, WorldChunk);
    }
    
    if (IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_ITEM) {
        NOTIFICATION_DATA_ITEMS_SPAWN* Notification = RTNotificationInit(ITEMS_SPAWN);
        Notification->Count = 1;
        
        RTWorldItemRef Item = RTWorldContextGetItem(WorldChunk->WorldContext, Entity);
        assert(Item);

        NOTIFICATION_DATA_ITEMS_SPAWN_INDEX* NotificationItem = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_ITEMS_SPAWN_INDEX);
        NotificationItem->Entity = Entity;
        NotificationItem->ItemOptions = Item->ItemOptions;
        NotificationItem->SourceIndex = Item->ItemSourceIndex;
        NotificationItem->ItemID = Item->Item.Serial;
        NotificationItem->X = Item->X;
        NotificationItem->Y = Item->Y;
        NotificationItem->UniqueKey = Item->ItemUniqueKey;
        NotificationItem->ContextType = Item->ContextType;
        NotificationItem->ItemProperty = Item->ItemProperty;
        RTNotificationDispatchToNearby(Notification, WorldChunk);
    }

    if (!IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_ITEM) {
        NOTIFICATION_DATA_ITEMS_DESPAWN* Notification = RTNotificationInit(ITEMS_DESPAWN);
        Notification->Entity = Entity;
        Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_DISAPPEAR;
        RTNotificationDispatchToNearby(Notification, WorldChunk);
    }

    if (!IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
        RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Entity);
        assert(Character);

        NOTIFICATION_DATA_CHARACTER_DESPAWN* Notification = RTNotificationInit(CHARACTER_DESPAWN);
        Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
        Notification->DespawnType = Reason;

        switch (Reason) {
        case RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT:
            Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_INIT;
            break;

        case RUNTIME_WORLD_CHUNK_UPDATE_REASON_MOVE:
            Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_DISAPPEAR;
            break;

        case RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP:
            Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_WARP;
            break;

        default:
            Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_DISAPPEAR;
            break;
        }

        RTNotificationDispatchToNearby(Notification, WorldChunk);
    }

    if (IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
        NOTIFICATION_DATA_CHARACTERS_SPAWN* Notification = RTNotificationInit(CHARACTERS_SPAWN);
        Notification->Count = 1;

        switch (Reason) {
            case RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT:
                Notification->SpawnType = NOTIFICATION_SPAWN_TYPE_INIT;
                break;
                
            case RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP:
                Notification->SpawnType = NOTIFICATION_SPAWN_TYPE_WARP;
                break;
                
            case RUNTIME_WORLD_CHUNK_UPDATE_REASON_MOVE:
                Notification->SpawnType = NOTIFICATION_SPAWN_TYPE_MOVE;
                break;
                
            default:
                Notification->SpawnType = NOTIFICATION_SPAWN_TYPE_INIT;
                break;
        }
        
        RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Entity);
        assert(Character);
        RTNotificationAppendCharacterSpawnIndex(Runtime, Notification, Character);
        RTNotificationDispatchToNearby(Notification, WorldChunk);

        RTWorldChunkBroadcastNearbyCharactersToCharacter(WorldChunk, Entity);
        RTWorldChunkBroadcastNearbyMobsToCharacter(WorldChunk, Entity);
        RTWorldChunkBroadcastNearbyItemsToCharacter(WorldChunk, Entity);
    }
}
