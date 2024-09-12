#include "Mob.h"
#include "PartyManager.h"
#include "Runtime.h"
#include "World.h"
#include "WorldManager.h"
#include "NotificationProtocol.h"

Index RTCalculateWorldTileIndex(
    UInt16 X,
    UInt16 Y
) {
    return (Index)Y + (Index)X * RUNTIME_WORLD_SIZE;
}

Index RTCalculateWorldChunkIndex(
    UInt16 X,
    UInt16 Y
) {
    Index ChunkX = MIN(RUNTIME_WORLD_CHUNK_COUNT, X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Index ChunkY = MIN(RUNTIME_WORLD_CHUNK_COUNT, Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    return (Index)ChunkX + ChunkY * RUNTIME_WORLD_CHUNK_COUNT;
}

RTWorldChunkRef RTWorldContextGetChunk(
    RTWorldContextRef WorldContext,
    UInt16 X,
    UInt16 Y
) {
    Index ChunkIndex = RTCalculateWorldChunkIndex(X, Y);
    assert(ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT);
    return &WorldContext->Chunks[ChunkIndex];
}
/*
RTWorldTile RTWorldContextGetTile(
    RTWorldContextRef WorldContext,
    UInt16 X,
    UInt16 Y
) {
    Index TileIndex = RTCalculateWorldTileIndex(X, Y);
    RTWorldTile Tile = WorldContext->WorldData->Tiles[TileIndex];
    Tile.MobCount = WorldContext->TileMobCounter[TileIndex];
    return Tile;
}
*/

Void RTWorldContextAddReferenceCount(
    RTWorldContextRef WorldContext,
    UInt16 X,
    UInt16 Y,
    Int32 ReferenceCount
) {
    Int32 ChunkX = MIN(RUNTIME_WORLD_CHUNK_COUNT, X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Int32 ChunkY = MIN(RUNTIME_WORLD_CHUNK_COUNT, Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Int32 StartChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, ChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 StartChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, ChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, ChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, ChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    for (Int32 DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int32 DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Index ChunkIndex = (Index)DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT* RUNTIME_WORLD_CHUNK_COUNT);
            RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
            WorldChunk->ReferenceCount += ReferenceCount;
        }
    }
}

Void RTWorldContextUpdate(
    RTWorldContextRef WorldContext
) {
    if (WorldContext->WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON ||
        WorldContext->WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
        RTDungeonUpdate(WorldContext);
    }

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldContext->EntityToMob);
    while (Iterator.Key) {
        RTEntityID MobID = *(RTEntityID*)Iterator.Key;
        RTMobRef Mob = RTWorldContextGetMob(WorldContext, MobID);
        assert(Mob);
        RTMobUpdate(WorldContext->WorldManager->Runtime, WorldContext, Mob);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    Timestamp Timestamp = PlatformGetTickCount();
    if (WorldContext->NextItemUpdateTimestamp <= Timestamp) {
        WorldContext->NextItemUpdateTimestamp = INT64_MAX;

        for (Int32 ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
            RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
            if (WorldChunk->NextItemUpdateTimestamp > Timestamp) continue;
            WorldChunk->NextItemUpdateTimestamp = INT64_MAX;

            for (Int32 ItemIndex = (Int32)ArrayGetElementCount(WorldChunk->Items) - 1; ItemIndex >= 0; ItemIndex -= 1) {
                RTEntityID ItemEntity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Items, ItemIndex);
                RTWorldItemRef Item = RTWorldContextGetItem(WorldContext, ItemEntity);
                if (Item->DespawnTimestamp <= Timestamp) {
                    RTWorldDespawnItem(WorldContext->WorldManager->Runtime, WorldContext, Item);
                }
                else {
                    WorldChunk->NextItemUpdateTimestamp = MIN(WorldChunk->NextItemUpdateTimestamp, Item->DespawnTimestamp);
                }
            }

            WorldContext->NextItemUpdateTimestamp = MIN(WorldContext->NextItemUpdateTimestamp, WorldChunk->NextItemUpdateTimestamp);
        }
    }
}

Void* RTWorldContextGetEntityContext(
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    switch (Entity.EntityType) {
    case RUNTIME_ENTITY_TYPE_CHARACTER:
        return RTWorldManagerGetCharacter(WorldContext->WorldManager, Entity);

    case RUNTIME_ENTITY_TYPE_MOB:
        return RTWorldContextGetMob(WorldContext, Entity);

    case RUNTIME_ENTITY_TYPE_ITEM:
        return RTWorldContextGetItem(WorldContext, Entity);

    default:
        UNREACHABLE("Invalid entity given!");
    }

    return NULL;
}

Void RTWorldSpawnCharacterWithoutNotification(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Entity);
    assert(Character);

    UInt16 X = Character->Movement.PositionCurrent.X;
    UInt16 Y = Character->Movement.PositionCurrent.Y;
    RTWorldChunkRef WorldChunk = RTWorldContextGetChunk(WorldContext, X, Y);
    RTWorldChunkInsert(WorldChunk, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE);
    RTWorldContextAddReferenceCount(WorldContext, X, Y, 1);

    Character->Movement.WorldContext = WorldContext;
    Character->Movement.WorldChunk = WorldChunk;
    Character->Movement.Entity = Character->ID;
}

Void RTWorldSpawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    RTWorldSpawnCharacterWithoutNotification(Runtime, WorldContext, Entity);
    RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Entity);
    assert(Character);
    
    RTWorldChunkNotify(Character->Movement.WorldChunk, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT, true);
}

Void RTWorldDespawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID Entity,
    Int32 Reason
) {
    RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Entity);
    assert(Character);

    if (Character->Movement.IsDeadReckoning) {
        RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);
    }

    UInt16 X = Character->Movement.PositionCurrent.X;
    UInt16 Y = Character->Movement.PositionCurrent.Y;
    RTWorldChunkRemove(Character->Movement.WorldChunk, Entity, Reason);
    RTWorldContextAddReferenceCount(WorldContext, X, Y, -1);

    Character->Movement.WorldContext = NULL;
    Character->Movement.Entity = kEntityIDNull;
}

Void RTWorldSpawnMobEvent(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    Timestamp Delay
) {
    Mob->NextTimestamp = 0;
    Mob->EventSpawnTimestamp = PlatformGetTickCount() + Delay;
}

Void RTWorldSpawnMob(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTMobRef Mob
) {
    assert(WorldContext->WorldData->WorldIndex == Mob->ID.WorldIndex);
    //assert(Mob->DespawnTimestamp == 0 || Mob->DespawnTimestamp + Mob->Spawn.SpawnInterval < PlatformGetTickCount());
    //assert(Mob->State == RUNTIME_MOB_STATE_DEAD);
    // assert(Mob->IsDead);

    if (Mob->IsSpawned)
        return;

    UInt32 CollisionMask = RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN;
    Int32 X = Mob->Spawn.AreaX;
    Int32 Y = Mob->Spawn.AreaY;

    if (!Mob->Spawn.IsMissionGate && (Mob->Spawn.AreaWidth > 1 || Mob->Spawn.AreaHeight > 1)) {
        Int32 RemainingRandomCount = 3;
        while (RemainingRandomCount >= 0) {
            Int32 RandomX = RandomRange(&WorldContext->Seed, Mob->Spawn.AreaX, Mob->Spawn.AreaX + Mob->Spawn.AreaWidth);
            Int32 RandomY = RandomRange(&WorldContext->Seed, Mob->Spawn.AreaY, Mob->Spawn.AreaY + Mob->Spawn.AreaHeight);

            if (!RTWorldIsTileColliding(Runtime, WorldContext, RandomX, RandomY, CollisionMask)) {
                X = RandomX;
                Y = RandomY;
                break;
            }

            RemainingRandomCount -= 1;
        }
    }

    RTMovementInitialize(
        Runtime,
        &Mob->Movement,
        X,
        Y,
        Mob->SpeciesData->MoveSpeed,
        CollisionMask
    );

    RTMobInit(Runtime, Mob);
    Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
    Mob->IsDead = false;
    Mob->IsSpawned = true;
    Mob->EventSpawnTimestamp = 0;
    Mob->EventDespawnTimestamp = 0;

    RTWorldChunkRef WorldChunk = RTWorldContextGetChunk(WorldContext, X, Y);
    RTWorldChunkInsert(WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);

    Mob->Movement.WorldContext = WorldContext;
    Mob->Movement.WorldChunk = WorldChunk;
    Mob->Movement.Entity = Mob->ID;

    if (Mob->Spawn.SpawnTriggerID) {
        RTDungeonTriggerEvent(WorldContext, Mob->Spawn.SpawnTriggerID);
    }

    if (WorldContext->DungeonIndex) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);
        assert(DungeonData);

        Index MobIndex = Mob->ID.EntityIndex;
        RTDungeonTimeControlDataRef TimeControl = (RTDungeonTimeControlDataRef)DictionaryLookup(DungeonData->TimeControls, &MobIndex);
        if (TimeControl && TimeControl->Event == RUNTIME_DUNGEON_TIME_CONTROL_TYPE_SPAWN) {
            RTDungeonAddTime(WorldContext, TimeControl->Value);
        }
    }

    RTMobOnEvent(Runtime, WorldContext, Mob, MOB_EVENT_SPAWN);
}

RTMobRef RTWorldContextGetMob(
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    Index* MobPoolIndex = (Index*)DictionaryLookup(WorldContext->EntityToMob, &Entity);
    if (!MobPoolIndex) return NULL;

    return (RTMobRef)MemoryPoolFetch(WorldContext->MobPool, *MobPoolIndex);
}

Void RTWorldDespawnMobEvent(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    Timestamp Delay
) {
    Mob->NextTimestamp = 0;
    Mob->EventDespawnTimestamp = PlatformGetTickCount() + Delay;
}

Void RTWorldDespawnMob(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTMobRef Mob
) {
    assert(WorldContext->WorldData->WorldIndex == Mob->ID.WorldIndex);
    /*
    assert(Mob->State != RUNTIME_MOB_STATE_DEAD);
    assert(!Mob->IsDead);
    
    Mob->DespawnTimestamp = PlatformGetTickCount();
    Mob->State = RUNTIME_MOB_STATE_DEAD;
    Mob->IsDead = true;
    */
    if (!Mob->IsSpawned) 
        return;

    Mob->IsSpawned = false;
    Mob->IsDead = true;

    Mob->Movement.WorldContext = NULL;
    Mob->Movement.Entity = kEntityIDNull;

    RTWorldChunkRemove(Mob->Movement.WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);
    RTMobOnEvent(Runtime, WorldContext, Mob, MOB_EVENT_DESPAWN);

    // TODO: This should be evaluated inside the mob it self!
    if (!RTEntityIsNull(Mob->DropOwner)) {
        RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Mob->DropOwner);
        if (Character) {
            RTDropResult Drop = { 0 };

            if (RTCalculateDrop(Runtime, WorldContext, Mob, Character, &Drop)) {
                RTWorldSpawnItem(
                    Runtime,
                    WorldContext,
                    Mob->ID,
                    Character->Movement.PositionCurrent.X,
                    Character->Movement.PositionCurrent.Y,
                    Drop
                );
            }
            else {
                RTDropCurrency(Runtime, WorldContext, Mob, Character);
            }

            if (RTCalculateQuestDrop(Runtime, WorldContext, Mob, Character, &Drop)) {
                RTWorldSpawnItem(
                    Runtime,
                    WorldContext,
                    Mob->ID,
                    Character->Movement.PositionCurrent.X,
                    Character->Movement.PositionCurrent.Y,
                    Drop
                );
            }

            if (!RTEntityIsNull(Character->PartyID)) {
                RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, Character->PartyID);
                assert(Party);

                RTDropResult Drop = { 0 };
                if (RTCalculatePartyQuestDrop(Runtime, WorldContext, Mob, Party, &Drop)) {
                    RTWorldSpawnItem(
                        Runtime,
                        WorldContext,
                        Mob->ID,
                        Character->Movement.PositionCurrent.X,
                        Character->Movement.PositionCurrent.Y,
                        Drop
                    );
                }
            }
        }
    }

    if (Mob->Spawn.KillTriggerID) {
        RTDungeonTriggerEvent(WorldContext, Mob->Spawn.KillTriggerID);
    }

    for (Index Index = 0; Index < RUNTIME_MOB_MAX_EVENT_COUNT; Index += 1) {
        if (Mob->Spawn.EventProperty[Index] == RUNTIME_MOB_EVENT_NONE) continue;

        RTEntityID MobID = { 0 };
        MobID.EntityIndex = Mob->Spawn.EventMobs[Index];
        MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
        MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        RTMobRef EventMob = RTWorldContextGetMob(WorldContext, MobID);
        if (!EventMob) continue;

        if (Mob->Spawn.EventProperty[Index] == RUNTIME_MOB_EVENT_SPAWN) {
            if (EventMob->IsPermanentDeath) continue;
            if (RTMobIsAlive(EventMob)) continue;

            RTWorldSpawnMobEvent(Runtime, WorldContext, EventMob, Mob->Spawn.EventInterval[Index]);
        }
        else if (Mob->Spawn.EventProperty[Index] == RUNTIME_MOB_EVENT_KILL) {
            if (!RTMobIsAlive(EventMob)) continue;

            RTWorldDespawnMobEvent(Runtime, WorldContext, EventMob, Mob->Spawn.EventInterval[Index]);
        }
        else if (Mob->Spawn.EventProperty[Index] == RUNTIME_MOB_EVENT_DELETE) {
            EventMob->IsPermanentDeath = true;
            RTWorldDespawnMobEvent(Runtime, WorldContext, EventMob, Mob->Spawn.EventInterval[Index]);
        }
        else if (Mob->Spawn.EventProperty[Index] == RUNTIME_MOB_EVENT_REVIVE) {
            if (RTMobIsAlive(EventMob)) continue;

            RTWorldSpawnMobEvent(Runtime, WorldContext, EventMob, Mob->Spawn.EventInterval[Index]);
        }
    }

    RTWorldIncrementQuestMobCounter(Runtime, WorldContext, Mob->Spawn.MobSpeciesIndex);

    if (WorldContext->DungeonIndex) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);
        assert(DungeonData);

        Index MobIndex = Mob->ID.EntityIndex;
        RTDungeonTimeControlDataRef TimeControl = (RTDungeonTimeControlDataRef)DictionaryLookup(DungeonData->TimeControls, &MobIndex);
        if (TimeControl && TimeControl->Event == RUNTIME_DUNGEON_TIME_CONTROL_TYPE_DESPAWN) {
            RTDungeonAddTime(WorldContext, TimeControl->Value);
        }
    }
}

RTWorldItemRef RTWorldContextGetItem(
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    Index* ItemPoolIndex = (Index*)DictionaryLookup(WorldContext->EntityToItem, &Entity);
    if (!ItemPoolIndex) return NULL;

    return (RTWorldItemRef)MemoryPoolFetch(WorldContext->ItemPool, *ItemPoolIndex);
}

RTWorldItemRef RTWorldGetItem(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID Entity,
    UInt32 EntityKey
) {
    RTWorldItemRef Item = RTWorldGetItemByEntity(Runtime, WorldContext, Entity);
    if (!Item || Item->ItemUniqueKey != EntityKey) return NULL;
    
    return Item;
}

RTWorldItemRef RTWorldGetItemByEntity(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    Index *ItemPoolIndex = (Index*)DictionaryLookup(WorldContext->EntityToItem, &Entity);
    if (!ItemPoolIndex) return NULL;
    
    return (RTWorldItemRef)MemoryPoolFetch(WorldContext->ItemPool, *ItemPoolIndex);
}

RTWorldItemRef RTWorldSpawnItem(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID SourceID,
    Int32 X,
    Int32 Y,
    RTDropResult Drop
) {
    if (MemoryPoolIsFull(WorldContext->ItemPool)) {
        // TODO: Delete oldest item
        RTWorldItemRef Item = (RTWorldItemRef)MemoryPoolFetch(WorldContext->ItemPool, 1);
        RTWorldDespawnItem(Runtime, WorldContext, Item);
    }
    
    Index ItemPoolIndex = 0;
    RTWorldItemRef Item = (RTWorldItemRef)MemoryPoolReserveNext(WorldContext->ItemPool, &ItemPoolIndex);
    Item->ID.EntityIndex = (UInt16)ItemPoolIndex;
    Item->ID.WorldIndex = WorldContext->WorldData->WorldIndex;
    Item->ID.EntityType = RUNTIME_ENTITY_TYPE_ITEM;
    Item->Index = ItemPoolIndex;
    Item->ItemOptions = Drop.ItemOptions;
    Item->ItemSourceIndex = RTEntityGetSerial(SourceID);
    Item->Item = Drop.ItemID;
    Item->X = X;
    Item->Y = Y;
    Item->ItemUniqueKey = RandomRange(&WorldContext->Seed, 0, UINT16_MAX);
    Item->ContextType = NOTIFICATION_ITEMS_SPAWN_CONTEXT_TYPE_NONE;
    if (SourceID.EntityType == RUNTIME_ENTITY_TYPE_MOB) {
        Item->ContextType = NOTIFICATION_ITEMS_SPAWN_CONTEXT_TYPE_MOBS;
    }
    if (SourceID.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
        RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, SourceID);
        if (Character) {
            Item->ItemSourceIndex = (UInt32)Character->CharacterIndex;
        }

        Item->ContextType = NOTIFICATION_ITEMS_SPAWN_CONTEXT_TYPE_USER;
    }
    Item->ItemProperty = Drop.ItemProperty;
    Item->ItemDuration = Drop.ItemDuration;
    Item->Timestamp = PlatformGetTickCount();
    Item->DespawnTimestamp = Item->Timestamp + Runtime->Config.WorldItemDespawnInterval;
    WorldContext->NextItemUpdateTimestamp = MIN(WorldContext->NextItemUpdateTimestamp, Item->DespawnTimestamp);

    DictionaryInsert(WorldContext->EntityToItem, &Item->ID, &ItemPoolIndex, sizeof(Index));
    
    RTWorldChunkRef WorldChunk = RTWorldContextGetChunk(WorldContext, X, Y);
    WorldChunk->NextItemUpdateTimestamp = MIN(WorldChunk->NextItemUpdateTimestamp, Item->DespawnTimestamp);
    RTWorldChunkInsert(WorldChunk, Item->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);

    return Item;
}

Void RTWorldDespawnItem(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTWorldItemRef Item
) {
    RTWorldChunkRef WorldChunk = RTWorldContextGetChunk(WorldContext, Item->X, Item->Y);
    RTWorldChunkRemove(WorldChunk, Item->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);
    
    DictionaryRemove(WorldContext->EntityToItem, &Item->ID);
    MemoryPoolRelease(WorldContext->ItemPool, Item->Index);
}

Bool RTWorldIsTileColliding(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y,
    UInt32 CollisionMask
) {
    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) {
        return true;
    }

    // TODO: Check if map orientation is correct!
//    return (World->Tiles[X + Y * RUNTIME_WORLD_SIZE] & CollisionMask) > 0;
    return (World->WorldData->Tiles[Y + X * RUNTIME_WORLD_SIZE] & CollisionMask) > 0;
}

Bool RTWorldTraceMovement(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 StartX,
    Int32 StartY,
    Int32 EndX,
    Int32 EndY,
    Int32* ResultX,
    Int32* ResultY,
    UInt32 CollisionMask,
    UInt32 IgnoreMask
) {
    StartX <<= RUNTIME_WORLD_TILE_SIZE_EXPONENT;
    StartY <<= RUNTIME_WORLD_TILE_SIZE_EXPONENT;
    EndX <<= RUNTIME_WORLD_TILE_SIZE_EXPONENT;
    EndY <<= RUNTIME_WORLD_TILE_SIZE_EXPONENT;

    if (ResultX) *ResultX = StartX >> RUNTIME_WORLD_TILE_SIZE_EXPONENT;
    if (ResultY) *ResultY = StartY >> RUNTIME_WORLD_TILE_SIZE_EXPONENT;

    Int32 DeltaX = EndX - StartX;
    Int32 DeltaY = EndY - StartY;
    Int32 AbsDeltaX = (DeltaX < 0) ? -DeltaX : DeltaX;
    Int32 AbsDeltaY = (DeltaY < 0) ? -DeltaY : DeltaY;
    Int32 AbsDelta[] = { AbsDeltaX, AbsDeltaY };
    if (AbsDeltaX < AbsDeltaY) {
        AbsDelta[0] = AbsDeltaY;
        AbsDelta[1] = AbsDeltaX;
    }

    Int32 Length = AbsDelta[0] + 1;
    Int32 D = (AbsDelta[1] << 1) - AbsDelta[0];
    Int32 SignX = (StartX > EndX) ? -1 : 1;
    Int32 SignY = (StartY > EndY) ? -1 : 1;
    Int32 StepD = (D < 0) ? (AbsDelta[1] << 1) : (AbsDelta[1] - AbsDelta[0]) << 1;
    Int32 StepX = (D < 0 && AbsDeltaX < AbsDeltaY) ? 0 : SignX;
    Int32 StepY = (D < 0 && AbsDeltaX >= AbsDeltaY) ? 0 : SignY;
    Int32 X = StartX;
    Int32 Y = StartY;

    for (Int32 Index = 0; Index < Length; Index++) {
        D += StepD;
        X += StepX;
        Y += StepY;

        Int32 TileX = X >> RUNTIME_WORLD_TILE_SIZE_EXPONENT;
        Int32 TileY = Y >> RUNTIME_WORLD_TILE_SIZE_EXPONENT;

        Bool Ignore = false;
        if (IgnoreMask) Ignore = RTWorldIsTileColliding(Runtime, World, TileX, TileY, IgnoreMask);

        if (!Ignore && RTWorldIsTileColliding(Runtime, World, TileX, TileY, CollisionMask)) {
            return false;
        }

        if (ResultX) *ResultX = TileX;
        if (ResultY) *ResultY = TileY;
    }

    return true;
}

Void RTWorldIncrementQuestMobCounter(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Index MobSpeciesIndex
) {
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return;

    for (Int32 Index = 0; Index < World->MissionMobCount; Index++) {
        if (World->MissionMobs[Index].MobID != MobSpeciesIndex) continue;

        World->MissionMobs[Index].Count = MIN(World->MissionMobs[Index].Count + 1, World->MissionMobs[Index].MaxCount);
    }

    RTDungeonStartNextPatternPart(World);

    if (!RTEntityIsNull(World->Party)) {
        RTPartyIncrementQuestMobCounter(Runtime, World->Party, MobSpeciesIndex);
    }
}

Void RTWorldSetMobTable(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    ArrayRef MobTable
) {
    for (Index ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
        for (Int32 MobIndex = (Int32)ArrayGetElementCount(WorldChunk->Mobs) - 1; MobIndex >= 0; MobIndex -= 1) {
            RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Mobs, MobIndex);
            RTWorldChunkRemove(WorldChunk, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);
        }
    }

    MemoryPoolClear(WorldContext->MobPool);
    DictionaryRemoveAll(WorldContext->EntityToMob);

    for (Index MobIndex = 0; MobIndex < ArrayGetElementCount(MobTable); MobIndex += 1) {
        RTMobRef TableMob = (RTMobRef)ArrayGetElementAtIndex(MobTable, MobIndex);

        Index MemoryPoolIndex = 0;
        RTMobRef Mob = (RTMobRef)MemoryPoolReserveNext(WorldContext->MobPool, &MemoryPoolIndex);
        memcpy(Mob, TableMob, sizeof(struct _RTMob));

        Mob->ID.EntityIndex = TableMob->ID.EntityIndex;
        Mob->ID.WorldIndex = WorldContext->WorldData->WorldIndex;
        Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
        Mob->IsSpawned = false;
        Mob->RemainingFindCount = Mob->SpeciesData->FindCount;
        Mob->RemainingSpawnCount = Mob->Spawn.SpawnCount;

        DictionaryInsert(WorldContext->EntityToMob, &Mob->ID, &MemoryPoolIndex, sizeof(Index));
    }

    for (Index MobIndex = 0; MobIndex < ArrayGetElementCount(MobTable); MobIndex += 1) {
        RTMobRef TableMob = (RTMobRef)ArrayGetElementAtIndex(MobTable, MobIndex);

        RTEntityID MobID = { 0 };
        MobID.EntityIndex = TableMob->ID.EntityIndex;
        MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
        MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        Index* MemoryPoolIndex = DictionaryLookup(WorldContext->EntityToMob, &MobID);
        assert(MemoryPoolIndex);

        RTMobRef Mob = (RTMobRef)MemoryPoolFetch(WorldContext->MobPool, *MemoryPoolIndex);
        assert(Mob);

        if (Mob->Spawn.SpawnDefault) {
            RTWorldSpawnMob(Runtime, WorldContext, Mob);
        }
    }
}

Void RTWorldContextEnumerateEntitiesInRange(
    RTWorldContextRef WorldContext,
    UInt8 EntityType,
    UInt16 X,
    UInt16 Y,
    UInt16 Range,
    UInt32 CollisionMask,
    UInt32 IgnoreMask,
    RTEntityVisitorByDistanceCallback Callback,
    Void* Userdata
) {
    RTRuntimeRef Runtime = WorldContext->WorldManager->Runtime;
    RTWorldManagerRef WorldManager = WorldContext->WorldManager;

    Int32 StartChunkX = MIN(RUNTIME_WORLD_CHUNK_COUNT, X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Int32 StartChunkY = MIN(RUNTIME_WORLD_CHUNK_COUNT, Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Int32 EndChunkX = MIN(RUNTIME_WORLD_CHUNK_COUNT, X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Int32 EndChunkY = MIN(RUNTIME_WORLD_CHUNK_COUNT, Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);

    for (Int32 DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int32 DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Index ChunkIndex = (Index)DeltaChunkX + (Index)DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT* RUNTIME_WORLD_CHUNK_COUNT);

            RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
            ArrayRef Entities = NULL;

            switch (EntityType) {
            case RUNTIME_ENTITY_TYPE_CHARACTER:
                Entities = WorldChunk->Characters;
                break;

            case RUNTIME_ENTITY_TYPE_MOB:
                Entities = WorldChunk->Mobs;
                break;

            case RUNTIME_ENTITY_TYPE_ITEM:
                Entities = WorldChunk->Items;
                break;

            default:
                UNREACHABLE("Invalid entity type given!");
            }

            for (Int32 Index = 0; Index < ArrayGetElementCount(Entities); Index += 1) {
                RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(Entities, Index);
                UInt16 TargetX = 0;
                UInt16 TargetY = 0;

                switch (EntityType) {
                case RUNTIME_ENTITY_TYPE_CHARACTER: {
                    RTCharacterRef Character = RTWorldManagerGetCharacter(WorldManager, Entity);
                    TargetX = Character->Movement.PositionCurrent.X;
                    TargetY = Character->Movement.PositionCurrent.Y;
                }   break;

                case RUNTIME_ENTITY_TYPE_MOB: {
                    RTMobRef Mob = RTWorldContextGetMob(WorldContext, Entity);
                    TargetX = Mob->Movement.PositionCurrent.X;
                    TargetY = Mob->Movement.PositionCurrent.Y;
                }   break;

                case RUNTIME_ENTITY_TYPE_ITEM: {
                    RTWorldItemRef Item = RTWorldContextGetItem(WorldContext, Entity);
                    TargetX = Item->X;
                    TargetY = Item->Y;
                }   break;

                default:
                    UNREACHABLE("Invalid entity type given!");
                }

                Int32 Distance = RTCalculateDistance(
                    X,
                    Y,
                    TargetX,
                    TargetY
                );

                if (Distance > Range) continue;

                Bool IsReachable = RTWorldTraceMovement(
                    Runtime,
                    WorldContext,
                    X,
                    Y,
                    TargetX,
                    TargetY,
                    NULL,
                    NULL,
                    CollisionMask,
                    IgnoreMask
                );

                if (!IsReachable) continue;

                Callback(Entity, Distance, Userdata);
            }
        }
    }
}

Void RTWorldContextEnumerateBroadcastTargets(
    RTWorldContextRef WorldContext,
    UInt8 EntityType,
    UInt16 X,
    UInt16 Y,
    RTEntityVisitorCallback Callback,
    Void* Userdata
) {
    Int32 ChunkX = MIN(RUNTIME_WORLD_CHUNK_COUNT, X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Int32 ChunkY = MIN(RUNTIME_WORLD_CHUNK_COUNT, Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Int32 StartChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, ChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 StartChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, ChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, ChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, ChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    for (Int32 DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int32 DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Index ChunkIndex = (Index)DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT);

            RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
            ArrayRef Entities = NULL;

            switch (EntityType) {
            case RUNTIME_ENTITY_TYPE_CHARACTER:
                Entities = WorldChunk->Characters;
                break;

            case RUNTIME_ENTITY_TYPE_MOB:
                Entities = WorldChunk->Mobs;
                break;

            case RUNTIME_ENTITY_TYPE_ITEM:
                Entities = WorldChunk->Items;
                break;

            default:
                UNREACHABLE("Invalid entity type given!");
            }

            for (Int32 Index = 0; Index < ArrayGetElementCount(Entities); Index += 1) {
                RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(Entities, Index);
                Callback(Entity, Userdata);
            }
        }
    }
}

Bool RTChunkAreaIntersectsPoint(
    Int32 AreaX,
    Int32 AreaY,
    Int32 AreaRadius,
    Int32 X,
    Int32 Y
) {
    if (AreaX < 0 || AreaY < 0) return true;
    
    Int32 StartChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, AreaX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 StartChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, AreaY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, AreaX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, AreaY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    return (X >= StartChunkX && X <= EndChunkX && Y >= StartChunkY && Y <= EndChunkY);
}

Void RTWorldContextEnumerateBroadcastChunks(
    RTWorldContextRef WorldContext,
    UInt8 EntityType,
    Int32 PreviousChunkX,
    Int32 PreviousChunkY,
    Int32 CurrentChunkX,
    Int32 CurrentChunkY,
    RTEntityVisitorCallback Callback,
    Void* Userdata
) {
    Int32 StartChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, CurrentChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 StartChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, CurrentChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, CurrentChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, CurrentChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    for (Int32 DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int32 DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            if (RTChunkAreaIntersectsPoint(
                PreviousChunkX,
                PreviousChunkY,
                RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS,
                DeltaChunkX,
                DeltaChunkY
            )) {
                continue;
            }
        
            Index ChunkIndex = (Index)DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT);

            RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
            ArrayRef Entities = NULL;

            switch (EntityType) {
            case RUNTIME_ENTITY_TYPE_CHARACTER:
                Entities = WorldChunk->Characters;
                break;

            case RUNTIME_ENTITY_TYPE_MOB:
                Entities = WorldChunk->Mobs;
                break;

            case RUNTIME_ENTITY_TYPE_ITEM:
                Entities = WorldChunk->Items;
                break;

            default:
                UNREACHABLE("Invalid entity type given!");
            }

            for (Int32 Index = 0; Index < ArrayGetElementCount(Entities); Index += 1) {
                RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(Entities, Index);
                Callback(Entity, Userdata);
            }
        }
    }
}
