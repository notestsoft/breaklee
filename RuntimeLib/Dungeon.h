#pragma once

#include "Base.h"
#include "Constants.h"
#include "Drop.h"
#include "Item.h"
#include "Mob.h"

EXTERN_C_BEGIN

enum {
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_SPAWN			 = 1,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_KILL			 = 2,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_REVIVE			 = 3,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_DELETE			 = 4,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_CHANGE			 = 5,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_EVENT_CALL		 = 6,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_SOFT_DELETE		 = 7,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_UNKNOWN_2	     = 8,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_KILL_BY_LINK_MOB = 9,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_UNKNOWN_3	     = 10,
	RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_UNKNOWN_4	     = 11,
};

enum {
	RUNTIME_DUNGEON_TIME_CONTROL_TYPE_SPAWN			= 1,
	RUNTIME_DUNGEON_TIME_CONTROL_TYPE_DESPAWN		= 2,
};

enum {
	RUNTIME_DUNGEON_TIMER_TARGET_EVENT_TYPE_SPAWN	= 1,
	RUNTIME_DUNGEON_TIMER_TARGET_EVENT_TYPE_DESPAWN = 2,
};

enum {
	RUNTIME_DUNGEON_IMMUNE_ACTIVATION_TYPE_DEAD = 0,
	RUNTIME_DUNGEON_IMMUNE_ACTIVATION_TYPE_ALIVE = 1,
};

struct _RTDungeonTriggerData {
	Int32 Type;
	Int32 LiveMobCount;
	Int32 LiveMobIndexList[RUNTIME_DUNGEON_MAX_TRIGGER_MOB_COUNT];
	Int32 DeadMobCount;
	Int32 DeadMobIndexList[RUNTIME_DUNGEON_MAX_TRIGGER_MOB_COUNT];
	Int32 NpcIndex;
	Int32 ActionGroupIndex;
};

struct _RTDungeonTriggerActionData {
	Timestamp Delay;
	Int32 TargetMobIndex;
	Int32 TargetAction;
	Int32 TargetSpawnInterval;
	Int32 TargetSpawnCount;
};

struct _RTDungeonTimeControlData {
	Int32 Event;
	Int32 Value;
};

struct _RTTimerData {
	Int32 Type;
	Int32 TargetMobIndex;
	Int32 TargetEvent;
	Int32 Interval;
};

struct _RTDungeonTimerData {
	Bool Active;
	UInt32 ItemID;
	Int32 MobIndexCount;
	Int32 MobIndexList[RUNTIME_DUNGEON_MAX_TIMER_MOB_COUNT];
	Int32 TimerCount;
	struct _RTTimerData Timers[RUNTIME_DUNGEON_MAX_TIMER_COUNT];
};

struct _RTImmuneData {
	Int32 TargetIndex;
	Int32 ActivationType;
	Int32 CanAttack;
	Int32 CanSelect;
};

struct _RTDungeonImmuneControlData {
	Int32 ImmuneCount;
	struct _RTImmuneData ImmuneList[RUNTIME_DUNGEON_MAX_IMMUNE_CONTROL_COUNT];
};

struct _RTDungeonGateControlData {
	Int32 Rotation;
	RTPosition Position;
	Int32 CanAttack;
	Int32 CanSelect;
	Int32 CanDebuff;
	Int32 CellCount;
	RTPosition CellList[RUNTIME_DUNGEON_MAX_GATE_CELL_COUNT];
};

struct _RTDungeonData {
	Int32 DungeonType;
	Int32 CreateType;
	Int32 EntryType;
	Int32 PlayerType;
	Int32 MaxPlayerCount;
	Int32 ObjectCount;
	RTItem EntryItemID;
	UInt32 EntryItemOption;
	Int32 RemoveItem;
	Int32 EntryConditionLevel;
	Int32 EntryConditionClass[2];
	Int32 EntryWarpID;
	Int32 FailWarpNpcID;
	Int32 DeadWarpID;
	Int32 DungeonIndex;
	Int32 PenaltyValue;
	Int32 PatternPartCount;
	Int32 PatternPartIndices[RUNTIME_DUNGEON_MAX_PATTERN_PART_COUNT];
	Int32 Reward[8];
	Int32 SuccessWarpNpcID;
	Int32 WorldIndex;
	Int32 NextDungeonIndex;
	Int32 WarpNpcSetID;
	Int32 UseOddCircleCount;
	Int32 UseVeradrixCount;
	Int32 IsElite;
	Int32 EliteDungeonBoost;
	Int32 MissionTimeout;
	Int32 StartKillMobCount;
	Int32 StartKillMobList[RUNTIME_DUNGEON_MAX_START_KILL_MOB_COUNT];
	DictionaryRef TriggerGroups;
	DictionaryRef ActionGroups;
	DictionaryRef TimeControls;
	DictionaryRef ImmuneControls;
	DictionaryRef GateControls;
	struct _RTDungeonTimerData TimerData;
	struct _RTDropTable DropTable;
};

struct _RTQuestUnitItemData {
	RTItem Item;
	Int32 Count;
	Int32 MaxCount;
};

struct _RTQuestUnitMobData {
	Int32 MobID;
	Int32 Count;
	Int32 MaxCount;
};

struct _RTQuestDungeonPatternPartData {
	Int32 ID;
	Int32 Type;
	Int32 CreateMethod;
	Int32 MissionNpcID;
	Int32 Mesh[2];
	Int32 MissionGate[2];
	Int32 MissionItemCount;
	Int32 MissionMobCount;
	struct _RTQuestUnitItemData MissionItems[RUNTIME_MAX_QUEST_COUNTER_COUNT];
	struct _RTQuestUnitMobData MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];
};

struct _RTMissionDungeonPatternPartData {
	Int32 MissionNpcIndex;
	Int32 MissionMobCount;
	struct _RTQuestUnitMobData MissionMobs[RUNTIME_MAX_QUEST_COUNTER_COUNT];
	ArrayRef MobTable;
};

Bool RTDungeonStart(
	RTWorldContextRef World
);

Bool RTDungeonUpdate(
	RTWorldContextRef World
);

Bool RTDungeonPause(
	RTWorldContextRef World
);

Bool RTDungeonResume(
	RTWorldContextRef World
);

Bool RTDungeonEnd(
	RTWorldContextRef World
);

Bool RTDungeonFail(
	RTWorldContextRef World
);

Bool RTDungeonStartNextPatternPart(
	RTWorldContextRef World
);

Bool RTDungeonTriggerEvent(
	RTWorldContextRef World,
	RTEntityID TriggerSource,
	Int TriggerIndex
);

Void RTDungeonAddTime(
	RTWorldContextRef WorldContext,
	Int32 Value
);

Void RTDungeonUpdateTimer(
	RTWorldContextRef WorldContext,
	Int32 TargetEvent,
	Int32 TargetMobIndex
);

Void RTDungeonUpdateTimerMobHP(
	RTWorldContextRef WorldContext,
	RTMobRef Mob
);

Void RTDungeonUpdateTimerItemCount(
	RTWorldContextRef WorldContext,
	RTItem ItemID,
	UInt64 ItemOptions
);

EXTERN_C_END
