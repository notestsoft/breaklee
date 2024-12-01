#include "Runtime.h"
#include "Skill.h"
#include "BattleSystem.h"

static inline Int32 CalculateLevelDifference(
	Int32 AttackerLevel,
	Int32 DefenderLevel
) {
	return AttackerLevel - DefenderLevel;
}

static inline Int32 CalculateLevelDifferencePenalty(
	Int32 AttackerLevel,
	Int32 DefenderLevel
) {
	return (AttackerLevel - DefenderLevel) * (AttackerLevel + 30) * 1000 / (22 * AttackerLevel + 102);
}

static inline Int64 CalculateAttribute(
	Int64 MaxValue,
	Int64 Value,
	Int64 PVEValue,
	Int64 PVPValue,
	Int64 Multiplier,
	Bool IsPVE
) {
	Int64 Result = Value;
	Result += (IsPVE) ? PVEValue : PVPValue;
	Result *= 100 + Multiplier;
	Result /= 100;
	return MIN(MaxValue, Result);
}

static inline Int64 CalculateResistAttribute(
	Int64 Value,
	Int64 ResistValue,
	Int64 IgnoreResistValue
) {
	return MAX(0, Value - (ResistValue - IgnoreResistValue));
}

static inline Int64 _CalculateFinalDefense(
	Int64 AttackerLevel,
	Int64 DefenderLevel,
    Int64 Defense,
    Int64 Penetration
) {
	Int64 LevelDifference = MAX(0, MIN(25, DefenderLevel - AttackerLevel));
	Int64 PenetrationPenalty = (Penetration * 1000000) / (Penetration + LevelDifference * 2 + 400);
	return Defense * MAX(50000, MIN(1000000, 1000000 - PenetrationPenalty)) / 1000000;
}

static inline Int64 _CalculateFinalMissRate(
    Int64 Evasion,
    Int64 Accuracy
) {
	return MAX(250000, MIN(950000, (Evasion - Accuracy) * 100));
}

static inline Int64 _CalculateFinalMinDamage(
    Int64 MinDamage,
    Int64 AttackRate,
    Int64 DefenseRate,
	Bool IsSword
) {
	return MIN(100, (IsSword ? MAX(80, MinDamage + 80) : 100));
}

static inline Int64 _CalculateFinalBlockRate(
	Int64 AttackerLevel,
	Int64 DefenderLevel,
	Int64 AttackRate,
    Int64 DefenseRate
) {
	Int64 LevelDifferenceBonus = MAX(25, DefenderLevel - AttackerLevel) * 4000;
	Int64 AppliedDefenseRate = DefenseRate - AttackRate;
	return MAX(0, MIN(950000, (AppliedDefenseRate * 1000000) / (ABS(AppliedDefenseRate) + 10000) + LevelDifferenceBonus));
}

static inline RTFinalBattleAttributes CalculateFinalBattleAttributes(
	Int32 DamageType,
	Bool IsPVE,
	RTBattleAttributesRef Attacker,
	RTBattleAttributesRef Defender
) {
	RTFinalBattleAttributes Result = { 0 };
	Int32 AttackerLevel = MAX(1, (Int32)Attacker->Values[RUNTIME_ATTRIBUTE_LEVEL]);
	Int32 DefenderLevel = MAX(1, (Int32)Defender->Values[RUNTIME_ATTRIBUTE_LEVEL]);
	
	/*
		RUNTIME_ATTRIBUTE_HP_CURRENT,
		RUNTIME_ATTRIBUTE_HP_MAX,
		RUNTIME_ATTRIBUTE_HP_ABSORB,
		RUNTIME_ATTRIBUTE_HP_ABSORB_LIMIT,
		RUNTIME_ATTRIBUTE_HP_REGEN,
		RUNTIME_ATTRIBUTE_HP_REGEN_UP,
		RUNTIME_ATTRIBUTE_HP_REGEN_DOWN,
		RUNTIME_ATTRIBUTE_HP_CURSE,
		RUNTIME_ATTRIBUTE_HP_CURSE_RESIST,
		RUNTIME_ATTRIBUTE_HP_HEAL_INCREASE,
		RUNTIME_ATTRIBUTE_HP_HEAL_DECREASE,

		RUNTIME_ATTRIBUTE_MP_CURRENT,
		RUNTIME_ATTRIBUTE_MP_MAX,
		RUNTIME_ATTRIBUTE_MP_ABSORB,
		RUNTIME_ATTRIBUTE_MP_ABSORB_LIMIT,
		RUNTIME_ATTRIBUTE_MP_REGEN,
		RUNTIME_ATTRIBUTE_MP_CURSE,

		RUNTIME_ATTRIBUTE_SP_CURRENT,
		RUNTIME_ATTRIBUTE_SP_MAX,
		RUNTIME_ATTRIBUTE_SP_REGEN,
		RUNTIME_ATTRIBUTE_SP_CONSUMPTION_DECREASE,

		RUNTIME_ATTRIBUTE_BP_CURRENT,
		RUNTIME_ATTRIBUTE_BP_MAX,
		RUNTIME_ATTRIBUTE_BP_CONSUMPTION_DECREASE,

		RUNTIME_ATTRIBUTE_RAGE_CURRENT,
		RUNTIME_ATTRIBUTE_RAGE_MAX,
		*/

	Bool IsSword = DamageType == RUNTIME_SKILL_DAMAGE_TYPE_SWORD;

	Result.Attack = CalculateAttribute(
		INT64_MAX,
		Attacker->Values[IsSword ? RUNTIME_ATTRIBUTE_ATTACK : RUNTIME_ATTRIBUTE_MAGIC_ATTACK],
		Attacker->Values[IsSword ? RUNTIME_ATTRIBUTE_ATTACK_PVE : RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVE],
		Attacker->Values[IsSword ? RUNTIME_ATTRIBUTE_ATTACK_PVP : RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVP],
		Attacker->Values[IsSword ? RUNTIME_ATTRIBUTE_ATTACK_PERCENT : RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PERCENT],
		IsPVE
	);

	Result.Defense = _CalculateFinalDefense(
		AttackerLevel, 
		DefenderLevel, 
		CalculateAttribute(
			INT64_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE],
			Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_PVE],
			Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_PVP],
			Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_PERCENT],
			IsPVE
		),
		CalculateResistAttribute(
			CalculateAttribute(
				INT64_MAX,
				Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION],
				Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION_PVE],
				Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION_PVP],
				0,
				IsPVE
			),
			CalculateAttribute(
				INT64_MAX,
				Defender->Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST],
				Defender->Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST_PVE],
				Defender->Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST_PVP],
				0,
				IsPVE
			),
			CalculateAttribute(
				INT64_MAX,
				Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST],
				Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST_PVE],
				Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST_PVP],
				0,
				IsPVE
			)
		)
	);

	Result.CriticalDamage = CalculateResistAttribute(
		CalculateAttribute(
			INT64_MAX,
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_PVE],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_RESIST],
			Defender->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_RESIST_PVE],
			Defender->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_RESIST_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST_PVE],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST_PVP],
			0,
			IsPVE
		)
	);

	Result.SkillAmp = CalculateResistAttribute(
		CalculateAttribute(
			INT64_MAX,
			Attacker->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP],
			Attacker->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_PVE : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_PVE],
			Attacker->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_PVP : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Defender->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_RESIST : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_RESIST],
			Defender->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_RESIST_PVE : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_RESIST_PVE],
			Defender->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_RESIST_PVP : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_RESIST_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Attacker->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_IGNORE_RESIST : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_IGNORE_RESIST],
			Attacker->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_IGNORE_RESIST_PVE : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_IGNORE_RESIST_PVE],
			Attacker->Values[(IsSword) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_IGNORE_RESIST_PVP : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_IGNORE_RESIST_PVP],
			0,
			IsPVE
		)
	);

	Result.SkillAmp += CalculateResistAttribute(
		CalculateAttribute(
			INT64_MAX,
			Attacker->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP],
			Attacker->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_PVE],
			Attacker->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST],
			Defender->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST_PVE],
			Defender->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Attacker->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST],
			Attacker->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST_PVE],
			Attacker->Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST_PVP],
			0,
			IsPVE
		)
	);

	Result.AddDamage = CalculateAttribute(
		INT64_MAX,
		Attacker->Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE],
		Attacker->Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE_PVE],
		Attacker->Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE_PVP],
		0,
		IsPVE
	);

	Int64 AttackRate = CalculateAttribute(
		INT64_MAX,
		Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE],
		Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE_PVE],
		Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE_PVP],
		0,
		IsPVE
	);

	Int64 DefenseRate = CalculateAttribute(
		INT64_MAX,
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE],
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE_PVE],
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE_PVP],
		0,
		IsPVE
	);

	Result.MinDamage = _CalculateFinalMinDamage(
		Attacker->Values[RUNTIME_ATTRIBUTE_MIN_DAMAGE],
		AttackRate,
		DefenseRate,
		IsSword
	);

	Result.AbsoluteDamage = CalculateResistAttribute(
		Defender->Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE],
		Attacker->Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE_RESIST],
		0
	);

	Result.DamageReduction = CalculateResistAttribute(
		CalculateAttribute(
			INT64_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION],
			Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PVE],
			Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Attacker->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST],
			Attacker->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST_PVE],
			Attacker->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_IGNORE_RESIST],
			Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_IGNORE_RESIST_PVE],
			Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_IGNORE_RESIST_PVP],
			0,
			IsPVE
		)
	);

	Result.DamageReductionPercent = Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PERCENT];

	Result.NormalDamageAmp = CalculateAttribute(
		INT64_MAX,
		Attacker->Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE],
		Attacker->Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE_PVE],
		Attacker->Values[RUNTIME_ATTRIBUTE_NORMAL_DAMAGE_PVP],
		0,
		IsPVE
	);

	/*
	RUNTIME_ATTRIBUTE_EVASION_ABSOLUTE,
	*/

	Result.FinalDamageIncrease = CalculateAttribute(
		INT64_MAX,
		Attacker->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE],
		Attacker->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE_PVE],
		Attacker->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE_PVP],
		0,
		IsPVE
	);

	Result.FinalDamageDecrease = CalculateAttribute(
		INT64_MAX,
		Defender->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE],
		Defender->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE_PVE],
		Defender->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE_PVP],
		0,
		IsPVE
	);

	Int64 Stun = CalculateResistAttribute(
		Attacker->Values[RUNTIME_ATTRIBUTE_STUN],
		Defender->Values[RUNTIME_ATTRIBUTE_STUN_RESIST],
		Attacker->Values[RUNTIME_ATTRIBUTE_STUN_IGNORE_RESIST]
	);

	Int64 Down = CalculateResistAttribute(
		Attacker->Values[RUNTIME_ATTRIBUTE_DOWN],
		Defender->Values[RUNTIME_ATTRIBUTE_DOWN_RESIST],
		Attacker->Values[RUNTIME_ATTRIBUTE_DOWN_IGNORE_RESIST]
	);

	Int64 KnockBack = CalculateResistAttribute(
		Attacker->Values[RUNTIME_ATTRIBUTE_KNOCK_BACK],
		Defender->Values[RUNTIME_ATTRIBUTE_KNOCK_BACK_RESIST],
		Attacker->Values[RUNTIME_ATTRIBUTE_KNOCK_BACK_IGNORE_RESIST]
	);

	Int64 Immobility = CalculateResistAttribute(
		Attacker->Values[RUNTIME_ATTRIBUTE_IMMOBILITY],
		Defender->Values[RUNTIME_ATTRIBUTE_IMMOBILITY_RESIST],
		0
	);

	/*
	RUNTIME_ATTRIBUTE_SUPPRESSION, // When Suppression is in effect, afflicted character cannot move or use skills.
	RUNTIME_ATTRIBUTE_SUPPRESSION_PERCENT,
	RUNTIME_ATTRIBUTE_SUPPRESSION_DURATION,
	RUNTIME_ATTRIBUTE_SUPPRESSION_RESIST,

	RUNTIME_ATTRIBUTE_SILENCE, // When Silence is in effect, afflicted character cannot use any skills.
	RUNTIME_ATTRIBUTE_SILENCE_PERCENT,
	RUNTIME_ATTRIBUTE_SILENCE_DURATION,
	RUNTIME_ATTRIBUTE_SILENCE_RESIST,

	RUNTIME_ATTRIBUTE_ROOT, // When Root is in effect, afflicted character cannot move or use skills related to movement.
	RUNTIME_ATTRIBUTE_ROOT_PERCENT,
	RUNTIME_ATTRIBUTE_ROOT_DURATION,

	RUNTIME_ATTRIBUTE_DAMAGE_ABSORB,
	RUNTIME_ATTRIBUTE_DAMAGE_REFLECT,

	RUNTIME_ATTRIBUTE_MOVEMENT_SPEED,
	RUNTIME_ATTRIBUTE_MOVEMENT_SPEED_PERCENT,
	*/

	Result.CriticalRate = CalculateResistAttribute(
		CalculateAttribute(
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_PVE],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_RESIST],
			Defender->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_RESIST_PVE],
			Defender->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_RESIST_PVP],
			0,
			IsPVE
		),
		CalculateAttribute(
			INT64_MAX,
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST_PVE],
			Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST_PVP],
			0,
			IsPVE
		)
	);

	Result.MissRate = _CalculateFinalMissRate(
		CalculateResistAttribute(
			CalculateAttribute(
				INT64_MAX,
				Defender->Values[RUNTIME_ATTRIBUTE_EVASION],
				Defender->Values[RUNTIME_ATTRIBUTE_EVASION_PVE],
				Defender->Values[RUNTIME_ATTRIBUTE_EVASION_PVP],
				0,
				IsPVE
			),
			CalculateAttribute(
				INT64_MAX,
				Attacker->Values[RUNTIME_ATTRIBUTE_EVASION_RESIST],
				Attacker->Values[RUNTIME_ATTRIBUTE_EVASION_RESIST_PVE],
				Attacker->Values[RUNTIME_ATTRIBUTE_EVASION_RESIST_PVP],
				0,
				IsPVE
			),
			CalculateAttribute(
				INT64_MAX,
				Defender->Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST],
				Defender->Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST_PVE],
				Defender->Values[RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST_PVP],
				0,
				IsPVE
			)
		),
		CalculateResistAttribute(
			CalculateAttribute(
				INT64_MAX,
				Attacker->Values[RUNTIME_ATTRIBUTE_ACCURACY],
				Attacker->Values[RUNTIME_ATTRIBUTE_ACCURACY_PVE],
				Attacker->Values[RUNTIME_ATTRIBUTE_ACCURACY_PVP],
				0,
				IsPVE
			),
			CalculateAttribute(
				INT64_MAX,
				Defender->Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST],
				Defender->Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST_PVE],
				Defender->Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST_PVP],
				0,
				IsPVE
			),
			0
		)
	);

	Result.BlockRate = _CalculateFinalBlockRate(AttackerLevel, DefenderLevel, AttackRate, DefenseRate);
	return Result;
}

Int64 RTCalculateBaseHP(
	Int64 SkillRank,
	Int64 BaseHP,
	Int64 DeltaHP,
	Int64 Level,
	Int64 BattleRank,
	Int64 DeltaHP2
) {
	Int64 BaseValue = BaseHP - 15;
	Int64 LevelBonus = (Level - 1) * DeltaHP / 10;
	Int64 SkillRankBonus = (SkillRank + 1) * SkillRank * 15 / 2;
	Int64 BattleRankBonus = ((BattleRank * 5 - 14) * BattleRank + 9) * DeltaHP2 / 500;
	return BaseValue + LevelBonus + SkillRankBonus + (BattleRank > 7 ? BattleRankBonus : 0);
}

Int32 RTCalculateSPReward(
	Int32 SkillExp,
	Int32 ComboLevel,
	Int32 ComboTiming
) {
	if (ComboLevel && (ComboTiming == RUNTIME_COMBO_TIMING_MISS || ComboTiming == RUNTIME_COMBO_TIMING_INVALID)) {
		return 0;
	}

	return (SkillExp * 55 * (10 + ComboLevel * (ComboTiming - 1))) / 1000;
}

Int32 RTCalculateSPRegen(
	Int32 CurrentSP
) {
	if (CurrentSP < 5000) return 150;
	if (CurrentSP < 10000) return 80;
	if (CurrentSP < 15000) return 70;
	if (CurrentSP < 20000) return 40;
	return 25;
}

RTBattleResult RTCalculateNormalAttackResult(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	Int32 DamageType,
	Bool IsPVE,
	RTBattleAttributesRef Attacker,
	RTBattleAttributesRef Defender,
	RTMovementRef AttackerMovement,
	RTMovementRef DefenderMovement
) {
	RTBattleResult Result = { 0 };
	RTFinalBattleAttributes Attributes = CalculateFinalBattleAttributes(
		DamageType,
		IsPVE,
		Attacker,
		Defender
	);

//	Int32 LevelDifference = CalculateLevelDifference(AttackerLevel, DefenderLevel);
//	Int32 LevelDifferencePenalty = CalculateLevelDifferencePenalty(AttackerLevel, DefenderLevel);

	Int32 Rate = RandomRange(&Attacker->Seed, 0, 1000000);
	if (Rate < Attributes.BlockRate) {
		Result.AttackType = RUNTIME_ATTACK_TYPE_BLOCK;
	}
	else if (Rate < Attributes.BlockRate + Attributes.MissRate) {
		Result.AttackType = RUNTIME_ATTACK_TYPE_MISS;
	}
	else {
		if (Rate < Attributes.CriticalRate * 10000) {
			Result.AttackType = RUNTIME_ATTACK_TYPE_CRITICAL;
		}
		else {
			Result.AttackType = RUNTIME_ATTACK_TYPE_NORMAL;
		}
	}

	if (RTWorldTileIsImmune(Runtime, WorldContext, DefenderMovement->PositionCurrent.X, DefenderMovement->PositionCurrent.Y)) {
		Result.AttackType = RUNTIME_ATTACK_TYPE_PROTECT;
	}

	if (Defender->Values[RUNTIME_ATTRIBUTE_EVASION_COMPLETE] > 0) {
		Result.AttackType = RUNTIME_ATTACK_TYPE_MISS;
	}

	if (Defender->Values[RUNTIME_ATTRIBUTE_WING_PROTECT_ACTIVATE] > 0 &&
		(Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL || Result.AttackType == RUNTIME_ATTACK_TYPE_CRITICAL)) {
		Defender->Values[RUNTIME_ATTRIBUTE_WING_PROTECT_ACTIVATE] -= 1;
		Result.AttackType = RUNTIME_ATTACK_TYPE_PROTECT;
	}

	Int32 AttackerLevel = MAX(1, (Int32)Attacker->Values[RUNTIME_ATTRIBUTE_LEVEL]);
	Int32 DefenderLevel = MAX(1, (Int32)Defender->Values[RUNTIME_ATTRIBUTE_LEVEL]);
	Int32 MinDamageRate = RandomRange(&Attacker->Seed, (Int32)Attributes.MinDamage, 100);

	Result.TotalDamage = Attributes.Attack * MinDamageRate / 100;
	Result.TotalDamage = Result.TotalDamage * (100 + Attributes.SkillAmp) / 100;
	Result.TotalDamage = Result.TotalDamage * MIN(100, 100 - MIN(25, DefenderLevel - AttackerLevel) * 2) / 100; //  * 2
	Result.TotalDamage = 2 * Result.TotalDamage * (10000 - (Attributes.Defense * 10000) / (Attributes.Defense + Result.TotalDamage)) / 10000;

	if (Result.AttackType == RUNTIME_ATTACK_TYPE_CRITICAL) {
		Result.TotalDamage = (Result.TotalDamage * (100 + Attributes.CriticalDamage)) / 100;
	}
	if (Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result.TotalDamage = (Result.TotalDamage * (100 + Attributes.NormalDamageAmp)) / 100;
	}

	Result.TotalDamage += Attributes.AddDamage;
	Result.TotalDamage -= Attributes.DamageReduction;
	Result.TotalDamage = (Result.TotalDamage * (100 - Attributes.DamageReductionPercent)) / 100;
	Result.TotalDamage = (Result.TotalDamage * (100 + Attributes.FinalDamageIncrease)) / 100;
	Result.TotalDamage = (Result.TotalDamage * (100 - Attributes.FinalDamageDecrease)) / 100;
	Result.TotalDamage = MAX(1, Result.TotalDamage);

	if (Attacker->Values[RUNTIME_ATTRIBUTE_WING_DAMAGE_ACTIVATE] > 0) {
		Attacker->Values[RUNTIME_ATTRIBUTE_WING_DAMAGE_ACTIVATE] -= 1;
		if ((Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL || Result.AttackType == RUNTIME_ATTACK_TYPE_CRITICAL)) {
			Result.AdditionalDamage = Result.TotalDamage * Attacker->Values[RUNTIME_ATTRIBUTE_WING_DAMAGE] / 100;
		}
	}

	Result.TotalDamage += Result.AdditionalDamage;

	Result.AppliedDamage = 0;
	if (Result.AttackType == RUNTIME_ATTACK_TYPE_CRITICAL || Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result.AppliedDamage = MIN(Result.TotalDamage, Defender->Values[RUNTIME_ATTRIBUTE_HP_CURRENT]);
	}

	if (Attributes.AbsoluteDamage > 0) {
		Result.AppliedDamage = MIN(Result.AppliedDamage, Attributes.AbsoluteDamage);
	}

	if (Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB] > 0) {
		if (Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB] <= Result.TotalDamage) {
			Result.AppliedDamage -= Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
			Result.AttackType = RUNTIME_ATTACK_TYPE_BLOCK;
			Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB] = 0;
		}
		else {
			Result.AppliedDamage = 0;
			Result.AttackType = RUNTIME_ATTACK_TYPE_BLOCK;
			Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB] -= 1;
		}
	}

	Result.IsDead = (Defender->Values[RUNTIME_ATTRIBUTE_HP_CURRENT] - Result.AppliedDamage) <= 0;

	// TODO: Add damage absorb shield and other stuff

	Float64 Exp = Defender->Values[RUNTIME_ATTRIBUTE_EXP];
	Exp *= Result.AppliedDamage;
	Exp /= MAX(1, Defender->Values[RUNTIME_ATTRIBUTE_HP_MAX]);
	Result.Exp = (Int64)Exp;
	return Result;
}

RTBattleResult RTCalculateSkillAttackResult(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	Int32 SkillLevel,
	RTCharacterSkillDataRef Skill,
	Bool IsPVE,
	RTBattleAttributesRef Attacker,
	RTBattleAttributesRef Defender,
	RTMovementRef AttackerMovement,
	RTMovementRef DefenderMovement
) {
	Bool IsDefenderCharacter = false; // TODO: Add entity type to battle attributes

	RTCharacterSkillValue SkillValue = RTCalculateSkillBasicValue(Skill, SkillLevel, (Int32)Attacker->Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT]);
	Attacker->Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP] += SkillValue.SwordSkillAmp;
	Attacker->Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP] += SkillValue.MagicSkillAmp;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK] += SkillValue.Attack;
	Attacker->Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK] += SkillValue.MagicAttack;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] += SkillValue.AttackRate;
	Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION] += SkillValue.Penetration;
	Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE] += SkillValue.CriticalDamage;

	// TODO: Check if Skill->SkillValues have to be evaluated here and if they apply on attacker or defender

	RTBattleResult Result = RTCalculateNormalAttackResult(
		Runtime,
		WorldContext,
		Skill->DamageType,
		IsPVE,
		Attacker,
		Defender,
		AttackerMovement,
		DefenderMovement
	);

	Attacker->Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP] -= SkillValue.SwordSkillAmp;
	Attacker->Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP] -= SkillValue.MagicSkillAmp;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK] -= SkillValue.Attack;
	Attacker->Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK] -= SkillValue.MagicAttack;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] -= SkillValue.AttackRate;
	Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION] -= SkillValue.Penetration;
	Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE] -= SkillValue.CriticalDamage;

	if (Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result.SkillExp = Skill->SkillExp1;
	}
	else {
		Result.SkillExp = Skill->SkillExp2;
	}

	Int32 AbsDeltaX = MIN(ABS(DefenderMovement->PositionCurrent.X - AttackerMovement->PositionCurrent.X), RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH - 1);
	Int32 AbsDeltaY = MIN(ABS(DefenderMovement->PositionCurrent.Y - AttackerMovement->PositionCurrent.Y), RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH - 1);
	Float32 TileDistance = Runtime->MovementDistanceCache[AbsDeltaX + AbsDeltaY * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH];
	Result.Delay = 1000LL * Skill->FiringFrame / 30;
	Result.Delay += TileDistance * 100 * Skill->HitFrame / 30;

	return Result;
}

// NOTE: Shield is only updating its value if attack type is RUNTIME_ATTACK_TYPE_MISS
RTBattleResult RTCalculateMobAttackResult(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTDataMobAttackDataRef AttackData,
	RTDataMobSkillRef SkillData,
	Int32 PhysicalAttackMin,
	Int32 PhysicalAttackMax,
	RTBattleAttributesRef Attacker,
	RTBattleAttributesRef Defender,
	RTMovementRef AttackerMovement,
	RTMovementRef DefenderMovement
) {
	RTBattleResult Result = { 0 };
	return Result;
}
