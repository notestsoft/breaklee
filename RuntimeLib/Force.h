#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
	RUNTIME_FORCE_EFFECT_NONE = 0,
	RUNTIME_FORCE_EFFECT_HP_UP = 1,
	RUNTIME_FORCE_EFFECT_MP_UP = 2,
	RUNTIME_FORCE_EFFECT_ATTACK_UP = 3,
	RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP = 4,
	RUNTIME_FORCE_EFFECT_DEFENSE_UP = 5,
	RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP = 6,
	RUNTIME_FORCE_EFFECT_DEFENSE_RATE_UP = 7,
	RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_UP = 8,
	RUNTIME_FORCE_EFFECT_CRITICAL_RATE_UP = 9,
	RUNTIME_FORCE_EFFECT_MIN_DAMAGE_UP = 10,
	RUNTIME_FORCE_EFFECT_MIN_DAMAGE_UP2 = 11,
	RUNTIME_FORCE_EFFECT_HP_ABSORB_LIMIT_UP = 12,
	RUNTIME_FORCE_EFFECT_MP_ABSORB_LIMIT_UP = 13,
	RUNTIME_FORCE_EFFECT_UNKNOWN_14 = 14,
	RUNTIME_FORCE_EFFECT_UNKNOWN_15 = 15,
	RUNTIME_FORCE_EFFECT_UNKNOWN_16 = 16,
	RUNTIME_FORCE_EFFECT_UNKNOWN_17 = 17,
	RUNTIME_FORCE_EFFECT_UNKNOWN_18 = 18,
	RUNTIME_FORCE_EFFECT_UNKNOWN_19 = 19,
	RUNTIME_FORCE_EFFECT_UNKNOWN_20 = 20,
	RUNTIME_FORCE_EFFECT_UNKNOWN_21 = 21,
	RUNTIME_FORCE_EFFECT_UNKNOWN_22 = 22,
	RUNTIME_FORCE_EFFECT_ATTACK_UP2 = 23,
	RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP2 = 24,
	RUNTIME_FORCE_EFFECT_CRITICAL_RATE_LIMIT_UP = 25,
	RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP2 = 26,
	RUNTIME_FORCE_EFFECT_UNKNOWN_27 = 27,
	RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_UP2 = 28,
	RUNTIME_FORCE_EFFECT_CRITICAL_RATE_UP2 = 29,
	RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_UP = 30,
	RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_UP2 = 31,
	RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_UP = 32,
	RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_UP2 = 33,
	RUNTIME_FORCE_EFFECT_HP_ABSORB = 34,
	RUNTIME_FORCE_EFFECT_MP_ABSORB = 35,
	RUNTIME_FORCE_EFFECT_EVADE = 36,
	RUNTIME_FORCE_EFFECT_HP_REGEN = 37,
	RUNTIME_FORCE_EFFECT_MP_REGEN = 38,
	RUNTIME_FORCE_EFFECT_ADD_DAMAGE = 39,
	RUNTIME_FORCE_EFFECT_ADD_DAMAGE2 = 40,
	RUNTIME_FORCE_EFFECT_SKILL_EXP_UP = 41,
	RUNTIME_FORCE_EFFECT_SKILL_EXP_UP2 = 42,
	RUNTIME_FORCE_EFFECT_ALZ_DROP_UP = 43,
	RUNTIME_FORCE_EFFECT_ALZ_DROP_UP2 = 44,
	RUNTIME_FORCE_EFFECT_ADD_STAT_STR = 45,
	RUNTIME_FORCE_EFFECT_ADD_STAT_INT = 46,
	RUNTIME_FORCE_EFFECT_ADD_STAT_DEX = 47,
	RUNTIME_FORCE_EFFECT_ADD_STAT_ALL = 48,
	RUNTIME_FORCE_EFFECT_ONE_SLOT_DROP_RATE_UP = 49,
	RUNTIME_FORCE_EFFECT_ONE_SLOT_DROP_RATE_UP2 = 50,
	RUNTIME_FORCE_EFFECT_TWO_SLOT_DROP_RATE_UP = 51,
	RUNTIME_FORCE_EFFECT_TWO_SLOT_DROP_RATE_UP2 = 52,
	RUNTIME_FORCE_EFFECT_UNKNOWN_53 = 53,
	RUNTIME_FORCE_EFFECT_RELEASE_UNMOVABLE_STATUS = 54,
	RUNTIME_FORCE_EFFECT_EVADE_DEBUFF_SKILL = 55,
	RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_INCREASED = 56,
	RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_DECREASED = 57,
	RUNTIME_FORCE_EFFECT_ALL_ATTACK_UP = 58,
	RUNTIME_FORCE_EFFECT_DEFENSE = 59,
	RUNTIME_FORCE_EFFECT_HP_UP2 = 60,
	RUNTIME_FORCE_EFFECT_ENERVATION = 61,
	RUNTIME_FORCE_EFFECT_STUN = 62,
	RUNTIME_FORCE_EFFECT_HEAL = 63,
	RUNTIME_FORCE_EFFECT_DOWN = 64,
	RUNTIME_FORCE_EFFECT_KNOCK_BACK = 65,
	RUNTIME_FORCE_EFFECT_IMMOBILITY = 66,
	RUNTIME_FORCE_EFFECT_ABSORB_DAMAGE = 67,
	RUNTIME_FORCE_EFFECT_REFLECT_DAMAGE = 68,
	RUNTIME_FORCE_EFFECT_CRAFT_MASTERY_UP = 69,
	RUNTIME_FORCE_EFFECT_INCREASE_BATTLE_MODE_DURATION = 70,
	RUNTIME_FORCE_EFFECT_IGNORE_EVASION = 71,
	RUNTIME_FORCE_EFFECT_IGNORE_ACCURACY = 72,
	RUNTIME_FORCE_EFFECT_IGNORE_DAMAGE_REDUCTION = 73,
	RUNTIME_FORCE_EFFECT_IGNORE_PENETRATION = 74,
	RUNTIME_FORCE_EFFECT_CANCEL_IGNORE_EVASION = 75,
	RUNTIME_FORCE_EFFECT_AXP_BONUS = 76,
	RUNTIME_FORCE_EFFECT_SKILL_EXP_BONUS = 77,
	RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION = 78,
	RUNTIME_FORCE_EFFECT_ACCURACY = 79,
	RUNTIME_FORCE_EFFECT_PENETRATION = 80,
	RUNTIME_FORCE_EFFECT_CANCEL_IGNORE_DAMAGE_REDUCTION = 81,
	RUNTIME_FORCE_EFFECT_SUPPRESSION = 82,
	RUNTIME_FORCE_EFFECT_RESIST_SUPPRESSION = 83,
	RUNTIME_FORCE_EFFECT_SUPPRESSION2 = 84,
	RUNTIME_FORCE_EFFECT_RESIST_SILENCE = 85,
	RUNTIME_FORCE_EFFECT_ROOT = 86,
	RUNTIME_FORCE_EFFECT_SILENCE = 87,
	RUNTIME_FORCE_EFFECT_INCREASE_SP_REGEN = 88,
	RUNTIME_FORCE_EFFECT_DECREASE_SP_USE = 89,
	RUNTIME_FORCE_EFFECT_UNKNOWN_90 = 90,
	RUNTIME_FORCE_EFFECT_MEZZ_IMMUNE = 91,
	RUNTIME_FORCE_EFFECT_COMPLETE_EVASION = 92,
	RUNTIME_FORCE_EFFECT_INCREASE_AGGRO = 93,
	RUNTIME_FORCE_EFFECT_UNKNOWN_94 = 94,
	RUNTIME_FORCE_EFFECT_HP_DOWN = 95,
	RUNTIME_FORCE_EFFECT_MP_DOWN = 96,
	RUNTIME_FORCE_EFFECT_RESIST_CRITICAL_RATE = 97,
	RUNTIME_FORCE_EFFECT_RESIST_CRITICAL_DAMAGE = 98,
	RUNTIME_FORCE_EFFECT_RESIST_IMMOBILITY = 99,
	RUNTIME_FORCE_EFFECT_REFLECT_DAMAGE_2 = 100,
	RUNTIME_FORCE_EFFECT_RESIST_DOWN = 101,
	RUNTIME_FORCE_EFFECT_RESIST_KNOCK_BACK = 102,
	RUNTIME_FORCE_EFFECT_RESIST_STUN = 103,
	RUNTIME_FORCE_EFFECT_INVINCIBLE_MODE = 104,
	RUNTIME_FORCE_EFFECT_WAR_EXP_BONUS = 105,
	RUNTIME_FORCE_EFFECT_PARTY_EXP_BONUS = 106,
	RUNTIME_FORCE_EFFECT_EXP_BONUS = 107,
	RUNTIME_FORCE_EFFECT_PET_EXP_BONUS = 108,
	RUNTIME_FORCE_EFFECT_ALZ_DROP_RATE_INCREASE = 109,
	RUNTIME_FORCE_EFFECT_ALZ_BOMB_RATE_INCREASE = 110,
	RUNTIME_FORCE_EFFECT_UNKNOWN_111 = 111,
	RUNTIME_FORCE_EFFECT_RESIST_SKILL_AMP = 112,
	RUNTIME_FORCE_EFFECT_ALL_ATTACK_UP2 = 113,
	RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_UP = 114,
	RUNTIME_FORCE_EFFECT_ALL_ATTACK_UP3 = 115,
	RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_UP2 = 116,
	RUNTIME_FORCE_EFFECT_INCREASE_MOVEMENT_SPEED = 117,
	RUNTIME_FORCE_EFFECT_SUPPRESSION_RATE = 118,
	RUNTIME_FORCE_EFFECT_SUPPRESSION_DURATION = 119,
	RUNTIME_FORCE_EFFECT_SILENCE_RATE = 120,
	RUNTIME_FORCE_EFFECT_SILENCE_DURATION = 121,
	RUNTIME_FORCE_EFFECT_ROOT_RATE = 122,
	RUNTIME_FORCE_EFFECT_ROOT_DURATION = 123,
	RUNTIME_FORCE_EFFECT_BOOST_HP_RESTORATION = 124,
	RUNTIME_FORCE_EFFECT_IGNORE_RESIST_CRITICAL_RATE = 125,
	RUNTIME_FORCE_EFFECT_RESIST_ABSOLUTE_DAMAGE = 126,
	RUNTIME_FORCE_EFFECT_IGNORE_RESIST_CRITICAL_DAMAGE = 127,
	RUNTIME_FORCE_EFFECT_IGNORE_RESIST_SKILL_AMP = 128,
	RUNTIME_FORCE_EFFECT_IGNORE_RESIST_DOWN = 129,
	RUNTIME_FORCE_EFFECT_IGNORE_RESIST_KNOCK_BACK = 130,
	RUNTIME_FORCE_EFFECT_IGNORE_RESIST_STUN = 131,
	RUNTIME_FORCE_EFFECT_NORMAL_DAMAGE_UP = 132,
	RUNTIME_FORCE_EFFECT_ADD_FORCE_WING_DAMAGE = 133,
	RUNTIME_FORCE_EFFECT_ACTIVATE_WING_DAMAGE = 134,
	RUNTIME_FORCE_EFFECT_ACTIVATE_WING_PROTECT = 135,
	RUNTIME_FORCE_EFFECT_ABSOLUTE_EVASION = 136,
	RUNTIME_FORCE_EFFECT_ABSOLUTE_BLOCK = 137,
	RUNTIME_FORCE_EFFECT_AURA_MODE_DURATION_INCREASE = 138,
	RUNTIME_FORCE_EFFECT_CANCEL_IGNORE_PENETRATION = 139,
	RUNTIME_FORCE_EFFECT_WAR_EXP_UP2 = 140,
	RUNTIME_FORCE_EFFECT_FORCE_WING_EXP_UP = 141,
	RUNTIME_FORCE_EFFECT_INCREASED_BOX_DROP_RATE = 142,
	RUNTIME_FORCE_EFFECT_RESIST_AUTO_DAMAGE = 143,
	RUNTIME_FORCE_EFFECT_INCREASE_MAX_BP = 144,
	RUNTIME_FORCE_EFFECT_DECREASE_BP_CONSUMPTION = 145,
	RUNTIME_FORCE_EFFECT_UNKNOWN_146 = 146,
	RUNTIME_FORCE_EFFECT_UNKNOWN_147 = 147,
	RUNTIME_FORCE_EFFECT_PVP_RESIST_MAGIC_SKILL_AMP = 148,
	RUNTIME_FORCE_EFFECT_PVP_RESIST_SWORD_SKILL_AMP = 149,
	RUNTIME_FORCE_EFFECT_PVP_RESIST_ALL_SKILL_AMP = 150,
	RUNTIME_FORCE_EFFECT_PVE_RESIST_MAGIC_SKILL_AMP = 151,
	RUNTIME_FORCE_EFFECT_PVE_RESIST_SWORD_SKILL_AMP = 152,
	RUNTIME_FORCE_EFFECT_PVE_ALL_SKILL_AMP_UP = 153,
	RUNTIME_FORCE_EFFECT_NUMBER_OF_TARGET_FOR_VALID_SKILL = 154,
	RUNTIME_FORCE_EFFECT_PVE_DEFENSE_UP = 155,
	RUNTIME_FORCE_EFFECT_PVE_DAMAGE_REDUCTION = 156,
	RUNTIME_FORCE_EFFECT_PVE_IGNORE_PENETRATION = 157,
	RUNTIME_FORCE_EFFECT_PVE_CRITICAL_DAMAGE = 158,
	RUNTIME_FORCE_EFFECT_INCREASE_RESTORATION_AMOUNT = 159,
	RUNTIME_FORCE_EFFECT_DEDUCT_PROTECTION = 160,
	RUNTIME_FORCE_EFFECT_FLUIDIZATION = 161,
	RUNTIME_FORCE_EFFECT_DMG_REDUCTION = 162,
	RUNTIME_FORCE_EFFECT_PVE_PENETRATION = 163,
	RUNTIME_FORCE_EFFECT_FURY_EFFECT_OBTAINED = 164,
	RUNTIME_FORCE_EFFECT_REDUCE_RESTORATION_AMOUNT = 165,
	RUNTIME_FORCE_EFFECT_PREVENT_HP_RESTORATION = 166,
	RUNTIME_FORCE_EFFECT_MOVEMENT_SPEED = 167,
	RUNTIME_FORCE_EFFECT_PETRIFIED = 168,
	RUNTIME_FORCE_EFFECT_TARGET_FIX = 169,
	RUNTIME_FORCE_EFFECT_RESIST_IRRECOVERABLE_CONDITION = 170,
	RUNTIME_FORCE_EFFECT_HP = 171,
	RUNTIME_FORCE_EFFECT_PVP_ALL_SKILL_AMP_UP = 172,
	RUNTIME_FORCE_EFFECT_PVP_CRITICAL_DAMAGE = 173,
	RUNTIME_FORCE_EFFECT_PVP_MAGIC_SKILL_AMP = 174,
	RUNTIME_FORCE_EFFECT_PVP_SWORD_SKILL_AMP = 175,
	RUNTIME_FORCE_EFFECT_PVP_ATTACK = 176,
	RUNTIME_FORCE_EFFECT_PVE_ATTACK = 177,
	RUNTIME_FORCE_EFFECT_PVP_MAGIC_ATTACK = 178,
	RUNTIME_FORCE_EFFECT_PVE_MAGIC_ATTACK = 179,
	RUNTIME_FORCE_EFFECT_PVP_ALL_ATTACK_UP = 180,
	RUNTIME_FORCE_EFFECT_PVE_ALL_ATTACK_UP = 181,
	RUNTIME_FORCE_EFFECT_PVP_ATTACK_RATE_UP = 182,
	RUNTIME_FORCE_EFFECT_PVE_ATTACK_RATE_UP = 183,
	RUNTIME_FORCE_EFFECT_PVP_CRITICAL_RATE = 184,
	RUNTIME_FORCE_EFFECT_PVE_CRITICAL_RATE = 185,
	RUNTIME_FORCE_EFFECT_PVP_ACCURACY = 186,
	RUNTIME_FORCE_EFFECT_PVE_ACCURACY = 187,
	RUNTIME_FORCE_EFFECT_PVP_PENETRATION = 188,
	RUNTIME_FORCE_EFFECT_PVP_DEFENSE_UP = 189,
	RUNTIME_FORCE_EFFECT_PVP_DEFENSE_RATE_UP = 190,
	RUNTIME_FORCE_EFFECT_PVE_DEFENSE_RATE_UP = 191,
	RUNTIME_FORCE_EFFECT_PVP_EVASION = 192,
	RUNTIME_FORCE_EFFECT_PVE_EVASION = 193,
	RUNTIME_FORCE_EFFECT_PVP_DAMAGE_REDUCTION = 194,
	RUNTIME_FORCE_EFFECT_PVP_IGNORE_PENETRATION = 195,
	RUNTIME_FORCE_EFFECT_PVP_FINAL_DAMAGE_DECREASED = 196,
	RUNTIME_FORCE_EFFECT_PVE_FINAL_DAMAGE_DECREASED = 197,
	RUNTIME_FORCE_EFFECT_PVP_NORMAL_DAMAGE_UP = 198,
	RUNTIME_FORCE_EFFECT_PVE_NORMAL_DAMAGE_UP = 199,
	RUNTIME_FORCE_EFFECT_PVP_FINAL_DAMAGE_INCREASED = 200,
	RUNTIME_FORCE_EFFECT_PVE_FINAL_DAMAGE_INCREASED = 201,
	RUNTIME_FORCE_EFFECT_PVP_ADD_DAMAGE = 202,
	RUNTIME_FORCE_EFFECT_PVE_ADD_DAMAGE = 203,
	RUNTIME_FORCE_EFFECT_PVP_CANCEL_IGNORE_DAMAGE_REDUCTION = 204,
	RUNTIME_FORCE_EFFECT_PVE_CANCEL_IGNORE_DAMAGE_REDUCTION = 205,
	RUNTIME_FORCE_EFFECT_PVP_IGNORE_RESIST_CRITICAL_DAMAGE = 206,
	RUNTIME_FORCE_EFFECT_PVE_IGNORE_RESIST_CRITICAL_DAMAGE = 207,
	RUNTIME_FORCE_EFFECT_PVP_CANCEL_IGNORE_PENETRATION = 208,
	RUNTIME_FORCE_EFFECT_PVE_CANCEL_IGNORE_PENETRATION = 209,
	RUNTIME_FORCE_EFFECT_PVP_IGNORE_ACCURACY = 210,
	RUNTIME_FORCE_EFFECT_PVE_IGNORE_ACCURACY = 211,
	RUNTIME_FORCE_EFFECT_PVP_IGNORE_EVASION = 212,
	RUNTIME_FORCE_EFFECT_PVE_IGNORE_EVASION = 213,
	RUNTIME_FORCE_EFFECT_PVP_IGNORE_RESIST_SKILL_AMP = 214,
	RUNTIME_FORCE_EFFECT_PVE_IGNORE_RESIST_SKILL_AMP = 215,
	RUNTIME_FORCE_EFFECT_PVP_IGNORE_DAMAGE_REDUCTION = 216,
	RUNTIME_FORCE_EFFECT_PVE_IGNORE_DAMAGE_REDUCTION = 217,
	RUNTIME_FORCE_EFFECT_PVP_IGNORE_RESIST_CRITICAL_RATE = 218,
	RUNTIME_FORCE_EFFECT_PVE_IGNORE_RESIST_CRITICAL_RATE = 219,
	RUNTIME_FORCE_EFFECT_ARRIVAL_SKILL_COOLTIME_DECREASED = 220,
	RUNTIME_FORCE_EFFECT_ARRIVAL_SKILL_BUFFTIME_UP = 221,
	RUNTIME_FORCE_EFFECT_ALL_ATTACK_UP_PERCENT = 222,
	RUNTIME_FORCE_EFFECT_DEFENSE_UP_PERCENT = 223,
	RUNTIME_FORCE_EFFECT_ATTACK_UP3 = 224,
	RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP3 = 225,
	RUNTIME_FORCE_EFFECT_COUNT,
};

enum {
	RUNTIME_FORCE_VALUE_TYPE_ADDITIVE = 0,
	RUNTIME_FORCE_VALUE_TYPE_MULTIPLICATIVE = 1,
};

enum {
    RUNTIME_FORCE_EFFECT_TARGET_TYPE_NONE,
    RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE,
    RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP,
};

typedef Void (*RTForceEffectFormulaCallback)(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
);

struct _RTForceEffectFormula {
    Index ForceEffectIndex;
    UInt8 ForceEffectTargetType;
    Index AttributeCount;
	Index AttributeIndices[RUNTIME_MAX_FORCE_EFFECT_ATTRIBUTE_COUNT];
	RTForceEffectFormulaCallback OnApply;
    RTForceEffectFormulaCallback OnCancel;
};

#pragma pack(pop)

Void RTRuntimeInitForceEffectFormulas(
    RTRuntimeRef Runtime
);

Void RTCharacterApplyForceEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Index ForceEffectIndex,
	Int64 ForceValue,
	Int32 ForceValueType
);

Void RTCharacterCancelForceEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Index ForceEffectIndex,
	Int64 ForceValue,
	Int32 ForceValueType
);

Void RTMobApplyForceEffect(
	RTRuntimeRef Runtime,
	RTMobRef Mob,
	Index ForceEffectIndex,
	Int64 ForceValue,
	Int32 ForceValueType
);

Void RTMobCancelForceEffect(
	RTRuntimeRef Runtime,
	RTMobRef Mob,
	Index ForceEffectIndex,
	Int64 ForceValue,
	Int32 ForceValueType
);

EXTERN_C_END
