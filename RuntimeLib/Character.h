
#pragma once

#include "Base.h"

#include "BattleSystem.h"
#include "BlendedAbility.h"
#include "Collection.h"
#include "Warehouse.h"
#include "Entity.h"
#include "Equipment.h"
#include "EssenceAbility.h"
#include "ForceWing.h"
#include "Inventory.h"
#include "Item.h"
#include "KarmaAbility.h"
#include "Movement.h"
#include "NewbieSupport.h"
#include "OverlordMastery.h"
#include "Quest.h"
#include "Quickslot.h"
#include "Recovery.h"
#include "RequestCraft.h"
#include "Skill.h"
#include "SkillSlot.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

union _RTCharacterSyncMask {
    struct {
        UInt64 Info : 1;
        UInt64 EquipmentInfo : 1;
        UInt64 EquipmentLockInfo : 1;
        UInt64 InventoryInfo : 1;
        UInt64 SkillSlotInfo : 1;
        UInt64 QuickSlotInfo : 1;
        UInt64 EssenceAbilityInfo : 1;
        UInt64 BlendedAbilityInfo : 1;
        UInt64 KarmaAbilityInfo : 1;
        UInt64 BlessingBeadInfo : 1;
        UInt64 PremiumServiceInfo : 1;
        UInt64 QuestSlotInfo : 1;
        UInt64 QuestFlagInfo : 1;
        UInt64 DungeonQuestFlagInfo : 1;
        UInt64 DailyQuestInfo : 1;
        UInt64 MercenaryInfo : 1;
        UInt64 EquipmentAppearanceInfo : 1;
        UInt64 InventoryAppearanceInfo : 1;
        UInt64 AchievementInfo : 1;
        UInt64 AchievementRewardInfo : 1;
        UInt64 RequestCraftInfo : 1;
        UInt64 BuffInfo : 1;
        UInt64 VehicleInventoryInfo : 1;
        UInt64 MeritMasteryInfo : 1;
        UInt64 PlatinumMeritMasteryInfo : 1;
        UInt64 OverlordMasteryInfo : 1;
        UInt64 HonorMedalInfo : 1;
        UInt64 ForceWingInfo : 1;
        UInt64 GiftboxInfo : 1;
        UInt64 CollectionInfo : 1;
        UInt64 TransformInfo : 1;
        UInt64 TranscendenceInfo : 1;
        UInt64 StellarMasteryInfo : 1;
        UInt64 MythMasteryInfo : 1;
        UInt64 NewbieSupportInfo : 1;
        UInt64 EventPassInfo : 1;
        UInt64 CostumeInfo : 1;
        UInt64 _Padding : 11;
        UInt64 WarehouseInfo : 1;
    };
    UInt64 RawValue;
};

union _RTCharacterSyncPriority {
    struct {
        UInt64 Low : 1;
        UInt64 High : 1;
        UInt64 Immediate : 1;
    };
    UInt64 RawValue;
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

union _RTCharacterExtendedStyle {
    struct {
        UInt8 Unknown0 : 1;
        UInt8 IsAstralWeaponActive : 1;
        UInt8 Unknown1 : 1;
        UInt8 IsAuraActive : 1;
        UInt8 IsBattleMode1Active : 1;
        UInt8 IsBattleMode2Active : 1;
        UInt8 IsBattleMode3Active : 1;
        UInt8 IsComboActive : 1;
    };

    UInt8 RawValue;
};

union _RTCharacterPKState {
    struct {
        UInt16 Level : 3;
        UInt16 Count : 13;
    };

    UInt16 RawValue;
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
    Int32 Rage;
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
    union _RTCharacterLiveStyle LiveStyle;
    union _RTCharacterExtendedStyle ExtendedStyle;
    union _RTCharacterPKState PKState;
    struct _RTCharacterProfile Profile;
    struct _RTCharacterLevel Basic;
    struct _RTCharacterLevel Overlord;
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
    Index CharacterIndex;
    union _RTCharacterSyncMask SyncMask;
    union _RTCharacterSyncPriority SyncPriority;
    Timestamp SyncTimestamp;

//    Int32 Index;
//    Timestamp CreationDate;
//    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1];
    struct _RTCharacterInfo Info;
    struct _RTCharacterEquipmentInfo EquipmentInfo;
    // struct _RTCharacterEquipmentLockInfo EquipmentLockInfo;
    struct _RTCharacterInventoryInfo InventoryInfo;
    struct _RTCharacterSkillSlotInfo SkillSlotInfo;
    struct _RTCharacterQuickSlotInfo QuickSlotInfo;
    struct _RTCharacterEssenceAbilityInfo EssenceAbilityInfo;
    // struct _RTCharacterBlendedAbilityInfo BlendedAbilityInfo;
    // struct _RTCharacterKarmaAbilityInfo KarmaAbilityInfo;
    // struct _RTCharacterBlessingBeadInfo BlessingBeadInfo;
    // struct _RTCharacterPremiumServiceInfo PremiumServiceInfo;
    struct _RTCharacterQuestSlotInfo QuestSlotInfo;
    struct _RTCharacterQuestFlagInfo QuestFlagInfo;
    struct _RTCharacterDungeonQuestFlagInfo DungeonQuestFlagInfo;
    // struct _RTCharacterDailyQuestInfo DailyQuestInfo;
    // struct _RTCharacterMercenaryInfo MercenaryInfo;
    // struct _RTCharacterEquipmentAppearanceInfo EquipmentAppearanceInfo;
    // struct _RTCharacterInventoryAppearanceInfo InventoryAppearanceInfo;
    // struct _RTCharacterAchievementInfo AchievementInfo;
    // struct _RTCharacterAchievementRewardInfo AchievementRewardInfo;
    struct _RTCharacterRequestCraftInfo RequestCraftInfo;
    // struct _RTCharacterBuffInfo BuffInfo;
    // struct _RTCharacterVehicleInventoryInfo VehicleInventoryInfo;
    // struct _RTCharacterMeritMasteryInfo MeritMasteryInfo;
    // struct _RTCharacterPlatinumMeritMasteryInfo PlatinumMeritMasteryInfo;
    struct _RTCharacterOverlordMasteryInfo OverlordMasteryInfo;
    // struct _RTCharacterHonorMedalInfo HonorMedalInfo;
    struct _RTCharacterForceWingInfo ForceWingInfo;
    // struct _RTCharacterGiftboxInfo GiftboxInfo;
    struct _RTCharacterCollectionInfo CollectionInfo;
    // struct _RTCharacterTransformInfo TransformInfo;
    // struct _RTCharacterTranscendenceInfo TranscendenceInfo;
    // struct _RTCharacterStellarMasteryInfo StellarMasteryInfo;
    // struct _RTCharacterMythMasteryInfo MythMasteryInfo;
    struct _RTCharacterNewbieSupportInfo NewbieSupportInfo;
    // struct _RTCharacterEventPassInfo EventPassInfo;
    // struct _RTCharacterCostumeInfo CostumeInfo;

    struct _RTCharacterWarehouseInfo WarehouseInfo;
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
    RTCharacterDungeonQuestFlagInfoRef DungeonQuestFlagInfo,
    RTCharacterEssenceAbilityInfoRef EssenceAbilityInfo,
    RTCharacterOverlordMasteryInfoRef OverlordMasteryInfo,
    RTCharacterForceWingInfoRef ForceWingInfo,
    RTCharacterCollectionInfoRef CollectionInfo,
    RTCharacterNewbieSupportInfoRef NewbieSupportInfo,
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

Void RTCharacterAddWingExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt64 Exp
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

Bool RTCharacterResetStats(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterSetHP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 NewValue,
    Bool IsPotion
);

Void RTCharacterAddHP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 HP,
    Bool IsPotion
);

Void RTCharacterSetMP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 NewValue,
    Bool IsPotion
);

Void RTCharacterAddMP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MP,
    Bool IsPotion
);

Void RTCharacterSetSP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 NewValue
);

Void RTCharacterAddSP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SP
);

Void RTCharacterSetBP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 NewValue
);

Void RTCharacterAddBP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 BP
);

Void RTCharacterApplyDamage(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTEntityID Source,
    Int32 Damage
);

EXTERN_C_END
