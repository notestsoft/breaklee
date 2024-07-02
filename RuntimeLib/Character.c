#include "Character.h"
#include "Inventory.h"
#include "Force.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Void RTCharacterInitialize(
	RTRuntimeRef Runtime, 
	RTCharacterRef Character,
    CString Name,
	RTCharacterDataRef Data
) {
	Character->DungeonEntryItemSlotIndex = -1;
	Character->SyncMask.RawValue = 0;
	Character->SyncTimestamp = PlatformGetTickCount();

    CStringCopySafe(Character->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Name);

#define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__, __SCOPE__) \
	memcpy(&Character->Data.__NAME__, &Data->__NAME__, sizeof(__TYPE__));
#include "CharacterDataDefinition.h"

	RTCharacterInitializeAttributes(Runtime, Character);
	RTMovementInitialize(
		Runtime,
		&Character->Movement,
		Data->Info.Position.X,
		Data->Info.Position.Y,
		RUNTIME_MOVEMENT_SPEED_BASE,
		RUNTIME_WORLD_TILE_WALL
	);
}

Void RTCharacterInitializeBattleStyleLevel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.Info.Style.BattleStyle | (Character->Data.Info.Style.ExtendedBattleStyle << 3);

	RTBattleStyleLevelFormulaDataRef LevelFormula = RTRuntimeGetBattleStyleLevelFormulaData(Runtime, BattleStyleIndex);
	RTDataSpiritPointLimitRef SpiritPointLimitData = RTRuntimeDataSpiritPointLimitGet(Runtime->Context, Character->Data.Info.Style.BattleRank);
	assert(SpiritPointLimitData);

	RTDataRageLimitRef RageLimitData = RTRuntimeDataRageLimitGet(Runtime->Context, BattleStyleIndex);
	assert(RageLimitData);

	RTDataRageLimitLevelRef RageLimitLevelData = RTRuntimeDataRageLimitLevelGet(RageLimitData, Character->Data.Info.Style.BattleRank);
	assert(RageLimitLevelData);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] = RTCalculateBaseHP(
		Character->Data.Info.Skill.Rank,
		LevelFormula->BaseHP,
		LevelFormula->DeltaHP,
		Character->Data.Info.Basic.Level,
		Character->Data.Info.Style.BattleRank,
		LevelFormula->DeltaHP2
	);
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX] = LevelFormula->BaseMP + LevelFormula->DeltaMP * (Character->Data.Info.Basic.Level - 1) / 10;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX] = SpiritPointLimitData->Value;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX] = 0;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX] = RageLimitLevelData->Value;
}

Void RTCharacterInitializeBattleStyleClass(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.Info.Style.BattleStyle | (Character->Data.Info.Style.ExtendedBattleStyle << 3);

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
		{ RUNTIME_ATTRIBUTE_IGNORE_ACCURACY, ClassFormula->AccuracyPenalty },
		{ RUNTIME_ATTRIBUTE_IGNORE_PENETRATION, ClassFormula->PenetrationPenalty },
	};

	Int32 ClassFormulaIndexCount = sizeof(ClassFormulaIndices) / sizeof(ClassFormulaIndices[0]);
	for (Int32 Index = 0; Index < ClassFormulaIndexCount; Index++) {
		Character->Attributes.Values[ClassFormulaIndices[Index].AttributeIndex] += (
			(Int64)Character->Data.Info.Style.BattleRank * ClassFormulaIndices[Index].Values[0] + ClassFormulaIndices[Index].Values[1]
		);
	}
}

Void RTCharacterInitializeBattleStyleStats(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.Info.Style.BattleStyle | (Character->Data.Info.Style.ExtendedBattleStyle << 3);

	RTBattleStyleStatsFormulaDataRef StatsFormula = RTRuntimeGetBattleStyleStatsFormulaData(Runtime, BattleStyleIndex);

	struct { Int32 AttributeIndex; Int32* Values; Int32 SlopeID; } StatsFormulaIndices[] = {
		{ RUNTIME_ATTRIBUTE_ATTACK, StatsFormula->Attack, StatsFormula->AttackSlopeID },
		{ RUNTIME_ATTRIBUTE_MAGIC_ATTACK, StatsFormula->MagicAttack, StatsFormula->MagicAttackSlopeID },
		{ RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION, StatsFormula->DamageReduction, StatsFormula->DamageReductionSlopeID },
		{ RUNTIME_ATTRIBUTE_EVASION, StatsFormula->Evasion, StatsFormula->EvasionSlopeID },
		{ RUNTIME_ATTRIBUTE_ATTACK_RATE, StatsFormula->AttackRate, StatsFormula->AttackRateSlopeID },
		{ RUNTIME_ATTRIBUTE_DEFENSE_RATE, StatsFormula->DefenseRate, StatsFormula->DefenseRateSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_CRITICAL_RATE, StatsFormula->ResistCriticalRate, StatsFormula->ResistCriticalRateSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_SKILL_AMP, StatsFormula->ResistSkillAmp, StatsFormula->ResistSkillAmpSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_CRITICAL_DAMAGE, StatsFormula->ResistCriticalDamage, StatsFormula->ResistCriticalDamageSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_UNMOVABLE, StatsFormula->ResistUnmovable, StatsFormula->ResistUnmovableSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_STUN, StatsFormula->ResistStun, StatsFormula->ResistStunSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_DOWN, StatsFormula->ResistDown, StatsFormula->ResistDownSlopeID },
		{ RUNTIME_ATTRIBUTE_RESIST_KNOCK_BACK, StatsFormula->ResistKnockback, StatsFormula->ResistKnockbackSlopeID },
		{ RUNTIME_ATTRIBUTE_HP_MAX, StatsFormula->HP, StatsFormula->HPSlopeID },
		{ RUNTIME_ATTRIBUTE_MP_MAX, StatsFormula->MP, StatsFormula->MPSlopeID },
		{ RUNTIME_ATTRIBUTE_IGNORE_PENETRATION, StatsFormula->IgnorePenetration, StatsFormula->IgnorePenetrationSlopeID },
	};

	Int32 StatsFormulaIndexCount = sizeof(StatsFormulaIndices) / sizeof(StatsFormulaIndices[0]);
	Int32 StatsIndices[] = {
		RUNTIME_CHARACTER_STAT_STR,
		RUNTIME_CHARACTER_STAT_DEX,
		RUNTIME_CHARACTER_STAT_INT,
	};
	Int32 StatsIndexCount = sizeof(StatsIndices) / sizeof(StatsIndices[0]);
	for (Int32 Index = 0; Index < StatsFormulaIndexCount; Index++) {
		Int32 AttributeValue = 0;

		for (Int32 StatsIndex = 0; StatsIndex < StatsIndexCount; StatsIndex++) {
			if (StatsFormulaIndices[Index].Values[StatsIndex] > 0) {
				RTBattleStyleSlopeDataRef SlopeData = RTRuntimeGetBattleStyleSlopeData(
					Runtime,
					StatsFormulaIndices[Index].SlopeID,
					Character->Data.Info.Stat[StatsIndex]
				);

				AttributeValue += Character->Data.Info.Stat[StatsIndex] * SlopeData->Slope + SlopeData->Intercept;
			}
		}

		Character->Attributes.Values[StatsFormulaIndices[Index].AttributeIndex] += AttributeValue / 1000;
	}
}

Void RTCharacterInitializeEquipment(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    for (Int32 Index = 0; Index < Character->Data.EquipmentInfo.Count; Index += 1) {
        RTItemSlotRef ItemSlot = &Character->Data.EquipmentInfo.Slots[Index];
        RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
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
	for (Int32 Index = 0; Index < Character->Data.SkillSlotInfo.Count; Index += 1) {
		RTSkillSlotRef SkillSlot = &Character->Data.SkillSlotInfo.Skills[Index];
		assert(SkillSlot);

		RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
		assert(SkillData);

		if (SkillData->SkillGroup != RUNTIME_SKILL_GROUP_PASSIVE) continue;
		
		for (Int32 ValueIndex = 0; ValueIndex < SkillData->SkillValueCount; ValueIndex += 1) {
			RTSkillValueDataRef SkillValue = &SkillData->SkillValues[ValueIndex];
			Int64 ForceValue = (SkillValue->ForceEffectValue[0] + SkillValue->ForceEffectValue[1] * SkillSlot->Level + SkillValue->ForceEffectValue[2]) / 10;
			
			RTCharacterApplyForceEffect(
				Runtime,
				Character,
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
    for (Int32 Index = 0; Index < Character->Data.EssenceAbilityInfo.Count; Index += 1) {
		RTEssenceAbilitySlotRef AbilitySlot = &Character->Data.EssenceAbilityInfo.Slots[Index];

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
	for (Int32 Index = 0; Index < Character->Data.KarmaAbilityInfo.Count; Index += 1) {
		RTEssenceAbilitySlotRef AbilitySlot = &Character->Data.KarmaAbilityInfo.Slots[Index];

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

Void RTCharacterInitializeBuffs(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Buff values
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

Void RTCharacterInitializeOverlordMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	for (Int32 Index = 0; Index < Character->Data.OverlordMasteryInfo.Count; Index += 1) {
		RTOverlordMasterySlotRef MasterySlot = &Character->Data.OverlordMasteryInfo.Slots[Index];
		RTDataOverlordMasteryValueRef MasteryValue = RTRuntimeDataOverlordMasteryValueGet(Runtime->Context, MasterySlot->MasteryIndex);

		// TODO: ValueType refers to numeric or percentage ...

		Bool Found = false;
		for (Int32 LevelIndex = 0; LevelIndex < MasteryValue->OverlordMasteryValueLevelCount; LevelIndex += 1) {
			if (MasteryValue->OverlordMasteryValueLevelList[LevelIndex].MasteryLevel == MasterySlot->Level) {
				RTCharacterApplyForceEffect(
					Runtime,
					Character,
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
    // TODO: Honor medal values
}

Void RTCharacterInitializeCollection(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    // TODO: Honor medal values
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

    RTCharacterInitializeBattleStyleLevel(Runtime, Character);
	RTCharacterInitializeBattleStyleClass(Runtime, Character);
    RTCharacterInitializeBattleStyleStats(Runtime, Character);
	RTCharacterInitializeEquipment(Runtime, Character);
    RTCharacterInitializeSkillStats(Runtime, Character);
    RTCharacterInitializeEssenceAbilities(Runtime, Character);
    RTCharacterInitializeBlendedAbilities(Runtime, Character);
    RTCharacterInitializeKarmaAbilities(Runtime, Character);
    RTCharacterInitializeBlessingBeads(Runtime, Character);
    RTCharacterInitializePremiumServices(Runtime, Character);
    RTCharacterInitializeAchievements(Runtime, Character);
    RTCharacterInitializeBuffs(Runtime, Character);
    RTCharacterInitializeGoldMeritMastery(Runtime, Character);
    RTCharacterInitializePlatinumMeritMastery(Runtime, Character);
    RTCharacterInitializeOverlordMastery(Runtime, Character);
    RTCharacterInitializeHonorMedalMastery(Runtime, Character);
    RTCharacterInitializeForceWingMastery(Runtime, Character);
    RTCharacterInitializeCollection(Runtime, Character);
    RTCharacterInitializeTranscendenceMastery(Runtime, Character);
    RTCharacterInitializeStellarMastery(Runtime, Character);
    RTCharacterInitializeMythMastery(Runtime, Character);
    
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX],
		Character->Data.Info.Resource.HP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX],
		Character->Data.Info.Resource.MP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX],
		Character->Data.Info.Resource.SP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX],
		Character->Data.Info.Resource.BP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX],
		Character->Data.Info.Resource.Rage
	);

	// TODO: Values are still broken and check HP! It is way too big, new deltas don't make sense or whats broken with the packets actually?
	
	RTDataAxpFieldRateRef AxpFieldRate = RTRuntimeDataAxpFieldRateGet(Runtime->Context, Character->Data.Info.Basic.Level);
	if (AxpFieldRate) {
		Character->AxpFieldRate = AxpFieldRate->Rate;
	}

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK] = 5000;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_ATTACK] = 5000;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] = 5000;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE] = 50;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE] = 500;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SWORD_SKILL_AMP] = 500;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MAGIC_SKILL_AMP] = 500;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY] = 500;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION] = 500;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_ADD_DAMAGE] = 500;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MIN_DAMAGE] = 5;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE] = 5000;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE] = 5000;
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
	if (Character->Data.Info.Position.WorldID != WorldID) return false;

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

	Character->Data.Info.Position.X = PositionX;
	Character->Data.Info.Position.Y = PositionY;
	Character->SyncMask.Info = true;

	return true;
}

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

Bool RTCharacterBattleRankUp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Data.Info.Style.BattleStyle | (Character->Data.Info.Style.ExtendedBattleStyle << 3);
	RTBattleStyleRankDataRef RankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Data.Info.Style.BattleRank);
	assert(RankData);

	RTBattleStyleRankDataRef NextRankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Data.Info.Style.BattleRank + 1);
	if (!NextRankData) return false;

	if (RankData->ConditionSTR > Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] ||
		RankData->ConditionDEX > Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] ||
		RankData->ConditionINT > Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT]) {
		return false;
	}

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

	Character->Data.Info.Style.BattleRank += 1;
	Character->SyncMask.Info = true;

    {
        NOTIFICATION_DATA_CHARACTER_BATTLE_RANK_UP* Notification = RTNotificationInit(CHARACTER_BATTLE_RANK_UP);
        Notification->Level = Character->Data.Info.Style.BattleRank;
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

Void RTCharacterAddCurrency(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 CurrencyType,
	UInt64 Amount
) {
	assert(0 <= CurrencyType && CurrencyType < RUNTIME_CHARACTER_CURRENCY_COUNT);

	Character->Data.Info.Currency[CurrencyType] += Amount;
}

Void RTCharacterAddExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	UInt64 Axp = (Exp / 100) * Character->AxpFieldRate;
	RTCharacterAddAbilityExp(Runtime, Character, Axp);

	if (Character->Data.Info.Overlord.Level > 0) {
		RTCharacterAddOverlordExp(Runtime, Character, Exp);
		return;
	}

	Int32 CurrentLevel = Character->Data.Info.Basic.Level;
	
	// TODO: Limit exp accumulation to the max reachable value from data!
	Character->Data.Info.Basic.Exp += Exp;
	Character->Data.Info.Basic.Level = RTRuntimeGetLevelByExp(Runtime, CurrentLevel, Character->Data.Info.Basic.Exp);

	Character->SyncMask.Info = true;

	Int32 LevelDiff = Character->Data.Info.Basic.Level - CurrentLevel;
    if (LevelDiff > 0) {
		RTCharacterInitializeAttributes(Runtime, Character);
		RTCharacterSetHP(Runtime, Character, Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX], false);
		RTCharacterSetMP(Runtime, Character, (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX], false);
		Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += LevelDiff * 5;

		for (Index Index = 0; Index < LevelDiff; Index += 1) {
			RTRuntimeBroadcastCharacterData(
				Runtime,
				Character,
				NOTIFICATION_CHARACTER_DATA_TYPE_LEVEL
			);

			{
				NOTIFICATION_DATA_CHARACTER_EVENT* Notification = RTNotificationInit(CHARACTER_EVENT);
				Notification->Type = NOTIFICATION_CHARACTER_EVENT_TYPE_LEVEL_UP;
				Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
				RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
			}
		}

		if (Character->Data.Info.Overlord.Level < 1) {
			RTDataOverlordMasteryStartRef Start = RTRuntimeDataOverlordMasteryStartGet(Runtime->Context);
			if (Character->Data.Info.Basic.Level == Start->RequiredLevel) {
				Character->Data.Info.Overlord.Level = 1;
				Character->Data.Info.Overlord.Exp = 0;
				Character->Data.Info.Overlord.Point = Start->MasteryPointCount;
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

	Int32 CurrentSkillLevel = Character->Data.Info.Skill.Level;
	Int32 BattleStyleIndex = Character->Data.Info.Style.BattleStyle | (Character->Data.Info.Style.ExtendedBattleStyle << 3);
	RTBattleStyleSkillRankDataRef SkillRankData = RTRuntimeGetBattleStyleSkillRankData(
		Runtime,
		BattleStyleIndex,
		Character->Data.Info.Skill.Rank
	);

	Int32 SkillLevelMax = RTRuntimeDataCharacterRankUpConditionGet(
		Runtime->Context,
		Character->Data.Info.Skill.Rank,
		BattleStyleIndex
	);
    
	Int32 CurrentSkillExp = CurrentSkillLevel * SkillRankData->SkillLevelExp + (Int32)Character->Data.Info.Skill.Exp;
	Int32 MaxSkillExp = SkillLevelMax * SkillRankData->SkillLevelExp;
	Int32 FinalSkillExp = MIN(MaxSkillExp, CurrentSkillExp + (Int32)SkillExp);
	Int32 FinalSkillLevel = FinalSkillExp / SkillRankData->SkillLevelExp;
	Int32 SkillLevelDiff = FinalSkillLevel - CurrentSkillLevel;
	Int32 ReceivedSkillExp = FinalSkillExp - CurrentSkillExp;

	Character->Data.Info.Skill.Exp = FinalSkillExp % SkillRankData->SkillLevelExp;
	Character->Data.Info.Skill.Level += SkillLevelDiff;
	Character->Data.Info.Skill.Point += SkillLevelDiff;
	Character->SyncMask.Info = true;
 
    {
        NOTIFICATION_DATA_CHARACTER_SKILL_MASTERY_UPDATE* Notification = RTNotificationInit(CHARACTER_SKILL_MASTERY_UPDATE);
        Notification->SkillRank = Character->Data.Info.Skill.Rank;
        Notification->SkillLevel = Character->Data.Info.Skill.Level;
        Notification->SkillLevelMax = SkillLevelMax;
        Notification->SkillExp = (UInt32)Character->Data.Info.Skill.Exp;
        Notification->SkillPoint = Character->Data.Info.Skill.Point;
        RTNotificationDispatchToCharacter(Notification, Character);
    }

	Bool CanRankUp = Character->Data.Info.Skill.Level >= SkillLevelMax;
	if (CanRankUp && Runtime->Config.IsSkillRankUpLimitEnabled) {
		RTDataCharacterRankUpLimitRef RankUpLimit = RTRuntimeDataCharacterRankUpLimitGet(Runtime->Context, Character->Data.Info.Skill.Rank + 1);
		if (RankUpLimit) {
			CanRankUp &= RankUpLimit->Level <= Character->Data.Info.Basic.Level;
			CanRankUp &= RankUpLimit->BattleRank <= Character->Data.Info.Style.BattleRank;
		}
	}

	if (CanRankUp) {
		RTBattleStyleSkillRankDataRef NextSkillRankData = RTRuntimeGetBattleStyleSkillRankData(
			Runtime,
			BattleStyleIndex,
			Character->Data.Info.Skill.Rank + 1
		);
		
		if (NextSkillRankData) {
			RTDataCharacterRankUpBonusRef RankUpBonus = RTRuntimeDataCharacterRankUpBonusGet(Runtime->Context, Character->Data.Info.Skill.Rank);
			if (RankUpBonus) {
				// TODO: Add skill slot count to character data!
				Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += RankUpBonus->StatPoints;
			}

			Character->Data.Info.Skill.Rank = NextSkillRankData->SkillRank;
			Character->Data.Info.Skill.Level = 0;
			Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += 
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
	Character->Data.Info.Honor.Point += HonorPoint;

	RTDataHonorLevelFormulaRef CurrentLevelData = RTRuntimeDataHonorLevelFormulaGet(
		Runtime->Context,
		Character->Data.Info.Honor.Rank
	);
	assert(CurrentLevelData);

	while (Character->Data.Info.Honor.Point < CurrentLevelData->MinPoint) {
		RTDataHonorLevelFormulaRef NextLevelData = RTRuntimeDataHonorLevelFormulaGet(
			Runtime->Context,
			Character->Data.Info.Honor.Rank - 1
		);
		if (!NextLevelData) break;

		Character->Data.Info.Honor.Rank = NextLevelData->Rank;
		CurrentLevelData = NextLevelData;
	}

	while (Character->Data.Info.Honor.Point > CurrentLevelData->MaxPoint) {
		RTDataHonorLevelFormulaRef NextLevelData = RTRuntimeDataHonorLevelFormulaGet(
			Runtime->Context,
			Character->Data.Info.Honor.Rank + 1
		);
		if (!NextLevelData) break;

		Character->Data.Info.Honor.Rank = NextLevelData->Rank;
		CurrentLevelData = NextLevelData;
	}

	Character->SyncMask.Info = true;
}

Void RTCharacterAddAbilityExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	if (Exp < 1) return;

	Character->Data.Info.Ability.Exp += Exp;

	while (Character->Data.Info.Ability.Exp >= RUNTIME_CHARACTER_AXP_PER_LEVEL) {
		Character->Data.Info.Ability.Exp -= RUNTIME_CHARACTER_AXP_PER_LEVEL;
		Character->Data.Info.Ability.Point += 1;
	}

	Character->SyncMask.Info = true;
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
	Int32 BattleStyleIndex = Character->Data.Info.Style.BattleStyle | (Character->Data.Info.Style.ExtendedBattleStyle << 3);
	RTBattleStyleRankDataRef PreviousRankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Data.Info.Style.BattleRank - 1);
	if (!PreviousRankData) return false;

	Int32 AccumulatedPoints = 0;
	AccumulatedPoints += Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] - PreviousRankData->ConditionSTR;
	AccumulatedPoints += Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] - PreviousRankData->ConditionDEX;
	AccumulatedPoints += Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT] - PreviousRankData->ConditionINT;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_STR] = PreviousRankData->ConditionSTR;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_DEX] = PreviousRankData->ConditionDEX;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_INT] = PreviousRankData->ConditionINT;
	Character->Data.Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += AccumulatedPoints;

	return true;
}

Void RTCharacterSetHP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int64 NewValue,
	Bool IsPotion
) {
	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT]) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = NewValue;
		Character->Data.Info.Resource.HP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		Character->SyncMask.Info = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			IsPotion ? NOTIFICATION_CHARACTER_DATA_TYPE_HPPOTION : NOTIFICATION_CHARACTER_DATA_TYPE_HP
		);
	}
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

Void RTCharacterSetMP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 NewValue,
	Bool IsPotion
) {
	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT]) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = NewValue;
		Character->Data.Info.Resource.MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		Character->SyncMask.Info = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			IsPotion ? NOTIFICATION_CHARACTER_DATA_TYPE_MPPOTION : NOTIFICATION_CHARACTER_DATA_TYPE_MP
		);
	}
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
	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT]) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] = NewValue;
		Character->Data.Info.Resource.SP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
		Character->SyncMask.Info = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			NOTIFICATION_CHARACTER_DATA_TYPE_SP
		);
	}
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
	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT]) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT] = NewValue;
		Character->Data.Info.Resource.BP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
		Character->SyncMask.Info = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			NOTIFICATION_CHARACTER_DATA_TYPE_BP
		);
	}
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
		Character->Data.Info.Resource.Rage = Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
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
		Character->Data.Info.Resource.Rage = Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
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
