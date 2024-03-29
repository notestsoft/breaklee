#include "Character.h"
#include "Inventory.h"
#include "Force.h"
#include "Runtime.h"

// TODO: Load from file soon..

static const Int32 kRageLimit[] = {
	0, 1, 1, 1, 1, 3, 3, 5, 5, 6, 7, 7, 8, 8, 9, 9, 10, 10, 10, 10
};

Void RTCharacterInitialize(
	RTRuntimeRef Runtime, 
	RTCharacterRef Character,
	RTCharacterInfoRef Info,
	RTCharacterEquipmentInfoRef EquipmentInfo,
	RTCharacterInventoryInfoRef InventoryInfo,
	RTCharacterSkillSlotInfoRef SkillSlotInfo,
	RTCharacterQuickSlotInfoRef QuickSlotInfo,
	RTCharacterQuestSlotInfoRef QuestSlotInfo,
	RTCharacterQuestFlagInfoRef QuestFlagInfo,
	RTCharacterDungeonQuestFlagInfoRef DungeonQuestFlagInfo,
	RTCharacterEssenceAbilityInfoRef EssenceAbilityInfo,
	RTCharacterOverlordMasteryInfoRef OverlordMasteryInfo,
	RTCharacterForceWingInfoRef ForceWingInfo,
	RTCharacterCollectionInfoRef CollectionInfo,
	RTCharacterNewbieSupportInfoRef NewbieSupportInfo,
	RTCharacterWarehouseInfoRef WarehouseInfo
) {
	Character->SyncMask.RawValue = 0;
	Character->SyncPriority.RawValue = 0;
	Character->SyncTimestamp = PlatformGetTickCount();

	memcpy(&Character->Info, Info, sizeof(struct _RTCharacterInfo));
	memcpy(&Character->EquipmentInfo, EquipmentInfo, sizeof(struct _RTCharacterEquipmentInfo));
	memcpy(&Character->InventoryInfo, InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Character->SkillSlotInfo, SkillSlotInfo, sizeof(struct _RTCharacterSkillSlotInfo));
	memcpy(&Character->QuickSlotInfo, QuickSlotInfo, sizeof(struct _RTCharacterQuickSlotInfo));
	memcpy(&Character->QuestSlotInfo, QuestSlotInfo, sizeof(struct _RTCharacterQuestSlotInfo));
	memcpy(&Character->QuestFlagInfo, QuestFlagInfo, sizeof(struct _RTCharacterQuestFlagInfo));
	memcpy(&Character->DungeonQuestFlagInfo, DungeonQuestFlagInfo, sizeof(struct _RTCharacterDungeonQuestFlagInfo));
	memcpy(&Character->EssenceAbilityInfo, EssenceAbilityInfo, sizeof(struct _RTCharacterEssenceAbilityInfo));
	memcpy(&Character->OverlordMasteryInfo, OverlordMasteryInfo, sizeof(struct _RTCharacterOverlordMasteryInfo));
	memcpy(&Character->ForceWingInfo, ForceWingInfo, sizeof(struct _RTCharacterForceWingInfo));
	memcpy(&Character->CollectionInfo, CollectionInfo, sizeof(struct _RTCharacterCollectionInfo));
	memcpy(&Character->NewbieSupportInfo, NewbieSupportInfo, sizeof(struct _RTCharacterNewbieSupportInfo));
	memcpy(&Character->WarehouseInfo, WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));
	memset(&Character->TemporaryInventoryInfo, 0, sizeof(struct _RTCharacterInventoryInfo));
	memset(&Character->RecoveryInfo, 0, sizeof(struct _RTCharacterRecoveryInfo));

	RTCharacterInitializeAttributes(Runtime, Character);
	RTMovementInitialize(
		Runtime,
		&Character->Movement,
		Info->Position.X,
		Info->Position.Y,
		RUNTIME_MOVEMENT_SPEED_BASE,
		RUNTIME_WORLD_TILE_WALL
	);
}

// TODO: Split this up to resources and runtime attributes!!!
Void RTCharacterInitializeAttributes(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	memset(Character->Attributes.Values, 0, sizeof(Character->Attributes.Values));

	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);

	// Battle Style Level Formula 

	RTBattleStyleLevelFormulaDataRef LevelFormula = RTRuntimeGetBattleStyleLevelFormulaData(Runtime, BattleStyleIndex);
	
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] = LevelFormula->BaseHP + LevelFormula->DeltaHP * (Character->Info.Basic.Level - 1) / 10;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX] = LevelFormula->BaseMP + LevelFormula->DeltaMP * (Character->Info.Basic.Level - 1) / 10;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX] = 0;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX] = 0;

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX] = 0;
	Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] = 0;
	if (BattleStyleIndex == RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX] = MIN(
			RUNTIME_CHARACTER_MAX_RAGE,
			kRageLimit[Character->Info.Style.BattleRank]
		);
	}

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX],
		Character->Info.Resource.HP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX],
		Character->Info.Resource.MP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX],
		Character->Info.Resource.SP
	);

	Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT] = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX],
		Character->Info.Resource.BP
	);

	// Battle Style Class Formula 

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
			Character->Info.Style.BattleRank * ClassFormulaIndices[Index].Values[0] + ClassFormulaIndices[Index].Values[1]
		);
	}

	// Battle Style Stats Formula 

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
					Character->Info.Stat[StatsIndex]
				);

				AttributeValue += Character->Info.Stat[StatsIndex] * SlopeData->Slope + SlopeData->Intercept;
			}
		}

		Character->Attributes.Values[StatsFormulaIndices[Index].AttributeIndex] += AttributeValue / 1000;
	}

	// TODO: Skill Rank Formula

	// Overlord Mastery Force Effects

	for (Int32 Index = 0; Index < Character->OverlordMasteryInfo.Count; Index += 1) {
		RTOverlordMasterySlotRef MasterySlot = &Character->OverlordMasteryInfo.Slots[Index];
		RTDataOverlordMasteryValueRef MasteryValue = RTRuntimeDataOverlordMasteryValueGet(Runtime->Context, MasterySlot->MasteryIndex);
		
		// TODO: Solve this with a macro..
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
	
	// TODO: Values are still broken and check HP! It is way too big, new deltas don't make sense or whats broken with the packets actually?
	RTCharacterApplyEquipmentAttributes(Runtime, Character);
	RTCharacterApplyEssenceAbilityAttributes(Runtime, Character);

	RTDataAxpFieldRateRef AxpFieldRate = RTRuntimeDataAxpFieldRateGet(Runtime->Context, Character->Info.Basic.Level);
	if (AxpFieldRate) {
		Character->AxpFieldRate = AxpFieldRate->Rate;
	}
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
	if (Character->Info.Position.WorldID != WorldID) return false;

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
	
	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

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

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

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

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

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
	Character->SyncPriority.Low = true;

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

	if (DeltaChunkX == 0 && DeltaChunkY == 0) {
		return true;
	}

	if (DeltaChunkX < -1 || DeltaChunkX > 1 || DeltaChunkY < -1 || DeltaChunkY > 1) {
		return false;
	}

    RTMovementSetPosition(Runtime, &Character->Movement, PositionCurrentX, PositionCurrentY);

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	RTRuntimeBroadcastEvent(
		Runtime,
		RUNTIME_EVENT_CHARACTER_CHUNK_UPDATE,
		World,
		kEntityIDNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

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
	return RequiredMP;
}

Bool RTCharacterBattleRankUp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
	RTBattleStyleRankDataRef RankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Info.Style.BattleRank);
	assert(RankData);

	RTBattleStyleRankDataRef NextRankData = RTRuntimeGetBattleStyleRankData(Runtime, BattleStyleIndex, Character->Info.Style.BattleRank + 1);
	if (!NextRankData) return false;

	if (RankData->ConditionSTR > Character->Info.Stat[RUNTIME_CHARACTER_STAT_STR] ||
		RankData->ConditionDEX > Character->Info.Stat[RUNTIME_CHARACTER_STAT_DEX] ||
		RankData->ConditionINT > Character->Info.Stat[RUNTIME_CHARACTER_STAT_INT]) {
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
		Character->SyncPriority.Low = true;
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
		Character->SyncPriority.Low = true;
	}

	Character->Info.Style.BattleRank += 1;

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

	RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	RTRuntimeBroadcastEvent(
		Runtime,
		RUNTIME_EVENT_CHARACTER_BATTLE_RANK_UP,
		World,
		kEntityIDNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

	return true;
}

Void RTCharacterAddCurrency(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 CurrencyType,
	UInt64 Amount
) {
	assert(0 <= CurrencyType && CurrencyType < RUNTIME_CHARACTER_CURRENCY_COUNT);

	Character->Info.Currency[CurrencyType] += Amount;
}

Void RTCharacterAddExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	UInt64 Axp = (Exp / 100) * Character->AxpFieldRate;
	RTCharacterAddAbilityExp(Runtime, Character, Axp);

	if (Character->Info.Overlord.Level > 0) {
		return RTCharacterAddOverlordExp(Runtime, Character, Exp);
	}

	UInt8 CurrentLevel = Character->Info.Basic.Level;
	
	// TODO: Limit exp accumulation to the max reachable value from data!
	Character->Info.Basic.Exp += Exp;
	Character->Info.Basic.Level = RTRuntimeGetLevelByExp(Runtime, CurrentLevel, Character->Info.Basic.Exp);

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

	// TODO: Check if LevelDiff is calculated correctly, large exp potions seam to break the reward of stat alloc
	Bool LevelDiff = Character->Info.Basic.Level - CurrentLevel;
    if (LevelDiff > 0) {
		RTCharacterInitializeAttributes(Runtime, Character);
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
		Character->Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += LevelDiff * 5;

		if (Character->Info.Overlord.Level < 1) {
			RTDataOverlordMasteryStartRef Start = RTRuntimeDataOverlordMasteryStartGet(Runtime->Context);
			if (Character->Info.Basic.Level == Start->RequiredLevel) {
				Character->Info.Overlord.Level = 1;
				Character->Info.Overlord.Exp = 0;
				Character->Info.Overlord.Point = Start->MasteryPointCount;

                RTRuntimeBroadcastEvent(
                    Runtime,
                    RUNTIME_EVENT_CHARACTER_OVERLORD_LEVEL_UP,
                    RTRuntimeGetWorldByCharacter(Runtime, Character),
                    kEntityIDNull,
                    Character->ID,
                    Character->Movement.PositionCurrent.X,
                    Character->Movement.PositionCurrent.Y
                );
            }
		}

		RTRuntimeBroadcastEvent(
			Runtime,
			RUNTIME_EVENT_CHARACTER_LEVEL_UP,
			RTRuntimeGetWorldByCharacter(Runtime, Character),
			kEntityIDNull,
			Character->ID,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y
		);
    }
}

Int32 RTCharacterAddSkillExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 SkillExp
) {
	RTCharacterAddWingExp(Runtime, Character, SkillExp);

	Int32 CurrentSkillLevel = Character->Info.Skill.Level;
	Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
	RTBattleStyleSkillRankDataRef SkillRankData = RTRuntimeGetBattleStyleSkillRankData(
		Runtime,
		BattleStyleIndex,
		Character->Info.Skill.Rank
	);

	Int32 SkillLevelMax = RTRuntimeDataCharacterRankUpConditionGet(
		Runtime->Context,
		Character->Info.Skill.Rank,
		BattleStyleIndex
	);
		
	Int32 CurrentSkillExp = CurrentSkillLevel * SkillRankData->SkillLevelExp + (Int32)Character->Info.Skill.Exp;
	Int32 MaxSkillExp = SkillLevelMax * SkillRankData->SkillLevelExp;
	Int32 FinalSkillExp = MIN(MaxSkillExp, CurrentSkillExp + SkillExp);
	Int32 FinalSkillLevel = FinalSkillExp / SkillRankData->SkillLevelExp;
	Int32 SkillLevelDiff = FinalSkillLevel - CurrentSkillLevel;
	Int32 ReceivedSkillExp = FinalSkillExp - CurrentSkillExp;

	Character->Info.Skill.Exp = FinalSkillExp % SkillRankData->SkillLevelExp;
	Character->Info.Skill.Level += SkillLevelDiff;
	Character->Info.Skill.Point += SkillLevelDiff;

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

	RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	if (SkillLevelDiff > 0) {
		RTRuntimeBroadcastEvent(
			Runtime,
			RUNTIME_EVENT_CHARACTER_SKILL_LEVEL_UP,
			World,
			kEntityIDNull,
			Character->ID,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y
		);
	}

	if (Character->Info.Skill.Level >= SkillLevelMax && 
		Character->Info.Style.BattleRank > Character->Info.Skill.Rank) {
		RTBattleStyleSkillRankDataRef NextSkillRankData = RTRuntimeGetBattleStyleSkillRankData(
			Runtime,
			BattleStyleIndex,
			Character->Info.Skill.Rank + 1
		);
		
		if (NextSkillRankData) {
			Character->Info.Skill.Rank += 1;
			Character->Info.Skill.Level = 0;
		}

		RTRuntimeBroadcastEvent(
			Runtime,
			RUNTIME_EVENT_CHARACTER_SKILL_RANK_UP,
			World,
			kEntityIDNull,
			Character->ID,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y
		);
	}

	return ReceivedSkillExp;
}

Void RTCharacterAddHonorPoint(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int64 HonorPoint
) {
	Character->Info.Honor.Point += HonorPoint;

	RTDataHonorLevelFormulaRef CurrentLevelData = RTRuntimeDataHonorLevelFormulaGet(
		Runtime->Context,
		Character->Info.Honor.Rank
	);
	assert(CurrentLevelData);

	while (Character->Info.Honor.Point < CurrentLevelData->MinPoint) {
		RTDataHonorLevelFormulaRef NextLevelData = RTRuntimeDataHonorLevelFormulaGet(
			Runtime->Context,
			Character->Info.Honor.Rank - 1
		);
		if (!NextLevelData) break;

		Character->Info.Honor.Rank = NextLevelData->Rank;
		CurrentLevelData = NextLevelData;
	}

	while (Character->Info.Honor.Point > CurrentLevelData->MaxPoint) {
		RTDataHonorLevelFormulaRef NextLevelData = RTRuntimeDataHonorLevelFormulaGet(
			Runtime->Context,
			Character->Info.Honor.Rank + 1
		);
		if (!NextLevelData) break;

		Character->Info.Honor.Rank = NextLevelData->Rank;
		CurrentLevelData = NextLevelData;
	}

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;
}

Void RTCharacterAddAbilityExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	if (Exp < 1) return;

	Character->Info.Ability.Exp += Exp;

	while (Character->Info.Ability.Exp >= RUNTIME_CHARACTER_AXP_PER_LEVEL) {
		Character->Info.Ability.Exp -= RUNTIME_CHARACTER_AXP_PER_LEVEL;
		Character->Info.Ability.Point += 1;
	}

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;
}

Void RTCharacterAddRage(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 Rage
) {
	Int32 NewValue = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX],
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] + MAX(0, Rage)
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] != NewValue) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] = NewValue;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			RUNTIME_EVENT_CHARACTER_DATA_TYPE_RAGE
		);
	}
}

Bool RTCharacterConsumeRage(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 Rage
) {
	assert(Rage >= 0);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] < Rage) return false;

	Int32 NewValue = MIN(
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX],
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] - Rage
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] != NewValue) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT] = NewValue;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			RUNTIME_EVENT_CHARACTER_DATA_TYPE_RAGE
		);
	}

	return true;
}

Bool RTCharacterAddStats(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32* Stats
) {
	Int32 RequiredPoints = Stats[RUNTIME_CHARACTER_STAT_STR] + Stats[RUNTIME_CHARACTER_STAT_DEX] + Stats[RUNTIME_CHARACTER_STAT_INT];
	if (Character->Info.Stat[RUNTIME_CHARACTER_STAT_PNT] < RequiredPoints) {
		return false;
	}

	Character->Info.Stat[RUNTIME_CHARACTER_STAT_STR] += Stats[RUNTIME_CHARACTER_STAT_STR];
	Character->Info.Stat[RUNTIME_CHARACTER_STAT_DEX] += Stats[RUNTIME_CHARACTER_STAT_DEX];
	Character->Info.Stat[RUNTIME_CHARACTER_STAT_INT] += Stats[RUNTIME_CHARACTER_STAT_INT];
	Character->Info.Stat[RUNTIME_CHARACTER_STAT_PNT] -= RequiredPoints;

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

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

	if (Character->Info.Stat[StatIndex] < Amount) return false;

	Character->Info.Stat[StatIndex] -= Amount;
	Character->Info.Stat[RUNTIME_CHARACTER_STAT_PNT] += Amount;

	Character->SyncMask.Info = true;
	Character->SyncPriority.Low = true;

	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

Void RTCharacterAddHP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 HP,
	Bool IsPotion
) {
	Int32 NewValue = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] + HP;
	NewValue = MAX(NewValue, 0);
	NewValue = MIN(NewValue, Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX]);

	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT]) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = NewValue;
		Character->Info.Resource.MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		Character->SyncMask.Info = true;
		Character->SyncPriority.Low = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			IsPotion ? RUNTIME_EVENT_CHARACTER_DATA_TYPE_HP_POTION : RUNTIME_EVENT_CHARACTER_DATA_TYPE_HP
		);
	}
}

Void RTCharacterAddMP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MP,
	Bool IsPotion
) {
	Int32 NewValue = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] + MP;
	NewValue = MAX(NewValue, 0);
	NewValue = MIN(NewValue, Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX]);

	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT]) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] = NewValue;
		Character->Info.Resource.MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		Character->SyncMask.Info = true;
		Character->SyncPriority.Low = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			IsPotion ? RUNTIME_EVENT_CHARACTER_DATA_TYPE_MP_POTION : RUNTIME_EVENT_CHARACTER_DATA_TYPE_MP
		);
	}
}

Void RTCharacterAddSP(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SP
) {
	Int32 NewValue = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] + SP;
	NewValue = MAX(NewValue, 0);
	NewValue = MIN(NewValue, Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX]);

	if (NewValue != Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT]) {
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] = NewValue;
		Character->Info.Resource.MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
		Character->SyncMask.Info = true;
		Character->SyncPriority.Low = true;

		RTRuntimeBroadcastCharacterData(
			Runtime,
			Character,
			RUNTIME_EVENT_CHARACTER_DATA_TYPE_SP
		);
	}
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
	Character->SyncPriority.Low = true;
}
