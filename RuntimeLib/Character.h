
#pragma once

#include "Base.h"

#include "BattleSystem.h"
#include "BlendedAbility.h"
#include "Collection.h"
#include "Warehouse.h"
#include "Entity.h"
#include "Equipment.h"
#include "EssenceAbility.h"
#include "Inventory.h"
#include "Item.h"
#include "KarmaAbility.h"
#include "Movement.h"
#include "OverlordMastery.h"
#include "Quest.h"
#include "Quickslot.h"
#include "Recovery.h"
#include "Skill.h"
#include "SkillSlot.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
    RUNTIME_CHARACTER_SYNC_INFO                 = 1 << 0,
    RUNTIME_CHARACTER_SYNC_EQUIPMENT            = 1 << 1,
    RUNTIME_CHARACTER_SYNC_INVENTORY            = 1 << 2,
    RUNTIME_CHARACTER_SYNC_SKILLSLOT            = 1 << 3,
    RUNTIME_CHARACTER_SYNC_QUICKSLOT            = 1 << 4,
    RUNTIME_CHARACTER_SYNC_QUESTSLOT            = 1 << 5,
    RUNTIME_CHARACTER_SYNC_QUESTFLAG            = 1 << 6,
    RUNTIME_CHARACTER_SYNC_DUNGEONQUESTFLAG     = 1 << 7,
    RUNTIME_CHARACTER_SYNC_ESSENCE_ABILITY      = 1 << 8,
    RUNTIME_CHARACTER_SYNC_OVERLORD             = 1 << 9,
    RUNTIME_CHARACTER_SYNC_COLLECTION           = 1 << 10,
    RUNTIME_CHARACTER_SYNC_WAREHOUSE            = 1 << 11,
};

enum {
    RUNTIME_CHARACTER_SYNC_PRIORITY_LOW     = 1 << 0,
    RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH    = 1 << 1,
    RUNTIME_CHARACTER_SYNC_PRIORITY_INSTANT = 1 << 2,
};

enum {
    RUNTIME_CHARACTER_LEVEL_BASIC,
    RUNTIME_CHARACTER_LEVEL_OVERLORD,
    RUNTIME_CHARACTER_LEVEL_FORCEWING,

    RUNTIME_CHARACTER_LEVEL_COUNT
};

enum {
    RUNTIME_CHARACTER_STAT_STR,
    RUNTIME_CHARACTER_STAT_DEX,
    RUNTIME_CHARACTER_STAT_INT,
    RUNTIME_CHARACTER_STAT_PNT,

    RUNTIME_CHARACTER_STAT_COUNT
};

enum {
    RUNTIME_CHARACTER_CURRENCY_ALZ,
    RUNTIME_CHARACTER_CURRENCY_GEM,

    RUNTIME_CHARACTER_CURRENCY_COUNT
};

union _RTCharacterStyle {
    struct {
        UInt32 AuraCode : 2;
        UInt32 Gender : 1;
        UInt32 Padding1 : 1;
        UInt32 Unknown1 : 1;
        UInt32 Padding2 : 4;
        UInt32 HairStyle : 3;
        UInt32 Padding3 : 2;
        UInt32 ShowHelmed : 1;
        UInt32 ExtendedBattleStyle : 1;
        UInt32 Face : 2;
        UInt32 Padding4 : 3;
        UInt32 HairColor : 3;
        UInt32 BattleStyle : 3;
        UInt32 BattleRank : 5;
    };
    UInt32 RawValue;
};

union _RTCharacterLiveStyle {
    struct {
        UInt32 IsDancing : 1;
        UInt32 IsSitting : 1;
        UInt32 IsDancing2 : 1;
        UInt32 IsDancing3 : 1;
        UInt32 Padding1 : 28;
    };

    UInt32 RawValue;
};

struct _RTCharacterProfile {
    UInt16 Nation;
    UInt32 WarpMask;
    UInt32 MapsMask;
};

struct _RTCharacterLevel {
    UInt8 Level;
    UInt64 Exp;
    UInt32 Point;
};

struct _RTCharacterHonor {
    Int8 Rank;
    Int64 Point;
    UInt64 Exp;
};

struct _RTCharacterAbility {
    UInt16 Point;
    UInt32 Exp;
    UInt32 MaxEssenceAbilitySlotCount;
    UInt32 MaxBlendedAbilitySlotCount;
};

struct _RTCharacterResource {
    Int32 HP;
    Int32 MP;
    Int32 SP;
    Int32 BP;
    Int32 DP;
};

struct _RTCharacterSkill {
    UInt8 Rank;
    UInt16 Level;
    UInt64 Exp;
    UInt16 Point;
};

struct _RTCharacterPosition {
    UInt8 WorldID;
    UInt16 X;
    UInt16 Y;
    Index DungeonIndex;
};

struct _RTCharacterInfo {
    union _RTCharacterStyle Style;
    struct _RTCharacterProfile Profile;
    struct _RTCharacterLevel Basic;
    struct _RTCharacterLevel Overlord;
    struct _RTCharacterLevel ForceWing;
    struct _RTCharacterHonor Honor;
    struct _RTCharacterAbility Ability;
    struct _RTCharacterSkill Skill;
    struct _RTCharacterResource Resource;
    UInt16 Stat[RUNTIME_CHARACTER_STAT_COUNT];
    UInt64 Currency[RUNTIME_CHARACTER_CURRENCY_COUNT];
    struct _RTCharacterPosition Position;
};

struct _RTCharacter {
    RTEntityID ID;
    RTEntityID PartyID;
    UInt32 CharacterIndex;
    UInt32 SyncMask;
    UInt32 SyncPriority;
    Timestamp SyncTimestamp;

//    Int32 Index;
//    Timestamp CreationDate;
//    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1];
    struct _RTCharacterInfo Info;
    struct _RTCharacterEquipmentInfo EquipmentInfo;
    struct _RTCharacterInventoryInfo InventoryInfo;
    struct _RTCharacterSkillSlotInfo SkillSlotInfo;
    struct _RTCharacterQuickSlotInfo QuickSlotInfo;
    struct _RTCharacterQuestSlotInfo QuestSlotInfo;
    struct _RTCharacterQuestFlagInfo QuestFlagInfo;
    struct _RTCharacterQuestFlagInfo DungeonQuestFlagInfo;
    
    //GAME_DATA_CHARACTER_ACHIEVEMENT AchievementData;
    struct _RTCharacterEssenceAbilityInfo EssenceAbilityInfo;
    /*
    GAME_DATA_CHARACTER_BLENDEDABILITY BlendedAbilityData;
    GAME_DATA_CHARACTER_HONORMEDAL HonorMedalData;
    */

    struct _RTCharacterOverlordMasteryInfo OverlordMasteryInfo;
    struct _RTCharacterCollectionInfo CollectionInfo;
    struct _RTCharacterWarehouseInfo WarehouseInfo;
    /*
    GAME_DATA_CHARACTER_TRANSFORM TransformData;
    GAME_DATA_CHARACTER_TRANSCENDENCE TranscendenceData;
    GAME_DATA_CHARACTER_MERCENARY MercenaryData;
    GAME_DATA_CHARACTER_CRAFT CraftData;
    */

    struct _RTCharacterInventoryInfo TemporaryInventoryInfo;
    struct _RTCharacterRecoveryInfo RecoveryInfo;
    struct _RTMovement Movement;
    struct _RTBattleAttributes Attributes;

    Int32 AxpFieldRate;
    Int32 SkillComboLevel;
    Timestamp DebugSpRegenTimeout;
};

#pragma pack(pop)

Void RTCharacterInitialize(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTCharacterInfoRef Info,
    RTCharacterEquipmentInfoRef EquipmentInfo,
    RTCharacterInventoryInfoRef InventoryInfo,
    RTCharacterSkillSlotInfoRef SkillSlotInfo,
    RTCharacterQuickSlotInfoRef QuickSlotInfo,
    RTCharacterQuestSlotInfoRef QuestSlotInfo,
    RTCharacterQuestFlagInfoRef QuestFlagInfo,
    RTCharacterQuestFlagInfoRef DungeonQuestFlagInfo,
    RTCharacterEssenceAbilityInfoRef EssenceAbilityInfo,
    RTCharacterOverlordMasteryInfoRef OverlordMasteryInfo,
    RTCharacterCollectionInfoRef CollectionInfo,
    RTCharacterWarehouseInfoRef WarehouseInfo
);

Void RTCharacterInitializeAttributes(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterIsAlive(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterIsUnmovable(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterMovementBegin(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 WorldID,
    Int32 PositionBeginX,
    Int32 PositionBeginY,
    Int32 PositionCurrentX,
    Int32 PositionCurrentY,
    Int32 PositionEndX,
    Int32 PositionEndY
);

Bool RTCharacterMovementChange(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 WorldID,
    Int32 PositionBeginX,
    Int32 PositionBeginY,
    Int32 PositionCurrentX,
    Int32 PositionCurrentY,
    Int32 PositionEndX,
    Int32 PositionEndY
);

Bool RTCharacterMovementEnd(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PositionX,
    Int32 PositionY
);

Bool RTCharacterMovementChangeWaypoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PositionCurrentX,
    Int32 PositionCurrentY,
    Int32 PositionNextX,
    Int32 PositionNextY
);

Bool RTCharacterMovementChangeChunk(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PositionCurrentX,
    Int32 PositionCurrentY
);

Int32 RTCharacterCalculateRequiredMP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CoefficientA,
    Int32 CoefficientB,
    Int32 SkillLevel
);

Bool RTCharacterBattleRankUp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterAddCurrency(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CurrencyType,
    UInt64 Amount
);

Void RTCharacterAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt64 Exp
);

Int32 RTCharacterAddSkillExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 SkillExp
);

Void RTCharacterAddHonorPoint(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int64 HonorPoint
);

Void RTCharacterAddAbilityExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt64 Exp
);

Void RTCharacterAddRage(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 Rage
);

Bool RTCharacterConsumeRage(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 Rage
);

Bool RTCharacterAddStats(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32* Stats
);

Bool RTCharacterRemoveStat(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 StatIndex,
    Int32 Amount
);

Void RTCharacterAddHP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 HP,
    Bool IsPotion
);

Void RTCharacterAddMP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MP,
    Bool IsPotion
);

Void RTCharacterAddSP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SP
);

Void RTCharacterApplyDamage(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTEntityID Source,
    Int32 Damage
);

EXTERN_C_END
