#include "Runtime.h"
#include "WorldManager.h"

RTWorldManagerRef RTWorldManagerCreate(
    RTRuntimeRef Runtime,
    Int32 MaxWorldDataCount,
    Int32 MaxGlobalWorldContextCount,
    Int32 MaxPartyWorldContextCount,
    Int32 MaxCharacterCount
) {
    RTWorldManagerRef WorldManager = (RTWorldManagerRef)AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTWorldManager));
    if (!WorldManager) Fatal("Memory allocation failed");

    WorldManager->Allocator = Runtime->Allocator;
    WorldManager->Runtime = Runtime;
    WorldManager->MaxWorldDataCount = MaxWorldDataCount;
    WorldManager->MaxGlobalWorldContextCount = MaxGlobalWorldContextCount;
    WorldManager->MaxPartyWorldContextCount = MaxPartyWorldContextCount;
    WorldManager->MaxCharacterCount = MaxCharacterCount;
    WorldManager->WorldDataPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTWorldData),
        MaxWorldDataCount
    );
    WorldManager->GlobalWorldContextPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTWorldContext),
        MaxGlobalWorldContextCount
    );
    WorldManager->PartyWorldContextPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTWorldContext),
        MaxPartyWorldContextCount
    );
    WorldManager->CharacterContextPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTCharacter),
        MaxCharacterCount
    );
    WorldManager->IndexToGlobalWorldContextPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        MaxGlobalWorldContextCount
    );
    WorldManager->PartyToWorldContextPoolIndex = EntityDictionaryCreate(
        Runtime->Allocator,
        MaxPartyWorldContextCount
    );
    WorldManager->IndexToCharacterContextPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        MaxCharacterCount
    );

    // NOTE: WorldIndex 0 is reserved for an unused world context to preserve it for null checks
    Int NullIndex = 0;
    MemoryPoolReserve(WorldManager->GlobalWorldContextPool, NullIndex);
    MemoryPoolReserve(WorldManager->PartyWorldContextPool, NullIndex);
    MemoryPoolReserve(WorldManager->CharacterContextPool, NullIndex);
    
    return WorldManager;
}

Void RTWorldManagerDestroy(
    RTWorldManagerRef WorldManager
) {
    for (Int WorldIndex = 0; WorldIndex < WorldManager->MaxGlobalWorldContextCount; WorldIndex += 1) {
        if (!MemoryPoolIsReserved(WorldManager->GlobalWorldContextPool, WorldIndex)) continue;
        
        RTWorldContextDestroyGlobal(WorldManager, WorldIndex);
    }

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldManager->PartyToWorldContextPoolIndex);
    while (Iterator.Key) {
        RTEntityID Party = *(RTEntityID*)Iterator.Key;
        RTWorldContextDestroyParty(WorldManager, Party);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    for (Int WorldIndex = 0; WorldIndex < WorldManager->MaxWorldDataCount; WorldIndex += 1) {
        if (!MemoryPoolIsReserved(WorldManager->WorldDataPool, WorldIndex)) continue;
        
        RTWorldDataRef WorldData = (RTWorldDataRef)MemoryPoolFetch(WorldManager->WorldDataPool, WorldIndex);

        DictionaryKeyIterator NodeIterator = DictionaryGetKeyIterator(WorldData->DropTable.MobDropPool);
        while (NodeIterator.Key) {
            ArrayRef Array = DictionaryLookup(WorldData->DropTable.MobDropPool, NodeIterator.Key);
            ArrayDealloc(Array);
            NodeIterator = DictionaryKeyIteratorNext(NodeIterator);
        }

        NodeIterator = DictionaryGetKeyIterator(WorldData->DropTable.QuestDropPool);
        while (NodeIterator.Key) {
            ArrayRef Array = DictionaryLookup(WorldData->DropTable.QuestDropPool, NodeIterator.Key);
            ArrayDealloc(Array);
            NodeIterator = DictionaryKeyIteratorNext(NodeIterator);
        }

        ArrayDestroy(WorldData->DropTable.WorldDropPool);
        ArrayDestroy(WorldData->MobTable);
        ArrayDestroy(WorldData->MobScriptTable);
    }

    MemoryPoolDestroy(WorldManager->WorldDataPool);
    MemoryPoolDestroy(WorldManager->GlobalWorldContextPool);
    MemoryPoolDestroy(WorldManager->PartyWorldContextPool);
    MemoryPoolDestroy(WorldManager->CharacterContextPool);
    DictionaryDestroy(WorldManager->IndexToGlobalWorldContextPoolIndex);
    DictionaryDestroy(WorldManager->PartyToWorldContextPoolIndex);
    DictionaryDestroy(WorldManager->IndexToCharacterContextPoolIndex);
    AllocatorDeallocate(WorldManager->Allocator, WorldManager);
}

Void RTWorldManagerUpdate(
    RTWorldManagerRef WorldManager
) {
    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldManager->IndexToGlobalWorldContextPoolIndex);
    while (Iterator.Key) {
        Int WorldIndex = *(Int64*)Iterator.Key;
        RTWorldContextRef WorldContext = RTWorldContextGetGlobal(WorldManager, WorldIndex);
        if (WorldContext->Active) RTWorldContextUpdate(WorldContext);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    Iterator = DictionaryGetKeyIterator(WorldManager->PartyToWorldContextPoolIndex);
    while (Iterator.Key) {
        RTEntityID Party = *(RTEntityID*)Iterator.Key;
        RTWorldContextRef WorldContext = RTWorldContextGetParty(WorldManager, Party);
        if (WorldContext->Active) RTWorldContextUpdate(WorldContext);

        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    // TODO: This routine shouldn't be on high frequency 100ms is sufficient
    Iterator = DictionaryGetKeyIterator(WorldManager->IndexToCharacterContextPoolIndex);
    while (Iterator.Key) {
        UInt32 CharacterIndex = *(Int*)Iterator.Key;
        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(WorldManager, CharacterIndex);
        RTCharacterUpdate(WorldManager->Runtime, Character);

        Iterator = DictionaryKeyIteratorNext(Iterator);
    }
}

RTWorldDataRef RTWorldDataCreate(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
) {
    return (RTWorldDataRef)MemoryPoolReserve(WorldManager->WorldDataPool, WorldIndex);
}

Bool RTWorldDataExists(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
) {
    return MemoryPoolIsReserved(WorldManager->WorldDataPool, WorldIndex);
}

RTWorldDataRef RTWorldDataGet(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
) {
    return (RTWorldDataRef)MemoryPoolFetch(WorldManager->WorldDataPool, WorldIndex);
}

RTWorldContextRef RTWorldContextGet(
    RTWorldManagerRef WorldManager,
    Int WorldIndex,
    Int WorldInstanceIndex
) {
    RTWorldDataRef WorldData = RTWorldDataGet(WorldManager, WorldIndex);
    if (WorldData->Type == RUNTIME_WORLD_TYPE_GLOBAL) {
        return RTWorldContextGetGlobal(WorldManager, WorldIndex);
    }

    return (RTWorldContextRef)MemoryPoolFetch(WorldManager->PartyWorldContextPool, WorldInstanceIndex);
}

RTWorldContextRef RTWorldContextCreateGlobal(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
) {
    assert(!DictionaryLookup(WorldManager->IndexToGlobalWorldContextPoolIndex, &WorldIndex));

    Int WorldPoolIndex = 0;
    RTWorldContextRef WorldContext = (RTWorldContextRef)MemoryPoolReserveNext(WorldManager->GlobalWorldContextPool, &WorldPoolIndex);
    assert(WorldContext);
    WorldContext->WorldManager = WorldManager;
    WorldContext->WorldPoolIndex = WorldIndex;
    WorldContext->WorldData = RTWorldDataGet(WorldManager, WorldIndex);
    WorldContext->DungeonIndex = 0;
    WorldContext->Party = kEntityIDNull;
    WorldContext->Seed = (Int32)PlatformGetTickCount();
    WorldContext->Active = false;
    WorldContext->Closed = false;
    WorldContext->DungeonTimeout = 0;
    WorldContext->TimerIndex = -1;
    WorldContext->TimerItemCount = 0;
    WorldContext->TimerTimeout = UINT64_MAX;
    WorldContext->NextMobEntityIndex = 0;
    WorldContext->MobPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTMob), RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->MobPatternPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTMobPattern), RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->ItemPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTWorldItem), RUNTIME_MEMORY_MAX_ITEM_COUNT);
    WorldContext->EntityToMob = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->EntityToMobPattern = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->EntityToItem = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_ITEM_COUNT);
    memcpy(WorldContext->Tiles, WorldContext->WorldData->Tiles, sizeof(RTWorldTile) * RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE);
    MemoryPoolReserve(WorldContext->ItemPool, 0);
    
    for (Int ChunkX = 0; ChunkX < RUNTIME_WORLD_CHUNK_COUNT; ChunkX += 1) {
        for (Int ChunkY = 0; ChunkY < RUNTIME_WORLD_CHUNK_COUNT; ChunkY += 1) {
            Int ChunkIndex = ChunkX + ChunkY * RUNTIME_WORLD_CHUNK_COUNT; // TODO: Check which axis is the first one
            RTWorldChunkInitialize(
                WorldManager->Runtime,
                WorldContext,
                &WorldContext->Chunks[ChunkIndex],
                WorldIndex,
                0,
                ChunkX,
                ChunkY
            );
        }
    }

    DictionaryInsert(WorldManager->IndexToGlobalWorldContextPoolIndex, &WorldIndex, &WorldPoolIndex, sizeof(Int));

    if (WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_DUNGEON &&
        WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
        WorldContext->Active = true;
    }

    return WorldContext;
}

RTWorldContextRef RTWorldContextGetGlobal(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
) {
    Int* WorldPoolIndex = (Int*)DictionaryLookup(WorldManager->IndexToGlobalWorldContextPoolIndex, &WorldIndex);
    if (!WorldPoolIndex) return NULL;
    return (RTWorldContextRef)MemoryPoolFetch(WorldManager->GlobalWorldContextPool, *WorldPoolIndex);
}

Void RTWorldContextDestroyGlobal(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
) {
    RTWorldContextRef WorldContext = RTWorldContextGetGlobal(WorldManager, WorldIndex);

    for (Int ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkDeinitialize(&WorldContext->Chunks[ChunkIndex]);
    }

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldContext->EntityToMobPattern);
    while (Iterator.Key) {
        Int MemoryPoolIndex = *(Int*)DictionaryLookup(WorldContext->EntityToMobPattern, Iterator.Key);
        RTMobPatternRef MobPattern = (RTMobPatternRef)MemoryPoolFetch(WorldContext->MobPatternPool, MemoryPoolIndex);
        ArrayDestroy(MobPattern->ActionStates);
        ArrayDestroy(MobPattern->LinkMobs);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    MemoryPoolDestroy(WorldContext->MobPool);
    MemoryPoolDestroy(WorldContext->MobPatternPool);
    MemoryPoolDestroy(WorldContext->ItemPool);
    DictionaryDestroy(WorldContext->EntityToMob);
    DictionaryDestroy(WorldContext->EntityToMobPattern);
    DictionaryDestroy(WorldContext->EntityToItem);
    MemoryPoolRelease(WorldManager->GlobalWorldContextPool, WorldContext->WorldData->WorldIndex);
}

RTWorldContextRef RTWorldContextCreateParty(
    RTWorldManagerRef WorldManager,
    Int WorldIndex,
    Int64 DungeonIndex,
    RTEntityID Party
) {
    assert(!RTEntityIsNull(Party));

    Int* WorldPoolIndexPtr = DictionaryLookup(WorldManager->PartyToWorldContextPoolIndex, &Party);
    if (WorldPoolIndexPtr) {
        return (RTWorldContextRef)MemoryPoolFetch(WorldManager->PartyWorldContextPool, *WorldPoolIndexPtr);
    }

    assert(!DictionaryLookup(WorldManager->PartyToWorldContextPoolIndex, &Party));

    Int WorldPoolIndex = 0;
    RTWorldContextRef WorldContext = (RTWorldContextRef)MemoryPoolReserveNext(WorldManager->PartyWorldContextPool, &WorldPoolIndex);
    assert(WorldContext);
    WorldContext->WorldManager = WorldManager;
    WorldContext->WorldPoolIndex = WorldPoolIndex;
    WorldContext->WorldData = RTWorldDataGet(WorldManager, WorldIndex);
    WorldContext->DungeonIndex = DungeonIndex;
    WorldContext->Party = Party;
    WorldContext->Seed = (Int32)PlatformGetTickCount();
    WorldContext->Active = false;
    WorldContext->Closed = false;
    WorldContext->DungeonTimeout = 0;
    WorldContext->TimerIndex = -1;
    WorldContext->TimerItemCount = 0;
    WorldContext->TimerTimeout = UINT64_MAX;
    WorldContext->NextMobEntityIndex = 0;
    WorldContext->MobPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTMob), RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->MobPatternPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTMobPattern), RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->ItemPool = MemoryPoolCreate(WorldManager->Allocator, sizeof(struct _RTWorldItem), RUNTIME_MEMORY_MAX_ITEM_COUNT);
    WorldContext->EntityToMob = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->EntityToMobPattern = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_MOB_COUNT);
    WorldContext->EntityToItem = EntityDictionaryCreate(WorldManager->Allocator, RUNTIME_MEMORY_MAX_ITEM_COUNT);
    memcpy(WorldContext->Tiles, WorldContext->WorldData->Tiles, sizeof(RTWorldTile) * RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE);
    
    MemoryPoolReserve(WorldContext->ItemPool, 0);
    
    for (Int ChunkX = 0; ChunkX < RUNTIME_WORLD_CHUNK_COUNT; ChunkX += 1) {
        for (Int ChunkY = 0; ChunkY < RUNTIME_WORLD_CHUNK_COUNT; ChunkY += 1) {
            Int ChunkIndex = ChunkX + ChunkY * RUNTIME_WORLD_CHUNK_COUNT; // TODO: Check which axis is the first one
            RTWorldChunkInitialize(
                WorldManager->Runtime,
                WorldContext,
                &WorldContext->Chunks[ChunkIndex],
                WorldIndex,
                WorldPoolIndex,
                ChunkX,
                ChunkY
            );
        }
    }
    
    DictionaryInsert(WorldManager->PartyToWorldContextPoolIndex, &Party, &WorldPoolIndex, sizeof(Int));
    return WorldContext;
}

Bool RTWorldContextPartyIsFull(
    RTWorldManagerRef WorldManager
) {
    return MemoryPoolIsFull(WorldManager->PartyWorldContextPool);
}

Bool RTWorldContextHasParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
) {
    if (RTEntityIsNull(Party)) return false;

    Int* WorldPoolIndex = (Int*)DictionaryLookup(WorldManager->PartyToWorldContextPoolIndex, &Party);
    return (WorldPoolIndex != NULL);
}

RTWorldContextRef RTWorldContextGetParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
) {
    assert(!RTEntityIsNull(Party));
    Int* WorldPoolIndex = (Int*)DictionaryLookup(WorldManager->PartyToWorldContextPoolIndex, &Party);
    if (!WorldPoolIndex) return NULL;
    return (RTWorldContextRef)MemoryPoolFetch(WorldManager->PartyWorldContextPool, *WorldPoolIndex);
}

Void RTWorldContextDestroyParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
) {
    assert(!RTEntityIsNull(Party));

    RTWorldContextRef WorldContext = RTWorldContextGetParty(WorldManager, Party);

    for (Int ChunkIndex = 0; ChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT; ChunkIndex += 1) {
        RTWorldChunkDeinitialize(&WorldContext->Chunks[ChunkIndex]);
    }

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(WorldContext->EntityToMobPattern);
    while (Iterator.Key) {
        Int MemoryPoolIndex = *(Int*)DictionaryLookup(WorldContext->EntityToMobPattern, Iterator.Key);
        RTMobPatternRef MobPattern = (RTMobPatternRef)MemoryPoolFetch(WorldContext->MobPatternPool, MemoryPoolIndex);
        ArrayDestroy(MobPattern->ActionStates);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    MemoryPoolDestroy(WorldContext->MobPool);
    MemoryPoolDestroy(WorldContext->MobPatternPool);
    MemoryPoolDestroy(WorldContext->ItemPool);
    DictionaryDestroy(WorldContext->EntityToMob);
    DictionaryDestroy(WorldContext->EntityToMobPattern);
    DictionaryDestroy(WorldContext->EntityToItem);
    MemoryPoolRelease(WorldManager->PartyWorldContextPool, WorldContext->WorldPoolIndex);
    DictionaryRemove(WorldManager->PartyToWorldContextPoolIndex, &Party);
}

Int RTWorldContextGetPartyInstanceCount(
    RTWorldManagerRef WorldManager
) {
    return MemoryPoolGetReservedBlockCount(WorldManager->PartyWorldContextPool);
}

RTCharacterRef RTWorldManagerCreateCharacter(
    RTWorldManagerRef WorldManager,
    UInt32 CharacterIndex
) {
    assert(!DictionaryLookup(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex));

    Int CharacterPoolIndex = 0;
    RTCharacterRef Character = (RTCharacterRef)MemoryPoolReserveNext(WorldManager->CharacterContextPool, &CharacterPoolIndex);
    Character->CharacterIndex = CharacterIndex;
    Character->ID.EntityIndex = (UInt16)CharacterPoolIndex;
    Character->ID.EntityType = RUNTIME_ENTITY_TYPE_CHARACTER;
    DictionaryInsert(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex, &CharacterPoolIndex, sizeof(Int));
    return Character;
}

Bool RTWorldManagerCharacterPoolIsFull(
    RTWorldManagerRef WorldManager
) {
    return MemoryPoolIsFull(WorldManager->CharacterContextPool);
}

Bool RTWorldManagerHasCharacter(
    RTWorldManagerRef WorldManager,
    RTEntityID Entity
) {
    if (RTEntityIsNull(Entity)) return false;

    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);

    return MemoryPoolIsReserved(WorldManager->CharacterContextPool, Entity.EntityIndex);
}

RTCharacterRef RTWorldManagerGetCharacter(
    RTWorldManagerRef WorldManager,
    RTEntityID Entity
) {
    if (RTEntityIsNull(Entity) || Entity.EntityType != RUNTIME_ENTITY_TYPE_CHARACTER) return NULL;

    return (RTCharacterRef)MemoryPoolFetch(WorldManager->CharacterContextPool, Entity.EntityIndex);
}

RTCharacterRef RTWorldManagerGetCharacterByIndex(
    RTWorldManagerRef WorldManager,
    UInt32 CharacterIndex
) {
    Int* CharacterPoolIndex = DictionaryLookup(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex);
    if (!CharacterPoolIndex) return NULL;

    return (RTCharacterRef)MemoryPoolFetch(WorldManager->CharacterContextPool, *CharacterPoolIndex);
}

Void RTWorldManagerDestroyCharacter(
    RTWorldManagerRef WorldManager,
    UInt32 CharacterIndex
) {
    Int* CharacterPoolIndex = DictionaryLookup(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex);
    assert(CharacterPoolIndex);

    MemoryPoolRelease(WorldManager->CharacterContextPool, *CharacterPoolIndex);
    DictionaryRemove(WorldManager->IndexToCharacterContextPoolIndex, &CharacterIndex);
}
