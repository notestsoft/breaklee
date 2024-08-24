#include "Movement.h"
#include "PartyManager.h"
#include "Runtime.h"
#include "WorldChunk.h"
#include "WorldManager.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

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
    Index WorldIndex,
    Index WorldInstanceIndex,
    Int32 ChunkX,
    Int32 ChunkY
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
}

Void RTWorldChunkDeinitialize(
	RTWorldChunkRef Chunk
) {
	ArrayDestroy(Chunk->Characters);
	ArrayDestroy(Chunk->Mobs);
	ArrayDestroy(Chunk->Items);
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

    NOTIFICATION_DATA_CHARACTERS_SPAWN_INDEX* NotificationCharacter = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_INDEX);
    NotificationCharacter->CharacterIndex = (UInt32)Character->CharacterIndex;
    NotificationCharacter->Entity = Character->ID;
    NotificationCharacter->Level = Character->Data.Info.Level;
    NotificationCharacter->OverlordLevel = Character->Data.OverlordMasteryInfo.Info.Level;
    NotificationCharacter->ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
    NotificationCharacter->ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
    NotificationCharacter->MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    NotificationCharacter->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    NotificationCharacter->MovementSpeed = (UInt32)(Character->Movement.Speed * RUNTIME_MOVEMENT_SPEED_SCALE);
    NotificationCharacter->PositionBeginX = Character->Movement.PositionBegin.X;
    NotificationCharacter->PositionBeginY = Character->Movement.PositionBegin.Y;
    NotificationCharacter->PositionEndX = Character->Movement.PositionEnd.X;
    NotificationCharacter->PositionEndY = Character->Movement.PositionEnd.Y;
    NotificationCharacter->Nation = Character->Data.StyleInfo.Nation;
    NotificationCharacter->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
    NotificationCharacter->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
    NotificationCharacter->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
    NotificationCharacter->IsDead = RTCharacterIsAlive(Runtime, Character) ? 0 : 1;
    NotificationCharacter->EquipmentSlotCount = Character->Data.EquipmentInfo.Info.EquipmentSlotCount;
    NotificationCharacter->NameLength = strlen(Character->Name) + 1;
    RTNotificationAppendCString(Notification, Character->Name);
    
    NOTIFICATION_DATA_CHARACTERS_SPAWN_GUILD* Guild = (NOTIFICATION_DATA_CHARACTERS_SPAWN_GUILD*)&NotificationCharacter->Name[NotificationCharacter->NameLength];
    Guild->GuildNameLength = 0;

    for (Index EquipmentIndex = 0; EquipmentIndex < Character->Data.EquipmentInfo.Info.EquipmentSlotCount; EquipmentIndex += 1) {
        RTItemSlotRef ItemSlot = &Character->Data.EquipmentInfo.EquipmentSlots[EquipmentIndex];

        NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* NotificationSlot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
        NotificationSlot->EquipmentSlotIndex = ItemSlot->SlotIndex;
        NotificationSlot->ItemID = ItemSlot->Item.Serial;
        NotificationSlot->ItemOptions = ItemSlot->ItemOptions;
        NotificationSlot->ItemDuration = ItemSlot->ItemDuration.Serial;
    }

    // TODO: This is a placeholder for costumes add them after adding the costume system
    NotificationCharacter->CostumeSlotCount = RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT;
    for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT; Index += 1) {
        NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* Slot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
        Slot->EquipmentSlotIndex = Index;
    }

    for (Int32 DeltaChunkX = NewBeginX; DeltaChunkX <= NewEndX; DeltaChunkX += 1) {
        for (Int32 DeltaChunkY = NewBeginY; DeltaChunkY <= NewEndY; DeltaChunkY += 1) {
            if (DeltaChunkX >= OldBeginX && DeltaChunkX <= OldEndX && DeltaChunkY >= OldBeginY && DeltaChunkY <= OldEndY) continue;
            
            Index WorldChunkIndex = (Index)DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
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

    for (Int32 DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int32 DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Index WorldChunkIndex = (Index)DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(WorldChunkIndex < RUNTIME_WORLD_CHUNK_COUNT* RUNTIME_WORLD_CHUNK_COUNT);

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
    
    for (Index EntityIndex = 0; EntityIndex < ArrayGetElementCount(WorldChunk->Characters); EntityIndex += 1) {
        RTEntityID CharacterEntity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Characters, EntityIndex);
        if (RTEntityIsEqual(Entity, CharacterEntity)) continue;
    
        Notification->Count += 1;
        
        RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, CharacterEntity);
    
        NOTIFICATION_DATA_CHARACTERS_SPAWN_INDEX* NotificationCharacter = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_INDEX);
        NotificationCharacter->CharacterIndex = (UInt32)Character->CharacterIndex;
        NotificationCharacter->Entity = Character->ID;
        NotificationCharacter->Level = Character->Data.Info.Level;
        NotificationCharacter->OverlordLevel = Character->Data.OverlordMasteryInfo.Info.Level;
        NotificationCharacter->ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
        NotificationCharacter->ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
        NotificationCharacter->MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        NotificationCharacter->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
        NotificationCharacter->MovementSpeed = (UInt32)(Character->Movement.Speed * RUNTIME_MOVEMENT_SPEED_SCALE);
        NotificationCharacter->PositionBeginX = Character->Movement.PositionBegin.X;
        NotificationCharacter->PositionBeginY = Character->Movement.PositionBegin.Y;
        NotificationCharacter->PositionEndX = Character->Movement.PositionEnd.X;
        NotificationCharacter->PositionEndY = Character->Movement.PositionEnd.Y;
        NotificationCharacter->Nation = Character->Data.StyleInfo.Nation;
        NotificationCharacter->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
        NotificationCharacter->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
        NotificationCharacter->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
        NotificationCharacter->IsDead = RTCharacterIsAlive(Runtime, Character) ? 0 : 1;
        NotificationCharacter->EquipmentSlotCount = Character->Data.EquipmentInfo.Info.EquipmentSlotCount;
        NotificationCharacter->NameLength = strlen(Character->Name) + 1;
        RTNotificationAppendCString(Notification, Character->Name);
        
        NOTIFICATION_DATA_CHARACTERS_SPAWN_GUILD* Guild = (NOTIFICATION_DATA_CHARACTERS_SPAWN_GUILD*)&NotificationCharacter->Name[NotificationCharacter->NameLength];
        Guild->GuildNameLength = 0;

        for (Index EquipmentIndex = 0; EquipmentIndex < Character->Data.EquipmentInfo.Info.EquipmentSlotCount; EquipmentIndex += 1) {
            RTItemSlotRef ItemSlot = &Character->Data.EquipmentInfo.EquipmentSlots[EquipmentIndex];

            NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* NotificationSlot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
            NotificationSlot->EquipmentSlotIndex = ItemSlot->SlotIndex;
            NotificationSlot->ItemID = ItemSlot->Item.Serial;
            NotificationSlot->ItemOptions = ItemSlot->ItemOptions;
            NotificationSlot->ItemDuration = ItemSlot->ItemDuration.Serial;
        }

        // TODO: This is a placeholder for costumes add them after adding the costume system
        NotificationCharacter->CostumeSlotCount = RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT;
        for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT; Index += 1) {
            NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* Slot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
            Slot->EquipmentSlotIndex = Index;
        }
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

    for (Int32 DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int32 DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Index WorldChunkIndex = (Index)DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
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
    
    for (Index Index = 0; Index < ArrayGetElementCount(WorldChunk->Mobs); Index += 1) {
        RTEntityID MobEntity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Mobs, Index);
        RTMobRef Mob = RTWorldContextGetMob(WorldChunk->WorldContext, MobEntity);
        assert(Mob);

        NOTIFICATION_DATA_MOBS_SPAWN_INDEX* NotificationMob = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOBS_SPAWN_INDEX);
        NotificationMob->Entity = MobEntity;
        NotificationMob->PositionBeginX = Mob->Movement.PositionBegin.X;
        NotificationMob->PositionBeginY = Mob->Movement.PositionBegin.Y;
        NotificationMob->PositionEndX = Mob->Movement.PositionEnd.X;
        NotificationMob->PositionEndY = Mob->Movement.PositionEnd.Y;
        NotificationMob->MobSpeciesIndex = Mob->SpeciesData->MobSpeciesIndex;
        NotificationMob->MaxHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        NotificationMob->CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
        NotificationMob->IsChasing = Mob->IsChasing;
        NotificationMob->Level = Mob->SpeciesData->Level;
        NotificationMob->Nation = 0;
        NotificationMob->Unknown1 = 0;
        NotificationMob->UnknownAnimationID = 0;
        NotificationMob->UnknownAnimationTickCount = 0;
        NotificationMob->Unknown2 = 0;
        NotificationMob->UnknownEvent = 0;
        NotificationMob->Unknown3 = 0;
        NotificationMob->UnknownCharacterIndex = 0;
        memset(NotificationMob->Unknown4, 0, 12);
        memset(NotificationMob->Unknown5, 0, 22);
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

    for (Int32 DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int32 DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Index WorldChunkIndex = (Index)DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
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
    
    for (Index Index = 0; Index < ArrayGetElementCount(WorldChunk->Items); Index += 1) {
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

    RTPartyRef Party = NULL;
    if (!RTEntityIsNull(WorldChunk->WorldContext->Party)) {
        Party = RTPartyManagerGetParty(Runtime->PartyManager, WorldChunk->WorldContext->Party);
    }

    if (IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_MOB && Reason == RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT) {
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
        NotificationMob->Level = Mob->SpeciesData->Level;
        NotificationMob->Nation = 0;
        NotificationMob->Unknown1 = 0;
        NotificationMob->UnknownAnimationID = 0;
        NotificationMob->UnknownAnimationTickCount = 0;
        NotificationMob->Unknown2 = 0;
        NotificationMob->UnknownEvent = 0;
        NotificationMob->Unknown3 = 0;
        NotificationMob->UnknownCharacterIndex = 0;
        memset(NotificationMob->Unknown4, 0, 12);
        memset(NotificationMob->Unknown5, 0, 22);

        if (Party) {
            RTNotificationDispatchToParty(Notification, Party);
        }
        else {
            RTNotificationDispatchToNearby(Notification, WorldChunk);
        }
    }

    if (!IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_MOB && Reason == RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT) {
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

        if (Party) {
            RTNotificationDispatchToParty(Notification, Party);
        }
        else {
            RTNotificationDispatchToNearby(Notification, WorldChunk);
        }
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

        if (Party) {
            RTNotificationDispatchToParty(Notification, Party);
        }
        else {
            RTNotificationDispatchToNearby(Notification, WorldChunk);
        }
    }

    if (!IsInsertion && Entity.EntityType == RUNTIME_ENTITY_TYPE_ITEM) {
        NOTIFICATION_DATA_ITEMS_DESPAWN* Notification = RTNotificationInit(ITEMS_DESPAWN);
        Notification->Entity = Entity;
        Notification->DespawnType = NOTIFICATION_DESPAWN_TYPE_DISAPPEAR;

        if (Party) {
            RTNotificationDispatchToParty(Notification, Party);
        }
        else {
            RTNotificationDispatchToNearby(Notification, WorldChunk);
        }
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

        if (Party) {
            RTNotificationDispatchToParty(Notification, Party);
        }
        else {
            RTNotificationDispatchToNearby(Notification, WorldChunk);
        }
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
        
        NOTIFICATION_DATA_CHARACTERS_SPAWN_INDEX* NotificationCharacter = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_INDEX);
        NotificationCharacter->CharacterIndex = (UInt32)Character->CharacterIndex;
        NotificationCharacter->Entity = Character->ID;
        NotificationCharacter->Level = Character->Data.Info.Level;
        NotificationCharacter->OverlordLevel = Character->Data.OverlordMasteryInfo.Info.Level;
        NotificationCharacter->ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
        NotificationCharacter->ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
        NotificationCharacter->MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        NotificationCharacter->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
        NotificationCharacter->MovementSpeed = (UInt32)(Character->Movement.Speed * RUNTIME_MOVEMENT_SPEED_SCALE);
        NotificationCharacter->PositionBeginX = Character->Movement.PositionBegin.X;
        NotificationCharacter->PositionBeginY = Character->Movement.PositionBegin.Y;
        NotificationCharacter->PositionEndX = Character->Movement.PositionEnd.X;
        NotificationCharacter->PositionEndY = Character->Movement.PositionEnd.Y;
        NotificationCharacter->Nation = Character->Data.StyleInfo.Nation;
        NotificationCharacter->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
        NotificationCharacter->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
        NotificationCharacter->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
        NotificationCharacter->IsDead = RTCharacterIsAlive(Runtime, Character) ? 0 : 1;
        NotificationCharacter->EquipmentSlotCount = Character->Data.EquipmentInfo.Info.EquipmentSlotCount;
        NotificationCharacter->NameLength = strlen(Character->Name) + 1;
        RTNotificationAppendCString(Notification, Character->Name);
        
        NOTIFICATION_DATA_CHARACTERS_SPAWN_GUILD* Guild = (NOTIFICATION_DATA_CHARACTERS_SPAWN_GUILD*)&NotificationCharacter->Name[NotificationCharacter->NameLength];
        Guild->GuildNameLength = 0;

        for (Index EquipmentIndex = 0; EquipmentIndex < Character->Data.EquipmentInfo.Info.EquipmentSlotCount; EquipmentIndex += 1) {
            RTItemSlotRef ItemSlot = &Character->Data.EquipmentInfo.EquipmentSlots[EquipmentIndex];

            NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* NotificationSlot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
            NotificationSlot->EquipmentSlotIndex = ItemSlot->SlotIndex;
            NotificationSlot->ItemID = ItemSlot->Item.Serial;
            NotificationSlot->ItemOptions = ItemSlot->ItemOptions;
            NotificationSlot->ItemDuration = ItemSlot->ItemDuration.Serial;
        }

        // TODO: This is a placeholder for costumes add them after adding the costume system
        NotificationCharacter->CostumeSlotCount = RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT;
        for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT; Index += 1) {
            NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT* Slot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT);
            Slot->EquipmentSlotIndex = Index;
        }

        if (Party) {
            RTNotificationDispatchToParty(Notification, Party);
        }
        else {
            RTNotificationDispatchToNearby(Notification, WorldChunk);
        }

        RTWorldChunkBroadcastNearbyCharactersToCharacter(WorldChunk, Entity);
        RTWorldChunkBroadcastNearbyMobsToCharacter(WorldChunk, Entity);
        RTWorldChunkBroadcastNearbyItemsToCharacter(WorldChunk, Entity);
    }
}
