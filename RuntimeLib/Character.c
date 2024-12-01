#include "BattleMode.h"
#include "Character.h"
#include "Inventory.h"
#include "Force.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Void RTCharacterInitializeConstantAttributes(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_EXP] = Character->Data.Info.Exp;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_LEVEL] = Character->Data.Info.Level;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE] += 20;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE] += 5;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_MAX] += 50;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = RUNTIME_MOVEMENT_SPEED_BASE;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_STR] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR];
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEX] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX];
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_INT] = Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT];
}

Void RTCharacterInitializeBattleStyleLevel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);

	RTBattleStyleLevelFormulaDataRef LevelFormula = RTRuntimeGetBattleStyleLevelFormulaData(Runtime, BattleStyleIndex);
	RTDataSpiritPointLimitRef SpiritPointLimitData = RTRuntimeDataSpiritPointLimitGet(Runtime->Context, Character->Data.StyleInfo.Style.BattleRank);
	assert(SpiritPointLimitData);

	RTDataRageLimitRef RageLimitData = RTRuntimeDataRageLimitGet(Runtime->Context, BattleStyleIndex);
	assert(RageLimitData);

	RTDataRageLimitLevelRef RageLimitLevelData = RTRuntimeDataRageLimitLevelGet(RageLimitData, Character->Data.StyleInfo.Style.BattleRank);
	assert(RageLimitLevelData);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] = RTCalculateBaseHP(
		Character->Data.Info.SkillRank,
		LevelFormula->BaseHP,
		LevelFormula->DeltaHP,
		Character->Data.Info.Level,
		Character->Data.StyleInfo.Style.BattleRank,
		LevelFormula->DeltaHP2
	);
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX] = (Int64)LevelFormula->BaseMP + LevelFormula->DeltaMP * (Character->Data.Info.Level - 1) / 10;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX] = SpiritPointLimitData->Value;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX] = 0;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX] = RageLimitLevelData->Value;
}

Void RTCharacterInitializeBattleStyleClass(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);

	RTBattleStyleClassFormulaDataRef ClassFormula = RTRuntimeGetBattleStyleClassFormulaData(Runtime, BattleStyleIndex);

	struct { Int32 AttributeIndex; Int32* Values; } ClassFormulaIndices[] = {
		{ RUNTIME_ATTRIBUTE_ATTACK, ClassFormula->Attack },
		{ RUNTIME_ATTRIBUTE_MAGIC_ATTACK, ClassFormula->MagicAttack },
		{ RUNTIME_ATTRIBUTE_DEFENSE, ClassFormula->Defense },
		{ RUNTIME_ATTRIBUTE_ATTACK_RATE, ClassFormula->AttackRate },
		{ RUNTIME_ATTRIBUTE_DEFENSE_RATE, ClassFormula->DefenseRate },
		{ RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION, ClassFormula->DamageReduction },
		{ RUNTIME_ATTRIBUTE_ACCURACY, ClassFormula->Accuracy },
		{ RUNTIME_ATTRIBUTE_PENETRATION, ClassFormula->Penetration },
		{ RUNTIME_ATTRIBUTE_ACCURACY_RESIST, ClassFormula->AccuracyPenalty },
		{ RUNTIME_ATTRIBUTE_PENETRATION_RESIST, ClassFormula->PenetrationPenalty },
	};

	Int32 ClassFormulaIndexCount = sizeof(ClassFormulaIndices) / sizeof(ClassFormulaIndices[0]);
	for (Int Index = 0; Index < ClassFormulaIndexCount; Index++) {
		Character->Attributes.Values[ClassFormulaIndices[Index].AttributeIndex] += (
			(Int64)Character->Data.StyleInfo.Style.BattleRank * ClassFormulaIndices[Index].Values[0] + ClassFormulaIndices[Index].Values[1]
		);
	}
}

Void RTCharacterInitializeBattleStyleStats(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);

	RTBattleStyleStatsFormulaDataRef StatsFormula = RTRuntimeGetBattleStyleStatsFormulaData(Runtime, BattleStyleIndex);

	struct { Int32 AttributeIndex; Int32* Values; Int32 SlopeID; } StatsFormulaIndices[] = {
		{ RUNTIME_ATTRIBUTE_ATTACK, StatsFormula->Attack, StatsFormula->AttackSlopeID },
		{ RUNTIME_ATTRIBUTE_MAGIC_ATTACK, StatsFormula->MagicAttack, StatsFormula->MagicAttackSlopeID },
		{ RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION, StatsFormula->DamageReduction, StatsFormula->DamageReductionSlopeID },
		{ RUNTIME_ATTRIBUTE_EVASION, StatsFormula->Evasion, StatsFormula->EvasionSlopeID },
		{ RUNTIME_ATTRIBUTE_ATTACK_RATE, StatsFormula->AttackRate, StatsFormula->AttackRateSlopeID },
		{ RUNTIME_ATTRIBUTE_DEFENSE_RATE, StatsFormula->DefenseRate, StatsFormula->DefenseRateSlopeID },
		{ RUNTIME_ATTRIBUTE_CRITICAL_RATE_RESIST, StatsFormula->ResistCriticalRate, StatsFormula->ResistCriticalRateSlopeID },
		{ RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST, StatsFormula->ResistSkillAmp, StatsFormula->ResistSkillAmpSlopeID },
		{ RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_RESIST, StatsFormula->ResistCriticalDamage, StatsFormula->ResistCriticalDamageSlopeID },
		{ RUNTIME_ATTRIBUTE_IMMOBILITY_RESIST, StatsFormula->ResistUnmovable, StatsFormula->ResistUnmovableSlopeID },
		{ RUNTIME_ATTRIBUTE_STUN_RESIST, StatsFormula->ResistStun, StatsFormula->ResistStunSlopeID },
		{ RUNTIME_ATTRIBUTE_DOWN_RESIST, StatsFormula->ResistDown, StatsFormula->ResistDownSlopeID },
		{ RUNTIME_ATTRIBUTE_KNOCK_BACK_RESIST, StatsFormula->ResistKnockback, StatsFormula->ResistKnockbackSlopeID },
		{ RUNTIME_ATTRIBUTE_HP_MAX, StatsFormula->HP, StatsFormula->HPSlopeID },
		{ RUNTIME_ATTRIBUTE_MP_MAX, StatsFormula->MP, StatsFormula->MPSlopeID },
		{ RUNTIME_ATTRIBUTE_PENETRATION_RESIST, StatsFormula->IgnorePenetration, StatsFormula->IgnorePenetrationSlopeID },
	};

	Int32 StatsFormulaIndexCount = sizeof(StatsFormulaIndices) / sizeof(StatsFormulaIndices[0]);
	Int32 StatsIndices[] = {
		RUNTIME_ATTRIBUTE_STR,
		RUNTIME_ATTRIBUTE_DEX,
		RUNTIME_ATTRIBUTE_INT,
	};
	Int32 StatsIndexCount = sizeof(StatsIndices) / sizeof(StatsIndices[0]);
	for (Int Index = 0; Index < StatsFormulaIndexCount; Index++) {
		Int32 AttributeValue = 0;

		for (Int StatsIndex = 0; StatsIndex < StatsIndexCount; StatsIndex++) {
			if (StatsFormulaIndices[Index].Values[StatsIndex] > 0) {
				RTBattleStyleSlopeDataRef SlopeData = RTRuntimeGetBattleStyleSlopeData(
					Runtime,
					StatsFormulaIndices[Index].SlopeID,
					Character->Attributes.Values[StatsIndices[StatsIndex]]
				);

				AttributeValue += Character->Attributes.Values[StatsIndices[StatsIndex]] * SlopeData->Slope + SlopeData->Intercept;
			}
		}

		Character->Attributes.Values[StatsFormulaIndices[Index].AttributeIndex] += AttributeValue / 1000;
	}
}

Void RTCharacterInitializeEquipment(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	// TODO: Apply equipment preset slots

    for (Int Index = 0; Index < Character->Data.EquipmentInfo.Info.EquipmentSlotCount; Index += 1) {
        RTItemSlotRef ItemSlot = &Character->Data.EquipmentInfo.EquipmentSlots[Index];
        RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
		if (ItemData->MinLevel > Character->Data.Info.Level) continue;

        if (!ItemData) {
            Warn("No item data found for item id: %d", ItemSlot->Item.ID);
            continue;
        }

        RTItemUseInternal(Runtime, Character, ItemSlot, ItemData, NULL);
    }
}

Void RTCharacterInitializeSkillStats(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	for (Int Index = 0; Index < Character->Data.SkillSlotInfo.Info.SlotCount; Index += 1) {
		RTSkillSlotRef SkillSlot = &Character->Data.SkillSlotInfo.Slots[Index];
		assert(SkillSlot);

		RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
		assert(SkillData);

		if (SkillData->SkillGroup != RUNTIME_SKILL_GROUP_PASSIVE) continue;
		
		for (Int ValueIndex = 0; ValueIndex < SkillData->SkillValueCount; ValueIndex += 1) {
			RTSkillValueDataRef SkillValue = &SkillData->SkillValues[ValueIndex];
			Int64 ForceValue = RTCalculateSkillValue(SkillValue, SkillSlot->Level, (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT]);

			RTCharacterApplyForceEffect(
				Runtime,
				Character, 
				kEntityIDNull,
				SkillValue->ForceEffectIndex,
				ForceValue,
				SkillValue->ValueType
			);
		}
	}
}

Void RTCharacterInitializeEssenceAbilities(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    for (Int Index = 0; Index < Character->Data.AbilityInfo.Info.EssenceAbilityCount; Index += 1) {
		RTEssenceAbilitySlotRef AbilitySlot = &Character->Data.AbilityInfo.EssenceAbilitySlots[Index];

		RTDataPassiveAbilityValueRef AbilityValue = RTRuntimeDataPassiveAbilityValueGet(
			Runtime->Context, 
			AbilitySlot->AbilityID
		);
		assert(AbilityValue);

		RTDataPassiveAbilityValueLevelRef AbilityLevel = RTRuntimeDataPassiveAbilityValueLevelGet(
			AbilityValue, 
			AbilitySlot->Level
		);
		assert(AbilityLevel);

		RTCharacterApplyForceEffect(
			Runtime,
			Character,
			kEntityIDNull,
			AbilityValue->ForceCode,
			AbilityLevel->ForceValue,
			AbilityValue->ValueType
		);
	}
}

Void RTCharacterInitializeBlendedAbilities(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Blended ability triggers
}

Void RTCharacterInitializeKarmaAbilities(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	for (Int Index = 0; Index < Character->Data.AbilityInfo.Info.KarmaAbilityCount; Index += 1) {
		RTKarmaAbilitySlotRef AbilitySlot = &Character->Data.AbilityInfo.KarmaAbilitySlots[Index];

		RTDataKarmaAbilityValueRef AbilityValue = RTRuntimeDataKarmaAbilityValueGet(
			Runtime->Context,
			AbilitySlot->AbilityID
		);
		assert(AbilityValue);

		RTDataKarmaAbilityValueLevelRef AbilityLevel = RTRuntimeDataKarmaAbilityValueLevelGet(
			AbilityValue,
			AbilitySlot->Level
		);
		assert(AbilityLevel);

		RTCharacterApplyForceEffect(
			Runtime,
			Character,
			kEntityIDNull,
			AbilityValue->ForceCode,
			AbilityLevel->ForceValue,
			AbilityValue->ValueType
		);
	}
}

Void RTCharacterInitializeBlessingBeads(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Blessing bead values
}

Void RTCharacterInitializePremiumServices(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Premium service values
}

Void RTCharacterInitializeAchievements(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Achievement values
}

Void RTCharacterInitializeGoldMeritMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Gold merit values
}

Void RTCharacterInitializePlatinumMeritMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Platinum merit values
}

Void RTCharacterInitializeDiamondMeritMastery(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	// TODO: Diamond merit values
}

Void RTCharacterInitializeOverlordMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	for (Int Index = 0; Index < Character->Data.OverlordMasteryInfo.Info.SlotCount; Index += 1) {
		RTOverlordMasterySlotRef MasterySlot = &Character->Data.OverlordMasteryInfo.Slots[Index];
		RTDataOverlordMasteryValueRef MasteryValue = RTRuntimeDataOverlordMasteryValueGet(Runtime->Context, MasterySlot->MasteryIndex);

		// TODO: ValueType refers to numeric or percentage ...

		Bool Found = false;
		for (Int LevelIndex = 0; LevelIndex < MasteryValue->OverlordMasteryValueLevelCount; LevelIndex += 1) {
			if (MasteryValue->OverlordMasteryValueLevelList[LevelIndex].MasteryLevel == MasterySlot->Level) {
				RTCharacterApplyForceEffect(
					Runtime,
					Character,
					kEntityIDNull,
					MasteryValue->OverlordMasteryValueLevelList[LevelIndex].ForceEffect,
					MasteryValue->OverlordMasteryValueLevelList[LevelIndex].ForceValue,
					MasteryValue->OverlordMasteryValueLevelList[LevelIndex].ForceValueType
				);

				Found = true;
				break;
			}

		}

		assert(Found);
	}
}

Void RTCharacterInitializeHonorMedalMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Honor medal values
}

Void RTCharacterInitializeForceWingMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	Int32 ForceValue = MAX(0, Character->Data.ForceWingInfo.Info.Grade - 1) * 100 + Character->Data.ForceWingInfo.Info.Level;
	RTCharacterApplyForceEffect(Runtime, Character, kEntityIDNull, RUNTIME_FORCE_EFFECT_HP_UP_1, ForceValue, RUNTIME_FORCE_VALUE_TYPE_ADDITIVE);
	RTCharacterApplyForceEffect(Runtime, Character, kEntityIDNull, RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_1, ForceValue, RUNTIME_FORCE_VALUE_TYPE_ADDITIVE);
	RTCharacterApplyForceEffect(Runtime, Character, kEntityIDNull, RUNTIME_FORCE_EFFECT_DEFENSE_UP_1, ForceValue, RUNTIME_FORCE_VALUE_TYPE_ADDITIVE);

    // TODO: Force wing training values
}

Void RTCharacterInitializeCollection(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Collection values
}

Void RTCharacterInitializeTranscendenceMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Transcendence values
}

Void RTCharacterInitializeStellarMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Stellar mastery values
}

Void RTCharacterInitializeMythMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Myth mastery values
}

// TODO: Split this up to resources and runtime attributes!!!
Void RTCharacterInitializeAttributes(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	memset(Character->Attributes.Values, 0, sizeof(Character->Attributes.Values));

	Character->Attributes.Seed = (Int32)PlatformGetTickCount();
	RTCharacterInitializeConstantAttributes(Runtime, Character);
    RTCharacterInitializeBattleStyleLevel(Runtime, Character);
	RTCharacterInitializeBattleStyleClass(Runtime, Character);
    RTCharacterInitializeSkillStats(Runtime, Character);
    RTCharacterInitializeEssenceAbilities(Runtime, Character);
    RTCharacterInitializeBlendedAbilities(Runtime, Character);
    RTCharacterInitializeKarmaAbilities(Runtime, Character);
    RTCharacterInitializeBlessingBeads(Runtime, Character);
    RTCharacterInitializePremiumServices(Runtime, Character);
    RTCharacterInitializeAchievements(Runtime, Character);
    RTCharacterInitializeGoldMeritMastery(Runtime, Character);
    RTCharacterInitializePlatinumMeritMastery(Runtime, Character);
	RTCharacterInitializeDiamondMeritMastery(Runtime, Character);
    RTCharacterInitializeOverlordMastery(Runtime, Character);
    RTCharacterInitializeHonorMedalMastery(Runtime, Character);
    RTCharacterInitializeForceWingMastery(Runtime, Character);
    RTCharacterInitializeCollection(Runtime, Character);
    RTCharacterInitializeTranscendenceMastery(Runtime, Character);
    RTCharacterInitializeStellarMastery(Runtime, Character);
    RTCharacterInitializeMythMastery(Runtime, Character);
	RTCharacterInitializeAnimaMastery(Runtime, Character);
	RTCharacterInitializeBattleStyleStats(Runtime, Character);
	RTCharacterInitializeEquipment(Runtime, Character);
	RTCharacterInitializeBattleMode(Runtime, Character);
	RTCharacterInitializeBuffs(Runtime, Character);

	RTCharacterSetHP(Runtime, Character, MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX],
		Character->Data.Info.CurrentHP
	), false);

	RTCharacterSetMP(Runtime, Character, MIN(
		(Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX],
		Character->Data.Info.CurrentMP
	), false);

	RTCharacterSetSP(Runtime, Character, MIN(
		(Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX],
		Character->Data.Info.CurrentSP
	));

	RTCharacterSetBP(Runtime, Character, MIN(
		(Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX],
		Character->Data.Info.CurrentBP
	));

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX],
		Character->Data.Info.CurrentRage
	);

	// TODO: Values are still broken and check HP! It is way too big, new deltas don't make sense or whats broken with the packets actually?
	
	RTDataAbilityExpRateRef AbilityExpRate = RTRuntimeDataAbilityExpRateGet(Runtime->Context, Character->Data.Info.Level);
	if (AbilityExpRate) {
		Character->AbilityExpRate = AbilityExpRate->Rate;
	}

	RTMovementSetSpeed(Runtime, &Character->Movement, (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED]);
}

Void RTCharacterUpdate(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return;

	Timestamp CurrentTimestamp = GetTimestampMs();
	if (Character->RegenUpdateTimestamp <= CurrentTimestamp) {
		Character->RegenUpdateTimestamp = CurrentTimestamp + RUNTIME_REGENERATION_INTERVAL;

		Int32 SpRegen = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_REGEN];
		Bool IsSpRegenBonusEnabled = (
			Character->Data.StyleInfo.LiveStyle.IsDancing ||
			Character->Data.StyleInfo.LiveStyle.IsDancing2 ||
			Character->Data.StyleInfo.LiveStyle.IsDancing3 ||
			Character->Data.StyleInfo.LiveStyle.IsTransforming
		);
		if (IsSpRegenBonusEnabled) {
			SpRegen += RTCalculateSPRegen((Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT]);
		}
		if (SpRegen > 0) {
			RTCharacterAddSP(Runtime, Character, SpRegen);
		}
	
		Int64 HpRegen = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_REGEN];
		HpRegen *= 100 + Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_REGEN_UP] - Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_REGEN_DOWN];
		HpRegen /= 100;
		if (HpRegen > 0) {
			RTCharacterAddHP(Runtime, Character, HpRegen, false);
		}

		Int32 MpRegen = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_REGEN];
		if (MpRegen > 0) {
			RTCharacterAddMP(Runtime, Character, MpRegen, false);
		}

		// TODO: Check if this is % value RUNTIME_ATTRIBUTE_HP_CURSE_RESIST,
		Int64 HpDown = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURSE];
		if (HpDown > 0) {
			RTCharacterSubtractHP(Runtime, Character, HpDown, true);
		}

		RTCharacterUpdateBattleMode(Runtime, Character);
	}

	RTCharacterUpdateBuffs(Runtime, Character, false);
}

Bool RTCharacterIsAlive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
    return Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] > 0;
}

Bool RTCharacterIsUnmovable(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	return false;
}

Bool RTCharacterMovementBegin(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 WorldID,
	Int32 PositionBeginX,
	Int32 PositionBeginY,
	Int32 PositionCurrentX,
	Int32 PositionCurrentY,
	Int32 PositionEndX,
	Int32 PositionEndY
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) return false;
	if (Character->Data.Info.WorldIndex != WorldID) return false;

	Int32 DeltaStartX = Character->Movement.PositionBegin.X - PositionBeginX;
	Int32 DeltaStartY = Character->Movement.PositionBegin.Y - PositionBeginY;
	Int32 DeltaCurrentX = Character->Movement.PositionCurrent.X - PositionBeginX;
	Int32 DeltaCurrentY = Character->Movement.PositionCurrent.Y - PositionBeginY;
	Int32 PositionChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 PositionChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;

	if ((Character->Movement.IsMoving) ||
		DeltaStartX * DeltaStartX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaStartY * DeltaStartY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaCurrentX * DeltaCurrentX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaCurrentY * DeltaCurrentY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	Int32 ChunkDeltaX = (PositionCurrentX >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkX;
	Int32 ChunkDeltaY = (PositionCurrentY >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkY;

	if (ChunkDeltaX < -2 || ChunkDeltaX > 2 ||
		ChunkDeltaY < -2 || ChunkDeltaY > 2) {
		return false;
	}

	Character->Movement.PositionBegin.X -= DeltaStartX;
	Character->Movement.PositionBegin.Y -= DeltaStartY;
	Character->Movement.PositionCurrent.X -= DeltaCurrentX;
	Character->Movement.PositionCurrent.Y -= DeltaCurrentY;
	Character->Movement.PositionEnd.X = PositionEndX;
	Character->Movement.PositionEnd.Y = PositionEndY;
	Character->Movement.Waypoints[0].X = PositionBeginX;
	Character->Movement.Waypoints[0].Y = PositionBeginY;
	Character->Movement.Waypoints[1].X = PositionCurrentX;
	Character->Movement.Waypoints[1].Y = PositionCurrentY;
	Character->Movement.WaypointCount = 2;

	RTMovementStartDeadReckoning(Runtime, &Character->Movement);
	
	return true;
}

Bool RTCharacterMovementChange(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 WorldID,
	Int32 PositionBeginX,
	Int32 PositionBeginY,
	Int32 PositionCurrentX,
	Int32 PositionCurrentY,
	Int32 PositionEndX,
	Int32 PositionEndY
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) return false;
	if (!(Character->Movement.IsMoving)) return false;
	assert(Character->Movement.WorldContext);

	RTPositionRef WaypointA = &Character->Movement.Waypoints[0];
	RTPositionRef WaypointB = &Character->Movement.Waypoints[1];

	if (PositionBeginX < 0 || PositionBeginX >= RUNTIME_WORLD_SIZE ||
		PositionBeginY < 0 || PositionBeginY >= RUNTIME_WORLD_SIZE ||
		PositionEndX < 0 || PositionEndX >= RUNTIME_WORLD_SIZE ||
		PositionEndY < 0 || PositionEndY >= RUNTIME_WORLD_SIZE ||
		PositionCurrentX < 0 || PositionCurrentX >= RUNTIME_WORLD_SIZE ||
		PositionCurrentY < 0 || PositionCurrentY >= RUNTIME_WORLD_SIZE) {
		return false;
	}

	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointA->Y;
	Int32 AbsDeltaX = (DeltaX < 0) ? -DeltaX : DeltaX;
	Int32 AbsDeltaY = (DeltaY < 0) ? -DeltaY : DeltaY;

	Int32 DeltaStartX, DeltaStartY;
	if (AbsDeltaX >= AbsDeltaY) {
		if (AbsDeltaX == 0) {
			AbsDeltaX = 1;
		}

		Int32 OffsetX = (DeltaX > 0) ? (PositionBeginX - WaypointA->X) : (WaypointA->X - PositionBeginX);
		Int32 OffsetY = (AbsDeltaY * OffsetX + (AbsDeltaX >> 1)) / AbsDeltaX;

		if (DeltaY < 0)
			OffsetY = -OffsetY;

		DeltaStartX = WaypointA->X + OffsetX * DeltaX / AbsDeltaX - PositionBeginX;
		DeltaStartY = WaypointA->Y + OffsetY - PositionBeginY;
	}
	else {
 		if (AbsDeltaY == 0) {
			AbsDeltaY = 1;
		}
   
		Int32 OffsetY = (DeltaY > 0) ? (PositionBeginY - WaypointA->Y) : (WaypointA->Y - PositionBeginY);
		Int32 OffsetX = (AbsDeltaX * OffsetY + (AbsDeltaY >> 1)) / AbsDeltaY;

		if (DeltaX < 0)
			OffsetX = -OffsetX;

		DeltaStartX = WaypointA->X + OffsetX - PositionBeginX;
		DeltaStartY = WaypointA->Y + OffsetY * DeltaY / AbsDeltaY - PositionBeginY;
	}

	if (DeltaStartX * DeltaStartX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaStartY * DeltaStartY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	Int32 PositionChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 PositionChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 DeltaChunkX = (PositionCurrentX >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkX;
	Int32 DeltaChunkY = (PositionCurrentY >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkY;

	if (DeltaChunkX < -2 || DeltaChunkX > 2 ||
		DeltaChunkY < -2 || DeltaChunkY > 2) {
		return false;
	}

	WaypointA->X = PositionBeginX;
	WaypointA->Y = PositionBeginY;
	WaypointB->X = PositionCurrentX;
	WaypointB->Y = PositionCurrentY;

	Character->Movement.WaypointCount = 2;
	Character->Movement.PositionCurrent.X = PositionBeginX;
	Character->Movement.PositionCurrent.Y = PositionBeginY;
	Character->Movement.PositionBegin.X = PositionBeginX;
	Character->Movement.PositionBegin.Y = PositionBeginY;
	Character->Movement.PositionEnd.X = PositionEndX;
	Character->Movement.PositionEnd.Y = PositionEndY;

	RTMovementStartDeadReckoning(Runtime, &Character->Movement);

	return true;
}

Bool RTCharacterMovementEnd(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 PositionX,
	Int32 PositionY
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) return false;
	if (!(Character->Movement.IsMoving)) return false;

	RTPositionRef WaypointA = &Character->Movement.Waypoints[0];
	RTPositionRef WaypointB = &Character->Movement.Waypoints[1];
	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointB->Y;
	Int32 AbsDeltaX = (DeltaX < 0) ? -DeltaX : DeltaX;
	Int32 AbsDeltaY = (DeltaY < 0) ? -DeltaY : DeltaY;

	Int32 DeltaStartX, DeltaStartY;
	if (AbsDeltaX >= AbsDeltaY) {
		if (AbsDeltaX == 0) {
			AbsDeltaX = 1;
		}

		Int32 OffsetX = (DeltaX > 0) ? (PositionX - WaypointA->X) : (WaypointA->X - PositionX);
		Int32 OffsetY = (AbsDeltaY * OffsetX + (AbsDeltaX >> 1)) / AbsDeltaX;

		if (DeltaY < 0) {
			OffsetY = -OffsetY;
		}

		DeltaStartX = WaypointA->X + OffsetX * DeltaX / AbsDeltaX - PositionX;
		DeltaStartY = WaypointA->Y + OffsetY - PositionY;
	}
	else {
		if (AbsDeltaY == 0) {
			AbsDeltaY = 1;
		}

		Int32 OffsetY = (DeltaY > 0) ? (PositionY - WaypointA->Y) : (WaypointA->Y - PositionY);
		Int32 OffsetX = (AbsDeltaX * OffsetY + (AbsDeltaY >> 1)) / AbsDeltaY;

		if (DeltaX < 0) {
			OffsetX = -OffsetX;
		}

		DeltaStartX = WaypointA->X + OffsetX - PositionX;
		DeltaStartY = WaypointA->Y + OffsetY * DeltaY / AbsDeltaY - PositionY;
	}

	if (DeltaStartX * DeltaStartX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaStartY * DeltaStartY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	Character->Movement.PositionBegin.X = PositionX;
	Character->Movement.PositionBegin.Y = PositionY;
	Character->Movement.PositionCurrent.X = PositionX;
	Character->Movement.PositionCurrent.Y = PositionY;
	Character->Movement.PositionEnd.X = PositionX;
	Character->Movement.PositionEnd.Y = PositionY;

	RTMovementEndDeadReckoning(Runtime, &Character->Movement);

	Character->Data.Info.PositionX = PositionX;
	Character->Data.Info.PositionY = PositionY;
	Character->SyncMask.Info = true;

	return true;
}

// TODO: Check if this function is correct
Bool RTCharacterMovementChangeWaypoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 PositionCurrentX,
	Int32 PositionCurrentY,
	Int32 PositionNextX,
	Int32 PositionNextY
) {
	if (!RTCharacterIsAlive(Runtime, Character)) return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) return false;
	if (!(Character->Movement.IsMoving)) return false;

	Int32 DeltaX = PositionNextX - PositionCurrentX;
	Int32 DeltaY = PositionNextY - PositionCurrentY;
	Int32 AbsDeltaX = (DeltaX >= 0) ? DeltaX : -DeltaX;
	Int32 AbsDeltaY = (DeltaY >= 0) ? DeltaY : -DeltaY;

	if (AbsDeltaX < 0 || AbsDeltaX >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH ||
		AbsDeltaY < 0 || AbsDeltaY >= RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH) {
		return false;
	}

	RTPositionRef WaypointA = &Character->Movement.Waypoints[0];
	RTPositionRef WaypointB = &Character->Movement.Waypoints[1];

	WaypointA->X = PositionCurrentX;
	WaypointA->Y = PositionCurrentY;
	WaypointB->X = PositionNextX;
	WaypointB->Y = PositionNextY;
	Character->Movement.WaypointCount = 2;

	RTMovementRestartDeadReckoning(Runtime, &Character->Movement);

	Character->Movement.PositionCurrent.X = PositionCurrentX;
	Character->Movement.PositionCurrent.Y = PositionCurrentY;
	Character->SyncMask.Info = true;

	return true;
}

Bool RTCharacterMovementChangeChunk(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 PositionCurrentX,
	Int32 PositionCurrentY
) {
	if (!RTCharacterIsAlive(Runtime, Character))
		return false;
	if (RTCharacterIsUnmovable(Runtime, Character)) 
		return false;
	if (!(Character->Movement.IsMoving)) 
		return false;

	if (PositionCurrentX >= RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		PositionCurrentY >= RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	RTPositionRef WaypointA = &Character->Movement.Waypoints[0];
	RTPositionRef WaypointB = &Character->Movement.Waypoints[1];
	Int32 DeltaX = WaypointB->X - WaypointA->X;
	Int32 DeltaY = WaypointB->Y - WaypointB->Y;
	Int32 AbsDeltaX = (DeltaX < 0) ? -DeltaX : DeltaX;
	Int32 AbsDeltaY = (DeltaY < 0) ? -DeltaY : DeltaY;
	Int32 DeltaStartX = 0;
	Int32 DeltaStartY = 0;

	if (AbsDeltaX >= AbsDeltaY) {
		if (AbsDeltaX == 0) { 
			AbsDeltaX = 1;
		}

		Int32 OffsetX = (DeltaX > 0) ? (PositionCurrentX - WaypointA->X) : (WaypointA->X - PositionCurrentX);
		Int32 OffsetY = (AbsDeltaY * OffsetX + (AbsDeltaX >> 1)) / AbsDeltaX;

		if (DeltaY < 0) {
			OffsetY = -OffsetY;
		}

		DeltaStartX = WaypointA->X + OffsetX * DeltaX / AbsDeltaX - PositionCurrentX;
		DeltaStartY = WaypointA->Y + OffsetY - PositionCurrentY;
	} 
	else {
		if (AbsDeltaY == 0) {
			AbsDeltaY = 1;
		}
        
		Int32 OffsetY = (DeltaY > 0) ? (PositionCurrentY - WaypointA->Y) : (WaypointA->Y - PositionCurrentY);
		Int32 OffsetX = (AbsDeltaX * OffsetY + (AbsDeltaY >> 1)) / AbsDeltaY;

		if (DeltaX < 0) {
			OffsetX = -OffsetX;
		}

		DeltaStartX = WaypointA->X + OffsetX - PositionCurrentX;
		DeltaStartY = WaypointA->Y + OffsetY * DeltaY / AbsDeltaY - PositionCurrentY;
	}

	if (DeltaStartY * DeltaStartY > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE ||
		DeltaStartX * DeltaStartX > RUNTIME_MOVEMENT_MAX_ERROR_TOLERANCE) {
		return false;
	}

	Int32 PositionChunkX = Character->Movement.PositionCurrent.X >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 PositionChunkY = Character->Movement.PositionCurrent.Y >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT;
	Int32 DeltaChunkX = (PositionCurrentX >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkX;
	Int32 DeltaChunkY = (PositionCurrentY >> RUNTIME_WORLD_CHUNK_SIZE_EXPONENT) - PositionChunkY;
	Trace("SetChunkPos(%d, %d)", PositionChunkX, PositionChunkY);
	if (DeltaChunkX == 0 && DeltaChunkY == 0) {
		return true;
	}

	if (DeltaChunkX < -1 || DeltaChunkX > 1 || DeltaChunkY < -1 || DeltaChunkY > 1) {
		return false;
	}

    RTMovementSetPosition(Runtime, &Character->Movement, PositionCurrentX, PositionCurrentY);

	Character->Movement.PositionCurrent.X = PositionCurrentX;
	Character->Movement.PositionCurrent.Y = PositionCurrentY;
	Character->SyncMask.Info = true;

	return true;
}

Int32 RTCharacterCalculateRequiredMP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 CoefficientA,
	Int32 CoefficientB,
	Int32 SkillLevel
) {
	Int32 RequiredMP = (CoefficientA * SkillLevel + CoefficientB) / 10;
	// TODO: Check if level table is calculating correct value
	RequiredMP += (Int32)(SkillLevel / 10) * CoefficientA / 10;
	RequiredMP += (Int32)(SkillLevel / 13) * CoefficientA / 10;
	RequiredMP += (Int32)(SkillLevel / 16) * CoefficientA / 10;
	RequiredMP += (Int32)(SkillLevel / 19) * CoefficientA / 10;
	
	// TODO: Add battle mode based MP usage delta
	RequiredMP -= Character->SkillComboLevel * RequiredMP * 2 / 10;
	return MAX(0, RequiredMP);
}

Void RTCharacterSetBattleRank(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 BattleRank
) {
	Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);
	for (Int NextRank = Character->Data.StyleInfo.Style.BattleRank + 1; NextRank <= BattleRank; NextRank += 1) {
		RTBattleStyleRankDataRef NextRankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Data.StyleInfo.Style.BattleRank + 1);
        if (!NextRankData) break;

        if (NextRankData->SkillSlot[0] > 0 && NextRankData->SkillIndex[0] > 0) {
            RTCharacterAddSkillSlot(
                Runtime,
                Character,
                NextRankData->SkillSlot[0],
                1,
                NextRankData->SkillIndex[0]
            );

            Character->SyncMask.SkillSlotInfo = true;
        }

        if (NextRankData->SkillSlot[1] > 0 && NextRankData->SkillIndex[1] > 0) {
            RTCharacterAddSkillSlot(
                Runtime,
                Character,
                NextRankData->SkillSlot[1],
                1,
                NextRankData->SkillIndex[1]
            );

            Character->SyncMask.SkillSlotInfo = true;
        }

        NOTIFICATION_DATA_CHARACTER_BATTLE_RANK_UP* RankUpNotification = RTNotificationInit(CHARACTER_BATTLE_RANK_UP);
        RankUpNotification->Level = NextRankData->Level;
        RTNotificationDispatchToCharacter(RankUpNotification, Character);

        NOTIFICATION_DATA_CHARACTER_EVENT* CharacterEventNotification = RTNotificationInit(CHARACTER_EVENT);
        CharacterEventNotification->Type = NOTIFICATION_CHARACTER_EVENT_TYPE_RANK_UP;
        CharacterEventNotification->CharacterIndex = (UInt32)Character->CharacterIndex;
        RTNotificationDispatchToNearby(CharacterEventNotification, Character->Movement.WorldChunk);
	}

	Character->Data.StyleInfo.Style.BattleRank = BattleRank;
	Character->SyncMask.StyleInfo = true;
}

Bool RTCharacterBattleRankUp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);
	RTBattleStyleRankDataRef RankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Data.StyleInfo.Style.BattleRank);
	assert(RankData);

	RTBattleStyleRankDataRef NextRankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Data.StyleInfo.Style.BattleRank + 1);
	if (!NextRankData) return false;

	if (RankData->ConditionSTR > Character->Attributes.Values[RUNTIME_ATTRIBUTE_STR] ||
		RankData->ConditionDEX > Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEX] ||
		RankData->ConditionINT > Character->Attributes.Values[RUNTIME_ATTRIBUTE_INT]) {
		return false;
	}

	// TODO: Check if we need to add the skills of current or next rank here..
	if (RankData->SkillSlot[0] > 0 && RankData->SkillIndex[0] > 0) {
		RTCharacterAddSkillSlot(
			Runtime,
			Character,
			RankData->SkillSlot[0],
			1,
			RankData->SkillIndex[0]
		);

		Character->SyncMask.SkillSlotInfo = true;
	}

	if (RankData->SkillSlot[1] > 0 && RankData->SkillIndex[1] > 0) {
		RTCharacterAddSkillSlot(
			Runtime,
			Character,
			RankData->SkillSlot[1],
			1,
			RankData->SkillIndex[1]
		);

		Character->SyncMask.SkillSlotInfo = true;
	}

	Character->Data.StyleInfo.Style.BattleRank += 1;
	Character->SyncMask.StyleInfo = true;

    {
        NOTIFICATION_DATA_CHARACTER_BATTLE_RANK_UP* Notification = RTNotificationInit(CHARACTER_BATTLE_RANK_UP);
        Notification->Level = Character->Data.StyleInfo.Style.BattleRank;
        RTNotificationDispatchToCharacter(Notification, Character);
    }
    
    {
        NOTIFICATION_DATA_CHARACTER_EVENT* Notification = RTNotificationInit(CHARACTER_EVENT);
        Notification->Type = NOTIFICATION_CHARACTER_EVENT_TYPE_RANK_UP;
        Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
        RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
    }

	return true;
}

Void RTCharacterAddExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	UInt64 Axp = (Exp / 100) * Character->AbilityExpRate;
	RTCharacterAddAbilityExp(Runtime, Character, Axp);

	if (Character->Data.OverlordMasteryInfo.Info.Level > 0) {
		RTCharacterAddOverlordExp(Runtime, Character, Exp);
		return;
	}

	Int32 CurrentLevel = Character->Data.Info.Level;
	
	Character->Data.Info.Exp += Exp;
	Character->Data.Info.Level = RTRuntimeGetLevelByExp(Runtime, CurrentLevel, &Character->Data.Info.Exp);
	Character->SyncMask.Info = true;

	Int32 LevelDiff = Character->Data.Info.Level - CurrentLevel;
    if (LevelDiff > 0) {
		RTCharacterInitializeAttributes(Runtime, Character);
		RTCharacterSetHP(Runtime, Character, Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX], false);
		RTCharacterSetMP(Runtime, Character, (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX], false);
		Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += LevelDiff * 5;
		Character->SyncMask.Info = true;

		for (Int Index = 0; Index < LevelDiff; Index += 1) {
			NOTIFICATION_DATA_CHARACTER_DATA* Notification = RTNotificationInit(CHARACTER_DATA);
			Notification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_LEVEL;
			Notification->Level = CurrentLevel + Index + 1;

			RTDataMapCodeRef MapCode = RTRuntimeDataMapCodeGet(Runtime->Context, CurrentLevel + Index + 1);
			if (MapCode) {
				Character->Data.StyleInfo.MapsMask |= 1 << (MapCode->WorldID - 1);
				Character->Data.StyleInfo.WarpMask |= 1 << (MapCode->WorldID - 1);
				Character->SyncMask.StyleInfo = true;

				Notification->MapCode = 1 << (MapCode->WorldID - 1);
				Notification->WarpCode = 1 << (MapCode->WorldID - 1);
			}

			RTNotificationDispatchToCharacter(Notification, Character);

			{
				NOTIFICATION_DATA_CHARACTER_EVENT* EventNotification = RTNotificationInit(CHARACTER_EVENT);
				EventNotification->Type = NOTIFICATION_CHARACTER_EVENT_TYPE_LEVEL_UP;
				EventNotification->CharacterIndex = (UInt32)Character->CharacterIndex;
				RTNotificationDispatchToNearby(EventNotification, Character->Movement.WorldChunk);
			}
		}

		RTDataLevelRef NextLevelData = RTRuntimeDataLevelGet(Runtime->Context, Character->Data.Info.Level + 1);
		if (!NextLevelData) {
			Character->Data.NewbieSupportInfo.Info.Timestamp = GetTimestamp() + Runtime->Config.NewbieSupportTimeout;
			Character->SyncMask.NewbieSupportInfo = true;
		}

		if (Character->Data.OverlordMasteryInfo.Info.Level < 1) {
			RTDataOverlordMasteryStartRef Start = RTRuntimeDataOverlordMasteryStartGet(Runtime->Context);
			if (Character->Data.Info.Level == Start->RequiredLevel) {
				Character->Data.OverlordMasteryInfo.Info.Level = 1;
				Character->Data.OverlordMasteryInfo.Info.Exp = 0;
				Character->Data.OverlordMasteryInfo.Info.Point = Start->MasteryPointCount;
				Character->SyncMask.OverlordMasteryInfo = true;

				RTRuntimeBroadcastCharacterData(
					Runtime,
					Character,
					NOTIFICATION_CHARACTER_DATA_TYPE_OVERLORD_LEVEL
				);

				{
					NOTIFICATION_DATA_CHARACTER_EVENT* Notification = RTNotificationInit(CHARACTER_EVENT);
					Notification->Type = NOTIFICATION_CHARACTER_EVENT_TYPE_OVERLORD_LEVEL_UP;
					Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
					RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
				}
            }
		}
    }
}

Int32 RTCharacterAddSkillExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int64 SkillExp
) {
	RTCharacterAddWingExp(Runtime, Character, SkillExp);

	Int32 CurrentSkillLevel = Character->Data.Info.SkillLevel;
	Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);
	RTBattleStyleSkillRankDataRef SkillRankData = RTRuntimeGetBattleStyleSkillRankData(
		Runtime,
		BattleStyleIndex,
		Character->Data.Info.SkillRank
	);

	Int32 SkillLevelMax = RTRuntimeDataCharacterRankUpConditionGet(
		Runtime->Context,
		Character->Data.Info.SkillRank,
		BattleStyleIndex
	);
    
	Int32 CurrentSkillExp = CurrentSkillLevel * SkillRankData->SkillLevelExp + (Int32)Character->Data.Info.SkillExp;
	Int32 MaxSkillExp = SkillLevelMax * SkillRankData->SkillLevelExp;
	Int32 FinalSkillExp = MIN(MaxSkillExp, CurrentSkillExp + (Int32)SkillExp);
	Int32 FinalSkillLevel = FinalSkillExp / SkillRankData->SkillLevelExp;
	Int32 SkillLevelDiff = FinalSkillLevel - CurrentSkillLevel;
	Int32 ReceivedSkillExp = FinalSkillExp - CurrentSkillExp;

    if (SkillLevelDiff > 0 || ReceivedSkillExp > 0) {
		Character->Data.Info.SkillExp = FinalSkillExp % SkillRankData->SkillLevelExp;
		Character->Data.Info.SkillLevel += SkillLevelDiff;
		Character->Data.Info.SkillPoint += SkillLevelDiff;
		Character->SyncMask.Info = true;
		
		NOTIFICATION_DATA_CHARACTER_SKILL_MASTERY_UPDATE* Notification = RTNotificationInit(CHARACTER_SKILL_MASTERY_UPDATE);
        Notification->SkillRank = Character->Data.Info.SkillRank;
        Notification->SkillLevel = Character->Data.Info.SkillLevel;
        Notification->SkillLevelMax = SkillLevelMax;
        Notification->SkillExp = (UInt32)Character->Data.Info.SkillExp;
        Notification->SkillPoint = Character->Data.Info.SkillPoint;
        RTNotificationDispatchToCharacter(Notification, Character);
    }

	Bool CanRankUp = Character->Data.Info.SkillLevel >= SkillLevelMax;
	if (CanRankUp && Runtime->Config.IsSkillRankUpLimitEnabled) {
		RTDataCharacterRankUpLimitRef RankUpLimit = RTRuntimeDataCharacterRankUpLimitGet(Runtime->Context, Character->Data.Info.SkillRank + 1);
		if (RankUpLimit) {
			CanRankUp &= RankUpLimit->Level <= Character->Data.Info.Level;
			CanRankUp &= RankUpLimit->BattleRank <= Character->Data.StyleInfo.Style.BattleRank;
		}
	}

	if (CanRankUp) {
		RTBattleStyleSkillRankDataRef NextSkillRankData = RTRuntimeGetBattleStyleSkillRankData(
			Runtime,
			BattleStyleIndex,
			Character->Data.Info.SkillRank + 1
		);
		
		if (NextSkillRankData) {
			RTDataCharacterRankUpBonusRef RankUpBonus = RTRuntimeDataCharacterRankUpBonusGet(Runtime->Context, Character->Data.Info.SkillRank);
			if (RankUpBonus) {
				// TODO: Add skill slot count to character data!
				Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += RankUpBonus->StatPoints;
			}

			Character->Data.Info.SkillRank = NextSkillRankData->SkillRank;
			Character->Data.Info.SkillLevel = 0;
			Character->SyncMask.Info = true;
            
            RTRuntimeBroadcastCharacterData(
                Runtime,
                Character,
                NOTIFICATION_CHARACTER_DATA_TYPE_RANK
            );
            
            {
                NOTIFICATION_DATA_CHARACTER_EVENT* Notification = RTNotificationInit(CHARACTER_EVENT);
                Notification->Type = NOTIFICATION_CHARACTER_EVENT_TYPE_RANK_UP;
                Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
                RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
            }
		}
	}

	return ReceivedSkillExp;
}

Void RTCharacterAddHonorPoint(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int64 HonorPoint
) {
	Int64 NewHonorPoint = Character->Data.Info.HonorPoint + HonorPoint;
	NewHonorPoint = MIN(Runtime->Config.MaxHonorPoint, NewHonorPoint);
	NewHonorPoint = MAX(Runtime->Config.MinHonorPoint, NewHonorPoint);
	Character->Data.Info.HonorPoint = NewHonorPoint;
	Character->SyncMask.Info = true;
}

Void RTCharacterAddAbilityExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	if (Exp < 1) return;

	Character->Data.AbilityInfo.Info.Axp += Exp;

	while (Character->Data.AbilityInfo.Info.Axp >= RUNTIME_CHARACTER_AXP_PER_LEVEL) {
		Character->Data.AbilityInfo.Info.Axp -= RUNTIME_CHARACTER_AXP_PER_LEVEL;
		Character->Data.AbilityInfo.Info.AP += 1;
	}

	Character->SyncMask.AbilityInfo = true;
}

Bool RTCharacterAddStats(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32* Stats
) {
	Int32 RequiredPoints = Stats[RUNTIME_CHARACTER_STAT_STR] + Stats[RUNTIME_CHARACTER_STAT_DEX] + Stats[RUNTIME_CHARACTER_STAT_INT];
	if (Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] < RequiredPoints) {
		return false;
	}

	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] += Stats[RUNTIME_CHARACTER_STAT_STR];
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] += Stats[RUNTIME_CHARACTER_STAT_DEX];
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT] += Stats[RUNTIME_CHARACTER_STAT_INT];
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] -= RequiredPoints;

	Character->SyncMask.Info = true;

	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

Bool RTCharacterRemoveStat(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 StatIndex,
	Int32 Amount
) {
	assert(0 <= StatIndex && StatIndex <= 3);
	
	// TODO: Game doesn't allow to delete stat points below the class rank level so we should check that also here..

	if (Character->Data.Info.Stat[StatIndex] < Amount) return false;

	Character->Data.Info.Stat[StatIndex] -= Amount;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += Amount;

	Character->SyncMask.Info = true;

	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

Bool RTCharacterResetStats(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);
	RTBattleStyleRankDataRef PreviousRankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Data.StyleInfo.Style.BattleRank - 1);
	if (!PreviousRankData) return false;

	Int32 AccumulatedPoints = 0;
	AccumulatedPoints += Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] - PreviousRankData->ConditionSTR;
	AccumulatedPoints += Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] - PreviousRankData->ConditionDEX;
	AccumulatedPoints += Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT] - PreviousRankData->ConditionINT;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] = PreviousRankData->ConditionSTR;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] = PreviousRankData->ConditionDEX;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT] = PreviousRankData->ConditionINT;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += AccumulatedPoints;
	Character->SyncMask.Info = true;

	return true;
}

Void RTCharacterSetHP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int64 NewValue,
	Bool IsPotion
) {
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = NewValue;
	Character->Data.Info.CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Character->SyncMask.Info = true;

	RTRuntimeBroadcastCharacterData(
		Runtime,
		Character,
		IsPotion ? NOTIFICATION_CHARACTER_DATA_TYPE_HPPOTION : NOTIFICATION_CHARACTER_DATA_TYPE_HP
	);
}

Void RTCharacterAddHP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int64 HP,
	Bool IsPotion
) {
	Int64 NewValue = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	NewValue += HP;
	NewValue = MAX(NewValue, 0);
	NewValue = MIN(NewValue, Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX]);
	RTCharacterSetHP(Runtime, Character, NewValue, IsPotion);
}

Void RTCharacterSubtractHP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int64 HP,
	Bool IsCurse
) {
	Int64 NewValue = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	NewValue -= HP;
	NewValue = MAX(NewValue, 0);
	NewValue = MIN(NewValue, Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX]);
	if (IsCurse) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = NewValue;
		Character->Data.Info.CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		Character->SyncMask.Info = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			NOTIFICATION_CHARACTER_DATA_TYPE_DAMAGE_CELL
		);
	}
	else {
		RTCharacterSetHP(Runtime, Character, NewValue, false);
	}
}

Void RTCharacterSetMP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 NewValue,
	Bool IsPotion
) {
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = NewValue;
	Character->Data.Info.CurrentMP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
	Character->SyncMask.Info = true;

	RTRuntimeBroadcastCharacterData(
		Runtime,
		Character,
		IsPotion ? NOTIFICATION_CHARACTER_DATA_TYPE_MPPOTION : NOTIFICATION_CHARACTER_DATA_TYPE_MP
	);
}

Void RTCharacterAddMP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MP,
	Bool IsPotion
) {
	Int64 NewValue = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
	NewValue += MP;
	NewValue = MAX(NewValue, 0);
	NewValue = MIN(NewValue, Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX]);
	RTCharacterSetMP(Runtime, Character, (Int32)NewValue, IsPotion);
}

Void RTCharacterSetSP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 NewValue
) {
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] = NewValue;
	Character->Data.Info.CurrentSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
	Character->SyncMask.Info = true;

	RTRuntimeBroadcastCharacterData(
		Runtime,
		Character,
		NOTIFICATION_CHARACTER_DATA_TYPE_SP
	);
}

Void RTCharacterAddSP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SP
) {
	Int64 NewValue = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
	NewValue += SP;
	NewValue = MAX(NewValue, 0);
	NewValue = MIN(NewValue, Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX]);
	RTCharacterSetSP(Runtime, Character, (Int32)NewValue);
}

Void RTCharacterSetBP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 NewValue
) {
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT] = NewValue;
	Character->Data.Info.CurrentBP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
	Character->SyncMask.Info = true;

	RTRuntimeBroadcastCharacterData(
		Runtime,
		Character,
		NOTIFICATION_CHARACTER_DATA_TYPE_BP
	);
}

Void RTCharacterAddBP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 BP
) {
	Int64 NewValue = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
	NewValue += BP;
	NewValue = MAX(NewValue, 0);
	NewValue = MIN(NewValue, Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX]);
	RTCharacterSetBP(Runtime, Character, (Int32)NewValue);
}

Void RTCharacterAddRage(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Rage
) {
	Int64 NewValue = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX],
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] + MAX(0, Rage)
	);

	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT]) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] = NewValue;
		Character->Data.Info.CurrentRage = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
		Character->SyncMask.Info = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			NOTIFICATION_CHARACTER_DATA_TYPE_RAGE
		);
	}
}

Bool RTCharacterConsumeRage(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Rage
) {
	assert(Rage >= 0);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] < Rage) return false;

	Int64 NewValue = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX],
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] - Rage
	);

	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT]) {
        Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] = NewValue;
		Character->Data.Info.CurrentRage = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
		Character->SyncMask.Info = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			NOTIFICATION_CHARACTER_DATA_TYPE_RAGE
		);
	}

	return true;
}

Void RTCharacterApplyDamage(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTEntityID Source,
	Int32 Damage
) {
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MAX(0,
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] -
		Damage
	);

	// TODO: Send notification
	Character->SyncMask.Info = true;
}

Void RTCharacterNotifyStatus(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	NOTIFICATION_DATA_CHARACTER_STATUS* Notification = RTNotificationInit(CHARACTER_STATUS);
	Notification->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Notification->CurrentMP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
	Notification->CurrentShield = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
	Notification->BattleModeInfo = Character->Data.BattleModeInfo;
	Notification->BuffInfo = Character->Data.BuffInfo.Info;

	Int32 BuffSlotCount = (
		Character->Data.BuffInfo.Info.SkillBuffCount +
		Character->Data.BuffInfo.Info.PotionBuffCount +
		Character->Data.BuffInfo.Info.GmBuffCount +
		Character->Data.BuffInfo.Info.ForceCaliburBuffCount +
		Character->Data.BuffInfo.Info.UnknownBuffCount2 +
		Character->Data.BuffInfo.Info.ForceWingBuffCount +
		Character->Data.BuffInfo.Info.FirePlaceBuffCount
	);
	if (BuffSlotCount > 0) {
		RTNotificationAppendCopy(Notification, &Character->Data.BuffInfo.Slots[0], sizeof(struct _RTBuffSlot) * BuffSlotCount);
	}

	RTNotificationDispatchToCharacter(Notification, Character);
}
