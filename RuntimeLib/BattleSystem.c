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
    Int64 Defense,
    Int64 Penetration
) {
	return Defense * 10000 / (10000 + Penetration);
}

static inline Int64 _CalculateFinalMissRate(
    Int64 Evasion,
    Int64 Accuracy
) {
	Int64 FinalEvasion = Evasion * 10000 / (10000 + Accuracy);
	
	return 100 - 100 * 10000 / (10000 + FinalEvasion);
}

static inline Int64 _CalculateFinalMinDamage(
    Int64 MinDamage,
    Int64 AttackRate,
    Int64 DefenseRate
) {
	Int64 BaseRate = 100 - MinDamage;
	if (DefenseRate < 1) return BaseRate;

	Int64 AppliedAttackRate = MAX(0, AttackRate - DefenseRate);

	return MinDamage + BaseRate - BaseRate * DefenseRate / (DefenseRate + AppliedAttackRate);
}

static inline Int64 _CalculateFinalBlockRate(
    Int64 AttackRate,
    Int64 DefenseRate
) {
	if (AttackRate < 1) return 0;

	Int64 AppliedDefenseRate = MAX(0, DefenseRate - AttackRate);

	return 100 - 100 * AttackRate / (AttackRate + AppliedDefenseRate);
}

// TODO: Add missing attributes...
Void CalculateFinalBattleAttributes(
	Int32 BattleSkillType,
	Int32 AttackerLevel,
	RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
	RTFinalBattleAttributesRef Result
) {
	assert(BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD || BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_MAGIC);

	Int32 AttackIndex = (BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD) ? RUNTIME_ATTRIBUTE_ATTACK : RUNTIME_ATTRIBUTE_MAGIC_ATTACK;
	Int32 SkillAmpIndex = (BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP;

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

	if (BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_MAGIC) {
		Result->MinDamage = 100;
	}

	Result->NormalDamageAmp = Attacker->Values[RUNTIME_ATTRIBUTE_NORMAL_ATTACK_DAMAGE_UP];
	Result->FinalDamageAmp = Attacker->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASED];
	Result->FinalDamageAmp -= Defender->Values[RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASED];
	Result->AccumulatedRate = 100 + Result->MissRate + Result->BlockRate;
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
	Int32 BattleSkillType,
	Int32 AttackerLevel,
    RTBattleAttributesRef Attacker,
	Int32 DefenderLevel,
	RTBattleAttributesRef Defender,
    RTBattleResultRef Result
) {
	assert(BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD || BattleSkillType == RUNTIME_BATTLE_SKILL_TYPE_MAGIC);

	struct _RTFinalBattleAttributes Attributes = { 0 };
	CalculateFinalBattleAttributes(
		BattleSkillType,
		AttackerLevel,
		Attacker,
		DefenderLevel,
		Defender,
		&Attributes
	);

//	Int32 LevelDifference = CalculateLevelDifference(AttackerLevel, DefenderLevel);
//	Int32 LevelDifferencePenalty = CalculateLevelDifferencePenalty(AttackerLevel, DefenderLevel);

	Int32 Rate = RandomRange(&Attacker->Seed, 0, (Int32)Attributes.AccumulatedRate);
	if (Rate < Attributes.CriticalRate) {
		Attributes.MinDamage = 100;

		Result->AttackType = RUNTIME_ATTACK_TYPE_CRITICAL;
	}
	else if (Rate < Attributes.CriticalRate + Attributes.MissRate) {
		Result->AttackType = RUNTIME_ATTACK_TYPE_MISS;
		return;
	}
	else if (Rate < Attributes.CriticalRate + Attributes.MissRate + Attributes.BlockRate) {
		Result->AttackType = RUNTIME_ATTACK_TYPE_BLOCK;
		return;
	}
	else {
		Result->AttackType = RUNTIME_ATTACK_TYPE_NORMAL;
	}

	Result->TotalDamage = MAX(1, Attributes.Attack - Attributes.Defense);
	Int32 DamageRate = RandomRange(&Attacker->Seed, (Int32)Attributes.MinDamage, 100);
	Result->TotalDamage = (Result->TotalDamage * DamageRate) / 100;
	Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.SkillAmp)) / 100;

	if (Result->AttackType == RUNTIME_ATTACK_TYPE_CRITICAL) {
		Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.CriticalDamage)) / 100;
	}

	if (Result->AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.NormalDamageAmp)) / 100;
	}

	Result->AdditionalDamage = Attributes.AddDamage;
	Result->TotalDamage += Attributes.AddDamage;
	Result->TotalDamage = (Result->TotalDamage * (100 + Attributes.FinalDamageAmp)) / 100;
	Result->TotalDamage = MAX(1, Result->TotalDamage);
	Result->AppliedDamage = MIN(Result->TotalDamage, Defender->Values[RUNTIME_ATTRIBUTE_HP_CURRENT]);
	Result->IsDead = (Defender->Values[RUNTIME_ATTRIBUTE_HP_CURRENT] - Result->TotalDamage) <= 0;

	// TODO: Add damage absorb shield and other stuff

	Float64 Exp = Defender->Values[RUNTIME_ATTRIBUTE_EXP];
	Exp *= Result->AppliedDamage;
	Exp /= MAX(1, Defender->Values[RUNTIME_ATTRIBUTE_HP_MAX]);
	Exp *= Runtime->Config.ExpMultiplier;

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

	// TODO: Skill Amp and Attack are slightly higher then they should be! 
	Int32 SkillAmp = RTCalculateSkillSlopeValue(Skill->SAmp[0], Skill->SAmp[1], SkillLevel);

	// TODO: Check what Atk[2] means...
	Int32 Attack = RTCalculateSkillSlopeValue(Skill->Atk[0], Skill->Atk[1], SkillLevel);
	Int32 AttackRate = RTCalculateSkillSlopeValue(Skill->SHit[0], Skill->SHit[1], SkillLevel);
	Int32 Penetration = RTCalculateSkillSlopeValue(Skill->SPenet[0], Skill->SPenet[1], SkillLevel);
	Int32 CriticalDamage = RTCalculateSkillSlopeValue(Skill->CritDmg[0], Skill->CritDmg[1], SkillLevel);

	// TODO: Add additional effect of skills like knock back, stun, ...

	Int32 SkillAmpIndex = (Skill->SkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD) ? RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP : RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP;
	Int32 AttackIndex = (Skill->SkillType == RUNTIME_BATTLE_SKILL_TYPE_SWORD) ? RUNTIME_ATTRIBUTE_ATTACK : RUNTIME_ATTRIBUTE_MAGIC_ATTACK;

	Attacker->Values[SkillAmpIndex] += SkillAmp;
	Attacker->Values[AttackIndex] += Attack;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] += AttackRate;
	Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION] += Penetration;
	Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE] += CriticalDamage;

	Int32 BattleSkillType = (Skill->SkillType == RUNTIME_SKILL_TYPE_MAGIC) ? RUNTIME_BATTLE_SKILL_TYPE_MAGIC : RUNTIME_BATTLE_SKILL_TYPE_SWORD;
	RTCalculateNormalAttackResult(
		Runtime,
		BattleSkillType,
		AttackerLevel,
		Attacker,
		DefenderLevel,
		Defender,
		Result
	);

	Attacker->Values[SkillAmpIndex] -= SkillAmp;
	Attacker->Values[AttackIndex] -= Attack;
	Attacker->Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] -= AttackRate;
	Attacker->Values[RUNTIME_ATTRIBUTE_PENETRATION] -= Penetration;
	Attacker->Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE] -= CriticalDamage;

	if (Result->AttackType == RUNTIME_ATTACK_TYPE_NORMAL) {
		Result->SkillExp = Skill->SkillExp1 * (Int32)Runtime->Config.SkillExpMultiplier;
	}
	else {
		Result->SkillExp = Skill->SkillExp2 * (Int32)Runtime->Config.SkillExpMultiplier;
	}
}
