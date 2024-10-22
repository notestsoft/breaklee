#include "Runtime.h"
#include "Skill.h"
#include "BattleSystem.h"

Int32 CalculateLevelDifference(
	Int32 AttackerLevel,
	Int32 DefenderLevel
) {
	return AttackerLevel - DefenderLevel;
}

Int32 CalculateLevelDifferencePenalty(
	Int32 AttackerLevel,
	Int32 DefenderLevel
) {
	return (AttackerLevel - DefenderLevel) * (AttackerLevel + 30) * 1000 / (22 * AttackerLevel + 102);
}

Int32 CalculateHitRate(
	Int32 AttackRate,
	Int32 DefenseRate
) {
	if (AttackRate + DefenseRate < 1) return 100;

	return 50 + 50 * AttackRate / (AttackRate + DefenseRate);
}

static inline Int64 _CalculateFinalAttribute(
	Int64 Value,
	Int64 MaxValue,
	Int64 ResistValue,
	Int64 IgnoreResistValue
) {
	return MAX(0, MIN(Value, MaxValue) - MAX(0, ResistValue - IgnoreResistValue));
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
	return MAX(250000, MIN(950000, Evasion - Accuracy * 100));
}

static inline Int64 _CalculateFinalMinDamage(
    Int64 MinDamage,
    Int64 AttackRate,
    Int64 DefenseRate
) {
	return MAX(80, MinDamage + 80);
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

// TODO: Add missing attributes...
Void CalculateFinalBattleAttributes(
	Int32 DamageType,
	Int32 AttackerLevel,
	RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
	RTFinalBattleAttributesRef Result
) {
	Int32 AttackIndex = (DamageType == RUNTIME_SKILL_DAMAGE_TYPE_SWORD) ? RUNTIME_ATTRIBUTE_ATTACK : RUNTIME_ATTRIBUTE_MAGIC_ATTACK;
	Int32 SkillAmpIndex = (DamageType == RUNTIME_SKILL_DAMAGE_TYPE_SWORD) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP;

	Result->Attack = Attacker->Values[AttackIndex];
	Result->CriticalRate = _CalculateFinalAttribute(
		Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE],
		Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX],
		Defender->Values[RUNTIME_ATTRIBUTE_RESIST_CRITICAL_RATE],
		Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_RESIST_CRITICAL_RATE]
	);
	Result->CriticalDamage = _CalculateFinalAttribute(
		Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE],
		INT32_MAX,
		Defender->Values[RUNTIME_ATTRIBUTE_RESIST_CRITICAL_DAMAGE],
		Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_RESIST_CRITICAL_DAMAGE]
	);
	Result->SkillAmp = _CalculateFinalAttribute(
		Attacker->Values[SkillAmpIndex],
		INT32_MAX,
		Defender->Values[RUNTIME_ATTRIBUTE_RESIST_SKILL_AMP],
		Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_RESIST_SKILL_AMP]
	);
	Result->Defense = _CalculateFinalDefense(
		AttackerLevel,
		DefenderLevel,
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE],
		_CalculateFinalAttribute(
			Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION],
			INT32_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_IGNORE_PENETRATION],
			Attacker->Values[RUNTIME_ATTRIBUTE_CANCEL_IGNORE_PENETRATION]
		)
	);
	Result->MissRate = _CalculateFinalMissRate(
		_CalculateFinalAttribute(
			Defender->Values[RUNTIME_ATTRIBUTE_EVASION],
			INT32_MAX,
			Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_EVASION],
			Defender->Values[RUNTIME_ATTRIBUTE_CANCEL_IGNORE_EVASION]
		),
		_CalculateFinalAttribute(
			Attacker->Values[RUNTIME_ATTRIBUTE_ACCURACY],
			INT32_MAX,
			Defender->Values[RUNTIME_ATTRIBUTE_IGNORE_ACCURACY],
			0
		)
	);
	Result->BlockRate = _CalculateFinalBlockRate(
		AttackerLevel,
		DefenderLevel,
		Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE],
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE]
	);
	Result->AddDamage = Attacker->Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE];
	Result->AddDamage -= _CalculateFinalAttribute(
		Defender->Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION],
		INT32_MAX,
		Attacker->Values[RUNTIME_ATTRIBUTE_IGNORE_DAMAGE_REDUCTION],
		Defender->Values[RUNTIME_ATTRIBUTE_CANCEL_IGNORE_DAMAGE_REDUCTION]
	);
	Result->MinDamage = _CalculateFinalMinDamage(
		Attacker->Values[RUNTIME_ATTRIBUTE_MIN_DAMAGE],
		Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE],
		Defender->Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE]
	);

	if (DamageType == RUNTIME_SKILL_DAMAGE_TYPE_MAGIC) {
		Result->MinDamage = 100;
	}

	Result->NormalDamageAmp = Attacker->Values[RUNTIME_ATTRIBUTE_NORMAL_ATTACK_DAMAGE_UP];
	Result->FinalDamageAmp = Attacker->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASED];
	Result->FinalDamageAmp -= Defender->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASED];
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

Void RTCalculateNormalAttackResult(
    RTRuntimeRef Runtime,
	Int32 DamageType,
	Int32 AttackerLevel,
    RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
    RTBattleResultRef Result
) {
	struct _RTFinalBattleAttributes Attributes = { 0 };
	CalculateFinalBattleAttributes(
		DamageType,
		AttackerLevel,
		Attacker,
		DefenderLevel,
		Defender,
		&Attributes
	);

//	Int32 LevelDifference = CalculateLevelDifference(AttackerLevel, DefenderLevel);
//	Int32 LevelDifferencePenalty = CalculateLevelDifferencePenalty(AttackerLevel, DefenderLevel);

	Int32 Rate = RandomRange(&Attacker->Seed, 0, 1000000);
	if (Rate < Attributes.BlockRate) {
		Result->AttackType = RUNTIME_ATTACK_TYPE_BLOCK;
	}
	else if (Rate < Attributes.CriticalRate * 10000) {
		Result->AttackType = RUNTIME_ATTACK_TYPE_CRITICAL;
	}
	else if (Rate < Attributes.MissRate) {
		Result->AttackType = RUNTIME_ATTACK_TYPE_MISS;
	}
	else {
		Result->AttackType = RUNTIME_ATTACK_TYPE_NORMAL;
	}

	if (Defender->Values[RUNTIME_ATTRIBUTE_COMPLETE_EVASION] > 0) {
		Result->AttackType = RUNTIME_ATTACK_TYPE_MISS;
	}

	Int32 MinDamageRate = RandomRange(&Attacker->Seed, (Int32)Attributes.MinDamage, 100);
	Result->TotalDamage = Attributes.Attack * MinDamageRate / 100;
	Result->TotalDamage = Result->TotalDamage * (100 + Attributes.SkillAmp) / 100;
	Result->TotalDamage = Result->TotalDamage * MIN(100, 100 - MIN(25, DefenderLevel - AttackerLevel) * 2) / 100; //  * 2
	Result->TotalDamage = 2 * Result->TotalDamage * (10000 - (Attributes.Defense * 10000) / (Attributes.Defense + Result->TotalDamage)) / 10000;

	if (Result->AttackType == RUNTIME_ATTACK_TYPE_CRITICAL) {
		Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.CriticalDamage)) / 100;
	}
	if (Result->AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.NormalDamageAmp)) / 100;
	}

	Result->TotalDamage += Attributes.AddDamage;
	Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.FinalDamageAmp)) / 100;
	Result->TotalDamage = MAX(1, Result->TotalDamage);

	if (Attacker->Values[RUNTIME_ATTRIBUTE_ACTIVATE_WING_DAMAGE] > 0) {
		Attacker->Values[RUNTIME_ATTRIBUTE_ACTIVATE_WING_DAMAGE] -= 1;
		Result->AdditionalDamage = Result->TotalDamage * Attacker->Values[RUNTIME_ATTRIBUTE_ADD_FORCE_WING_DAMAGE] / 100;
	}

	Result->TotalDamage += Result->AdditionalDamage;
	
	Result->AppliedDamage = 0;
	if (Result->AttackType == RUNTIME_ATTACK_TYPE_CRITICAL || Result->AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result->AppliedDamage = MIN(Result->TotalDamage, Defender->Values[RUNTIME_ATTRIBUTE_HP_CURRENT]);
	}

	if (Defender->Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE] > 0) {
		Result->AppliedDamage = MIN(Result->AppliedDamage, Defender->Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE]);
	}

	Result->IsDead = (Defender->Values[RUNTIME_ATTRIBUTE_HP_CURRENT] - Result->AppliedDamage) <= 0;

	// TODO: Add damage absorb shield and other stuff

	Float64 Exp = Defender->Values[RUNTIME_ATTRIBUTE_EXP];
	Exp *= Result->AppliedDamage;
	Exp /= MAX(1, Defender->Values[RUNTIME_ATTRIBUTE_HP_MAX]);
	Result->Exp = (Int64)Exp;
}

Void RTCalculateSkillAttackResult(
	RTRuntimeRef Runtime,
	Int32 SkillLevel,
	RTCharacterSkillDataRef Skill,
	Int32 AttackerLevel,
	RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
	RTBattleResultRef Result
) {
	Bool IsDefenderCharacter = false; // TODO: Add entity type to battle attributes

	RTCharacterSkillValue SkillValue = RTCalculateSkillValue(Skill, SkillLevel, Attacker->Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT]);
	Attacker->Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP] += SkillValue.SwordSkillAmp;
	Attacker->Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP] += SkillValue.MagicSkillAmp;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK] += SkillValue.Attack;
	Attacker->Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK] += SkillValue.MagicAttack;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] += SkillValue.AttackRate;
	Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION] += SkillValue.Penetration;
	Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE] += SkillValue.CriticalDamage;

	RTCalculateNormalAttackResult(
		Runtime,
		Skill->DamageType,
		AttackerLevel,
		Attacker,
		DefenderLevel,
		Defender,
		Result
	);

	Attacker->Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP] -= SkillValue.SwordSkillAmp;
	Attacker->Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP] -= SkillValue.MagicSkillAmp;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK] -= SkillValue.Attack;
	Attacker->Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK] -= SkillValue.MagicAttack;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] -= SkillValue.AttackRate;
	Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION] -= SkillValue.Penetration;
	Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE] -= SkillValue.CriticalDamage;

	if (Result->AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result->SkillExp = Skill->SkillExp1;
	}
	else {
		Result->SkillExp = Skill->SkillExp2;
	}
}
