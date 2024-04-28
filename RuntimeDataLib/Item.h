#ifndef RUNTIME_DATA_ITEM_TYPE_INDEX
#define RUNTIME_DATA_ITEM_TYPE_INDEX

enum {
	RUNTIME_ITEM_TYPE_UPGRADE_CORE = 0,
	RUNTIME_ITEM_TYPE_FORCE_CORE = 1,
	RUNTIME_ITEM_TYPE_DT_CARD = 2,
	RUNTIME_ITEM_TYPE_DUNGEON_COUNT = 3,
	RUNTIME_ITEM_TYPE_RETURN_STONE = 4,
	RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER = 5,
	RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND = 6,
	RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND = 7,
	RUNTIME_ITEM_TYPE_SUIT = 8,
	RUNTIME_ITEM_TYPE_GLOVES = 9,
	RUNTIME_ITEM_TYPE_BOOTS = 10,
	RUNTIME_ITEM_TYPE_HELMED1 = 11,
	RUNTIME_ITEM_TYPE_HELMED2 = 12,
	RUNTIME_ITEM_TYPE_CURRENCY = 13,
	RUNTIME_ITEM_TYPE_POTION = 14,
	RUNTIME_ITEM_TYPE_RING = 15,
	RUNTIME_ITEM_TYPE_AMULET = 16,
	RUNTIME_ITEM_TYPE_EPAULET = 17,
	RUNTIME_ITEM_TYPE_QUEST_S = 18,
	RUNTIME_ITEM_TYPE_QUEST_N = 19,
	RUNTIME_ITEM_TYPE_DUNGEON_MISSION = 20,
	RUNTIME_ITEM_TYPE_AUTO_TRAP = 21,
	RUNTIME_ITEM_TYPE_VEHICLE_BIKE = 22,
	RUNTIME_ITEM_TYPE_SKILL_CIRCUIT = 23,
	RUNTIME_ITEM_TYPE_COATING_KIT = 24,
	RUNTIME_ITEM_TYPE_EVENT = 25,
	RUNTIME_ITEM_TYPE_SERIAL_S = 26,
	RUNTIME_ITEM_TYPE_SPECIAL_POTION = 27,
	RUNTIME_ITEM_TYPE_FORMULA_CARD = 28,
	RUNTIME_ITEM_TYPE_SLOT_EXTENDER = 29,
	RUNTIME_ITEM_TYPE_TRANSMUTER = 30,
	RUNTIME_ITEM_TYPE_CRAFT_LICENSE = 31,
	RUNTIME_ITEM_TYPE_EARRING = 32,
	RUNTIME_ITEM_TYPE_BRACELET = 33,
	RUNTIME_ITEM_TYPE_LOTTO = 34,
	RUNTIME_ITEM_TYPE_CORE_ENHANCER = 35,
	RUNTIME_ITEM_TYPE_FX_ITEM_S = 36,
	RUNTIME_ITEM_TYPE_FX_ITEM_N = 37,
	RUNTIME_ITEM_TYPE_BLESSING_BEAD = 38,
	RUNTIME_ITEM_TYPE_RETURN_CORE = 39,
	RUNTIME_ITEM_TYPE_SUMMON_CARD = 40,
	RUNTIME_ITEM_TYPE_SPECIAL_WARP_CARD = 41,
	RUNTIME_ITEM_TYPE_RESURRECTION_POTION = 42,
	RUNTIME_ITEM_TYPE_CURE_POTION = 43,
	RUNTIME_ITEM_TYPE_CHANGE_KIT = 44,
	RUNTIME_ITEM_TYPE_RENAME_CARD = 45,
	RUNTIME_ITEM_TYPE_REMOTE_SHOP = 46,
	RUNTIME_ITEM_TYPE_TREASURE_KIT = 47,
	RUNTIME_ITEM_TYPE_SUMMONING_KIT = 48,
	RUNTIME_ITEM_TYPE_EPAULET2 = 49,
	RUNTIME_ITEM_TYPE_GUILD_EMBLEM = 50,
	RUNTIME_ITEM_TYPE_SUMMONING_PET = 51,
	RUNTIME_ITEM_TYPE_TR_FX_ITEM_S = 52,
	RUNTIME_ITEM_TYPE_TR_FX_ITEM_N = 53,
	RUNTIME_ITEM_TYPE_AVATAR = 54,
	RUNTIME_ITEM_TYPE_DIALOGUE_BUBBLE = 55,
	RUNTIME_ITEM_TYPE_GENDER_CHANGE = 56,
	RUNTIME_ITEM_TYPE_REMOTE_SHOP2 = 57,
	RUNTIME_ITEM_TYPE_RECALL_FORCE_TOWER = 58,
	RUNTIME_ITEM_TYPE_LINK_WEAPON = 59,
	RUNTIME_ITEM_TYPE_RECOVERY_KIT = 60,
	RUNTIME_ITEM_TYPE_PET_RENAME_CARD = 61,
	RUNTIME_ITEM_TYPE_PET_SEAL = 62,
	RUNTIME_ITEM_TYPE_PET_BUFF_REMOVER = 63,
	RUNTIME_ITEM_TYPE_GAMBLE_TICKET = 64,
	RUNTIME_ITEM_TYPE_PET_CHANGE_KIT = 65,
	RUNTIME_ITEM_TYPE_REMOTE_AGENT_SHOP = 66,
	RUNTIME_ITEM_TYPE_EVENT_S = 67,
	RUNTIME_ITEM_TYPE_EVENT_N = 68,
	RUNTIME_ITEM_TYPE_EVENT_LOTTERY = 69,
	RUNTIME_ITEM_TYPE_FONT_S = 70,
	RUNTIME_ITEM_TYPE_ABILITY_RUNE_ESSENCE = 71,
	RUNTIME_ITEM_TYPE_EVENT_FORMULA_CARD = 72,
	RUNTIME_ITEM_TYPE_PET_SAFETY_KIT = 73,
	RUNTIME_ITEM_TYPE_ANIMA_GEM = 74,
	RUNTIME_ITEM_TYPE_ABILITY_RUNE_BLENDED = 75,
	RUNTIME_ITEM_TYPE_WARP_STONE = 76,
	RUNTIME_ITEM_TYPE_SKILLBOOK = 77,
	RUNTIME_ITEM_TYPE_BELT = 78,
	RUNTIME_ITEM_TYPE_PREMIUM_ITEM = 79,
	RUNTIME_ITEM_TYPE_AGENT_SHOP_COUPON = 80,
	RUNTIME_ITEM_TYPE_MEGAPHONE = 81,
	RUNTIME_ITEM_TYPE_FRONTIER_STONE = 82,
	RUNTIME_ITEM_TYPE_MULTIBUFF_POTION = 83,
	RUNTIME_ITEM_TYPE_REMOTE_WAREHOUSE = 84,
	RUNTIME_ITEM_TYPE_EXPAND_PET_SLOT = 85,
	RUNTIME_ITEM_TYPE_GIFTBOX_SEAL = 86,
	RUNTIME_ITEM_TYPE_SUMMON_PARTY_MEMBER = 87,
	RUNTIME_ITEM_TYPE_RESURRECT_PARTY_MEMBER = 88,
	RUNTIME_ITEM_TYPE_GIFT_VOUCHER = 89,
	RUNTIME_ITEM_TYPE_YIELD_EXP_TO_PARTY = 90,
	RUNTIME_ITEM_TYPE_ABSORB_DMG_FOR_PARTY = 91,
	RUNTIME_ITEM_TYPE_DECORATE_SHOP = 92,
	RUNTIME_ITEM_TYPE_SAFEGUARD = 93,
	RUNTIME_ITEM_TYPE_MERCENARY_CARD = 94,
	RUNTIME_ITEM_TYPE_PROTECTION_OF_VERADRIX = 95,
	RUNTIME_ITEM_TYPE_DAMAGE_AMPLIFIER = 96,
	RUNTIME_ITEM_TYPE_BIKE_UPGRADE_CORE = 97,
	RUNTIME_ITEM_TYPE_BIKE_FORCE_CORE = 98,
	RUNTIME_ITEM_TYPE_BIKE_SLOT_EXTENDER = 99,
	RUNTIME_ITEM_TYPE_BIKE_ENHANCER = 100,
	RUNTIME_ITEM_TYPE_IMMEDIATE_REWARD = 101,
	RUNTIME_ITEM_TYPE_FORCE_CORE_ORDER_SHEET = 102,
	RUNTIME_ITEM_TYPE_ARMOR_SWITCH = 103,
	RUNTIME_ITEM_TYPE_VEHICLE_BOARD = 104,
	RUNTIME_ITEM_TYPE_CHANGE_APPEARANCE = 105,
	RUNTIME_ITEM_TYPE_PARTY_MEMBER_WARP = 106,
	RUNTIME_ITEM_TYPE_POTION_N = 107,
	RUNTIME_ITEM_TYPE_MULTIBUFF_POTION_N = 108,
	RUNTIME_ITEM_TYPE_QUEST_CAPSULE = 109,
	RUNTIME_ITEM_TYPE_POINT_SAVE = 110,
	RUNTIME_ITEM_TYPE_PET_UNTRAIN_KIT_OPTION_SELECTIVE = 111,
	RUNTIME_ITEM_TYPE_GUILD_RENAME_CARD = 112,
	RUNTIME_ITEM_TYPE_GUILD_CREATION = 113,
	RUNTIME_ITEM_TYPE_GUILD_LEVEL_UP = 114,
	RUNTIME_ITEM_TYPE_OPTION_SCROLL_PROTECTER = 115,
	RUNTIME_ITEM_TYPE_ITEM_OPTION_REMOVER = 116,
	RUNTIME_ITEM_TYPE_CHARM = 117,
	RUNTIME_ITEM_TYPE_SLOT_CONVERTER = 118,
	RUNTIME_ITEM_TYPE_EPIC_CONVERTER = 119,
	RUNTIME_ITEM_TYPE_BINDING_CONVERTER = 120,
	RUNTIME_ITEM_TYPE_PRISON_BREAK_ITEM = 121,
	RUNTIME_ITEM_TYPE_LOTTO_S = 122,
	RUNTIME_ITEM_TYPE_EXTREME_CORE = 123,
	RUNTIME_ITEM_TYPE_REPAIR_KIT = 124,
	RUNTIME_ITEM_TYPE_ACCOUNT_BIND_SEAL = 125,
	RUNTIME_ITEM_TYPE_EFFECTOR = 126,
	RUNTIME_ITEM_TYPE_MIRROR_OF_OBSERVATION = 127,
	RUNTIME_ITEM_TYPE_DAY_OF_WEEK_DUNGEON = 128,
	RUNTIME_ITEM_TYPE_OATH_OF_SOUL = 129,
	RUNTIME_ITEM_TYPE_CHARACTER_OPEN = 130,
	RUNTIME_ITEM_TYPE_CHAKRAM = 131,
	RUNTIME_ITEM_TYPE_CHAOS_CORE = 132,
	RUNTIME_ITEM_TYPE_MONSTER_SUMMON_STONE = 133,
	RUNTIME_ITEM_TYPE_MERITORIOUS_MEDAL = 134,
	RUNTIME_ITEM_TYPE_MERITORIOUS_MEDAL_AUXILIARY_ITEM = 135,
	RUNTIME_ITEM_TYPE_EPIC_BOOSTER = 136,
	RUNTIME_ITEM_TYPE_POTION_OF_WAR = 137,
	RUNTIME_ITEM_TYPE_FORCE_GEM = 138,
	RUNTIME_ITEM_TYPE_FORCE_GEM_PACKAGE = 139,
	RUNTIME_ITEM_TYPE_TALISMAN = 140,
	RUNTIME_ITEM_TYPE_DUMMY_COSTUME = 141,
	RUNTIME_ITEM_TYPE_COSTUME_SAFETY_KIT = 142,
	RUNTIME_ITEM_TYPE_COSTUME_SEAL_STONE = 143,
	RUNTIME_ITEM_TYPE_UNIQUE_COSTUME = 144,
	RUNTIME_ITEM_TYPE_UPGRADE_TOKEN = 145,
	RUNTIME_ITEM_TYPE_HEADGEAR3 = 146,
	RUNTIME_ITEM_TYPE_WAR_COIN = 147,
	RUNTIME_ITEM_TYPE_RUNE_SLOT_EXTENDER = 148,
	RUNTIME_ITEM_TYPE_CARNELIAN = 149,
	RUNTIME_ITEM_TYPE_BATTLE_STLYE_CHANGE_KIT = 150,
	RUNTIME_ITEM_TYPE_MERCENARY_CARD_PIECE = 151,
	RUNTIME_ITEM_TYPE_POTION_BUFF_SKILL = 152,
	RUNTIME_ITEM_TYPE_GROUP_BINDING_CONVERTER = 153,
	RUNTIME_ITEM_TYPE_BOSS_ARENA_TOKEN = 154,
	RUNTIME_ITEM_TYPE_ARCANA = 155,
	RUNTIME_ITEM_TYPE_CHAOS_SAFEGUARD = 156,
	RUNTIME_ITEM_TYPE_DIVINE_CONVERTER = 157,
	RUNTIME_ITEM_TYPE_DIVINE_CORE = 158,
	RUNTIME_ITEM_TYPE_DIVINE_SEAL_STONE = 159,
	RUNTIME_ITEM_TYPE_EXTREME_SEAL_STONE = 160,
	RUNTIME_ITEM_TYPE_MEDAL_SEAL_STONE = 161,
	RUNTIME_ITEM_TYPE_DIVINE_SAFE_GUARD = 162,
	RUNTIME_ITEM_TYPE_COSTUME_EPIC_SEAL_STONE = 163,
	RUNTIME_ITEM_TYPE_COSTUME_BAG = 164,
	RUNTIME_ITEM_TYPE_HONOR_MEDAL_RESET_SELECTIVE = 165,
	RUNTIME_ITEM_TYPE_MERCENARY_SEAL_KIT = 166,
	RUNTIME_ITEM_TYPE_CHAOS_CONVERTER = 167,
	RUNTIME_ITEM_TYPE_CHAOS_SEAL_STONE = 168,
	RUNTIME_ITEM_TYPE_HOLY_WATER = 169,
	RUNTIME_ITEM_TYPE_NATION_CHANGE_ITEM = 170,
	RUNTIME_ITEM_TYPE_UPGRADE_CORE_SET = 171,
	RUNTIME_ITEM_TYPE_CHAOS_CORE_SET = 172,
	RUNTIME_ITEM_TYPE_DIVINE_CORE_SET = 173,
	RUNTIME_ITEM_TYPE_HONOR_MEDAL_SEAL_CHEST = 174,
	RUNTIME_ITEM_TYPE_POTION_STACKABLE = 175,
	RUNTIME_ITEM_TYPE_INGREDIENTS_S = 176,
	RUNTIME_ITEM_TYPE_NORMAL_S = 177,
	RUNTIME_ITEM_TYPE_TRANSFORMATION_CARD = 178,
	RUNTIME_ITEM_TYPE_TIME_REDUCER = 179,
	RUNTIME_ITEM_TYPE_SPECIAL_MASTERY_CHEST = 180,
	RUNTIME_ITEM_TYPE_MEMORIZE_EXTENDER = 181,
	RUNTIME_ITEM_TYPE_COLLECTION_CHEST = 182,
	RUNTIME_ITEM_TYPE_BROOCH = 183,
	RUNTIME_ITEM_TYPE_GACHA_TICKET = 184,
	RUNTIME_ITEM_TYPE_MATERIAL_ITEM_FOR_IMPRINT = 185,
	RUNTIME_ITEM_TYPE_ABILITY_RUNE_KARMA = 186,
	RUNTIME_ITEM_TYPE_PROPERTY_RESTORATION_ITEM = 187,
	RUNTIME_ITEM_TYPE_NFT_COUPON = 188,

	RUNTIME_ITEM_TYPE_COUNT,
};

enum {
	RUNTIME_ITEM_CATEGORY_WEAPON_FORCE_CONTROLLER	= 1,
	RUNTIME_ITEM_CATEGORY_WEAPON_ONE_HAND			= 2,
	RUNTIME_ITEM_CATEGORY_WEAPON_TWO_HAND			= 3,
	RUNTIME_ITEM_CATEGORY_SUIT						= 4,
	RUNTIME_ITEM_CATEGORY_GLOVES					= 5,
	RUNTIME_ITEM_CATEGORY_BOOTS						= 6,
	RUNTIME_ITEM_CATEGORY_HELMED					= 7,
	RUNTIME_ITEM_CATEGORY_VEHICLE_BIKE				= RUNTIME_ITEM_TYPE_VEHICLE_BIKE,
	RUNTIME_ITEM_CATEGORY_CHAKRAM					= RUNTIME_ITEM_TYPE_CHAKRAM,
};

enum {
	RUNTIME_ITEM_GROUP_WEAPON_ONE_HAND = 1,
	RUNTIME_ITEM_GROUP_WEAPON_TWO_HAND = 2,
	RUNTIME_ITEM_GROUP_ARMOR = 3,
	RUNTIME_ITEM_GROUP_VEHICLE = 4,
};

typedef UInt32 RTDataItemType;
typedef Int32 RTDataItemTypeGrade;
typedef UInt32 RTDataItemCategory;
typedef UInt32 RTDataItemGroup;

Bool ParseAttributeRTDataItemType(
	ArchiveRef Object,
	Int64 NodeIndex,
	CString Name,
	RTDataItemType* Result
);

Bool ParseAttributeRTDataItemTypeGrade(
	ArchiveRef Object,
	Int64 NodeIndex,
	CString Name,
	RTDataItemTypeGrade* Result
);

Bool ParseAttributeRTDataItemCategory(
	ArchiveRef Object,
	Int64 NodeIndex,
	CString Name,
	RTDataItemCategory* Result
);

Bool ParseAttributeRTDataItemGroup(
	ArchiveRef Object,
	Int64 NodeIndex,
	CString Name,
	RTDataItemGroup* Result
);

#endif
