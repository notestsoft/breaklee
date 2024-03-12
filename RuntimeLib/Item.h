#pragma once

#include "Base.h"

#define RUNTIME_ITEM_PROCEDURE_BINDING(__NAME__) \
Bool __NAME__(                                   \
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
	RUNTIME_ITEM_SUBTYPE_IMMEDIATE_REWARD_OXP = 13,
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

#define RUNTIME_ITEM_MASK_ACCOUNT_BINDING	0x1000
#define RUNTIME_ITEM_MASK_CHARACTER_BINDING 0x80000
#define RUNTIME_ITEM_MASK_TRADABLE_BINDING	0x100000
#define RUNTIME_ITEM_MASK_UPGRADE_LEVEL		(0x2000 | 0x4000 | 0x8000 | 0x10000 | 0x20000)
#define RUNTIME_ITEM_MASK_EXTREME_LEVEL     (0b1111 << 32)
#define RUNTIME_ITEM_MASK_DIVINE_LEVEL      (0b1111 << 36)
#define RUNTIME_ITEM_MASK_LEVEL             RUNTIME_ITEM_MASK_UPGRADE_LEVEL | RUNTIME_ITEM_MASK_EXTREME_LEVEL | RUNTIME_ITEM_MASK_DIVINE_LEVEL
#define RUNTIME_ITEM_MASK_INDEX				~(RUNTIME_ITEM_MASK_ACCOUNT_BINDING | RUNTIME_ITEM_MASK_CHARACTER_BINDING | RUNTIME_ITEM_MASK_TRADABLE_BINDING | RUNTIME_ITEM_MASK_UPGRADE_LEVEL)

#define RUNTIME_QUEST_ITEM_INDEX_MASK		0x000FFF80
#define RUNTIME_QUEST_ITEM_INDEX_EXPONENT	7
#define RUNTIME_QUEST_ITEM_COUNT_MASK		0x0000007F

#define RTQuestItemOptions(Index, Count) ((Index << RUNTIME_QUEST_ITEM_INDEX_EXPONENT) | Count)
#define RTQuestItemGetOptions(ItemOptions) ((ItemOptions & RUNTIME_QUEST_ITEM_INDEX_MASK) >> RUNTIME_QUEST_ITEM_INDEX_EXPONENT)
#define RTQuestItemGetCount(ItemOptions) (ItemOptions & RUNTIME_QUEST_ITEM_COUNT_MASK)

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
			Int32 Unknown1;
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
			Int32 Unknown2;
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
			Int32 ForceEffect1;
			Int32 ForceValue1;
			Int32 MinLevel;
			Int32 Unknown2;
			Int32 ForceEffect2;
			Int32 ForceValue2;
			Int32 ForceEffect3;
			Int32 ForceValue3;
			Int32 ForceEffect4;
			Int32 ForceValue4;
			// TODO: Check if there can be more then 4 options on ring or other accessories...
			Int32 UnknownOptions1[3];
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
			Int32 Unknown2;
			Int32 Unknown3[3];
			Int32 StatType;
		} Potion;

		struct {
			Int32 IsPerfectCore;
			Int32 Unknown1[3];
			Int32 IsNormalCore;
		} UpgradeCore;

		// TODO: Add other item types like potion, pet, ...

		struct { Int32 Options[22]; };
	};

    Int32 Width;
    Int32 Height;
	Int32 MaxStackSize;
};

struct _RTItem {

    union {
        struct { UInt32 ID; UInt32 Extension; };
        struct { UInt64 _1 : 12; UInt64 IsAccountBinding : 1; };  
        struct { UInt64 _2 : 19; UInt64 IsCharacterBinding : 1; UInt64 IsTradableBinding : 1; };
        struct { UInt64 _3 : 13; UInt64 UpgradeLevel : 5; };
        struct { UInt64 _4 : 32; UInt64 ExtremeLevel : 4; UInt64 DivineLevel : 4; };
		struct { UInt32 PartyQuestItemID : 15; UInt32 PartyQuestItemStackable : 1; };
        struct { UInt64 Serial; };
    };
};
typedef struct _RTItem RTItem;

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

struct _RTItemSlot {
    RTItem Item;
    UInt64 Unknown1;
	UInt64 ItemOptions;
    UInt16 SlotIndex; // NOTE: Unused inside a party inventory.. @CleanUp
    RTItemDuration ItemDuration;
};

struct _RTItemSlotAppearance {
    UInt16 SlotIndex;
    UInt32 ItemID;
    UInt32 Duration;
};

#pragma pack(pop)

Bool RTItemUseInternal(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTItemSlotRef ItemSlot,
	RTItemDataRef ItemData,
	Void* Payload
);

UInt32 RTItemGetSerialID(
	RTItem Item
);

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemStub);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemPotion);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSkillBook);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemImmediateReward);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemWeapon);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemArmor);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemAccessory);
RUNTIME_ITEM_PROCEDURE_BINDING(RTItemEffector);

struct _RTItemSlotExtenderPayload {
	UInt16 TargetSlotIndex;
};

RUNTIME_ITEM_PROCEDURE_BINDING(RTItemSlotExtender);

EXTERN_C_END
