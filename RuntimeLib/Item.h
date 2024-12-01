#pragma once

#include "Base.h"
#include "Constants.h"

#define RUNTIME_ITEM_PROCEDURE_BINDING(__NAME__) \
Int32 __NAME__(                                  \
    RTRuntimeRef Runtime,                        \
    RTCharacterRef Character,                    \
    RTItemSlotRef ItemSlot,                      \
    RTItemDataRef ItemData,                      \
    Void* Payload                                \
)

EXTERN_C_BEGIN

#pragma pack(push, 1)

// TODO: Make upgrade point system data driven and configurable in enchant.xml

enum {
	RUNTIME_ITEM_USE_RESULT_SUCCESS = 0,
	RUNTIME_ITEM_USE_RESULT_FAILED = 1,
	RUNTIME_ITEM_USE_RESULT_SUCCESS_DESTROY_ITEM = 15,
	RUNTIME_ITEM_USE_RESULT_SUCCESS_SAFEGUARD_ITEM = 16,
	RUNTIME_ITEM_USE_RESULT_IS_DEAD = 17,
	RUNTIME_ITEM_USE_RESULT_IS_NOT_DEAD = 18,
	RUNTIME_ITEM_USE_RESULT_REJECTED = 19,
	RUNTIME_ITEM_USE_RESULT_GUILD_INFO_ERROR = 20,
	RUNTIME_ITEM_USE_RESULT_ITEM_EXPIRED = 21,
	RUNTIME_ITEM_USE_RESULT_COOLDOWN_1 = 22,
	RUNTIME_ITEM_USE_RESULT_COOLDOWN_2 = 23,
	RUNTIME_ITEM_USE_RESULT_COOLDOWN_3 = 24,
	RUNTIME_ITEM_USE_RESULT_WAR_USAGE_LIMIT = 25,
	RUNTIME_ITEM_USE_RESULT_WAR_USAGE = 26,
	RUNTIME_ITEM_USE_RESULT_EPIC_BOOST_FAILED = 26,
	RUNTIME_ITEM_USE_RESULT_DUNGEON_USAGE_LIMIT = 27,
};

enum {
    RUNTIME_ITEM_CONFIGURATION_FLAGS_NODROP    = 1 << 0,
    RUNTIME_ITEM_CONFIGURATION_FLAGS_NOTRADE   = 1 << 1,
    RUNTIME_ITEM_CONFIGURATION_FLAGS_NOSELL    = 1 << 2,
    RUNTIME_ITEM_CONFIGURATION_FLAGS_NOSTORAGE = 1 << 3,
};

enum {
	RUNTIME_ITEM_SUBTYPE_POTION_HP = 1,
	RUNTIME_ITEM_SUBTYPE_POTION_MP = 2,
	RUNTIME_ITEM_SUBTYPE_POTION_SP = 3,
	RUNTIME_ITEM_SUBTYPE_POTION_STAT = 4,
	RUNTIME_ITEM_SUBTYPE_POTION_HONOR = 5,
	
	RUNTIME_ITEM_SUBTYPE_POTION_AUTO_HP = 8, // TODO: Change notification from hp potion to auto hp potion!

	RUNTIME_ITEM_SUBTYPE_POTION_FULL_RECOVERY = 10,
	RUNTIME_ITEM_SUBTYPE_POTION_STAT_RESET = 11,
	RUNTIME_ITEM_SUBTYPE_POTION_RAGE = 12,
	RUNTIME_ITEM_SUBTYPE_POTION_BP = 13,
	// TODO: Add missing potion types
};

enum {
	RUNTIME_ITEM_POTION_STAT_TYPE_STR = 2,
	RUNTIME_ITEM_POTION_STAT_TYPE_DEX = 3,
	RUNTIME_ITEM_POTION_STAT_TYPE_INT = 4,
	// TODO: Add missing stat types
};

enum {
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_ALZ = 1,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_EXP = 2,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_SKILLEXP = 3,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_PETEXP = 4,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_GUILDEXP = 5,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_HONOR = 6,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_AXP = 7,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_WAREXP = 8,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_MEDALSCORE = 12,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_OXP = 13,
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_WINGEXP = 14,
};

enum {
	RUNTIME_ITEM_SUBTYPE_SAVER_DP = 1,
	RUNTIME_ITEM_SUBTYPE_SAVER_AP = 2,
	RUNTIME_ITEM_SUBTYPE_SAVER_WEXP = 3,
};

enum {
	RUNTIME_ITEM_MERIT_MEDAL_TYPE_NORMAL = 1,
	RUNTIME_ITEM_MERIT_MEDAL_TYPE_PREMIUM = 2,
	RUNTIME_ITEM_MERIT_MEDAL_TYPE_GM = 3,
};

enum {
	RUNTIME_ITEM_ID_CURRENCY = 13,
};

enum {
    ITEM_FLAG_NONE = 0,
    ITEM_FLAG_NO_DROP = 1 << 0,
    ITEM_FLAG_NO_TRADE = 1 << 1,
    ITEM_FLAG_NO_SELL = 1 << 2,
    ITEM_FLAG_NO_STORAGE = 1 << 3,

    ITEM_FLAG_ACCOUNT_BIND = ITEM_FLAG_NO_DROP | ITEM_FLAG_NO_TRADE,
    ITEM_FLAG_CHARACTER_BIND = ITEM_FLAG_ACCOUNT_BIND | ITEM_FLAG_NO_STORAGE,
    ITEM_FLAG_INVENTORY_BIND = ITEM_FLAG_CHARACTER_BIND | ITEM_FLAG_NO_SELL,
};

enum {
    RUNTIME_ITEM_SERIAL_WARP_CONSUMABLE = 12,
    RUNTIME_ITEM_SERIAL_WARP_PERMANENT = 944,
};

#define RUNTIME_ITEM_MASK_ACCOUNT_BINDING	(0b0000000000000000000000000001000000000000)
#define RUNTIME_ITEM_MASK_UPGRADE_LEVEL		(0b0000000000000000000000111110000000000000)
#define RUNTIME_ITEM_MASK_CHARACTER_BINDING (0b0000000000000000000010000000000000000000)
#define RUNTIME_ITEM_MASK_TRADABLE_BINDING	(0b0000000000000000000100000000000000000000)
#define RUNTIME_ITEM_MASK_VEHICLE_COLOR		(0b0000000000000000111000000000000000000000)
#define RUNTIME_ITEM_MASK_UNKNOWN			(0b0000000000000001000000000000000000000000)
#define RUNTIME_ITEM_MASK_BROKEN			(0b0000000000001000000000000000000000000000)
#define RUNTIME_ITEM_MASK_EXTREME_LEVEL     (0b0000111100000000000000000000000000000000)
#define RUNTIME_ITEM_MASK_DIVINE_LEVEL      (0b1111000000000000000000000000000000000000)
#define RUNTIME_ITEM_MASK_INDEX			   ~(0b1111111100001001111110111111000000000000)
#define RUNTIME_ITEM_MASK_LEVEL             RUNTIME_ITEM_MASK_UPGRADE_LEVEL | RUNTIME_ITEM_MASK_EXTREME_LEVEL | RUNTIME_ITEM_MASK_DIVINE_LEVEL

#define RUNTIME_QUEST_ITEM_INDEX_MASK		0x000FFF80
#define RUNTIME_QUEST_ITEM_INDEX_EXPONENT	7
#define RUNTIME_QUEST_ITEM_COUNT_MASK		0x0000007F
#define RUNTIME_SHORT_ITEM_COUNT_MASK	0x0000FFFF

#define RTQuestItemOptions(Index, Count) ((Index << RUNTIME_QUEST_ITEM_INDEX_EXPONENT) | Count)
#define RTQuestItemGetOptions(ItemOptions) ((ItemOptions & RUNTIME_QUEST_ITEM_INDEX_MASK) >> RUNTIME_QUEST_ITEM_INDEX_EXPONENT)
#define RTQuestItemGetCount(ItemOptions) (ItemOptions & RUNTIME_QUEST_ITEM_COUNT_MASK)

struct _RTItemProperty {
	union {
		struct { UInt8 Serial; };
		struct {
			UInt8 IsQuestItem : 1;
			UInt8 Unknown1 : 1;
			UInt8 Unknown2 : 1;
			UInt8 Unknown3 : 1;
			UInt8 Unknown4 : 1;
			UInt8 Unknown5 : 1;
			UInt8 Unknown6 : 1;
			UInt8 Unknown7 : 1;
		};
	};
};
typedef struct _RTItemProperty RTItemProperty;

struct _RTItemData {
    Int32 ItemID;
    Int32 ItemType;
	Int32 ItemGrade;
    Int32 SellPrice;
    Int32 BuyPrice;
    Int32 MinLevel;

	union {
		struct {
			Int32 AttackRate;
			Int32 Attack;
			Int32 MagicAttack;
			Int32 MaxSlotCount;
			Int32 Str;
			Int32 Dex;
			Int32 Int;
			Int32 UnknownOptions1[6];
			Int32 DeltaStr;
			Int32 DeltaDex;
			Int32 DeltaInt;
			Int32 UnknownOptions2[6];
		} Weapon;

		struct {
			Int32 HP;
			Int32 DefenseRate;
			Int32 Defense;
			Int32 MaxSlotCount;
			Int32 Str;
			Int32 Dex;
			Int32 Int;
			Int32 UnknownOptions1[6];
			Int32 DeltaStr;
			Int32 DeltaDex;
			Int32 DeltaInt;
			Int32 UnknownOptions2[6];
		} Armor;

		struct {
			Int32 MaybeGrade;
			Int32 Defense;
			Int32 MovementSpeed;
			Int32 MaxSlotCount;
			Int32 Unknown1;
			Int32 DefenseRate;
			Int32 UnknownOptions1[16];
		} VehicleBike;

		struct {
			Int32 ForceEffect1;
			Int32 ForceValue1;
			Int32 MinLevel;
			Int32 MaxSlotCount;
			Int32 ForceEffect2;
			Int32 ForceValue2;
			Int32 ForceEffect3;
			Int32 ForceValue3;
			Int32 ForceEffect4;
			Int32 ForceValue4;
			Int32 ForceEffect5;
			Int32 ForceValue5;
			Int32 UnknownOptions1;
			Int32 DeltaStr;
			Int32 DeltaDex;
			Int32 DeltaInt;
			Int32 UnknownOptions2[6];
		} Accessory;

		struct {
			Int32 SkillID;
		} SkillBook;

		struct {
			Int32 PotionType;
			Int32 Unknown1[3];
			Int32 PotionValue;
			Int32 Unknown2;
			Int32 PotionCooldown;
			Int32 Unknown3;
			Int32 StatType;
		} Potion;

		struct {
			Int32 IsPerfectCore;
			Int32 Unknown1[3];
			Int32 IsNormalCore;
		} UpgradeCore;

		struct {
			Int32 VehicleColor;
			Int32 Unknown1[4];
		} CoatingKit;

		struct {
			Int32 ForceEffectIndex;
			Int32 CooldownGroupIndex;
			Int32 Unknown1[2];
			Int32 PotionValue;
			Int32 Unknown2;
			Int32 Cooldown;
			Int32 Unknown3;
			Int32 Duration;
		} SpecialPotion;

		struct {
			Int32 Unknown1[6];
			Int32 SuccessRate;
			Int32 Unknown2;
			Int32 HasSafeguard;
		} RepairKit;

		struct {
			Int32 MinLevel;
			Int32 MaxLevel;
		} ExtremeSealStone;

		struct {
			Int32 TargetItemType;
			Int32 Padding0[4];
			Int32 MaxSlotCount;
			Int32 SuccessRate;
			Int32 Padding1;
			Int32 HasSafeguard;
		} SlotConverter;

		struct {
			Int32 TargetItemType;
			Int32 Padding0[3];
			Int32 RequiredItemLevel;
			Int32 RequiredSlotCount;
			Int32 SuccessRate;
			Int32 Padding1;
			Int32 HasSafeguard;
			Int32 PoolID;
		} EpicConverter;

		struct {
			Int32 TransformIndex;
		} TransformationCard;

		struct {
			Int32 PoolID;
			Int32 Unknown1;
		} Lottery;

		struct {
			Int32 UpgradeType;
		} CoreEnhancer;

		struct {
			Int32 AbilityType;
		} RuneSlotExtender;

		struct {
			Int32 Nation;
			Int32 Unknown1;
			Int32 Unknown2;
			Int32 Unknown3;
			Int32 Type;
			Int32 Unknown4;
			Int32 MaxEvaluationCount;
		} MeritMedal;

		struct {
			Int Index;
		} BlessingBead;

		// TODO: Add other item types like potion, pet, ...

		struct { Int32 Options[22]; };
	};

	union {
		struct {
			Int32 ForceEffect1;
			Int32 ForceValue1;
			Int32 ForceEffect2;
			Int32 ForceValue2;
			Int32 ForceEffect3;
			Int32 ForceValue3;
		} ExtendedAccessory;

		struct { Int32 ExtendedOptions[6]; };
	};

	Int32 MaxStackSize;
	Int32 UniqueGrade;
	Int32 MasterGrade;
	Int32 EnchantCodeLink;
	Int32 UniqueBattleStyleIndex;
};

struct _RTItem {
    union {
        struct { UInt32 ID; UInt32 Extension; };
        struct { UInt64 _1 : 12; UInt64 IsAccountBinding : 1; };  
		struct { UInt64 _2 : 18; UInt64 IsPremiumPcOnly : 1; UInt64 IsCharacterBinding : 1; UInt64 IsTradableBinding : 1; };
		struct { UInt64 _7 : 21; UInt64 VehicleColor : 3; };
		struct { UInt64 _3 : 23; UInt64 IsSpecialInventoryItem : 1; };
		//struct { UInt64 _4 : 26; UInt64 IsBroken : 1; UInt64 IsSealed : 1; };
		struct { UInt64 _4 : 27; UInt64 IsBroken : 1; };
		struct { UInt64 _5 : 13; UInt64 UpgradeLevel : 5; };
        struct { UInt64 _6 : 32; UInt64 ExtremeLevel : 4; UInt64 DivineLevel : 4; };
		struct { UInt32 PartyQuestItemID : 15; UInt32 PartyQuestItemStackable : 1; };
		struct { UInt64 _8 : 45; UInt64 IsProtected : 1; };
        struct { UInt64 Serial; };
    };
};
typedef struct _RTItem RTItem;

struct _RTItemOptionSlot {
	union {
		struct {
			UInt8 ForceIndex : 4;
			UInt8 ForceLevel : 3;
			UInt8 IsEpic : 1;
		};
		struct {
			UInt8 MasterIndex : 7;
		};
		struct {
			UInt8 ArtifactForceLevel : 4;
			UInt8 ArtifactForceIndex : 4;
		};
		UInt8 Serial;
	};
};
typedef struct _RTItemOptionSlot RTItemOptionSlot;

struct _RTItemOptionFrontierStone {
	UInt8 X;
	UInt8 Y;
	UInt8 WorldIndex;
	UInt8 Unknown1;
};
typedef struct _RTItemOptionFrontierStone RTItemOptionFrontierStone;

struct _RTItemOptions {
	union {
		struct { UInt64 Serial; };
		struct {
			RTItemOptionSlot Slots[RUNTIME_ITEM_MAX_OPTION_COUNT];

			struct {
				UInt8 ExtraForceIndex : 4;
				UInt8 SlotCount : 3;
				UInt8 _Padding : 1;
			};
			UInt32 _Padding1 : 32;
		} Equipment;

		struct {
			RTItemOptionSlot Slots[RUNTIME_ITEM_MAX_ARTIFACT_OPTION_COUNT];
		} Artifact;

		struct {
			UInt64 Level : 8;
			UInt64 Category : 8;
		} Seal;

		struct {
			UInt16 ItemLevel;
			UInt16 ItemGrade;
			UInt32 ItemCategory;
		} DivineSeal;

		struct {
			UInt16 ItemLevel;
			UInt16 ItemGroup;
			UInt32 ItemType;
		} ChaosSeal;

		struct {
			UInt8 Slot1 : 6;
			UInt8 Slot2 : 6;
			UInt8 Slot3 : 6;
			UInt8 Slot4 : 6;
			UInt8 Slot5 : 6;
			UInt8 Slot6 : 6;
			UInt8 Slot7 : 6;
			UInt8 Slot8 : 6;
			UInt8 Slot9 : 6;
			UInt8 Slot10 : 6;
			UInt8 Group : 4;
		} HonorMedalSeal;

		struct {
			UInt64 StackSize : 7;
			UInt64 ForceEffectIndex : 25;
		} OptionScroll;

		struct {
			UInt64 StackSize : 7;
			UInt64 Option : 25;
		} OptionStack;

		struct {
			UInt64 StackSize : 16;
			UInt64 EvaluationCount : 16;
		} MeritMedal;

		RTItemOptionFrontierStone FrontierStone;
	};
};
typedef struct _RTItemOptions RTItemOptions;

struct _RTItemDuration {
    union {
        struct { UInt32 Serial; };
        struct {
            UInt32 Year             : 7;
            UInt32 Month            : 4;
            UInt32 Day              : 5;
            UInt32 Hour             : 5;
            UInt32 Minute           : 6;
            UInt32 DurationIndex    : 5;
        };
    };
};
typedef struct _RTItemDuration RTItemDuration;

struct _RTItemHonorMedalSealData {
	Int32 Group;
	Int32 ForceEffectIndex[RUNTIME_CHARACTER_MAX_HONOR_MEDAL_SEAL_SLOT_COUNT];
};
typedef struct _RTItemHonorMedalSealData RTItemHonorMedalSealData;

struct _RTItemForceOptionData {
	Int32 EpicSlotCount; 
	RTItemOptionSlot EpicSlots[RUNTIME_ITEM_MAX_OPTION_COUNT];
	Int32 ForceSlotCount;
	Int32 FilledForceSlotCount;
	UInt8 ForceSlots[RUNTIME_ITEM_MAX_OPTION_COUNT];
};
typedef struct _RTItemForceOptionData RTItemForceOptionData;

struct _RTItemSlot {
    RTItem Item;
    UInt64 ItemSerial;
	UInt64 ItemOptions;
    UInt16 SlotIndex; // NOTE: Unused inside a party inventory.. @CleanUp
    RTItemDuration ItemDuration;
};

struct _RTItemSlotAppearance {
    UInt16 SlotIndex;
    UInt32 ItemID;
    UInt32 Duration;
};

Bool RTItemTypeIsProtectable(
	UInt32 ItemType
);

Int64 RTItemCalculateSellPrice(
	RTRuntimeRef Runtime,
	RTItemDataRef ItemData,
	RTItemSlotRef ItemSlot
);

RTItemHonorMedalSealData RTItemHonorMedalSealDecode(
	RTRuntimeRef Runtime,
	UInt64 Serial
);

UInt64 RTItemHonorMedalSealEncode(
	RTRuntimeRef Runtime,
	RTItemHonorMedalSealData Data
);

RTItemForceOptionData RTItemForceOptionDecode(
	UInt64 Serial
);

UInt64 RTItemForceOptionEncode(
	RTItemForceOptionData Data
);

UInt64 RTItemDataGetStackSizeMask(
	RTItemDataRef ItemData
);

UInt64 RTItemDataGetItemOptionsOffset(
	RTItemDataRef ItemData
);

Int32 RTItemUseInternal(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemSlotRef ItemSlot,
	RTItemDataRef ItemData,
	Void* Payload
);

Int32 RTItemOptionFirstEpicSlotIndex(
	RTItemOptions* Options
);

Int32 RTItemOptionLastEmptySlotIndex(
	RTItemOptions* Options
);

Bool RTItemOptionHasEpic(
	RTItemOptions Options
);

RTItemOptionSlot RTItemOptionGetLastFilledForceSlot(
	RTItemOptions Options
);

Int32 RTItemOptionGetForceSlotCount(
	RTItemOptions Options,
	Int32 ForceIndex
);

Int32 RTItemOptionGetFilledSlotCount(
	RTItemOptions Options
);

Bool RTItemOptionPushSlots(
	RTItemOptions* Options
);

Void RTItemOptionClearForceSlot(
	RTItemOptions* Options,
	Int Index
);

Void RTItemOptionClearSecondForceSlot(
	RTItemOptions* Options
);

Bool RTItemOptionAppendSlot(
	RTItemOptions* Options,
	RTItemOptionSlot Slot,
	Bool IsExtended
);

struct _RTItemConverterPayload {
	UInt16 TargetSlotIndex;
};

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemStub);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemPotion);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSkillBook);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemImmediateReward);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemCharacterSlotExtender);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemWeapon);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemArmor);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemAccessory);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemFrontierStone);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEffector);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemVehicleBike);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemVehicleBoard);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemCharm);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSpecialPotion);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSlotExtender);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSlotConverter);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEpicConverter);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemDivineConverter);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemChaosConverter);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEpicBooster);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemHolyWater);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemStackablePotion);

struct _RTItemHonorMedalResetSelectivePayload {
	UInt8 CategoryIndex;
	UInt8 GroupIndex;
	Int32 SlotIndex;
};

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemHonorMedalResetSelective);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemTransformationCard);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemChangeGender);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemCrest);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemLotto);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemCoreEnhancer);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemForceGemPackage);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemRemoteNpc);
RUNTIME_ITEM_PROCEDURE_BINDING(RTRuneSlotExtender);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEventLottery);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemBlessingBead);

#pragma pack(pop)

EXTERN_C_END
