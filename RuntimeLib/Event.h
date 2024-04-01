#pragma once

#include "Base.h"
#include "Constants.h"
#include "BattleSystem.h"
#include "Item.h"

EXTERN_C_BEGIN

enum {
    RUNTIME_EVENT_CHARACTER_SPAWN,
    RUNTIME_EVENT_CHARACTER_DESPAWN,
    RUNTIME_EVENT_CHARACTER_CHUNK_UPDATE,
    RUNTIME_EVENT_CHARACTER_LEVEL_UP,
    RUNTIME_EVENT_CHARACTER_SKILL_LEVEL_UP,
    RUNTIME_EVENT_CHARACTER_SKILL_RANK_UP,
    RUNTIME_EVENT_CHARACTER_BATTLE_RANK_UP,
    RUNTIME_EVENT_CHARACTER_OVERLORD_LEVEL_UP,
    RUNTIME_EVENT_CHARACTER_UPDATE_SKILL_STATUS,
    RUNTIME_EVENT_CHARACTER_UPDATE_FORCE_WING,
    RUNTIME_EVENT_CHARACTER_UPDATE_FORCE_WING_EXP,
    RUNTIME_EVENT_CHARACTER_UPDATE_FORCE_WING_LEVEL_UP,
    RUNTIME_EVENT_CHARACTER_DATA,
    RUNTIME_EVENT_ATTACK_TO_MOB,
    RUNTIME_EVENT_MOB_SPAWN,
    RUNTIME_EVENT_MOB_UPDATE,
    RUNTIME_EVENT_MOB_DESPAWN,
    RUNTIME_EVENT_MOB_MOVEMENT_BEGIN,
    RUNTIME_EVENT_MOB_MOVEMENT_END,
    RUNTIME_EVENT_MOB_CHASE_BEGIN,
    RUNTIME_EVENT_MOB_CHASE_END,
    RUNTIME_EVENT_MOB_ATTACK,
    RUNTIME_EVENT_ITEM_SPAWN,
    RUNTIME_EVENT_ITEM_DESPAWN,
    RUNTIME_EVENT_QUEST_DUNGEON_PATTERN_PART_COMPLETED,
    RUNTIME_EVENT_PARTY_QUEST_MISSION_MOB_KILL,
};

enum {
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_HP_POTION = 1,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_MP_POTION,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_HP,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_MP,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_SP,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_SP_INCREASE,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_SP_DECREASE,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_EXP,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_RANK,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_LEVEL,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_SP_DECREASE_EX,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_BUFF_POTION,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_REPUTATION,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_GUIDITEMFX,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_RESURRECTION,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_PENALTY_EXP,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_DAMAGE_CELL,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_DEFICIENCY,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_AUTH_HP_POTION,
    RUNTIME_EVENT_CHARACTER_DATA_TYPE_RAGE,

    RUNTIME_EVENT_CHARACTER_DATA_TYPE_BP = 24,
};

enum {
    RUNTIME_DELETE_TYPE_DEAD = 0x10,
    RUNTIME_DELETE_TYPE_WARP,
    RUNTIME_DELETE_TYPE_LOGOUT,
    RUNTIME_DELETE_TYPE_RETURN,
    RUNTIME_DELETE_TYPE_DISAPPEAR,
    RUNTIME_DELETE_TYPE_DEAD_UNKNOWN,
};

union _RTEventData {
    struct { Index CharacterIndex; struct _RTCharacterPosition Position; } CharacterSpawn;
    struct { Index CharacterIndex; struct _RTCharacterPosition Position; } CharacterDespawn;

    /*
    struct { } CharacterChunkUpdate;
    struct { } CharacterLevelUp;
    struct { } CharacterSkillLevelUp;
    struct { } CharacterSkillRankUp;
    struct { } CharacterBattleRankUp;
    */
    struct { 
        Index CharacterIndex; 
        UInt8 ForceWingGrade;
        UInt8 ForceWingLevel;
        Int64 ForceWingExp;
        UInt32 TrainingPointCount;
    } CharacterUpdateForceWing;

    struct {
        Int64 ForceWingExp;
    } CharacterUpdateForceWingExp;

    /*
    struct { } CharacterUpdateSkillStatus;
    */
    struct { 
        UInt32 Type;
    } CharacterData;

    struct {
        UInt32 TickCount;
        UInt16 PositionBeginX;
        UInt16 PositionBeginY;
        UInt16 PositionEndX;
        UInt16 PositionEndY;
        UInt16 MobSpeciesIndex;
        UInt64 MaxHP;
        UInt64 CurrentHP;
        UInt8 Level;
    } MobSpawnOrUpdate;

    /*
    struct { } MobDespawn;
    */
    struct {
        UInt32 TickCount;
        UInt16 PositionBeginX;
        UInt16 PositionBeginY;
        UInt16 PositionEndX;
        UInt16 PositionEndY;
    } MobMovementBegin;

    struct {
        UInt16 PositionCurrentX;
        UInt16 PositionCurrentY;
    } MobMovementEnd;

    struct {
        Bool IsDefaultSkill;
        Int32 ResultCount;
        UInt64 MobHP;

        struct {
            RTEntityID Entity;
            Bool IsDead;
            UInt8 Result;
            UInt32 AppliedDamage;
            UInt64 TargetHP;
        } Results[RUNTIME_MOB_MAX_ATTACK_RESULT_COUNT];
    } MobAttack;

    struct {
        RTEntityID Entity;
        UInt64 ItemOptions;
        UInt64 ItemID;
        UInt16 X;
        UInt16 Y;
        UInt16 UniqueKey;
        RTItemProperty ItemProperty;
    } ItemSpawn;

    /*
    struct { } ItemDespawn;
    struct { } QuestDungeonPatternPartCompleted;
    */

    struct {
        UInt16 QuestID;
        UInt16 MobSpeciesIndex;
    } PartyQuestMissionMobKill;

    struct {
        RTEntityID Character;
        RTEntityID Mob;
        UInt32 AttackType;
        UInt64 MobHP;
        UInt64 CharacterHP;
    } AttackToMob;
};

struct _RTEvent {
    Int32 Type;
    RTWorldContextRef World;
    RTEntityID SourceID;
    RTEntityID TargetID;
    Int32 X;
    Int32 Y;
    union _RTEventData Data;
};
typedef union _RTEventData RTEventData;

typedef Void (*RTEventCallback)(
    RTRuntimeRef Runtime,
    RTEventRef Event,
    Void* UserData
);

EXTERN_C_END
