#pragma once

#include "Base.h"
#include "Constants.h"
#include "Character.h"
#include "Dungeon.h"
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

union _RTRuntimeEnvironment {
    struct {
        UInt64 IsPKEnabled : 1;
        UInt64 _0 : 1;
        UInt64 IsPremiumEnabled : 1;
        UInt64 IsWarEnabled : 1;
        UInt64 _1 : 1;
        UInt64 IsRestrictedEnabled : 1;
        UInt64 _2 : 5;
        UInt64 IsEventEnabled : 1;
        UInt64 _3 : 2;
        UInt64 IsNoviceEnabled : 1;
        UInt64 _4 : 1;
        UInt64 IsOnly2FAEnabled : 1;
        UInt64 _5 : 2;
        UInt64 IsRaidBossEnabled : 1;
    };
    UInt64 RawValue;
};
typedef union _RTRuntimeEnvironment RTRuntimeEnvironment;

struct _RTRuntimeConfig {
    Bool IsSkillRankUpLimitEnabled;
    UInt64 WorldItemDespawnInterval;
    Int32 NewbieSupportTimeout;
    Int64 MaxHonorPoint;
    Int64 MinHonorPoint;
};

struct _RTRuntime {
    AllocatorRef Allocator;
    RTRuntimeEnvironment Environment;
    struct _RTRuntimeConfig Config;
    RTRuntimeDataContextRef Context;
    RTPartyManagerRef PartyManager;
    RTScriptManagerRef ScriptManager;
    RTWorldManagerRef WorldManager;
    RTNotificationManagerRef NotificationManager;

    Int32 SlopeFormulaDataCount;
    Int32 ItemDataCount;
    Int32 MobDataCount;
    Int32 NpcCount;
    Int32 QuestDataCount;
    Int32 QuestRewardItemSetDataCount;
    Int32 TrainerDataCount;
    Int32 WarpCount;

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
    struct _RTTrainerData TrainerData[RUNTIME_MEMORY_MAX_TRAINER_DATA_COUNT];
    struct _RTWarp Warps[RUNTIME_MEMORY_MAX_WARP_COUNT];
    Float32 MovementDistanceCache[RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH];
    MemoryPoolRef SkillDataPool;
    MemoryPoolRef ForceEffectFormulaPool;
    DictionaryRef DungeonData;
    DictionaryRef PatternPartData;
    Void* UserData;
};

RTRuntimeRef RTRuntimeCreate(
    AllocatorRef Allocator,
    Index MaxPartyCount,
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

RTItemDataRef RTRuntimeGetItemDataByIndex(
    RTRuntimeRef Runtime,
    UInt32 ItemIndex
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
    Int64 Penalty
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
    Index DungeonIndex
);

RTMissionDungeonPatternPartDataRef RTRuntimeGetPatternPartByID(
    RTRuntimeRef Runtime,
    Index PatternPartIndex
);

Void RTRuntimeBroadcastCharacterData(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 Type
);

EXTERN_C_END
