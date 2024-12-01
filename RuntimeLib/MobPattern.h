#pragma once

#include "Base.h"
#include "BattleSystem.h"
#include "Entity.h"
#include "Movement.h"

EXTERN_C_BEGIN

enum {
	RUNTIME_MOB_TRIGGER_TYPE_NONE,
	RUNTIME_MOB_TRIGGER_TYPE_START_TIME,
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
	RUNTIME_MOB_TRIGGER_TYPE_SPAWN_TIME,
	RUNTIME_MOB_TRIGGER_TYPE_INSTANCE_TIME,
};

enum {
	RUNTIME_MOB_ACTION_TYPE_NONE = 0,
	RUNTIME_MOB_ACTION_TYPE_WARP_TARGET = 1,
	RUNTIME_MOB_ACTION_TYPE_WARP_SELF = 2,
	RUNTIME_MOB_ACTION_TYPE_SPAWN_MOB = 3,
	RUNTIME_MOB_ACTION_TYPE_HEAL_TARGET = 4,
	RUNTIME_MOB_ACTION_TYPE_TIMEOUT = 5,
	RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_1 = 6,
	RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_2 = 7,
	RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_3 = 8,
	RUNTIME_MOB_ACTION_TYPE_SPECIAL_ACTION = 9,
	RUNTIME_MOB_ACTION_TYPE_ATTACK_1 = 10,
	RUNTIME_MOB_ACTION_TYPE_CANCEL_ACTION = 11,
	RUNTIME_MOB_ACTION_TYPE_DESPAWN_LINK_MOB = 12,
	RUNTIME_MOB_ACTION_TYPE_RESPAWN_SELF = 13,
	RUNTIME_MOB_ACTION_TYPE_SOCIAL_ACTION = 14,
	RUNTIME_MOB_ACTION_TYPE_ATTACK_2 = 15,
	RUNTIME_MOB_ACTION_TYPE_ATTACK_UP_TARGET = 16,
	RUNTIME_MOB_ACTION_TYPE_DEFENSE_UP_TARGET = 17,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_18 = 18,
	RUNTIME_MOB_ACTION_TYPE_DESPAWN_MOB = 19,
	RUNTIME_MOB_ACTION_TYPE_UNKNOWN_20 = 20,
};

enum {
	RUNTIME_MOB_ACTION_STATE_IDLE,
	RUNTIME_MOB_ACTION_STATE_PENDING,
	RUNTIME_MOB_ACTION_STATE_RUNNING,
};

enum {
	RUNTIME_MOB_PATTERN_VALUE_TYPE_DECIMAL = 1,
	RUNTIME_MOB_PATTERN_VALUE_TYPE_PERCENT = 2,
};

union _RTMobActionParameters {
	struct { Int32 PositionX; Int32 PositionY; } WarpTarget;
	struct { Int32 PositionX; Int32 PositionY; } WarpSelf;
	struct { Int32 LinkMobIndex; Int32 Unknown1; Int32 Unknown2; } LinkMobSpawn;
	struct { Int32 WorldType; Int32 MobIndex; Int32 ValueType; Int32 Value; } Heal;
	struct { Int32 Unknown1; } EvasionSelf;
	struct { Int32 ActionIndex; Int32 Unknown1; } SpecialAction;
	struct { Int32 AttackIndex; Int32 AttackRate; Int32 PhysicalAttackMin; Int32 PhysicalAttackMax; } Attack;
	struct { Int32 ActionIndex; Int32 ActionFinished; } CancelAction;
	struct { Int32 LinkMobIndex; } LinkMobDespawn;
	struct { Int32 ActionIndex; } SocialAction;
	struct { Int32 WorldType; Int32 MobIndex; Int32 ValueType; Int32 Value; } AttackUp;
	struct { Int32 WorldType; Int32 MobIndex; Int32 ValueType; Int32 Value; } DefenseUp;
	struct { Int32 MobIndex; } MobDespawn;
	struct { Int32 Values[RUNTIME_MOB_PATTERN_MAX_PARAMETER_COUNT]; } Memory;
};
typedef union _RTMobActionParameters RTMobActionParameters;

struct _RTMobActionData {
	Int32 Index;
	Int32 Type;
	RTMobActionParameters Parameters;
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
	struct { Int32 ActionIndex; Int32 ActionFinished; } ActionExecuted;
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

struct _RTMobPatternSpawnData {
	Int32 Index;
	Int32 MobSpeciesIndex;
	Int32 AreaX;
	Int32 AreaY;
	Int32 AreaWidth;
	Int32 AreaHeight;
	Int32 Interval;
	Int32 Count;
	Int32 MobPatternIndex;
	RTScriptRef Script;
};

struct _RTMobPatternData {
	Int32 Index;
	Int32 TriggerCount;
	struct _RTMobTriggerData Triggers[RUNTIME_MOB_PATTERN_MAX_TRIGGER_GROUP_COUNT];
	ArrayRef MobPool;
};

struct _RTMobTriggerState {
	Bool IsRunning;
	Timestamp Timestamp;
};

struct _RTMobActionState {
	RTMobTriggerDataRef TriggerData;
	RTMobTriggerStateRef TriggerState;
	RTMobActionGroupDataRef ActionGroupData;
	RTMobActionDataRef ActionData;
	Bool IsRunning;
	Timestamp StartTimestamp;
	Timestamp EndTimestamp;
};

struct _RTMobPattern {
	RTMobPatternDataRef Data;
	UInt32 EventTriggerMask;
	Timestamp StartTimestamp;
	Timestamp SpawnTimestamp;
	struct _RTMobTriggerState TriggerStates[RUNTIME_MOB_PATTERN_MAX_TRIGGER_GROUP_COUNT];
	ArrayRef ActionStates;
	ArrayRef LinkMobs;
};

Void RTMobPatternSpawn(
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

Void RTMobPatternActionStarted(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Int32 ActionIndex
);

Void RTMobPatternActionFinished(
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
