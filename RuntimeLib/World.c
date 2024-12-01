#include "Mob.h"
#include "MobPattern.h"
#include "PartyManager.h"
#include "Runtime.h"
#include "World.h"
#include "WorldManager.h"
#include "NotificationProtocol.h"

Int RTCalculateWorldTileIndex(
    UInt16 X,
    UInt16 Y
) {
    return (Int)Y + (Int)X * RUNTIME_WORLD_SIZE;
}

Int RTCalculateWorldChunkIndex(
    UInt16 X,
    UInt16 Y
) {
    Int ChunkX = MIN(RUNTIME_WORLD_CHUNK_COUNT, X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    Int ChunkY = MIN(RUNTIME_WORLD_CHUNK_COUNT, Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT);
    return ChunkX + ChunkY * RUNTIME_WORLD_CHUNK_COUNT;
}

RTWorldChunkRef RTWorldContextGetChunk(
    RTWorldContextRef WorldContext,
    UInt16 X,
    UInt16 Y
) {
    Int ChunkIndex = RTCalculateWorldChunkIndex(X, Y);
    assert(ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT);
    return &WorldContext->Chunks[ChunkIndex];
}
/*
RTWorldTile RTWorldContextGetTile(
    RTWorldContextRef WorldContext,
    UInt16 X,
    UInt16 Y
) {
    Int TileIndex = RTCalculateWorldTileIndex(X, Y);
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

    for (Int DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Int ChunkIndex = DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT* RUNTIME_WORLD_CHUNK_COUNT);
            RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
            WorldChunk->ReferenceCount += ReferenceCount;
        }
    }
}

Void RTWorldContextUpdate(
    RTWorldContextRef WorldContext
) {
    if (WorldContext->Paused) return;

    if (WorldContext->WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON ||
        WorldContext->WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
        RTDungeonUpdate(WorldContext);

        /* NOTE: See @MobUpdateImprovement
        DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldContext->EntityToMob);
        while (Iterator.Key) {
            RTEntityID MobID = *(RTEntityID*)Iterator.Key;
            RTMobRef Mob = RTWorldContextGetMob(WorldContext, MobID);
            assert(Mob);
            RTMobUpdate(WorldContext->WorldManager->Runtime, WorldContext, Mob);
            Iterator = DictionaryKeyIteratorNext(Iterator);
        }
        */
    } 

    // NOTE: See @MobUpdateImprovement
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

        for (Int ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
            RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
            /* TODO: @MobUpdateImprovement Fix world chunk mob update idling
            if (WorldContext->WorldData->Type < 1 && WorldChunk->ReferenceCount > 0) {
                for (Int Index = 0; Index < ArrayGetElementCount(WorldChunk->Mobs); Index += 1) {
                    RTEntityID MobID = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Mobs, Index);
                    RTMobRef Mob = RTWorldContextGetMob(WorldContext, MobID);
                    assert(Mob);
                    RTMobUpdate(WorldContext->WorldManager->Runtime, WorldContext, Mob);
                }
            }
            */
            if (WorldChunk->NextItemUpdateTimestamp > Timestamp) continue;
            WorldChunk->NextItemUpdateTimestamp = INT64_MAX;

            for (Int ItemIndex = (Int32)ArrayGetElementCount(WorldChunk->Items) - 1; ItemIndex >= 0; ItemIndex -= 1) {
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
    RTWorldTileIncreaseCharacterCount(Runtime, WorldContext, Character->Movement.PositionTile.X, Character->Movement.PositionTile.Y);

    Character->Movement.WorldContext = WorldContext;
    Character->Movement.WorldChunk = WorldChunk;
    Character->Movement.Entity = Character->ID;

    RTCharacterNotifyStatus(Runtime, Character);
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
    RTWorldTileDecreaseCharacterCount(Runtime, WorldContext, Character->Movement.PositionTile.X, Character->Movement.PositionTile.Y);

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
    Mob->EventSpawnTimestamp = GetTimestampMs() + Delay;
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

    if (X < 0 || Y < 0) {
        RTMobRef Target = RTWorldContextGetMob(WorldContext, Mob->LinkEntityID);
        X = Target->Movement.PositionCurrent.X;
        Y = Target->Movement.PositionCurrent.Y;
    }

    if (!Mob->Spawn.IsMissionGate && (Mob->Spawn.AreaWidth > 1 || Mob->Spawn.AreaHeight > 1)) {
        Int32 RemainingRandomCount = 3;
        while (RemainingRandomCount >= 0) {
            Int32 RandomX = RandomRange(&WorldContext->Seed, X, X + Mob->Spawn.AreaWidth);
            Int32 RandomY = RandomRange(&WorldContext->Seed, Y, Y + Mob->Spawn.AreaHeight);

            if (!RTWorldIsTileColliding(Runtime, WorldContext, RandomX, RandomY, CollisionMask)) {
                X = RandomX;
                Y = RandomY;
                break;
            }

            RemainingRandomCount -= 1;
        }
    }

    RTMobInit(Runtime, Mob);
    RTMovementInitialize(
        Runtime,
        &Mob->Movement,
        Mob->ID,
        X,
        Y,
        (Int32)Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED],
        RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN
    );
    RTWorldTileIncreaseMobCount(Runtime, WorldContext, Mob->Movement.PositionTile.X, Mob->Movement.PositionTile.Y);

    Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
    Mob->IsDead = false;
    Mob->IsIdle = true;
    Mob->IsSpawned = true;
    Mob->IsTimerMob = false;
    Mob->EventSpawnTimestamp = 0;
    Mob->EventDespawnTimestamp = 0;
    Mob->EventRespawnTimestamp = 0;

    RTWorldChunkRef WorldChunk = RTWorldContextGetChunk(WorldContext, X, Y);
    RTWorldChunkInsert(WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);

    Mob->Movement.WorldContext = WorldContext;
    Mob->Movement.WorldChunk = WorldChunk;
    Mob->Movement.Entity = Mob->ID;

    if (Mob->Pattern) RTMobPatternSpawn(Runtime, WorldContext, Mob, Mob->Pattern);

    if (Mob->Spawn.SpawnTriggerID > 0) {
        RTDungeonTriggerEvent(WorldContext, Mob->ID, Mob->Spawn.SpawnTriggerID);
    }

    if (WorldContext->DungeonIndex > 0) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);
        assert(DungeonData);

        Int MobIndex = Mob->ID.EntityIndex;
        RTDungeonTimeControlDataRef TimeControl = (RTDungeonTimeControlDataRef)DictionaryLookup(DungeonData->TimeControls, &MobIndex);
        if (TimeControl && TimeControl->Event == RUNTIME_DUNGEON_TIME_CONTROL_TYPE_SPAWN) {
            RTDungeonAddTime(WorldContext, TimeControl->Value);
        }

        if (DungeonData->TimerData.Active) {
            for (Int Index = 0; Index < DungeonData->TimerData.MobIndexCount; Index += 1) {
                if (Mob->ID.EntityIndex == DungeonData->TimerData.MobIndexList[Index]) {
                    Mob->IsTimerMob = true;
                    break;
                }
            }

            RTDungeonUpdateTimer(WorldContext, RUNTIME_DUNGEON_TIMER_TARGET_EVENT_TYPE_SPAWN, Mob->ID.EntityIndex);
        }

        RTDungeonImmuneControlDataRef ImmuneControl = (RTDungeonImmuneControlDataRef)DictionaryLookup(DungeonData->ImmuneControls, &MobIndex);
        if (ImmuneControl) {
            for (Int Index = 0; Index < ImmuneControl->ImmuneCount; Index += 1) {
                RTImmuneDataRef ImmuneData = &ImmuneControl->ImmuneList[Index];
                if (ImmuneData->CanAttack > 0) continue; // TODO: Check if this case is necessary 

                RTEntityID TargetID = {
                    .EntityIndex = ImmuneData->TargetIndex,
                    .WorldIndex = WorldContext->WorldData->WorldIndex,
                    .EntityType = RUNTIME_ENTITY_TYPE_MOB
                };

                RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
                if (!Target) continue;

                if (ImmuneData->ActivationType == RUNTIME_DUNGEON_IMMUNE_ACTIVATION_TYPE_ALIVE) {
                    Target->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_COMPLETE] += 1;
                }
                else {
                    Target->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_COMPLETE] = MAX(
                        Target->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_COMPLETE] - 1,
                        0
                    );
                }
            }
        }

        RTDungeonGateControlDataRef GateControl = (RTDungeonGateControlDataRef)DictionaryLookup(DungeonData->GateControls, &MobIndex);
        if (GateControl) {
            // assert(Mob->Spawn.IsMissionGate); TODO: Verify data quality
    
            if (!GateControl->CanAttack) {
                for (Int Index = 0; Index < GateControl->CellCount; Index += 1) {
                    RTWorldTileIncreaseImmunityCount(Runtime, WorldContext, GateControl->CellList[Index].X, GateControl->CellList[Index].Y);
                }
            }
        }
    }

    RTMobOnEvent(Runtime, WorldContext, Mob, MOB_EVENT_SPAWN);

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldContext->EntityToMob);
    while (Iterator.Key) {
        RTEntityID WorldMobID = *(RTEntityID*)Iterator.Key;
        RTMobRef WorldMob = RTWorldContextGetMob(WorldContext, WorldMobID);
        if (WorldMob->Pattern && !RTEntityIsEqual(WorldMob->ID, Mob->ID)) {
            RTMobPatternMobSpawned(Runtime, WorldContext, WorldMob, WorldMob->Pattern, Mob);
        }

        Iterator = DictionaryKeyIteratorNext(Iterator);
    }
}

Void RTWorldCreateMob(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTMobRef LinkMob,
    Int32 LinkMobIndex,
    Int32 MobSpeciesIndex,
    Int32 AreaX,
    Int32 AreaY,
    Int32 AreaWidth,
    Int32 AreaHeight,
    Int32 Interval,
    Int32 Count,
    Int32 MobPatternIndex,
    RTScriptRef Script,
    Timestamp Delay
) {
    assert(MobSpeciesIndex < Runtime->MobDataCount);
    assert(AreaX >= 0 || (AreaX < 0 && LinkMob && !RTEntityIsNull(LinkMob->ID)));
    assert(AreaX >= 0 || (AreaX < 0 && LinkMob && !RTEntityIsNull(LinkMob->ID)));
    assert(AreaY >= 0 || (AreaY < 0 && LinkMob && !RTEntityIsNull(LinkMob->ID)));

    for (Int SpawnIndex = 0; SpawnIndex < Count; SpawnIndex += 1) {
        Int MemoryPoolIndex = 0;
        RTMobRef Mob = (RTMobRef)MemoryPoolReserveNext(WorldContext->MobPool, &MemoryPoolIndex);
        Mob->ID.EntityIndex = WorldContext->NextMobEntityIndex;
        Mob->ID.WorldIndex = WorldContext->WorldData->WorldIndex;
        Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
        Mob->SpeciesData = &Runtime->MobData[MobSpeciesIndex];
        Mob->RemainingFindCount = Mob->SpeciesData->FindCount;
        Mob->RemainingSpawnCount = Count;
        Mob->IsAttackingCharacter = true;
        Mob->LinkMobIndex = LinkMobIndex;
        Mob->LinkEntityID = (LinkMob) ? LinkMob->ID : kEntityIDNull;
        Mob->Spawn.MobSpeciesIndex = MobSpeciesIndex;
        Mob->Spawn.MobPatternIndex = MobPatternIndex;
        Mob->Spawn.AreaX = AreaX;
        Mob->Spawn.AreaY = AreaY;
        Mob->Spawn.AreaWidth = AreaWidth;
        Mob->Spawn.AreaHeight = AreaHeight;
        Mob->Spawn.SpawnInterval = Interval;
        Mob->Spawn.SpawnCount = 1;
        Mob->Script = Script;

        if (LinkMob) {
            assert(LinkMob->Pattern);
            ArrayAppendElement(LinkMob->Pattern->LinkMobs, &Mob->ID);
        }

        if (Mob->Spawn.MobPatternIndex > 0) {
            Int MobPatternIndex = Mob->Spawn.MobPatternIndex;
            RTMobPatternDataRef MobPatternData = (RTMobPatternDataRef)MemoryPoolFetch(Runtime->MobPatternDataPool, MobPatternIndex);
            if (MobPatternData) {
                Int PatternMemoryPoolIndex = 0;
                Mob->Pattern = (RTMobPatternRef)MemoryPoolReserveNext(WorldContext->MobPatternPool, &PatternMemoryPoolIndex);
                memset(Mob->Pattern, 0, sizeof(struct _RTMobPattern));
                Mob->Pattern->Data = MobPatternData;
                Mob->Pattern->ActionStates = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMobActionState), RUNTIME_MOB_PATTERN_MAX_TRIGGER_GROUP_COUNT);
                Mob->Pattern->LinkMobs = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
                DictionaryInsert(WorldContext->EntityToMobPattern, &Mob->ID, &PatternMemoryPoolIndex, sizeof(Int));
            }
        }

        DictionaryInsert(WorldContext->EntityToMob, &Mob->ID, &MemoryPoolIndex, sizeof(Int));

        WorldContext->NextMobEntityIndex += 1;

        RTWorldSpawnMobEvent(Runtime, WorldContext, Mob, Delay);
    }
}

RTMobRef RTWorldContextGetMob(
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    Int* MobPoolIndex = (Int*)DictionaryLookup(WorldContext->EntityToMob, &Entity);
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
    Mob->EventDespawnTimestamp = GetTimestampMs() + Delay;
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

    if (Mob->Pattern) {
        for (Int Index = 0; Index < ArrayGetElementCount(Mob->Pattern->LinkMobs); Index += 1) {
            RTEntityID LinkMobID = *(RTEntityID*)ArrayGetElementAtIndex(Mob->Pattern->LinkMobs, Index);
            RTMobRef LinkMob = RTWorldContextGetMob(WorldContext, LinkMobID);
            assert(LinkMob);
        
            if (RTMobIsAlive(LinkMob)) {
                RTWorldDespawnMobEvent(Runtime, WorldContext, LinkMob, 0);
            }
        }

        RTMobPatternStop(Runtime, WorldContext, Mob, Mob->Pattern);
    }

    // TODO: This should be evaluated inside the mob it self!
    if (!RTEntityIsNull(Mob->DropOwner) && Mob->DropOwner.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) { // TODO: Why do we check here if the mob has a drop owner?
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

            if (Drop.ItemID.Serial > 0 && WorldContext->DungeonIndex > 0) {
                RTDungeonUpdateTimerItemCount(
                    WorldContext,
                    Drop.ItemID,
                    Drop.ItemOptions
                );
            }
        }
    }

    Mob->IsSpawned = false;
    Mob->IsDead = true;
    Mob->IsIdle = true;
    Mob->Movement.WorldContext = NULL;
    Mob->Movement.Entity = kEntityIDNull;

    RTWorldChunkRef WorldChunk = Mob->Movement.WorldChunk;
    Int32 UpdateReason = RTEntityIsNull(Mob->EventDespawnLinkID) ? RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT : RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE;
    RTWorldChunkRemove(WorldChunk, Mob->ID, UpdateReason);
    RTMobOnEvent(Runtime, WorldContext, Mob, MOB_EVENT_DESPAWN);
    RTWorldTileDecreaseMobCount(Runtime, WorldContext, Mob->Movement.PositionTile.X, Mob->Movement.PositionTile.Y);

    if (!RTEntityIsNull(Mob->EventDespawnLinkID)) {
        NOTIFICATION_DATA_MOBS_DESPAWN_BY_LINK_MOB* Notification = RTNotificationInit(MOBS_DESPAWN_BY_LINK_MOB);
        Notification->SourceID = Mob->EventDespawnLinkID;
        Notification->Count = 1;

        NOTIFICATION_DATA_MOB_DESPAWN_BY_LINK_MOB_INDEX* NotificationMob = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOB_DESPAWN_BY_LINK_MOB_INDEX);
        NotificationMob->MobID = Mob->ID;
        NotificationMob->Reason = NOTIFICATION_DESPAWN_TYPE_DEAD;
        RTNotificationDispatchToNearby(Notification, WorldChunk);

        Mob->EventDespawnLinkID = kEntityIDNull;
    }

    if (Mob->Spawn.KillTriggerID > 0) {
        RTDungeonTriggerEvent(WorldContext, Mob->ID, Mob->Spawn.KillTriggerID);
    }

    for (Int Index = 0; Index < RUNTIME_MOB_MAX_EVENT_COUNT; Index += 1) {
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

    if (WorldContext->DungeonIndex > 0) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);
        assert(DungeonData);

        Int MobIndex = Mob->ID.EntityIndex;
        RTDungeonTimeControlDataRef TimeControl = (RTDungeonTimeControlDataRef)DictionaryLookup(DungeonData->TimeControls, &MobIndex);
        if (TimeControl && TimeControl->Event == RUNTIME_DUNGEON_TIME_CONTROL_TYPE_DESPAWN) {
            RTDungeonAddTime(WorldContext, TimeControl->Value);
        }

        if (DungeonData->TimerData.Active) {
            RTDungeonUpdateTimer(WorldContext, RUNTIME_DUNGEON_TIMER_TARGET_EVENT_TYPE_DESPAWN, Mob->ID.EntityIndex);
        }

        RTDungeonImmuneControlDataRef ImmuneControl = (RTDungeonImmuneControlDataRef)DictionaryLookup(DungeonData->ImmuneControls, &MobIndex);
        if (ImmuneControl) {
            for (Int Index = 0; Index < ImmuneControl->ImmuneCount; Index += 1) {
                RTImmuneDataRef ImmuneData = &ImmuneControl->ImmuneList[Index];
                if (ImmuneData->CanAttack > 0) continue; // TODO: Check if this case is necessary 

                RTEntityID TargetID = {
                    .EntityIndex = ImmuneData->TargetIndex,
                    .WorldIndex = WorldContext->WorldData->WorldIndex,
                    .EntityType = RUNTIME_ENTITY_TYPE_MOB
                };

                RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
                if (!Target) continue;

                if (ImmuneData->ActivationType == RUNTIME_DUNGEON_IMMUNE_ACTIVATION_TYPE_DEAD) {
                    Target->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_COMPLETE] += 1;
                }
                else {
                    Target->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_COMPLETE] = MAX(
                        Target->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_COMPLETE] - 1,
                        0
                    );
                }
            }
        }

        RTDungeonGateControlDataRef GateControl = (RTDungeonGateControlDataRef)DictionaryLookup(DungeonData->GateControls, &MobIndex);
        if (GateControl) {
            if (!GateControl->CanAttack) {
                for (Int Index = 0; Index < GateControl->CellCount; Index += 1) {
                    RTWorldTileDecreaseImmunityCount(Runtime, WorldContext, GateControl->CellList[Index].X, GateControl->CellList[Index].Y);
                }
            }
        }
    }

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldContext->EntityToMob);
    while (Iterator.Key) {
        RTEntityID WorldMobID = *(RTEntityID*)Iterator.Key;
        RTMobRef WorldMob = RTWorldContextGetMob(WorldContext, WorldMobID);
        if (WorldMob->Pattern && !RTEntityIsEqual(WorldMob->ID, Mob->ID)) {
            RTMobPatternMobDespawned(Runtime, WorldContext, WorldMob, WorldMob->Pattern, Mob);
        }

        Iterator = DictionaryKeyIteratorNext(Iterator);
    }
}

Void RTWorldRespawnMobEvent(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    Timestamp Delay
) {
    Mob->NextTimestamp = 0;
    Mob->EventRespawnTimestamp = GetTimestampMs() + Delay;
}

Void RTWorldRespawnMob(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTMobRef Mob
) {
    assert(WorldContext->WorldData->WorldIndex == Mob->ID.WorldIndex);
    
    if (Mob->IsSpawned) {
        if (Mob->Pattern) {
            RTMobPatternStop(Runtime, WorldContext, Mob, Mob->Pattern);

            Int MemoryPoolIndex = *(Int*)DictionaryLookup(WorldContext->EntityToMobPattern, &Mob->ID);
            ArrayDestroy(Mob->Pattern->ActionStates);
            ArrayDestroy(Mob->Pattern->LinkMobs);
            MemoryPoolRelease(WorldContext->MobPatternPool, MemoryPoolIndex);
            DictionaryRemove(WorldContext->EntityToMobPattern, &Mob->ID);
            Mob->Pattern = NULL;
        }

        Mob->IsSpawned = false;
        Mob->IsDead = true;
        Mob->IsIdle = true;

        RTWorldChunkRef WorldChunk = Mob->Movement.WorldChunk;
        Int32 UpdateReason = RTEntityIsNull(Mob->EventDespawnLinkID) ? RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT : RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE;
        RTWorldChunkRemove(WorldChunk, Mob->ID, UpdateReason);
        RTWorldTileDecreaseMobCount(Runtime, WorldContext, Mob->Movement.PositionTile.X, Mob->Movement.PositionTile.Y);
    }
    
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

    RTMobInit(Runtime, Mob);
    RTMovementInitialize(
        Runtime,
        &Mob->Movement,
        Mob->ID,
        X,
        Y,
        (Int32)Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED],
        CollisionMask
    );
    RTWorldTileIncreaseMobCount(Runtime, WorldContext, Mob->Movement.PositionTile.X, Mob->Movement.PositionTile.Y);

    Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
    Mob->IsDead = false;
    Mob->IsIdle = true;
    Mob->IsSpawned = true;
    Mob->IsTimerMob = false;
    Mob->EventSpawnTimestamp = 0;
    Mob->EventDespawnTimestamp = 0;
    Mob->EventRespawnTimestamp = 0;

    RTWorldChunkRef WorldChunk = RTWorldContextGetChunk(WorldContext, X, Y);
    Mob->Movement.WorldContext = WorldContext;
    Mob->Movement.WorldChunk = WorldChunk;
    Mob->Movement.Entity = Mob->ID;
    RTWorldChunkInsert(WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);

    if (Mob->Pattern) RTMobPatternSpawn(Runtime, WorldContext, Mob, Mob->Pattern);
}

RTWorldItemRef RTWorldContextGetItem(
    RTWorldContextRef WorldContext,
    RTEntityID Entity
) {
    Int* ItemPoolIndex = (Int*)DictionaryLookup(WorldContext->EntityToItem, &Entity);
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
    Int* ItemPoolIndex = (Int*)DictionaryLookup(WorldContext->EntityToItem, &Entity);
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
    
    Int ItemPoolIndex = 0;
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

    DictionaryInsert(WorldContext->EntityToItem, &Item->ID, &ItemPoolIndex, sizeof(Int));
    
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

Bool RTWorldTileIsImmune(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
) {
    assert(World);

    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) return false;

    return World->Tiles[Y + X * RUNTIME_WORLD_SIZE].ImmunityCount > 0;
}

Void RTWorldTileIncreaseImmunityCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
) {
    assert(World);

    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) return;

    World->Tiles[Y + X * RUNTIME_WORLD_SIZE].ImmunityCount += 1;
}

Void RTWorldTileDecreaseImmunityCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
) {
    assert(World);

    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) return;

    World->Tiles[Y + X * RUNTIME_WORLD_SIZE].ImmunityCount -= 1;
}

Void RTWorldTileIncreaseCharacterCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
) {
    assert(World);

    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) return;

    World->Tiles[Y + X * RUNTIME_WORLD_SIZE].CharacterCount += 1;
}

Void RTWorldTileDecreaseCharacterCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
) {
    assert(World);

    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) return;

    World->Tiles[Y + X * RUNTIME_WORLD_SIZE].CharacterCount -= 1;
}

Void RTWorldTileIncreaseMobCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
) {
    assert(World);

    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) return;

    World->Tiles[Y + X * RUNTIME_WORLD_SIZE].MobCount += 1;
}

Void RTWorldTileDecreaseMobCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
) {
    assert(World);

    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) return;
    
    World->Tiles[Y + X * RUNTIME_WORLD_SIZE].MobCount -= 1;
}

Bool RTWorldIsTileColliding(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y,
    UInt32 CollisionMask
) {
    assert(World);

    if (X < 0 || X >= RUNTIME_WORLD_SIZE || Y < 0 || Y >= RUNTIME_WORLD_SIZE) {
        return true;
    }

    return (World->Tiles[Y + X * RUNTIME_WORLD_SIZE].Serial & CollisionMask) > 0;
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

    for (Int Index = 0; Index < Length; Index++) {
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
    Int64 MobSpeciesIndex
) {
    if (World->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return;

    for (Int Index = 0; Index < World->MissionMobCount; Index++) {
        if (World->MissionMobs[Index].MobID != MobSpeciesIndex) continue;

        World->MissionMobs[Index].Count = MIN(World->MissionMobs[Index].Count + 1, World->MissionMobs[Index].MaxCount);
    }

    if (World->MissionMobCount > 0) {
        RTDungeonStartNextPatternPart(World);
    }

    if (!RTEntityIsNull(World->Party)) {
        RTPartyIncrementQuestMobCounter(Runtime, World->Party, MobSpeciesIndex);
    }
}

Void RTWorldSetMobTable(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    ArrayRef MobTable
) {
    for (Int ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkRef WorldChunk = &WorldContext->Chunks[ChunkIndex];
        for (Int MobIndex = (Int32)ArrayGetElementCount(WorldChunk->Mobs) - 1; MobIndex >= 0; MobIndex -= 1) {
            RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Mobs, MobIndex);
            RTWorldChunkRemove(WorldChunk, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT);
        }
    }

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldContext->EntityToMobPattern);
    while (Iterator.Key) {
        Int MemoryPoolIndex = *(Int*)DictionaryLookup(WorldContext->EntityToMobPattern, Iterator.Key);
        RTMobPatternRef MobPattern = (RTMobPatternRef)MemoryPoolFetch(WorldContext->MobPatternPool, MemoryPoolIndex);
        ArrayDestroy(MobPattern->ActionStates);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    MemoryPoolClear(WorldContext->MobPatternPool);
    MemoryPoolClear(WorldContext->MobPool);
    DictionaryRemoveAll(WorldContext->EntityToMob);
    DictionaryRemoveAll(WorldContext->EntityToMobPattern);

    WorldContext->NextMobEntityIndex = 0;

    for (Int MobIndex = 0; MobIndex < ArrayGetElementCount(MobTable); MobIndex += 1) {
        RTMobRef TableMob = (RTMobRef)ArrayGetElementAtIndex(MobTable, MobIndex);

        Int MemoryPoolIndex = 0;
        RTMobRef Mob = (RTMobRef)MemoryPoolReserveNext(WorldContext->MobPool, &MemoryPoolIndex);
        memcpy(Mob, TableMob, sizeof(struct _RTMob));

        Mob->ID.EntityIndex = TableMob->ID.EntityIndex;
        Mob->ID.WorldIndex = WorldContext->WorldData->WorldIndex;
        Mob->ID.EntityType = RUNTIME_ENTITY_TYPE_MOB;
        Mob->IsSpawned = false;
        Mob->RemainingFindCount = Mob->SpeciesData->FindCount;
        Mob->RemainingSpawnCount = MAX(Mob->Spawn.SpawnCount - 1, 0);

        WorldContext->NextMobEntityIndex = MAX(WorldContext->NextMobEntityIndex, TableMob->ID.EntityIndex + 1);

        if (Mob->Spawn.MobPatternIndex > 0) {
            Int MobPatternIndex = Mob->Spawn.MobPatternIndex;
            RTMobPatternDataRef MobPatternData = (RTMobPatternDataRef)MemoryPoolFetch(Runtime->MobPatternDataPool, MobPatternIndex);
            if (MobPatternData) {
                Int PatternMemoryPoolIndex = 0;
                Mob->Pattern = (RTMobPatternRef)MemoryPoolReserveNext(WorldContext->MobPatternPool, &PatternMemoryPoolIndex);
                memset(Mob->Pattern, 0, sizeof(struct _RTMobPattern));
                Mob->Pattern->Data = MobPatternData;
                Mob->Pattern->ActionStates = ArrayCreateEmpty(Runtime->Allocator, sizeof(struct _RTMobActionState), RUNTIME_MOB_PATTERN_MAX_TRIGGER_GROUP_COUNT);
                Mob->Pattern->LinkMobs = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
                DictionaryInsert(WorldContext->EntityToMobPattern, &Mob->ID, &PatternMemoryPoolIndex, sizeof(Int));
            }
        }

        DictionaryInsert(WorldContext->EntityToMob, &Mob->ID, &MemoryPoolIndex, sizeof(Int));
    }

    for (Int MobIndex = 0; MobIndex < ArrayGetElementCount(MobTable); MobIndex += 1) {
        RTMobRef TableMob = (RTMobRef)ArrayGetElementAtIndex(MobTable, MobIndex);

        RTEntityID MobID = { 0 };
        MobID.EntityIndex = TableMob->ID.EntityIndex;
        MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
        MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        Int* MemoryPoolIndex = DictionaryLookup(WorldContext->EntityToMob, &MobID);
        assert(MemoryPoolIndex);

        RTMobRef Mob = (RTMobRef)MemoryPoolFetch(WorldContext->MobPool, *MemoryPoolIndex);
        assert(Mob);
         
        if (Mob->Spawn.SpawnDefault > 0) {
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

    for (Int DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Int ChunkIndex = DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
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

            for (Int Index = 0; Index < ArrayGetElementCount(Entities); Index += 1) {
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

    for (Int DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Int ChunkIndex = DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
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

            for (Int Index = 0; Index < ArrayGetElementCount(Entities); Index += 1) {
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

    for (Int DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            if (RTChunkAreaIntersectsPoint(
                PreviousChunkX,
                PreviousChunkY,
                RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS,
                DeltaChunkX,
                DeltaChunkY
            )) {
                continue;
            }
        
            Int ChunkIndex = DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
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

            for (Int Index = 0; Index < ArrayGetElementCount(Entities); Index += 1) {
                RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(Entities, Index);
                Callback(Entity, Userdata);
            }
        }
    }
}
