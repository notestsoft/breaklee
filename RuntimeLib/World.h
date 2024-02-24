#pragma once

#include "Base.h"
#include "Constants.h"
#include "Character.h"
#include "Dungeon.h"
#include "Drop.h"
#include "Mob.h"
#include "WorldChunk.h"

EXTERN_C_BEGIN

enum {
    RUNTIME_WORLD_TYPE_GLOBAL,
    RUNTIME_WORLD_TYPE_DUNGEON,
    RUNTIME_WORLD_TYPE_QUEST_DUNGEON,
};

enum {
    RUNTIME_WORLD_TILE_NONE = 0b00000000,
    RUNTIME_WORLD_TILE_WALL = 0b00000001,
    RUNTIME_WORLD_TILE_TOWN = 0b00000110,
};

enum {
    RUNTIME_WORLD_FLAGS_HAS_MAPCODE = 1 << 0,
    RUNTIME_WORLD_FLAGS_WAR_ALLOWED = 1 << 1,
    RUNTIME_WORLD_FLAGS_WAR_CONTROL = 1 << 2,
};

struct _RTWorldItem {
    RTEntityID ID;
    UInt16 EntityKey;
    Int32 Index;
    RTItem Item;
    UInt64 ItemOptions;
    Int32 X;
    Int32 Y;
    Timestamp Timestamp;
};

struct _RTWorldData {
    Index WorldIndex;
    Int32 Type;
    Int32 DeadWarpIndex;
    Int32 ReturnWarpIndex;
    Int32 MapCodeIndex;
    UInt32 Flags;
    Bool HasQuestDungeon;
    Bool HasMissionDungeon;
    ArrayRef MobTable;
    struct _RTDropTable DropTable;
    UInt32 Tiles[RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE];
};

struct _RTWorldContext {
    RTWorldManagerRef WorldManager;
    RTWorldDataRef WorldData;
    Index WorldPoolIndex;
    Index DungeonIndex;
    RTEntityID Owner;
    RTEntityID Party;
    Bool Active;
    Bool Closed;
    Bool Cleared;
    Int32 Seed;
    Int32 ItemCount;
    struct _RTWorldItem Items[RUNTIME_MEMORY_MAX_ITEM_COUNT];
    
    Int32 MissionItemCount;
    Int32 MissionMobCount;
    struct _RTQuestUnitItemData MissionItems[RUNTIME_MAX_QUEST_COUNTER_COUNT];
    struct _RTQuestUnitMobData MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];
    struct _RTWorldChunk Chunks[RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT];

    Timestamp DungeonTimeout;
    Int32 PatternPartIndex;
    MemoryPoolRef MobPool;
    MemoryPoolRef ItemPool;
    DictionaryRef EntityToMob;
    DictionaryRef EntityToItem;
};

RTWorldChunkRef RTWorldContextGetChunk(
    RTWorldContextRef WorldContext,
    UInt16 X,
    UInt16 Y
);

Void RTWorldContextUpdate(
    RTWorldContextRef WorldContext
);

Void* RTWorldContextGetEntityContext(
    RTWorldContextRef WorldContext,
    RTEntityID Entity
);

Void RTWorldSpawnCharacterWithoutNotification(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity
);

Void RTWorldBroadcastSpawnCharacterNotification(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity
);

Void RTWorldSpawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity
);

Void RTWorldDespawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity
);

Void RTWorldSpawnMobEvent(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    Timestamp Delay
);

Void RTWorldSpawnMob(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob
);

RTMobRef RTWorldContextGetMob(
    RTWorldContextRef WorldContext,
    RTEntityID Entity
);

Void RTWorldDespawnMobEvent(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    Timestamp Delay
);

Void RTWorldDespawnMob(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob
);

RTWorldItemRef RTWorldSpawnItem(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID SourceID,
    Int32 X,
    Int32 Y,
    RTDropResult Drop
);

RTWorldItemRef RTWorldContextGetItem(
    RTWorldContextRef WorldContext,
    RTEntityID Entity
);

RTWorldItemRef RTWorldGetItem(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity,
    UInt32 EntityKey
);

RTWorldItemRef RTWorldGetItemByEntity(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity
);

Void RTWorldDespawnItem(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTWorldItemRef Item
);

Bool RTWorldIsTileColliding(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y,
    UInt32 CollisionMask
);

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
);

Void RTWorldIncrementQuestMobCounter(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Index MobSpeciesIndex
);

Void RTWorldSetMobTable(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    ArrayRef MobTable
);

typedef Void(*RTEntityVisitorByDistanceCallback)(
    RTEntityID Entity,
    UInt16 Distance,
    Void* Userdata
);

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
);

Void RTWorldContextEnumerateBroadcastTargets(
    RTWorldContextRef WorldContext,
    UInt8 EntityType,
    UInt16 X,
    UInt16 Y,
    RTEntityVisitorCallback Callback,
    Void* Userdata
);

EXTERN_C_END
