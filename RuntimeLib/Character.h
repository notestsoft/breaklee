
#pragma once

#include "Base.h"

#include "Ability.h"
#include "Account.h"
#include "Achievement.h"
#include "AnimaMastery.h"
#include "Appearance.h"
#include "AuraMastery.h"
#include "BattleSystem.h"
#include "BlessingBead.h"
#include "Buff.h"
#include "Collection.h"
#include "Costume.h"
#include "Craft.h"
#include "DailyQuest.h"
#include "DamageBooster.h"
#include "Entity.h"
#include "Equipment.h"
#include "EventPass.h"
#include "Exploration.h"
#include "ForceWing.h"
#include "GiftBox.h"
#include "HonorMedal.h"
#include "HonorRank.h"
#include "Inventory.h"
#include "Item.h"
#include "Mercenary.h"
#include "MeritMastery.h"
#include "Movement.h"
#include "MythMastery.h"
#include "NewbieSupport.h"
#include "OverlordMastery.h"
#include "PremiumService.h"
#include "Preset.h"
#include "Quest.h"
#include "Quickslot.h"
#include "Recovery.h"
#include "RequestCraft.h"
#include "ResearchSupport.h"
#include "SecretShop.h"
#include "Settings.h"
#include "Skill.h"
#include "SkillSlot.h"
#include "StellarMastery.h"
#include "Transcendence.h"
#include "Transform.h"
#include "Upgrade.h"
#include "VehicleInventory.h"
#include "Warehouse.h"
#include "WarpService.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

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

union _RTCharacterStyle {
    struct {
        UInt32 BattleStyle : 3;
        UInt32 BattleRank : 5;
        UInt32 Face : 2;
        UInt32 Padding5 : 3;
        UInt32 HairColor : 3;
        UInt32 Padding3 : 1;
        UInt32 HairStyle : 3;
        UInt32 Padding4 : 2;
        UInt32 ShowHelmed : 1;
        UInt32 ExtendedBattleStyle : 1;
        UInt32 AuraCode : 2;
        UInt32 Gender : 1;
        UInt32 Padding1 : 1;
        UInt32 Unknown1 : 1;
        UInt32 Padding2 : 3;
    };
    UInt32 RawValue;
};

union _RTCharacterLiveStyle {
    struct {
        UInt32 Padding1 : 24;
        UInt32 IsDancing : 1;
        UInt32 IsSitting : 1;
        UInt32 IsDancing2 : 1;
        UInt32 IsDancing3 : 1;
        UInt32 Padding2 : 4;
    };

    UInt32 RawValue;
};

union _RTCharacterExtendedStyle {
    struct {
        UInt8 Unknown0 : 1;
        UInt8 IsAstralWeaponActive : 1;
        UInt8 IsVehicleActive : 1;
        UInt8 IsAuraActive : 1;
        UInt8 BattleModeFlags : 3;
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

struct _RTCharacterInfo {
    Int32 WorldIndex;
    Int32 DungeonIndex;
    Int16 PositionX;
    Int16 PositionY;
    UInt64 Exp;
    UInt64 Alz;
    UInt64 Wexp;
    Int32 Level;
    UInt32 Unknown1;
    Int32 Stat[RUNTIME_CHARACTER_STAT_COUNT];
    Int32 SkillRank;
    Int32 MagicSkillRank;
    UInt64 BaseHP;
    UInt64 CurrentHP;
    UInt32 MaxMP;
    UInt32 CurrentMP;
    UInt16 MaxSP;
    UInt16 CurrentSP;
    UInt32 MaxRage;
    UInt32 CurrentRage;
    UInt32 MaxBP;
    UInt32 CurrentBP;
    Timestamp DPDuration;
    UInt32 DP;
    UInt32 SkillLevel;
    UInt32 SkillExp;
    UInt32 SkillPoint;
    UInt64 RestExp;
    UInt64 HonorPoint;
    UInt64 DeathPenaltyExp;
    UInt64 DeathPenaltyHp;
    UInt32 DeathPenaltyMp;
    union _RTCharacterPKState PKState;
};

struct _RTCharacterStyleInfo {
    UInt8 Nation;
    UInt32 Unknown2;
    UInt32 WarpMask;
    UInt32 MapsMask;
    union _RTCharacterStyle Style;
    union _RTCharacterLiveStyle LiveStyle;
    union _RTCharacterExtendedStyle ExtendedStyle;
};

struct _RTCharacterData {
#define ACCOUNT_DATA_PROTOCOL(__TYPE__, __NAME__) \
    __TYPE__ __NAME__;

#define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__) \
    __TYPE__ __NAME__;
    
#include "CharacterDataDefinition.h"
};

union _RTCharacterSyncMask {
    struct {
        #define ACCOUNT_DATA_PROTOCOL(__TYPE__, __NAME__) \
            UInt64 __NAME__ : 1;

        #define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__) \
            UInt64 __NAME__ : 1;

        #include "CharacterDataDefinition.h"
    };
    UInt64 RawValue;
};

struct _RTCharacter {
    RTEntityID ID;
    RTEntityID PartyID;
    RTEntityID TargetCharacterID;
    Index CharacterIndex;
    Int32 DungeonEntryItemSlotIndex;

//    Int32 Index;
//    Timestamp CreationDate;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1];

    struct _RTCharacterData Data;
    union _RTCharacterSyncMask SyncMask;
    Timestamp SyncTimestamp;

    struct _RTMovement Movement;
    struct _RTBattleAttributes Attributes;
    Int32 AxpFieldRate;
    Int32 SkillComboLevel;
    Timestamp DebugSpRegenTimeout;

    Int32 BattleModeSkillIndex;
    Timestamp BattleModeTimeout; // TODO: Replace timeout with skill timer pool
    Timestamp GiftBoxUpdateTimestamps[RUNTIME_CHARACTER_MAX_GIFT_BOX_SLOT_COUNT];
};

#pragma pack(pop)

Void RTCharacterInitializeAttributes(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterUpdate(
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

Void RTCharacterAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt64 Exp
);

Int32 RTCharacterAddSkillExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int64 SkillExp
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
    Int32 Rage
);

Bool RTCharacterConsumeRage(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 Rage
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
    Int64 NewValue,
    Bool IsPotion
);

Void RTCharacterAddHP(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int64 HP,
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
