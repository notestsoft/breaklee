#pragma once

#include "Base.h"
#include "Constants.h"
#include "Character.h"
#include "Dungeon.h"
#include "Event.h"
#include "Mob.h"
#include "Npc.h"
#include "Party.h"
#include "Quest.h"
#include "Shop.h"
#include "Trainer.h"
#include "Upgrade.h"
#include "Warp.h"
#include "World.h"
#include "Level.h"

EXTERN_C_BEGIN

struct _RTRuntimeConfig {
    UInt64 ExpMultiplier;
    UInt64 SkillExpMultiplier;
};

struct _RTRuntime {
    AllocatorRef Allocator;
    struct _RTRuntimeConfig Config;
    RTRuntimeDataContextRef Context;
    RTWorldManagerRef WorldManager;

    Int32 EntityCount;
    Int32 PartyCount;

    RTEntityID Entities[RUNTIME_MEMORY_MAX_ENTITY_COUNT];
    Int32 EntityToCharacter[RUNTIME_MEMORY_MAX_CHARACTER_COUNT];

    struct _RTParty Parties[RUNTIME_MEMORY_MAX_PARTY_COUNT];

    Int32 SlopeFormulaDataCount;
    Int32 ItemDataCount;
    Int32 MobDataCount;
    Int32 NpcCount;
    Int32 QuestDataCount;
    Int32 QuestRewardItemSetDataCount;
    Int32 ShopDataCount;
    Int32 TrainerDataCount;
    Int32 WarpCount;
    Int32 WorldCount;
    Int32 DungeonDataCount;
    Int32 DungeonMobSpawnDataCount;
    Int32 MissionDungeonPatternPartDataCount;

    struct _RTBattleStyleLevelFormulaData BattleStyleLevelFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTBattleStyleClassFormulaData BattleStyleClassFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTBattleStyleSlopeFormulaData BattleStyleSlopeFormulaData[RUNTIME_MEMORY_MAX_SLOPE_FORMULA_COUNT];
    struct _RTBattleStyleStatsFormulaData BattleStyleStatsFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTBattleStyleSkillFormulaData BattleStyleSkillFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTBattleStyleRankFormulaData BattleStyleRankFormulaData[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
    struct _RTItemData ItemData[RUNTIME_MEMORY_MAX_ITEM_DATA_COUNT];
    struct _RTMobSpeciesData MobData[RUNTIME_MEMORY_MAX_MOB_DATA_COUNT];
    struct _RTNpc Npcs[RUNTIME_MEMORY_MAX_NPC_COUNT];
    struct _RTQuestData QuestData[RUNTIME_MEMORY_MAX_QUEST_DATA_COUNT];
    struct _RTQuestRewardItemSetData QuestRewardItemSetData[RUNTIME_MEMORY_MAX_QUEST_REWARD_ITEM_SET_DATA_COUNT];
    struct _RTShopData ShopData[RUNTIME_MEMORY_MAX_SHOP_DATA_COUNT];
    struct _RTTrainerData TrainerData[RUNTIME_MEMORY_MAX_TRAINER_DATA_COUNT];
    struct _RTWarp Warps[RUNTIME_MEMORY_MAX_WARP_COUNT];
    struct _RTDungeonData DungeonData[RUNTIME_MEMORY_MAX_DUNGEON_COUNT];
    struct _RTMissionDungeonPatternPartData MissionDungeonPatternPartData[RUNTIME_MEMORY_MAX_DUNGEON_PATTERN_PART_COUNT];
    Float32 MovementDistanceCache[RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH];

    MemoryPoolRef SkillDataPool;

    struct _RTEvent Event;
    RTEventCallback Callback;
    Void* UserData;
};

RTRuntimeRef RTRuntimeCreate(
    AllocatorRef Allocator,
    RTEventCallback Callback,
    Void* UserData
);

Void RTRuntimeDestroy(
    RTRuntimeRef Runtime
);

Bool RTRuntimeLoadData(
    RTRuntimeRef Runtime,
    CString RuntimeDataPath,
    CString ServerDataPath
);

Void RTRuntimeUpdate(
    RTRuntimeRef Runtime
);

Void RTRuntimeBroadcastEvent(
    RTRuntimeRef Runtime,
    Int32 EventType,
    RTWorldContextRef World,
    RTEntityID SourceID,
    RTEntityID TargetID,
    Int32 X,
    Int32 Y
);

Void RTRuntimeBroadcastEventData(
    RTRuntimeRef Runtime,
    Int32 EventType,
    RTWorldContextRef World,
    RTEntityID SourceID,
    RTEntityID TargetID,
    Int32 X,
    Int32 Y,
    RTEventData Data
);

RTEntityID RTRuntimeCreateEntity(
    RTRuntimeRef Runtime,
    UInt8 WorldIndex,
    UInt8 EntityType
);

Void RTRuntimeDeleteEntity(
    RTRuntimeRef Runtime,
    RTEntityID Entity
);

RTPartyRef RTRuntimeCreateParty(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PartyType
);

RTWorldContextRef RTRuntimeGetWorldByID(
    RTRuntimeRef Runtime,
    Int32 WorldID
);

RTWorldContextRef RTRuntimeGetWorldByParty(
    RTRuntimeRef Runtime,
    RTEntityID PartyID
);

RTWorldContextRef RTRuntimeGetWorldByCharacter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

RTNpcRef RTRuntimeGetNpcByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
);

RTWarpRef RTRuntimeGetWarpByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID,
    Int32 WarpIndex
);

RTWarpRef RTRuntimeGetWarpByIndex(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 WarpIndex
);

RTPartyRef RTRuntimeGetParty(
    RTRuntimeRef Runtime,
    RTEntityID Entity
);

RTWorldItemRef RTRuntimeGetItem(
    RTRuntimeRef Runtime,
    RTEntityID Entity
);

RTWorldItemRef RTRuntimeGetItemByIndex(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 ItemID
);

RTItemDataRef RTRuntimeGetItemDataByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemIndex
);

RTQuestDataRef RTRuntimeGetQuestByIndex(
    RTRuntimeRef Runtime,
    Int32 QuestIndex
);

RTQuestRewardItemSetDataRef RTRuntimeGetQuestRewardItemSetByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemSetIndex
);

RTQuestRewardItemDataRef RTRuntimeGetQuestRewardItemByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemSetIndex,
    Int32 ItemIndex,
    Int32 BattleStyleIndex
);

RTShopDataRef RTRuntimeGetShopByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
);

RTShopDataRef RTRuntimeGetShopByIndex(
    RTRuntimeRef Runtime,
    Int32 ShopIndex
);

RTTrainerDataRef RTRuntimeGetTrainerByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
);

RTCharacterSkillDataRef RTRuntimeGetCharacterSkillDataByID(
    RTRuntimeRef Runtime,
    Int32 SkillID
);

RTSkillLevelDataRef RTRuntimeGetSkillLevelDataByID(
    RTRuntimeRef Runtime,
    Int32 SkillID,
    Int32 SkillLevel
);

RTBattleStyleLevelFormulaDataRef RTRuntimeGetBattleStyleLevelFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
);

RTBattleStyleClassFormulaDataRef RTRuntimeGetBattleStyleClassFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
);

RTBattleStyleSlopeDataRef RTRuntimeGetBattleStyleSlopeData(
    RTRuntimeRef Runtime,
    Int32 SlopeID,
    Int32 Penalty
);

RTBattleStyleSlopeFormulaDataRef RTRuntimeGetBattleStyleSlopeFormulaData(
    RTRuntimeRef Runtime,
    Int32 SlopeID
);

RTBattleStyleStatsFormulaDataRef RTRuntimeGetBattleStyleStatsFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
);

RTBattleStyleSkillRankDataRef RTRuntimeGetBattleStyleSkillRankData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex,
    Int32 SkillRank
);

RTBattleStyleRankDataRef RTRuntimeGetBattleStyleRankData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex,
    Int32 Level
);

RTWorldContextRef RTRuntimeOpenDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Index WorldIndex,
    Index DungeonIndex
);

Void RTRuntimeCloseDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

RTDungeonDataRef RTRuntimeGetDungeonDataByID(
    RTRuntimeRef Runtime,
    Index DungeonID
);

RTMissionDungeonPatternPartDataRef RTRuntimeGetPatternPartByID(
    RTRuntimeRef Runtime,
    Int32 PatternPartID
);

Void RTRuntimeBroadcastCharacterData(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 Type
);

EXTERN_C_END
