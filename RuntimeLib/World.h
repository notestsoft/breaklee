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

struct _RTWorldTile {
    union {
        struct { UInt32 Serial; };
        struct {
            UInt32 IsWall : 1;
            UInt32 IsSafeZone : 1;
            UInt32 IsTownArea : 1;
            UInt32 IsDamageCell : 1;
            UInt32 IsOverAttack : 2;
            UInt32 Padding1 : 2;
            UInt32 ItemCount : 8;
            UInt32 Padding2 : 4;
            UInt32 ImmunityCount : 4;
            UInt32 CharacterCount : 4;
            UInt32 MobCount : 4;
        };
    };
};
typedef struct _RTWorldTile RTWorldTile;

enum {
    RUNTIME_WORLD_TILE_NONE = 0b00000000,
    RUNTIME_WORLD_TILE_WALL = 0b00000001,
    RUNTIME_WORLD_TILE_TOWN = 0b00000110,
    RUNTIME_WORLD_TILE_USER = 0x0F000000,
    RUNTIME_WORLD_TILE_MOBS = 0xF0000000,
};

enum {
    RUNTIME_WORLD_FLAGS_HAS_MAPCODE = 1 << 0,
    RUNTIME_WORLD_FLAGS_WAR_ALLOWED = 1 << 1,
    RUNTIME_WORLD_FLAGS_WAR_CONTROL = 1 << 2,
};

struct _RTWorldItem {
    Int Index;
    RTEntityID ID;
    UInt64 ItemOptions;
    UInt32 ItemSourceIndex;
    RTItem Item;
    UInt16 X;
    UInt16 Y;
    UInt16 ItemUniqueKey;
    UInt8 ContextType;
    RTItemProperty ItemProperty;
    RTItemDuration ItemDuration;
    Timestamp Timestamp;
    Timestamp DespawnTimestamp;
};

struct _RTWorldData {
    Int WorldIndex;
    Int32 Type;
    Int32 DeadWarpIndex;
    Int32 ReturnWarpIndex;
    Int32 MapCodeIndex;
    UInt32 Flags;
    Bool HasQuestDungeon;
    Bool HasMissionDungeon;
    ArrayRef MobTable;
    ArrayRef MobScriptTable;
    struct _RTDropTable DropTable;
    RTWorldTile Tiles[RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE];
};

struct _RTWorldContext {
    RTWorldManagerRef WorldManager;
    RTWorldDataRef WorldData;
    Int32 WorldPoolIndex;
    Int32 DungeonIndex;
    RTEntityID Owner;
    RTEntityID Party;
    Bool Active;
    Bool Paused;
    Bool Closed;
    Bool Cleared;
    Int32 Seed;
    Int32 ReferenceCount;
    Int32 MissionItemCount;
    Int32 MissionMobCount;
    struct _RTQuestUnitItemData MissionItems[RUNTIME_MAX_QUEST_COUNTER_COUNT];
    struct _RTQuestUnitMobData MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];
    struct _RTWorldChunk Chunks[RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT];
    RTWorldTile Tiles[RUNTIME_WORLD_SIZE * RUNTIME_WORLD_SIZE];
    Timestamp DungeonStartTimestamp;
    Timestamp DungeonTimeout;
    Timestamp PauseTimestamp;
    Timestamp NextItemUpdateTimestamp;
    Int32 PatternPartIndex;
    Int32 TimerIndex;
    Int32 TimerItemCount;
    Timestamp TimerTimeout;
    Int32 NextMobEntityIndex;
    MemoryPoolRef MobPool;
    MemoryPoolRef MobPatternPool;
    MemoryPoolRef ItemPool;
    DictionaryRef EntityToMob;
    DictionaryRef EntityToMobPattern;
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

Void RTWorldSpawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity
);

Void RTWorldDespawnCharacter(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTEntityID Entity,
    Int32 Reason
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

Void RTWorldRespawnMobEvent(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    Timestamp Delay
);

Void RTWorldRespawnMob(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
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

Bool RTWorldTileIsImmune(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
);

Void RTWorldTileIncreaseImmunityCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
);

Void RTWorldTileDecreaseImmunityCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
);

Void RTWorldTileIncreaseCharacterCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
);

Void RTWorldTileDecreaseCharacterCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
);

Void RTWorldTileIncreaseMobCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
);

Void RTWorldTileDecreaseMobCount(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    Int32 X,
    Int32 Y
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
    Int64 MobSpeciesIndex
);

Void RTWorldSetMobTable(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    ArrayRef MobTable
);

typedef Void (*RTEntityVisitorByDistanceCallback)(
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

Void RTWorldContextEnumerateBroadcastChunks(
    RTWorldContextRef WorldContext,
    UInt8 EntityType,
    Int32 PreviousChunkX,
    Int32 PreviousChunkY,
    Int32 CurrentChunkX,
    Int32 CurrentChunkY,
    RTEntityVisitorCallback Callback,
    Void* Userdata
);

EXTERN_C_END
