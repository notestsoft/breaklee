#pragma once

#include "Base.h"
#include "BattleSystem.h"
#include "Entity.h"
#include "Movement.h"

EXTERN_C_BEGIN

enum {
	RUNTIME_MOB_TRIGGER_TYPE_NONE,
	RUNTIME_MOB_TRIGGER_TYPE_TIME_ELAPSED,
	RUNTIME_MOB_TRIGGER_TYPE_HP_THRESHOLD,
	RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_DESPAWNED,
	RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_SPAWNED,
	RUNTIME_MOB_TRIGGER_TYPE_ACTION_EXECUTED,
	RUNTIME_MOB_TRIGGER_TYPE_DISTANCE_THRESHOLD_MAX,
	RUNTIME_MOB_TRIGGER_TYPE_DISTANCE_THRESHOLD_MIN,
	RUNTIME_MOB_TRIGGER_TYPE_MOB_DESPAWNED,
	RUNTIME_MOB_TRIGGER_TYPE_MOB_SPAWNED,
	RUNTIME_MOB_TRIGGER_TYPE_SKILL_RECEIVED,
	RUNTIME_MOB_TRIGGER_TYPE_TIME_IDLE,
	RUNTIME_MOB_TRIGGER_TYPE_UNKNOWN_12,
	RUNTIME_MOB_TRIGGER_TYPE_UNKNOWN_13,
};

enum {
	RUNTIME_MOB_ACTION_TYPE_NONE = 0,
	RUNTIME_MOB_ACTION_TYPE_WARP_TARGET = 1,
	RUNTIME_MOB_ACTION_TYPE_WARP_SELF = 2,
	RUNTIME_MOB_ACTION_TYPE_SPAWN_MOB = 3,
	RUNTIME_MOB_ACTION_TYPE_HEAL = 4,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_5 = 5,
	RUNTIME_MOB_ACTION_TYPE_EVASION_SELF = 6,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_7 = 7,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_8 = 8,
	RUNTIME_MOB_ACTION_TYPE_ANIMATION_STATE = 9,
	RUNTIME_MOB_ACTION_TYPE_ATTACK = 10,
	RUNTIME_MOB_ACTION_TYPE_CANCEL_ACTION = 11,
	RUNTIME_MOB_ACTION_TYPE_KILL_MOB = 12,
	RUNTIME_MOB_ACTION_TYPE_RESPAWN = 13,
	RUNTIME_MOB_ACTION_TYPE_ANIMATION = 14,
	RUNTIME_MOB_ACTION_TYPE_ATTACK_2 = 15,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_16 = 16,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_17 = 17,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_18 = 18,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_19 = 19,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_20 = 20,
};

enum {
	RUNTIME_MOB_ACTION_STATE_IDLE,
	RUNTIME_MOB_ACTION_STATE_PENDING,
	RUNTIME_MOB_ACTION_STATE_RUNNING,
};

struct _RTMobActionData {
	Int32 Index;
	Int32 Type;
	Int32 Parameters[RUNTIME_MOB_PATTERN_MAX_PARAMETER_COUNT]; // TODO: Replace with union
	Int32 Duration;
	Int32 TargetType;
	Int32 TargetParameters[RUNTIME_MOB_PATTERN_MAX_PARAMETER_COUNT]; // TODO: Replace with union
	Int32 Loop;
	Int32 LoopDelay;
	Int32 Priority;
};

struct _RTMobActionGroupData {
	Int32 Index;
	Int32 Delay;
	Int32 ActionCount;
	struct _RTMobActionData Actions[RUNTIME_MOB_PATTERN_MAX_ACTION_COUNT];
};

union _RTMobTriggerParameters {
	struct { Int32 Duration; } TimeElapsed;
	struct { Int32 HpPercent; } HpThreshold;
	struct { Int32 LinkMobIndex; Int32 Unknown1; } LinkMobKilled;
	struct { Int32 LinkMobIndex; } LinkMobSpawned;
	struct { Int32 ActionIndex; Int32 Count; } ActionExecuted;
	struct { Int32 Distance; Int32 Duration; } DistanceThreshold;
	struct { Int32 WorldIndex; Int32 DungeonIndex; Int32 EntityIndex; } MobKilled;
	struct { Int32 WorldIndex; Int32 DungeonIndex; Int32 EntityIndex; } MobSpawned;
	struct { Int32 SkillIndex; } SkillReceived;
	struct { Int32 Duration; } TimeIdle;
	struct { Int32 Values[RUNTIME_MOB_PATTERN_MAX_PARAMETER_COUNT]; } Memory;
};
typedef union _RTMobTriggerParameters RTMobTriggerParameters;

struct _RTMobTriggerData {
	Int32 Index;
	Int32 Type;
	RTMobTriggerParameters Parameters;
	Int32 ActionGroupCount;
	struct _RTMobActionGroupData ActionGroups[RUNTIME_MOB_PATTERN_MAX_ACTION_GROUP_COUNT];
};

struct _RTMobPatternData {
	Int32 Index;
	Int32 TriggerCount;
	struct _RTMobTriggerData Triggers[RUNTIME_MOB_PATTERN_MAX_TRIGGER_GROUP_COUNT];
};

struct _RTMobTriggerState {
	Bool IsRunning;
	Timestamp Timestamp;
};

struct _RTMobActionState {
	RTMobTriggerDataRef TriggerData;
	RTMobActionGroupDataRef ActionGroupData;
	RTMobActionDataRef ActionData;
	Bool IsRunning;
	Timestamp StartTimestamp;
	Timestamp EndTimestamp;
};

struct _RTMobPattern {
	RTMobPatternDataRef Data;
	Bool IsRunning;
	Bool IsIdle;
	UInt32 EventTriggerMask;
	Timestamp StartTimestamp;
	struct _RTMobTriggerState TriggerStates[RUNTIME_MOB_PATTERN_MAX_TRIGGER_GROUP_COUNT];
	ArrayRef ActionStates;
	// TODO: Add link mobs
};

Void RTMobPatternStart(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
);

Void RTMobPatternStop(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
);

Void RTMobPatternUpdate(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
);

Void RTMobPatternTimeElapsed(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
);

Void RTMobPatternHpChanged(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Int64 OldHp,
	Int64 NewHp
);

Void RTMobPatternActionExecuted(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Int32 ActionIndex
);

Void RTMobPatternDistanceChanged(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
);

Void RTMobPatternMobSpawned(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobRef SpawnedMob
);

Void RTMobPatternMobDespawned(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobRef DespawnedMob
);

Void RTMobPatternSkillReceived(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Int32 SkillIndex
);

Void RTMobPatternStateChanged(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Bool IsIdle
);

EXTERN_C_END
