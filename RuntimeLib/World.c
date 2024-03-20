#include "Mob.h"
#include "Runtime.h"
#include "World.h"
#include "WorldManager.h"

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
    assert(ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT* RUNTIME_WORLD_CHUNK_COUNT);
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
    Timestamp Timestamp = GetTimestamp();

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

    /*
    for (Index ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
        if (WorldChunk->ReferenceCount < 1) continue;

        for (Index MobIndex = 0; MobIndex < ArrayGetElementCount(WorldChunk->Mobs); MobIndex += 1) {
            RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Mobs, MobIndex);
            RTMobRef Mob = RTWorldContextGetMob(WorldContext, Entity);
            assert(Mob);
            RTMobUpdate(WorldContext->WorldManager->Runtime, WorldContext, Mob);
        }
    }
    */
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
    RTWorldChunkInsert(WorldChunk, Entity);
    RTWorldContextAddReferenceCount(WorldContext, X, Y, 1);

    Character->Movement.WorldContext = WorldContext;
    Character->Movement.WorldChunk = WorldChunk;
    Character->Movement.Entity = Character->ID;
}

Void RTWorldBroadcastSpawnCharacterNotification(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Entity);
    assert(Character);

    RTEventData EventData = { 0 };
    EventData.CharacterSpawn.CharacterIndex = Character->CharacterIndex;
    EventData.CharacterSpawn.Position = Character->Info.Position;

    RTRuntimeBroadcastEventData(
        Runtime,
        RUNTIME_EVENT_CHARACTER_SPAWN,
        WorldContext,
        Entity,
        kEntityIDNull,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y,
        EventData
    );

    RTRuntimeBroadcastEvent(
        Runtime,
        RUNTIME_EVENT_CHARACTER_CHUNK_UPDATE,
        WorldContext,
        kEntityIDNull,
        Entity,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y
    );
}

Void RTWorldSpawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    RTWorldSpawnCharacterWithoutNotification(Runtime, WorldContext, Entity);
    RTWorldBroadcastSpawnCharacterNotification(Runtime, WorldContext, Entity);
}

Void RTWorldDespawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Entity);
    assert(Character);

    if (Character->Movement.IsDeadReckoning) {
        RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);
    }

    UInt16 X = Character->Movement.PositionCurrent.X;
    UInt16 Y = Character->Movement.PositionCurrent.Y;
    RTWorldChunkRemove(Character->Movement.WorldChunk, Entity);
    RTWorldContextAddReferenceCount(WorldContext, X, Y, -1);

    Character->Movement.WorldContext = NULL;
    Character->Movement.Entity = kEntityIDNull;

    RTEventData EventData = { 0 };
    EventData.CharacterSpawn.CharacterIndex = Character->CharacterIndex;
    EventData.CharacterSpawn.Position = Character->Info.Position;

    RTRuntimeBroadcastEventData(
        Runtime,
        RUNTIME_EVENT_CHARACTER_DESPAWN,
        WorldContext,
        kEntityIDNull,
        Entity,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y,
        EventData
    );
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

    UInt32 CollisionMask = RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN;
    Int32 X = Mob->Spawn.AreaX;
    Int32 Y = Mob->Spawn.AreaY;

    if (!Mob->Spawn.MissionGate) {
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
    RTWorldChunkInsert(WorldChunk, Mob->ID);

    Mob->Movement.WorldContext = WorldContext;
    Mob->Movement.WorldChunk = WorldChunk;
    Mob->Movement.Entity = Mob->ID;

    RTEventData EventData = { 0 };
    EventData.MobSpawnOrUpdate.Level = Mob->SpeciesData->Level;
    EventData.MobSpawnOrUpdate.CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    EventData.MobSpawnOrUpdate.MaxHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    EventData.MobSpawnOrUpdate.MobSpeciesIndex = Mob->Spawn.MobSpeciesIndex;
    EventData.MobSpawnOrUpdate.PositionBeginX = Mob->Movement.PositionBegin.X;
    EventData.MobSpawnOrUpdate.PositionBeginY = Mob->Movement.PositionBegin.Y;
    EventData.MobSpawnOrUpdate.PositionEndX = Mob->Movement.PositionEnd.X;
    EventData.MobSpawnOrUpdate.PositionEndY = Mob->Movement.PositionEnd.Y;

    RTRuntimeBroadcastEventData(
        Runtime,
        RUNTIME_EVENT_MOB_SPAWN,
        WorldContext,
        kEntityIDNull,
        Mob->ID,
        Mob->Movement.PositionCurrent.X,
        Mob->Movement.PositionCurrent.Y,
        EventData
    );

    if (Mob->Spawn.SpawnTriggerID) {
        RTDungeonTriggerEvent(WorldContext, Mob->Spawn.SpawnTriggerID);
    }
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
    Mob->IsSpawned = false;
    Mob->IsDead = true;

    Mob->Movement.WorldContext = NULL;
    Mob->Movement.Entity = kEntityIDNull;

    RTWorldChunkRemove(Mob->Movement.WorldChunk, Mob->ID);

    RTRuntimeBroadcastEvent(
        Runtime,
        RUNTIME_EVENT_MOB_DESPAWN,
        WorldContext,
        kEntityIDNull,
        Mob->ID,
        Mob->Movement.PositionCurrent.X,
        Mob->Movement.PositionCurrent.Y
    );

    // TODO: This should be evaluated inside the mob it self!
    if (!RTEntityIsNull(Mob->DropOwner)) {
        RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Mob->DropOwner);
        if (Character) {
            RTDropResult Drop = { 0 };
            Bool Success = RTCalculateDrop(
                Runtime,
                WorldContext,
                Mob,
                Character,
                &Drop
            );

            if (Success) {
                RTWorldSpawnItem(
                    Runtime,
                    WorldContext,
                    Mob->ID,
                    Character->Movement.PositionCurrent.X,
                    Character->Movement.PositionCurrent.Y,
                    Drop
                );
            }

            Success = RTCalculateQuestDrop(
                Runtime,
                WorldContext,
                Mob,
                Character,
                &Drop
            );

            if (Success) {
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
                RTPartyRef Party = RTRuntimeGetParty(Runtime, Character->PartyID);
                assert(Party);

                RTDropResult Drop = { 0 };
                Bool Success = RTCalculatePartyQuestDrop(
                    Runtime,
                    WorldContext,
                    Mob,
                    Party,
                    &Drop
                );

                if (Success) {
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

            if (!Success) {
                Int32 Rate = RandomRange(&WorldContext->Seed, 0, 30000);
                if (Rate <= 10000) {
                    RTDropResult Drop = { 0 };
                    Drop.ItemID.ID = RUNTIME_ITEM_ID_CURRENCY;
                    Drop.ItemOptions = 10 * (100 + RandomRange(&WorldContext->Seed, 0, Mob->SpeciesData->Level)) / 100;
                    
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
    RTWorldContextRef World,
    RTEntityID Entity,
    UInt32 EntityKey
) {
    for (Int32 Index = 0; Index < World->ItemCount; Index += 1) {
        if (World->Items[Index].ID.EntityIndex == Entity.EntityIndex &&
            World->Items[Index].EntityKey == EntityKey) {
            return &World->Items[Index];
        }
    }

    return NULL;
}

RTWorldItemRef RTWorldGetItemByEntity(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity
) {
    assert(Entity.WorldIndex == World->WorldData->WorldIndex);
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_ITEM);

    for (Int32 Index = 0; Index < World->ItemCount; Index += 1) {
        if (World->Items[Index].ID.EntityIndex == Entity.EntityIndex) {
            return &World->Items[Index];
        }
    }

    return NULL;
}

RTWorldItemRef RTWorldSpawnItem(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID SourceID,
    Int32 X,
    Int32 Y,
    RTDropResult Drop
) {
    if (World->ItemCount >= RUNTIME_MEMORY_MAX_ITEM_COUNT) {
        RTWorldDespawnItem(
            Runtime,
            World,
            &World->Items[0]
        );
    }

    RTWorldItemRef Item = &World->Items[World->ItemCount];
    memset(Item, 0, sizeof(struct _RTWorldItem));

    Item->ID = RTRuntimeCreateEntity(
        Runtime,
        World->WorldData->WorldIndex,
        RUNTIME_ENTITY_TYPE_ITEM
    );
    Item->EntityKey = RandomRange(&World->Seed, 0, UINT16_MAX);
    Item->Index = World->ItemCount;
    Item->Item = Drop.ItemID;
    Item->ItemOptions = Drop.ItemOptions;
    Item->X = X;
    Item->Y = Y;
    Item->Timestamp = PlatformGetTickCount();
    World->ItemCount += 1;

    RTEventData EventData = { 0 };
    EventData.ItemSpawn.Entity = Item->ID;
    EventData.ItemSpawn.ItemOptions = Item->ItemOptions;
    EventData.ItemSpawn.ItemID = Item->Item.ID;
    EventData.ItemSpawn.X = Item->X;
    EventData.ItemSpawn.Y = Item->Y;
    EventData.ItemSpawn.UniqueKey = Item->EntityKey;
    EventData.ItemSpawn.ItemProperty = Drop.ItemProperty;

    RTRuntimeBroadcastEventData(
        Runtime,
        RUNTIME_EVENT_ITEM_SPAWN,
        World,
        SourceID,
        Item->ID,
        Item->X,
        Item->Y,
        EventData
    );

    return Item;
}

Void RTWorldDespawnItem(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTWorldItemRef Item
) {
    assert(World->WorldData->WorldIndex == Item->ID.WorldIndex);

    for (Int32 Index = 0; Index < World->ItemCount; Index++) {
        if (World->Items[Index].ID.EntityIndex == Item->ID.EntityIndex &&
            World->Items[Index].EntityKey == Item->EntityKey) {
            RTRuntimeBroadcastEvent(
                Runtime,
                RUNTIME_EVENT_ITEM_DESPAWN,
                World,
                kEntityIDNull,
                Item->ID,
                Item->X,
                Item->Y
            );

            Int32 TailLength = World->ItemCount - Index - 1;
            if (TailLength > 0) {
                memmove(&World->Items[Index], &World->Items[Index + 1], sizeof(struct _RTWorldItem) * TailLength);
            }

            World->ItemCount -= 1;
            break;
        }
    }
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

        if (WorldChunk->ReferenceCount > 0) {
            for (Index MobIndex = 0; MobIndex < ArrayGetElementCount(WorldChunk->Mobs); MobIndex += 1) {
                RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Mobs, MobIndex);
                RTMobRef Mob = RTWorldContextGetMob(WorldContext, Entity);
                assert(Mob);

                if (!Mob->IsDead) {
                    Mob->IsDead = true;

                    RTRuntimeBroadcastEvent(
                        Runtime,
                        RUNTIME_EVENT_MOB_DESPAWN,
                        WorldContext,
                        kEntityIDNull,
                        Mob->ID,
                        Mob->Movement.PositionCurrent.X,
                        Mob->Movement.PositionCurrent.Y
                    );
                }
            }
        }

        ArrayRemoveAllElements(WorldChunk->Mobs, true);
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

        DictionaryInsert(WorldContext->EntityToMob, &Mob->ID, &MemoryPoolIndex, sizeof(Index));
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
