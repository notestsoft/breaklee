#pragma once

#include "Base.h"
#include "Config.h"
#include "ControlUI.h"
#include "MCLArchive.h"

EXTERN_C_BEGIN

enum {
    EDITOR_STATE_INIT,
};

struct _EditorState {
    Int State;
    union {
        struct { UInt8 Padding; } Init;
    };
};
typedef struct _EditorState EditorState;

struct _MissionDungeonData {
    Int32 RowIndex;
    Int32 QDungeonIdx;
    Int32 InstanceLimit;
    Int32 Level;
    Int32 MaxUser;
    Int32 MissionTimeout;
    Int32 OpenItem[2];
    Int32 PPLink;
    Int32 Penalty;
    Int32 Reward[8];
    Int32 WarpIdx;
    Int32 WarpIdxForSucess;
    Int32 WarpIdxForFail;
    Int32 WarpIndexForDead;
    Int32 WorldIdx;
    Int32 bAddRange;
    Int32 NextQDIdxForSuccess;
    Int32 WarpNPC_Set;
    Int32 UseTerrain;
    Int32 BattleStyle[2];
    Int32 UseOddCircle_Count;
    Int32 Party_Type;
    Int32 RemoveItem;
    Int32 DBWrite;
    Int32 DungeonType;
    Int32 UseVeradrix_Count;
    Int32 Cool_Time;
    Int32 Reentry;
};
typedef struct _MissionDungeonData* MissionDungeonDataRef;

struct _PatternPartData {
    Int32 RowIndex;
    Int32 PPIdx;
    Int32 MissionMobs[3];
    Int32 MissionNPC;
};
typedef struct _PatternPartData* PatternPartDataRef;

struct _TimeControlData {
    Int32 RowIndex;
    Int32 Dungeon_Index;
    Int32 MMAP_Index;
    Int32 Event;
    Int32 Time_Value;
};
typedef struct _TimeControlData* TimeControlDataRef;

struct _ArenaDefenceData {
    Int32 RowIndex;
    Int32 Dungeon_ID;
    Int32 MobIdx;
    Int32 Goal_X;
    Int32 Goal_Y;
    Int32 SpeedType;
};
typedef struct _ArenaDefenceData* ArenaDefenceDataRef;

struct _DungeonPointRewardData {
    Int32 RowIndex;
    Int32 Dungeon_Index;
    Int32 DP;
};
typedef struct _DungeonPointRewardData* DungeonPointRewardDataRef;

struct _MobMapData {
    Int32 RowIndex;
    Int32 MobIdx;
    Int32 PPIdx;
    Int32 SpeciesIdx;
    Int32 PosX;
    Int32 PosY;
    Int32 Width;
    Int32 Height;
    Int32 SpwnInterval;
    Int32 SpwnCount;
    Int32 SpawnDefault;
    Int32 EvtProperty;
    Int32 EvtMobs;
    Int32 EvtInterval;
    Int32 Grade;
    Int32 Lv;
    Int32 MissionGate;
    Int32 PerfectDrop;
    Int32 TrgIdxSpawn;
    Int32 TrgIdxKill;
    Int32 Type;
    Int32 Min;
    Int32 Max;
    Int32 Authority;
    Int32 Server_Mob;
    Int32 Loot_Delay;
    Int32 Enemies;
};
typedef struct _MobMapData* MobMapDataRef;

struct _TriggerData {
    Int32 RowIndex;
    Int32 QDungeonIdx;
    Int32 TrgIdx;
    Int32 Order;
    Int32 TrgType;
    Int32 LiveStateMMapIdx;
    Int32 DeadStateMMapIdx;
    Int32 TrgNpcIdx;
    Int32 EvtActGroupIdx;
};
typedef struct _TriggerData* TriggerDataRef;

struct _ActionGroupData {
    Int32 RowIndex;
    Int32 QDungeonIdx;
    Int32 EvtActGroupIdx;
    Int32 Order;
    Int32 TgtMMapIdx;
    Int32 TgtAction;
    Int32 EvtDelay;
    Int32 TgtSpawnInterval;
    Int32 TgtSpawnCount;
};
typedef struct _ActionGroupData* ActionGroupDataRef;

struct _MissionMobMapData {
    Int32 RowIndex;
    Int32 QDungeonIdx;
    Int32 MMapidx;
    Int32 Cells;
};
typedef struct _MissionMobMapData* MissionMobMapDataRef;

struct _ClearConditionData {
    Int32 RowIndex;
    Int32 QDungeonIdx;
    Int32 MobIdx;
};
typedef struct _ClearConditionData* ClearConditionDataRef;

struct _StartKillData {
    Int32 RowIndex;
    Int32 QDungeonIdx;
    Int32 MobIdx;
};
typedef struct _StartKillData* StartKillDataRef;

struct _MissionDungeonTimerData {
    Int32 RowIndex;
    Int32 Dungeon_Index;
    Int32 MMAP_Event;
    Int32 MMAP_Index;
    Int32 Condition;
    Int32 Time_Value;
};
typedef struct _MissionDungeonTimerData* MissionDungeonTimerDataRef;

struct _MissionDungeonFlowCheckData {
    Int32 RowIndex;
    Int32 Dungeon_Index;
    Int32 Dead_MMAP;
    Int32 Kill_MMAP;
};
typedef struct _MissionDungeonFlowCheckData* MissionDungeonFlowCheckDataRef;

struct _MissionDungeonTimerValueData {
    Int32 RowIndex;
    Int32 Dungeon_Index;
    Int32 Item_Index;
    Int32 Mmap_Index;
};
typedef struct _MissionDungeonTimerValueData* MissionDungeonTimerValueDataRef;

struct _ClientImmuneData {
    Int32 RowIndex;
    Int32 QDungeonIdx;
    Int32 MobIdx;
    Int32 ImmuneMobIdx;
};
typedef struct _ClientImmuneData* ClientImmuneDataRef;

struct _EditorContext {
    AllocatorRef Allocator;
    EditorConfig Config;
    EditorState State;
    Camera Camera;
    struct _ControlUIState ControlState;

    Int32 ArchiveItemType;
    Void* Archive;

    DictionaryRef MissionDungeonNames;
    DictionaryRef MissionDungeonFiles;

    ArrayRef MissionDungeons;
    ArrayRef PatternParts;
    ArrayRef TimeControls;
    ArrayRef ArenaDefences;
    ArrayRef DungeonPointRewards;
    ArrayRef MobMaps;
    ArrayRef Triggers;
    ArrayRef ActionGroups;
    ArrayRef MissionMobMaps;
    ArrayRef ClearConditions;
    ArrayRef StartKills;
    ArrayRef MissionDungeonTimers;
    ArrayRef MissionDungeonFlowChecks;
    ArrayRef MissionDungeonTimerValues;
    ArrayRef ClientImmunes;
};

EXTERN_C_END
