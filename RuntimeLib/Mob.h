#pragma once

#include "Base.h"
#include "BattleSystem.h"
#include "Entity.h"
#include "Movement.h"

EXTERN_C_BEGIN

#define MOB_EVENT_SPAWN		"on_spawn"
#define MOB_EVENT_DESPAWN	"on_despawn"
#define MOB_EVENT_UPDATE	"on_update"
#define MOB_EVENT_DAMAGE	"on_damage"

enum {
	RUNTIME_MOB_AGGRESSIVE_TYPE_PASSIVE,
	RUNTIME_MOB_AGGRESSIVE_TYPE_AGGRESSIVE,
	RUNTIME_MOB_AGGRESSIVE_TYPE_NATIONONLY,
};

enum {
	RUNTIME_MOB_ATTACK_TYPE_CONCENTRATE,
	RUNTIME_MOB_ATTACK_TYPE_SELECT,
	RUNTIME_MOB_ATTACK_TYPE_REVENGE,
	RUNTIME_MOB_ATTACK_TYPE_NONE
};

enum {
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_NONE = 0,
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_HP_TRIGGER = 1,
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_SITUATIONAL = 2,
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_BY_REACH = 3,
	RUNTIME_MOB_ATTACK_PATTERN_TYPE_SELECTIVE = 4,
};

enum {
	RUNTIME_MOB_TRIGGER_TYPE_NONE = 0,
	RUNTIME_MOB_TRIGGER_TYPE_PATTERN_TIME = 1,
	RUNTIME_MOB_TRIGGER_TYPE_HP_THRESHOLD = 2,
	RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_KILLED = 3,
	RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_SPAWNED = 4,
	RUNTIME_MOB_TRIGGER_TYPE_ACTION_EXECUTED = 5,
	RUNTIME_MOB_TRIGGER_TYPE_DISTANCE_MAX = 6,
	RUNTIME_MOB_TRIGGER_TYPE_DISTANCE_MIN = 7,
	RUNTIME_MOB_TRIGGER_TYPE_MOB_KILLED = 8,
	RUNTIME_MOB_TRIGGER_TYPE_UNKNOWN_9 = 9,
	RUNTIME_MOB_TRIGGER_TYPE_SKILL_RECEIVED = 10,
	RUNTIME_MOB_TRIGGER_TYPE_IDLE_TIME = 11,
	RUNTIME_MOB_TRIGGER_TYPE_UNKNOWN_12 = 12,
	RUNTIME_MOB_TRIGGER_TYPE_UNKNOWN_13 = 13,
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
	RUNTIME_MOB_PHASE_NONE,
	RUNTIME_MOB_PHASE_MOVE,
	RUNTIME_MOB_PHASE_FIND_PASSIVE,
	RUNTIME_MOB_PHASE_FIND_AGGRESSIVE,
	RUNTIME_MOB_PHASE_FIND_INFINITE,
	RUNTIME_MOB_PHASE_CHASE_CONCENTRATE,
	RUNTIME_MOB_PHASE_CHASE_SELECT,
	RUNTIME_MOB_PHASE_CHASE_REVENGE,
	RUNTIME_MOB_PHASE_ATTACK_CONCENTRATE,
	RUNTIME_MOB_PHASE_ATTACK_SELECT,
	RUNTIME_MOB_PHASE_ATTACK_REVENGE,

	RUNTIME_MOB_PHASE_DEAD,
	RUNTIME_MOB_PHASE_STUN,
	RUNTIME_MOB_PHASE_CHASE_DUNGEON,
	RUNTIME_MOB_PHASE_REQUEST_HELP,
	RUNTIME_MOB_PHASE_ESCAPE,

	RUNTIME_MOB_PHASE_SPAWN,
	RUNTIME_MOB_PHASE_KILL,

	RUNTIME_MOB_PHASE_COUNT
};

enum {
	RUNTIME_MOB_SUBPHASE_NEUTRAL,
	RUNTIME_MOB_SUBPHASE_AGGRESSIVE,

	RUNTIME_MOB_SUBPHASE_COUNT
};

enum {
	RUNTIME_MOB_EVENT_NONE = 0,
	RUNTIME_MOB_EVENT_SPAWN = 1,
	RUNTIME_MOB_EVENT_KILL = 2,
	RUNTIME_MOB_EVENT_DELETE = 3,
	RUNTIME_MOB_EVENT_REVIVE = 4,
};

struct _RTMobSkillData {
	Int32 PhysicalAttackMin;
	Int32 PhysicalAttackMax;
	Int32 Reach;
	Int32 Range;
	Int32 Stance;
	Int32 SkillGroup;
	Timestamp Interval;

	Int32 Distance;
	Int32 Offset;
};

// TODO: Add additional missing fields like resists...
struct _RTMobSpeciesData {
	Int32 MobSpeciesIndex;
	Int32 Level;
	Int64 HP;
	Int32 HPRecharge;
	Int32 AttackRate;
	Int32 DefenseRate;
	Int32 Defense;
	Int32 DamageReduction;
	Int32 Accuracy;
	Int32 Penetration;
	Int32 ResistCriticalRate;
	Int32 IgnoreAccuracy;
	Int32 IgnoreDamageReduction;
	Int32 IgnorePenetration;
	Int32 AbsoluteDamage;
	Int32 ResistSkillAmp;
	Int32 ResistCriticalDamage;
	Int32 ResistSuppression;
	Int32 ResistSilence;
	Int32 ReflectDamage;
	Float32 MoveSpeed;
	Float32 ChaseSpeed;
	Float32 Radius;
	Int32 Property;
	Int32 AttackPattern;
	Int32 AggressiveType;
	Int32 Cooperate;
	Int32 Escape;
	Int32 AttackType;
	Int32 Scale;
	Int32 FindCount;
	Int32 FindInterval;
	Int32 MoveInterval;
	Int32 ChaseInterval;
	Int32 AlertRange;
	Int32 ChaseRange;
	Int32 LimitRangeB;
	struct _RTMobSkillData DefaultSkill;
	struct _RTMobSkillData SpecialSkill;
	Int32 AttackSignal;
	Int32 IsWorldBoss;
	Int32 CanAttack;
	Int32 CanMove;
	UInt64 Exp;
};

struct _RTMobPhaseData {
	Int32 Move;
	Int32 Find;
	Int32 Chase;
	Int32 Attack;
	Int32 Dead;
	Int32 Stun;
};

struct _RTMobSpawnData {
	Index PatternPartIndex;
	Index MobSpeciesIndex;
	Int32 AreaX;
	Int32 AreaY;
	Int32 AreaWidth;
	Int32 AreaHeight;
	Int32 SpawnInterval;
	Int32 SpawnCount;
	Int32 SpawnDefault;
	Int32 Level;
	Int32 IsMissionGate;
	Int32 PerfectDrop;
	Int32 SpawnTriggerID;
	Int32 KillTriggerID;
	Int32 ServerMobIndex;
	Int32 LootDelay;
	Int32 TileCount;
	UInt32 TileAttributes;
	RTPosition Tiles[RUNTIME_MOB_MAX_TILE_COUNT];
	Int32 EventProperty[RUNTIME_MOB_MAX_EVENT_COUNT];
	Int32 EventMobs[RUNTIME_MOB_MAX_EVENT_COUNT];
	Int32 EventInterval[RUNTIME_MOB_MAX_EVENT_COUNT];
	Char Script[MAX_PATH];
};

struct _RTMobAggroData {
	Int32 Count;
	RTEntityID Entities[RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT];
	Int64 ReceivedDamage[RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT];
};

struct _RTMob {
	RTEntityID ID;
	RTMobSpeciesDataRef SpeciesData;
	RTEntityID DropOwner;
	struct _RTMobPhaseData PhaseData;
	Int32 Phase;
	Int32 SubPhase;
	Bool IsSpawned;
	Bool IsKilled;
	Bool IsDead;
	Bool IsInfiniteSpawn;
	Bool IsPermanentDeath;
	Bool IsChasing;
	Bool IsWall;
	Bool IsAttacked;
	Int32 RemainingFindCount;
	Int32 RemainingSpawnCount;
	Timestamp SpawnTimestamp;
	Timestamp DespawnTimestamp;
	Timestamp EscapeTimestamp;
	Timestamp PhaseTimeout;
	Timestamp NextTimestamp;
	// TODO: Refactor this soon we do it currently for debugging but there is a bit more to the event handling than that
	Timestamp EventSpawnTimestamp;
	Timestamp EventDespawnTimestamp;

	RTMobSkillDataRef ActiveSkill;
	RTDataMobAttackDataRef SpecialAttack;
	RTDataMobSkillRef SpecialAttackSkill;

	Timestamp NextPhaseTimestamp;
	Int64 HPTriggerThreshold;

	struct _RTMobSpawnData Spawn;
	struct _RTMobAggroData Aggro;
    struct _RTMovement Movement;
	struct _RTBattleAttributes Attributes;

	RTScriptRef Script;
};

Void RTMobInit(
	RTRuntimeRef Runtime,
	RTMobRef Mob
);

Void RTMobInitFromSpeciesData(
	RTRuntimeRef Runtime,
	RTMobRef Mob,
	RTMobSpeciesDataRef MobSpeciesData
);

Void RTMobApplyDamage(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob,
	RTEntityID Source,
	Int64 Damage
);

Void RTMobUpdate(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob
);

Bool RTMobCanRespawn(
	RTMobRef Mob
);

Bool RTMobIsAlive(
	RTMobRef Mob
);

Void RTMobOnEvent(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob,
	CString Event
);

EXTERN_C_END
