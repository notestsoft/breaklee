#include "BattleSystem.h"
#include "Character.h"
#include "Constants.h"
#include "Force.h"
#include "Runtime.h"

Void RTForceEffectApplyIncreaseAttribute(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    for (Index Index = 0; Index < Formula->AttributeCount; Index += 1) {
        Attributes->Values[Formula->AttributeIndices[Index]] += Value;
    }
}

Void RTForceEffectCancelIncreaseAttribute(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    for (Index Index = 0; Index < Formula->AttributeCount; Index += 1) {
        Attributes->Values[Formula->AttributeIndices[Index]] -= Value;
    }
}

Void RTForceEffectApplyEnervation(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelEnervation(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectApplyStun(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelStun(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectApplyHeal(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelHeal(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectApplyDown(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelDown(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectApplyKnockBack(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelKnockBack(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectApplyImmobility(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelImmobility(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectApplyAbsorbDamage(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelAbsorbDamage(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectApplyReflectDamage(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelReflectDamage(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectApplyCraftMasteryUp(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

Void RTForceEffectCancelCraftMasteryUp(
    RTRuntimeRef Runtime,
    RTForceEffectFormulaRef Formula,
    RTBattleAttributesRef Attributes,
    Int64 Value
) {
    assert(false && "Implementation missing!");
}

RTForceEffectFormulaRef RTRuntimeAddForceEffectFormula(
    RTRuntimeRef Runtime,
    Index ForceEffectIndex,
	RTForceEffectFormulaCallback OnApply,
    RTForceEffectFormulaCallback OnCancel
) {
    RTForceEffectFormulaRef Formula = (RTForceEffectFormulaRef)MemoryPoolReserve(Runtime->ForceEffectFormulaPool, ForceEffectIndex);
    Formula->ForceEffectIndex = ForceEffectIndex;
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_NONE;
    Formula->OnApply = OnApply;
    Formula->OnCancel = OnCancel;
    return Formula;
}

Void RTForceEffectFormulaBindAttribute(
    RTForceEffectFormulaRef Formula,
    Index AttributeIndex
) {
    assert(Formula->AttributeCount < RUNTIME_MAX_FORCE_EFFECT_ATTRIBUTE_COUNT);
    
    Formula->AttributeIndices[Formula->AttributeCount] = AttributeIndex;
    Formula->AttributeCount += 1;
}

Void RTRuntimeInitForceEffectFormulas(
    RTRuntimeRef Runtime
) {
    RTForceEffectFormulaRef Formula = NULL;
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_MAX);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MP_MAX);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MIN_DAMAGE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MIN_DAMAGE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MIN_DAMAGE_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MIN_DAMAGE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_ABSORB_LIMIT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ABSORB_LIMIT_HP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_ABSORB_LIMIT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ABSORB_LIMIT_MP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_ATTACK_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_LIMIT_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ATTACK_RATE_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_DAMAGE_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRITICAL_RATE_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SWORD_SKILL_AMP_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MAGIC_SKILL_AMP_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_ABSORB, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ABSORB_RATE_HP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_ABSORB, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ABSORB_RATE_MP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EVADE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_REGEN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_AUTO_HEAL_HP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_MP_REGEN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_AUTO_HEAL_MP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_DAMAGE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ADD_DAMAGE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_DAMAGE2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ADD_DAMAGE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SKILL_EXP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SKILL_EXP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SKILL_EXP_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SKILL_EXP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALZ_DROP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALZ_DROP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALZ_DROP_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALZ_DROP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_STAT_STR, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STAT_STR);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_STAT_INT, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STAT_INT);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_STAT_DEX, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STAT_DEX);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ADD_STAT_ALL, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STAT_STR);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STAT_INT);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_STAT_DEX);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ONE_SLOT_DROP_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ONE_SLOT_DROP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ONE_SLOT_DROP_RATE_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ONE_SLOT_DROP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_TWO_SLOT_DROP_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_TWO_SLOT_DROP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_TWO_SLOT_DROP_RATE_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_TWO_SLOT_DROP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_INCREASED, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASED);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FINAL_DAMAGE_DECREASED, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASED);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_ATTACK_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DEFENSE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HP_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_HP_MAX);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ENERVATION, RTForceEffectApplyEnervation, RTForceEffectCancelEnervation);
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_STUN, RTForceEffectApplyStun, RTForceEffectCancelStun);
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_HEAL, RTForceEffectApplyHeal, RTForceEffectCancelHeal);
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DOWN, RTForceEffectApplyDown, RTForceEffectCancelDown);
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_KNOCK_BACK, RTForceEffectApplyKnockBack, RTForceEffectCancelKnockBack);
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IMMOBILITY, RTForceEffectApplyImmobility, RTForceEffectCancelImmobility);
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ABSORB_DAMAGE, RTForceEffectApplyAbsorbDamage, RTForceEffectCancelAbsorbDamage);
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_REFLECT_DAMAGE, RTForceEffectApplyReflectDamage, RTForceEffectCancelReflectDamage);
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CRAFT_MASTERY_UP, RTForceEffectApplyCraftMasteryUp, RTForceEffectCancelCraftMasteryUp);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_INCREASE_BATTLE_MODE_DURATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_INCREASE_BATTLE_MODE_DURATION);
 
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_EVASION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_EVASION);
       
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_ACCURACY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_ACCURACY);
       
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_DAMAGE_REDUCTION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_PENETRATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CANCEL_IGNORE_EVASION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CANCEL_IGNORE_EVASION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_AXP_BONUS, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_AXP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SKILL_EXP_BONUS, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SKILL_EXP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ACCURACY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CANCEL_IGNORE_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CANCEL_IGNORE_DAMAGE_REDUCTION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_SUPPRESSION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_SUPPRESSION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_SILENCE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_SILENCE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_INCREASE_SP_REGEN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_REGEN_SP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_CRITICAL_RATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_CRITICAL_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_CRITICAL_DAMAGE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_CRITICAL_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_IMMOBILITY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_UNMOVABLE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_DOWN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_KNOCK_BACK, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_KNOCK_BACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_STUN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_STUN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_WAR_EXP_BONUS, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_WAR_EXP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PARTY_EXP_BONUS, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PARTY_EXP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_EXP_BONUS, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EXP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PET_EXP_BONUS, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PET_EXP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALZ_DROP_RATE_INCREASE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALZ_DROP_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALZ_BOMP_RATE_INCREASE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ALZ_BOMB_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_SKILL_AMP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_ATTACK_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_ATTACK_UP3, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ALL_SKILL_AMP_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_INCREASE_MOVEMENT_SPEED, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MOVEMENT_SPEED);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SUPPRESSION_RATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SUPPRESSION_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SUPPRESSION_DURATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SUPPRESSION_DURATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SILENCE_RATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SILENCE_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_SILENCE_DURATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SILENCE_DURATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ROOT_RATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ROOT_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_ROOT_DURATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ROOT_DURATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_BOOST_HP_RESTORATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_BOOST_RESTORATION_HP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_RESIST_CRITICAL_RATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_RESIST_CRITICAL_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_ABSOLUTE_DAMAGE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_ABSOLUTE_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_RESIST_CRITICAL_DAMAGE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_RESIST_CRITICAL_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_INGORE_RESIST_SKILL_AMP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_RESIST_SKILL_AMP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_RESIST_DOWN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_RESIST_DOWN);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_RESIST_KNOCK_BACK, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_RESIST_KNOCK_BACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_IGNORE_RESIST_STUN, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_RESIST_STUN);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_NORMAL_DAMAGE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_NORMAL_ATTACK_DAMAGE_UP);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_AURA_MODE_DURATION_INCREASE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_INCREASE_AURA_MODE_DURATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_CANCEL_IGNORE_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CANCEL_IGNORE_PENETRATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_WAR_EXP_UP2, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_WAR_EXP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_FORCE_WING_EXP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FORCE_WING_EXP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_INCREASED_BOX_DROP_RATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_BOX_DROP_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_RESIST_AUTO_DAMAGE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_AUTO_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_INCREASE_MAX_BP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_BP_MAX);
        
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_DECREASE_BP_CONSUMPTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DECREASE_BP_CONSUMPTION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_RESIST_ALL_SKILL_AMP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_RESIST_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_ALL_SKILL_AMP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_DEFENSE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_IGNORE_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_PENETRATION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_CRITICAL_DAMAGE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_ALL_SKILL_AMP_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_CRITICAL_DAMAGE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_ALL_ATTACK_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_ALL_ATTACK_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK);
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_MAGIC_ATTACK);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_ATTACK_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_ATTACK_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ATTACK_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_CRITICAL_RATE, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CRITICAL_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_ACCURACY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_ACCURACY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_ACCURACY);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_PENETRATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_DEFENSE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_DEFENSE_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE_RATE);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_DEFENSE_RATE_UP, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DEFENSE_RATE);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_EVASION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_EVASION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_EVASION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_IGNORE_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_PENETRATION);

    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_FINAL_DAMAGE_DECREASED, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASED);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_FINAL_DAMAGE_DECREASED, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_DECREASED);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_FINAL_DAMAGE_INCREASED, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASED);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_FINAL_DAMAGE_INCREASED, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_FINAL_DAMAGE_INCREASED);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_CANCEL_IGNORE_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CANCEL_IGNORE_DAMAGE_REDUCTION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_CANCEL_IGNORE_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CANCEL_IGNORE_DAMAGE_REDUCTION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_CANCEL_IGNORE_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CANCEL_IGNORE_PENETRATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_CANCEL_IGNORE_PENETRATION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_CANCEL_IGNORE_PENETRATION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_IGNORE_ACCURACY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_ACCURACY);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_IGNORE_ACCURACY, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_ACCURACY);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_IGNORE_EVASION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_EVASION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_IGNORE_EVASION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_EVASION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVP_IGNORE_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVP;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_DAMAGE_REDUCTION);
    
    Formula = RTRuntimeAddForceEffectFormula(Runtime, RUNTIME_FORCE_EFFECT_PVE_IGNORE_DAMAGE_REDUCTION, RTForceEffectApplyIncreaseAttribute, RTForceEffectCancelIncreaseAttribute);
    Formula->ForceEffectTargetType = RUNTIME_FORCE_EFFECT_TARGET_TYPE_PVE;
    RTForceEffectFormulaBindAttribute(Formula, RUNTIME_ATTRIBUTE_IGNORE_DAMAGE_REDUCTION);
}

Void RTCharacterApplyForceEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Index ForceEffectIndex,
	Int64 ForceValue,
	Int32 ForceValueType
) {
	// TODO: Check if ForceValueType is relevant for the runtime
    if (ForceEffectIndex < 1) return;
    RTForceEffectFormulaRef Formula = (RTForceEffectFormulaRef)MemoryPoolFetch(Runtime->ForceEffectFormulaPool, ForceEffectIndex);
    Formula->OnApply(Runtime, Formula, &Character->Attributes, ForceValue);
}

Void RTCharacterCancelForceEffect(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Index ForceEffectIndex,
	Int64 ForceValue,
	Int32 ForceValueType
) {
    RTForceEffectFormulaRef Formula = (RTForceEffectFormulaRef)MemoryPoolFetch(Runtime->ForceEffectFormulaPool, ForceEffectIndex);
    Formula->OnCancel(Runtime, Formula, &Character->Attributes, ForceValue);
}
