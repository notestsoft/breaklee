#include "BattleSystem.h"
#include "Character.h"
#include "Constants.h"
#include "Force.h"
#include "Runtime.h"

Void RTForceEffectNone(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTEntityID Source,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
}

Void RTForceEffectApplyIncreaseAttribute(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTEntityID Source,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    for (Int Index = 0; Index < Formula->AttributeCount; Index += 1) {
        Attributes->Values[Formula->AttributeIndices[Index]] += Value;
    }
}

Void RTForceEffectCancelIncreaseAttribute(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTEntityID Source,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    for (Int Index = 0; Index < Formula->AttributeCount; Index += 1) {
        Attributes->Values[Formula->AttributeIndices[Index]] -= Value;
    }
}

Void RTForceEffectApplyIncreaseRage(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTEntityID Source,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    for (Int Index = 0; Index < Formula->AttributeCount; Index += 1) {
        Int AttributeIndex = Formula->AttributeIndices[Index];
        assert(AttributeIndex < RUNTIME_ATTRIBUTE_COUNT);
        Attributes->Values[AttributeIndex] = MIN(
            Attributes->Values[AttributeIndex] + Value,
            Attributes->Values[RUNTIME_ATTRIBUTE_RAGE_MAX]
        );
    }
}

Void RTForceEffectApplyHeal(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTEntityID Source,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    Int64 FinalValue = Value;
    FinalValue = (FinalValue * (100 + Attributes->Values[RUNTIME_ATTRIBUTE_HP_HEAL_INCREASE])) / 100;
    FinalValue = (FinalValue * (100 - Attributes->Values[RUNTIME_ATTRIBUTE_HP_HEAL_DECREASE])) / 100;

    Attributes->Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MIN(
        Attributes->Values[RUNTIME_ATTRIBUTE_HP_CURRENT] + FinalValue,
        Attributes->Values[RUNTIME_ATTRIBUTE_HP_MAX]
    );
}

Void RTForceEffectCancelHeal(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTEntityID Source,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectRemoveImmobility(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTEntityID Source,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    Attributes->Values[RUNTIME_ATTRIBUTE_IMMOBILITY_CURRENT_DURATION] = 0;
}

RTForceEffectFormulaRef RTRuntimeAddForceEffectFormula(
    RTRuntimeRef Runtime,
    Int ForceEffectIndex,
	RTForceEffectFormulaCallback OnApply,
    RTForceEffectFormulaCallback OnCancel
) {
    RTForceEffectFormulaRef Formula = (RTForceEffectFormulaRef)MemoryPoolReserve(Runtime->ForceEffectFormulaPool, ForceEffectIndex);
    Formula->ForceEffectIndex = ForceEffectIndex;
    Formula->OnApply = OnApply;
    Formula->OnCancel = OnCancel;
    return Formula;
}

Void RTForceEffectFormulaBindAttribute(
    RTForceEffectFormulaRef Formula,
    Int AttributeIndex
) {
    assert(Formula->AttributeCount < RUNTIME_MAX_FORCE_EFFECT_ATTRIBUTE_COUNT);
    
    Formula->AttributeIndices[Formula->AttributeCount] = AttributeIndex;
    Formula->AttributeCount += 1;
}

Void RTRuntimeInitForceEffectFormulas(
    RTRuntimeRef Runtime
) {
    RTForceEffectFormulaRef Formula = NULL;

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_LEVEL_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_LEVEL);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_STR_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STR);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_INT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_INT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEX_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEX);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_STR_DEX_INT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STR); 
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEX);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_INT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_MAX);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_MAX);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_UP_3, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_MAX);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_ABSORB_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_ABSORB);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_ABSORB_LIMIT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_ABSORB_LIMIT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_REGEN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_REGEN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_REGEN_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_REGEN_UP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_REGEN_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_REGEN_DOWN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_CURSE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_CURSE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_CURSE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_CURSE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_HEAL, RTForceEffectApplyHeal, RTForceEffectNone);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_HEAL_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_HEAL_INCREASE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_HEAL_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_HEAL_DECREASE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MP_MAX);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_ABSORB_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MP_ABSORB);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_ABSORB_LIMIT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MP_ABSORB_LIMIT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_REGEN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MP_REGEN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_CURSE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MP_CURSE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SP_REGEN_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SP_REGEN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SP_CONSUMPTION_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SP_CONSUMPTION_DECREASE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_BP_LIMIT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_BP_MAX);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_BP_CONSUMPTION_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_BP_CONSUMPTION_DECREASE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RAGE_UP, RTForceEffectApplyIncreaseRage, RTForceEffectNone);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_UP_3, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP_3, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_3, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_PVP);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_PVE);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_PERCENT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_PERCENT);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK_PERCENT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_RATE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_RATE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_RATE_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE_RATE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_RATE_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE_RATE_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_IGNORE_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_IGNORE_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE_IGNORE_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_LIMIT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_IGNORE_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_IGNORE_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_IGNORE_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_NORMAL_DAMAGE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_NORMAL_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_NORMAL_DAMAGE_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_NORMAL_DAMAGE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_NORMAL_DAMAGE_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_NORMAL_DAMAGE_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MIN_DAMAGE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MIN_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MIN_DAMAGE_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MIN_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_IGNORE_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_IGNORE_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_IGNORE_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_ABSOLUTE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION_ABSOLUTE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVASION_COMPLETE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION_COMPLETE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_DAMAGE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ADD_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_DAMAGE_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ADD_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_DAMAGE_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ADD_DAMAGE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_DAMAGE_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ADD_DAMAGE_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_UP_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_UP_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASE_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_DOWN_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_DOWN_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASE_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_PERCENT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_PERCENT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_IGNORE_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_IGNORE_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION_IGNORE_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_IGNORE_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ACCURACY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ACCURACY_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ACCURACY_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ACCURACY_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ACCURACY_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ACCURACY_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION_IGNORE_RESIST_PVP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST_PVP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION_IGNORE_RESIST_PVE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION_IGNORE_RESIST_PVE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_STUN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STUN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_STUN_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STUN_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_STUN_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STUN_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DOWN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DOWN_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DOWN_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DOWN_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DOWN_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_KNOCK_BACK, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_KNOCK_BACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_KNOCK_BACK_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_KNOCK_BACK_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_KNOCK_BACK_IGNORE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_KNOCK_BACK_IGNORE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IMMOBILITY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IMMOBILITY);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IMMOBILITY_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IMMOBILITY_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IMMOBILITY_REMOVE, RTForceEffectRemoveImmobility, RTForceEffectNone);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SUPPRESSION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SUPPRESSION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SUPPRESSION_PERCENT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SUPPRESSION_PERCENT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SUPPRESSION_DURATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SUPPRESSION_DURATION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SUPPRESSION_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SUPPRESSION_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SILENCE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SILENCE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SILENCE_PERCENT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SILENCE_PERCENT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SILENCE_DURATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SILENCE_DURATION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SILENCE_RESIST, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SILENCE_RESIST);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ROOT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ROOT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ROOT_PERCENT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ROOT_PERCENT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ROOT_DURATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ROOT_DURATION);
    


    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_ABSORB, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_ABSORB);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REFLECT_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REFLECT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REFLECT_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REFLECT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MOVEMENT_SPEED_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MOVEMENT_SPEED);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MOVEMENT_SPEED_PERCENT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MOVEMENT_SPEED_PERCENT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_WING_DAMAGE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_WING_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_WING_DAMAGE_ACTIVATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectNone);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_WING_DAMAGE_ACTIVATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_WING_PROTECT_ACTIVATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectNone);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_WING_PROTECT_ACTIVATE);


    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SKILL_EXP_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SKILL_EXP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SKILL_EXP_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SKILL_EXP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SKILL_EXP_UP_PERCENT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SKILL_EXP_PERCENT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALZ_DROP_AMOUNT_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALZ_DROP_AMOUNT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALZ_DROP_AMOUNT_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALZ_DROP_AMOUNT);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALZ_DROP_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALZ_DROP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALZ_BOMB_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALZ_BOMB_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DROP_RATE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DROP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DROP_RATE_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DROP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_BOX_DROP_RATE_UP_1, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_BOX_DROP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_BOX_DROP_RATE_UP_2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_BOX_DROP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_BOX_DROP_RATE_UP_3, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_BOX_DROP_RATE);




}

Void RTCharacterApplyForceEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
    RTEntityID Source,
    Int ForceEffectIndex,
	Int64 ForceValue,
    Int ForceValueType
) {
	// TODO: Check if ForceValueType is relevant for the runtime
    if (ForceEffectIndex < 1) return;
    RTForceEffectFormulaRef Formula = (RTForceEffectFormulaRef)MemoryPoolFetch(Runtime->ForceEffectFormulaPool, ForceEffectIndex);
    if (!Formula) {
        Warn("Force effect formula not found for index: %ull", ForceEffectIndex);
        return;
    }

    Formula->OnApply(Runtime, Formula, Source, &Character->Attributes, ForceValue);
}

Void RTCharacterCancelForceEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
    RTEntityID Source,
    Int ForceEffectIndex,
	Int64 ForceValue,
    Int ForceValueType
) {
    RTForceEffectFormulaRef Formula = (RTForceEffectFormulaRef)MemoryPoolFetch(Runtime->ForceEffectFormulaPool, ForceEffectIndex);
    if (!Formula) {
        Warn("No formula found for index(%d)", ForceEffectIndex);
        return;
    }
    
    Formula->OnCancel(Runtime, Formula, Source, &Character->Attributes, ForceValue);
}

Void RTMobApplyForceEffect(
    RTRuntimeRef Runtime,
    RTMobRef Mob,
    RTEntityID Source,
    Int ForceEffectIndex,
    Int64 ForceValue,
    Int ForceValueType
) {
    // TODO: Check if ForceValueType is relevant for the runtime
    if (ForceEffectIndex < 1) return;
    RTForceEffectFormulaRef Formula = (RTForceEffectFormulaRef)MemoryPoolFetch(Runtime->ForceEffectFormulaPool, ForceEffectIndex);
    if (!Formula) {
        Warn("No formula found for index(%d)", ForceEffectIndex);
        return;
    }

    Formula->OnApply(Runtime, Formula, Source, &Mob->Attributes, ForceValue);
}

Void RTMobCancelForceEffect(
    RTRuntimeRef Runtime,
    RTMobRef Mob,
    RTEntityID Source,
    Int ForceEffectIndex,
    Int64 ForceValue,
    Int ForceValueType
) {
    RTForceEffectFormulaRef Formula = (RTForceEffectFormulaRef)MemoryPoolFetch(Runtime->ForceEffectFormulaPool, ForceEffectIndex);
    if (!Formula) {
        Warn("No formula found for index(%d)", ForceEffectIndex);
        return;
    }

    Formula->OnCancel(Runtime, Formula, Source, &Mob->Attributes, ForceValue);
}
