#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

// TODO: Move this to a xml config file, data driven...
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
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_HP_ABSORB_LIMIT_UP = 12,
	RUNTIME_FORCE_EFFECT_MP_ABSORB_LIMIT_UP = 13,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_ATTACK_UP2 = 23, // NOT VERIFIED
	RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP2 = 24, // NOT VERIFIED
	RUNTIME_FORCE_EFFECT_CRITICAL_RATE_LIMIT_UP = 25,
	RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP2 = 26, // NOT VERIFIED
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_UP = 30,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_UP = 32,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_HP_ABSORB = 34,
	RUNTIME_FORCE_EFFECT_MP_ABSORB = 35,
	RUNTIME_FORCE_EFFECT_EVADE = 36,
	RUNTIME_FORCE_EFFECT_HP_REGEN = 37,
	RUNTIME_FORCE_EFFECT_MP_REGEN = 38,
	RUNTIME_FORCE_EFFECT_ADD_DAMAGE = 39,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_SKILL_EXP_UP = 41,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_ALZ_DROP_UP = 43,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_ADD_STAT_ALL = 48,
	RUNTIME_FORCE_EFFECT_ONE_SLOT_DROP_RATE_UP = 49,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_TWO_SLOT_DROP_RATE_UP = 51,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_54 = 54,
	RUNTIME_FORCE_EFFECT_UNKNOWN_55 = 55,
	RUNTIME_FORCE_EFFECT_UNKNOWN_56 = 56,
	RUNTIME_FORCE_EFFECT_UNKNOWN_57 = 57,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_59 = 59,
	RUNTIME_FORCE_EFFECT_UNKNOWN_60 = 60,
	RUNTIME_FORCE_EFFECT_ENERVATION = 61,
	RUNTIME_FORCE_EFFECT_STUN = 62,
	RUNTIME_FORCE_EFFECT_HEAL = 63,
	RUNTIME_FORCE_EFFECT_DOWN = 64,
	RUNTIME_FORCE_EFFECT_KNOCK_BACK = 65,
	RUNTIME_FORCE_EFFECT_IMMOBILITY = 66,
	RUNTIME_FORCE_EFFECT_ABSORB_DAMAGE = 67,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_71 = 71,
	RUNTIME_FORCE_EFFECT_IGNORE_ACCURACY = 72,
	RUNTIME_FORCE_EFFECT_UNKNOWN_73 = 73,
	RUNTIME_FORCE_EFFECT_IGNORE_PENETRATION = 74,
	RUNTIME_FORCE_EFFECT_UNKNOWN_75 = 75,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_77 = 77,
	RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION = 78,
	RUNTIME_FORCE_EFFECT_ACCURACY = 79,
	RUNTIME_FORCE_EFFECT_PENETRATION = 80,
	RUNTIME_FORCE_EFFECT_UNKNOWN_81 = 81,
	RUNTIME_FORCE_EFFECT_UNKNOWN_82 = 82,
	RUNTIME_FORCE_EFFECT_UNKNOWN_83 = 83,
	RUNTIME_FORCE_EFFECT_UNKNOWN_84 = 84,
	RUNTIME_FORCE_EFFECT_UNKNOWN_85 = 85,
	RUNTIME_FORCE_EFFECT_UNKNOWN_86 = 86,
	RUNTIME_FORCE_EFFECT_UNKNOWN_87 = 87,
	RUNTIME_FORCE_EFFECT_UNKNOWN_88 = 88,
	RUNTIME_FORCE_EFFECT_UNKNOWN_89 = 89,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_91 = 91,
	RUNTIME_FORCE_EFFECT_UNKNOWN_92 = 92,
	RUNTIME_FORCE_EFFECT_UNKNOWN_93 = 93,
	RUNTIME_FORCE_EFFECT_UNKNOWN_94 = 94,
	RUNTIME_FORCE_EFFECT_UNKNOWN_95 = 95,
	RUNTIME_FORCE_EFFECT_UNKNOWN_96 = 96,
	RUNTIME_FORCE_EFFECT_RESIST_CRITICAL_RATE = 97,
	RUNTIME_FORCE_EFFECT_RESIST_CRITICAL_DAMAGE = 98,
	RUNTIME_FORCE_EFFECT_RESIST_IMMOBILITY = 99,
	RUNTIME_FORCE_EFFECT_UNKNOWN_100 = 100,
	RUNTIME_FORCE_EFFECT_RESIST_DOWN = 101,
	RUNTIME_FORCE_EFFECT_RESIST_KNOCK_BACK = 102,
	RUNTIME_FORCE_EFFECT_RESIST_STUN = 103,
	RUNTIME_FORCE_EFFECT_UNKNOWN_104 = 104,
	RUNTIME_FORCE_EFFECT_UNKNOWN_105 = 105,
	RUNTIME_FORCE_EFFECT_UNKNOWN_106 = 106,
	RUNTIME_FORCE_EFFECT_UNKNOWN_107 = 107,
	RUNTIME_FORCE_EFFECT_UNKNOWN_108 = 108,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_110 = 110,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_RESIST_SKILL_AMP = 112,
	RUNTIME_FORCE_EFFECT_ALL_ATTACK_UP = 113,
	RUNTIME_FORCE_EFFECT_UNKNOWN_114 = 114,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_INCREASE_MOVEMENT_SPEED = 117,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_IGNORE_RESIST_CRITICAL_RATE = 125,
	RUNTIME_FORCE_EFFECT_UNKNOWN_126 = 126,
	RUNTIME_FORCE_EFFECT_UNKNOWN_127 = 127,
	RUNTIME_FORCE_EFFECT_UNKNOWN_128 = 128,
	RUNTIME_FORCE_EFFECT_UNKNOWN_129 = 129,
	RUNTIME_FORCE_EFFECT_UNKNOWN_130 = 130,
	RUNTIME_FORCE_EFFECT_UNKNOWN_131 = 131,
	RUNTIME_FORCE_EFFECT_UNKNOWN_132 = 132,
	RUNTIME_FORCE_EFFECT_UNKNOWN_133 = 133,
	RUNTIME_FORCE_EFFECT_UNKNOWN_134 = 134,
	RUNTIME_FORCE_EFFECT_UNKNOWN_135 = 135,
	RUNTIME_FORCE_EFFECT_UNKNOWN_136 = 136,
	RUNTIME_FORCE_EFFECT_UNKNOWN_137 = 137,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_CANCEL_IGNORE_PENETRATION = 139,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_147 = 147,
	RUNTIME_FORCE_EFFECT_UNKNOWN_148 = 148,
	RUNTIME_FORCE_EFFECT_UNKNOWN_149 = 149,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_151 = 151,
	RUNTIME_FORCE_EFFECT_UNKNOWN_152 = 152,
	RUNTIME_FORCE_EFFECT_UNKNOWN_153 = 153,
	RUNTIME_FORCE_EFFECT_UNKNOWN_154 = 154,
	RUNTIME_FORCE_EFFECT_UNKNOWN_155 = 155,
	RUNTIME_FORCE_EFFECT_UNKNOWN_156 = 156,
	RUNTIME_FORCE_EFFECT_UNKNOWN_157 = 157,
	RUNTIME_FORCE_EFFECT_UNKNOWN_158 = 158,
	RUNTIME_FORCE_EFFECT_UNKNOWN_159 = 159,
	RUNTIME_FORCE_EFFECT_UNKNOWN_160 = 160,
	RUNTIME_FORCE_EFFECT_UNKNOWN_161 = 161,
	RUNTIME_FORCE_EFFECT_UNKNOWN_162 = 162,
	RUNTIME_FORCE_EFFECT_UNKNOWN_163 = 163,
	RUNTIME_FORCE_EFFECT_UNKNOWN_164 = 164,
	RUNTIME_FORCE_EFFECT_UNKNOWN_165 = 165,
	RUNTIME_FORCE_EFFECT_UNKNOWN_166 = 166,
	RUNTIME_FORCE_EFFECT_UNKNOWN_167 = 167,
	RUNTIME_FORCE_EFFECT_UNKNOWN_168 = 168,
	RUNTIME_FORCE_EFFECT_UNKNOWN_169 = 169,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_180 = 180,
	RUNTIME_FORCE_EFFECT_UNKNOWN_181 = 181,
	RUNTIME_FORCE_EFFECT_UNKNOWN_182 = 182,
	RUNTIME_FORCE_EFFECT_UNKNOWN_183 = 183,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_189 = 189,
	RUNTIME_FORCE_EFFECT_UNKNOWN_190 = 190,
	RUNTIME_FORCE_EFFECT_UNKNOWN_191 = 191,
	/* RESERVED NOT IN USE */
	RUNTIME_FORCE_EFFECT_UNKNOWN_201 = 201,

	RUNTIME_FORCE_EFFECT_COUNT,
};

enum {
	RUNTIME_FORCE_CODE_HP_UP = 1,
	RUNTIME_FORCE_CODE_MP_UP = 2,
	RUNTIME_FORCE_CODE_ATTACK_UP = 3,
	RUNTIME_FORCE_CODE_MAGIC_ATTACK_UP = 4,
	RUNTIME_FORCE_CODE_DEFENSE_UP = 5,
	RUNTIME_FORCE_CODE_ATTACK_RATE_UP = 6,
	RUNTIME_FORCE_CODE_DEFENSE_RATE_UP = 7,
	RUNTIME_FORCE_CODE_CRITICAL_DAMAGE_UP = 8,
	RUNTIME_FORCE_CODE_CRITICAL_RATE_UP = 9,
	RUNTIME_FORCE_CODE_MIN_DAMAGE_UP = 10,
	RUNTIME_FORCE_CODE_MIN_DAMAGE_UP2 = 11,
	RUNTIME_FORCE_CODE_HP_ABSORB_LIMIT_UP = 12,
	RUNTIME_FORCE_CODE_MP_ABSORB_LIMIT_UP = 13,
	RUNTIME_FORCE_CODE_ATTACK_UP2 = 14,
	RUNTIME_FORCE_CODE_MAGIC_ATTACK_UP2 = 15,
	RUNTIME_FORCE_CODE_CRITICAL_RATE_LIMIT_UP = 16,
	RUNTIME_FORCE_CODE_ATTACK_RATE_UP2 = 17,
	RUNTIME_FORCE_CODE_CRITICAL_DAMAGE_UP2 = 18,
	RUNTIME_FORCE_CODE_CRITICAL_RATE_UP2 = 19,
	RUNTIME_FORCE_CODE_SWORD_SKILL_AMP_UP = 20,
	RUNTIME_FORCE_CODE_SWORD_SKILL_AMP_UP2 = 21,
	RUNTIME_FORCE_CODE_MAGIC_SKILL_AMP_UP = 22,
	RUNTIME_FORCE_CODE_MAGIC_SKILL_AMP_UP2 = 23,
	RUNTIME_FORCE_CODE_HP_ABSORB = 24,
	RUNTIME_FORCE_CODE_MP_ABSORB = 25,
	RUNTIME_FORCE_CODE_EVADE = 26,
	RUNTIME_FORCE_CODE_HP_REGEN = 27,
	RUNTIME_FORCE_CODE_MP_REGEN = 28,
	RUNTIME_FORCE_CODE_ADD_DAMAGE = 29,
	RUNTIME_FORCE_CODE_ADD_DAMAGE2 = 30,
	RUNTIME_FORCE_CODE_SKILL_EXP_UP = 31,
	RUNTIME_FORCE_CODE_SKILL_EXP_UP2 = 32,
	RUNTIME_FORCE_CODE_ALZ_DROP_UP = 33,
	RUNTIME_FORCE_CODE_ALZ_DROP_UP2 = 34,
	RUNTIME_FORCE_CODE_STR_UP = 35,
	RUNTIME_FORCE_CODE_INT_UP = 36,
	RUNTIME_FORCE_CODE_DEX_UP = 37,
	RUNTIME_FORCE_CODE_ALL_STAT_UP = 38,
	RUNTIME_FORCE_CODE_ONE_SLOT_DROP_RATE_UP = 39,
	RUNTIME_FORCE_CODE_ONE_SLOT_DROP_RATE_UP2 = 40,
	RUNTIME_FORCE_CODE_TWO_SLOT_DROP_RATE_UP = 41,
	RUNTIME_FORCE_CODE_TWO_SLOT_DROP_RATE_UP2 = 42,
	RUNTIME_FORCE_CODE_DASH_UP = 43,
	RUNTIME_FORCE_CODE_LIFT_IMMOBILITY = 44,
	RUNTIME_FORCE_CODE_IGNORE_DEBUFFS = 45,
	RUNTIME_FORCE_CODE_HP_REGEN2 = 46,
	RUNTIME_FORCE_CODE_MP_REGEN2 = 47,
	RUNTIME_FORCE_CODE_ALL_ATTACK_UP_PERCENT = 48,
	RUNTIME_FORCE_CODE_DEFENSE_UP_PERCENT = 49,
	RUNTIME_FORCE_CODE_HP_UP_REGEN = 50,
	RUNTIME_FORCE_CODE_ENERVATION = 51,
	RUNTIME_FORCE_CODE_STUN = 52,
	RUNTIME_FORCE_CODE_HEAL = 53,
	RUNTIME_FORCE_CODE_DOWN = 54,
	RUNTIME_FORCE_CODE_KNOCK_BACK = 55,
	RUNTIME_FORCE_CODE_IMMOBILITY = 56,
	RUNTIME_FORCE_CODE_ABSORB_DAMAGE = 57,
	RUNTIME_FORCE_CODE_REFLECT_DAMAGE = 58,
	RUNTIME_FORCE_CODE_CRAFT_MASTERY_UP = 59,
	RUNTIME_FORCE_CODE_BATTLE_MODE_DURATION_TIME_INCREASE = 60,
	RUNTIME_FORCE_CODE_IGNORE_EVASION = 61,
	RUNTIME_FORCE_CODE_IGNORE_ACCURACY = 62,
	RUNTIME_FORCE_CODE_IGNORE_DAMAGE_REDUCTION = 63,
	RUNTIME_FORCE_CODE_IGNORE_PENETRATION = 64,
	RUNTIME_FORCE_CODE_CANCEL_IGNORE_EVASION = 65,
	RUNTIME_FORCE_CODE_SOUL_ABILITY_EXP = 66,
	RUNTIME_FORCE_CODE_SKILL_EXP_UP3 = 67,
	RUNTIME_FORCE_CODE_DAMAGE_REDUCTION = 68,
	RUNTIME_FORCE_CODE_ACCURACY = 69,
	RUNTIME_FORCE_CODE_PENETRATION = 70,
	RUNTIME_FORCE_CODE_CANCEL_IGNORE_DAMAGE_REDUCTION = 71,
	RUNTIME_FORCE_CODE_INCREASE_ATTACK_RANGE = 72,
	RUNTIME_FORCE_CODE_RESIST_SUPPRESSION = 73,
	RUNTIME_FORCE_CODE_SUPPRESSION = 74,
	RUNTIME_FORCE_CODE_RESIST_SILENCE = 75,
	RUNTIME_FORCE_CODE_ROOT = 76,
	RUNTIME_FORCE_CODE_SILENCE = 77,
	RUNTIME_FORCE_CODE_SP_REGEN = 78,
	RUNTIME_FORCE_CODE_DOWN_SP_USE = 79,
	RUNTIME_FORCE_CODE_ATTACK = 80,
	RUNTIME_FORCE_CODE_MEZZ_IMMUNE = 81,
	RUNTIME_FORCE_CODE_COMPLETE_EVADE = 82,
	RUNTIME_FORCE_CODE_AGGRO_UP = 83,
	RUNTIME_FORCE_CODE_BATTLE_STANCE = 84,
	RUNTIME_FORCE_CODE_DAMAGE_OVER_TIME = 85,
	RUNTIME_FORCE_CODE_MP_DOWN_OVER_TIME = 86,
	RUNTIME_FORCE_CODE_RESIST_CRITICAL_RATE = 87,
	RUNTIME_FORCE_CODE_RESIST_CRITICAL_DAMAGE = 88,
	RUNTIME_FORCE_CODE_RESIST_IMMOBILITY = 89,
	RUNTIME_FORCE_CODE_REFLECT_DAMAGE2 = 90,
	RUNTIME_FORCE_CODE_RESIST_DOWN = 91,
	RUNTIME_FORCE_CODE_RESIST_KNOCK_BACK = 92,
	RUNTIME_FORCE_CODE_RESIST_STUN = 93,
	RUNTIME_FORCE_CODE_IMMORTAL = 94,
	RUNTIME_FORCE_CODE_HONOR_POINT_UP = 95,
	RUNTIME_FORCE_CODE_PARTY_EXP_UP = 96,
	RUNTIME_FORCE_CODE_EXP_UP = 97,
	RUNTIME_FORCE_CODE_PET_EXP_UP = 98,
	RUNTIME_FORCE_CODE_ALZ_DROP_RATE_UP = 99,
	RUNTIME_FORCE_CODE_ALZ_BOMP_CHANGE_UP = 100,
	RUNTIME_FORCE_CODE_CRAFT_EXP_UP = 101,
	RUNTIME_FORCE_CODE_RESIST_SKILL_AMP = 102,
	RUNTIME_FORCE_CODE_ALL_ATTACK_UP = 103, 
	RUNTIME_FORCE_CODE_ALL_SKILL_AMP = 104,
	RUNTIME_FORCE_CODE_ALL_ATTACK_UP2 = 105,
	RUNTIME_FORCE_CODE_ALL_SKILL_AMP2 = 106,
	RUNTIME_FORCE_CODE_INCREASE_MOVEMENT_SPEED = 107,
	RUNTIME_FORCE_CODE_SUPPRESSION_CHANCE_INCREASE = 108,
	RUNTIME_FORCE_CODE_SUPPRESSION_DURATION_INCREASE = 109,
	RUNTIME_FORCE_CODE_SILENCE_CHANGE_INCREASE = 110,
	RUNTIME_FORCE_CODE_SILENCE_DURATION_INCREASE = 111,
	RUNTIME_FORCE_CODE_ROOT_CHANCE_INCREASE = 112,
	RUNTIME_FORCE_CODE_ROOT_DURATION_INCREASE = 113,
};

enum {
	RUNTIME_FORCE_EFFECT_TYPE_ADDITIVE = 0,
	RUNTIME_FORCE_EFFECT_TYPE_MULTIPLICATIVE = 1,
};

#pragma pack(pop)

Void RTInitForceEffectFormulas();

Void RTCharacterApplyForceEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 ForceEffect,
	Int32 ForceValue,
	Int32 ForceValueType
);

EXTERN_C_END