#include "Character.h"
#include "ForceWing.h"
#include "Runtime.h"

Bool RTCharacterEnableForceWing(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (Character->ForceWingInfo.Level > 0) return false;

	RTDataForceWingActivationRef ForceWingActivation = RTRuntimeDataForceWingActivationGet(Runtime->Context);
	if (Character->Info.Basic.Level < ForceWingActivation->MinLevel) return false;

	RTDataForceWingGradeInfoRef GradeInfoData = RTRuntimeDataForceWingGradeInfoGet(Runtime->Context, 1);
	if (!GradeInfoData) return false;

	Character->ForceWingInfo.Grade = GradeInfoData->Grade;
	Character->ForceWingInfo.Level = GradeInfoData->MinLevel;

	for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT; Index += 1) {
		Character->ForceWingInfo.PresetTrainingPointCount[Index] += RUNTIME_CHARACTER_FORCE_WING_LEVEL_TRAINING_POINT_COUNT;
	}

	RTDataForceWingSkillRef SkillData = RTRuntimeDataForceWingSkillGet(Runtime->Context, Character->ForceWingInfo.Grade);
	if (SkillData) {
		assert(0 <= SkillData->SlotIndex && SkillData->SlotIndex < RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_SKILL_COUNT);

		RTForceWingArrivalSkillSlotRef SkillSlot = &Character->ForceWingInfo.ArrivalSkillSlots[SkillData->SlotIndex];
		memset(SkillSlot, 0, sizeof(struct _RTForceWingArrivalSkillSlot));
		SkillSlot->SlotIndex = SkillData->SlotIndex;
	}

	Character->SyncMask.ForceWingInfo = true;
	Character->SyncPriority.High = true;

	RTEventData EventData = { 0 };
	EventData.CharacterUpdateForceWing.CharacterIndex = (UInt32)Character->CharacterIndex;
	EventData.CharacterUpdateForceWing.ForceWingGrade = Character->ForceWingInfo.Grade;
	EventData.CharacterUpdateForceWing.ForceWingLevel = Character->ForceWingInfo.Level;
	EventData.CharacterUpdateForceWing.ForceWingExp = Character->ForceWingInfo.Exp;
	EventData.CharacterUpdateForceWing.TrainingPointCount = Character->ForceWingInfo.PresetTrainingPointCount[Character->ForceWingInfo.ActivePresetIndex];

	RTRuntimeBroadcastEventData(
		Runtime,
		RUNTIME_EVENT_CHARACTER_UPDATE_FORCE_WING,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		kEntityIDNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		EventData
	);

	return true;
}

Void RTCharacterAddWingExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	if (Character->ForceWingInfo.Grade < 1) return;

	Character->ForceWingInfo.Exp += Exp;
	Character->SyncMask.ForceWingInfo = true;
	Character->SyncPriority.High = true;

	RTEventData EventData = { 0 };
	EventData.CharacterUpdateForceWingExp.ForceWingExp = Character->ForceWingInfo.Exp;

	RTRuntimeBroadcastEventData(
		Runtime,
		RUNTIME_EVENT_CHARACTER_UPDATE_FORCE_WING_EXP,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		kEntityIDNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		EventData
	);
}

Bool RTCharacterForceWingLevelUp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 ItemStackCount1,
	UInt16 InventorySlotCount1,
	UInt16* InventorySlotIndex1,
	Int32 ItemStackCount2,
	UInt16 InventorySlotCount2,
	UInt16* InventorySlotIndex2
) {
	if (Character->ForceWingInfo.Grade < 1) return false;

	RTDataForceWingGradeInfoRef GradeInfoData = RTRuntimeDataForceWingGradeInfoGet(Runtime->Context, Character->ForceWingInfo.Grade);
	assert(GradeInfoData);

	if (Character->ForceWingInfo.Level >= GradeInfoData->MaxLevel) return false;

	RTDataForceWingGradeRef GradeData = RTRuntimeDataForceWingGradeGet(Runtime->Context, Character->ForceWingInfo.Grade);
	assert(GradeData);

	RTDataForceWingGradeLevelRef GradeLevelData = RTRuntimeDataForceWingGradeLevelGet(GradeData, Character->ForceWingInfo.Level);
	assert(GradeLevelData);

	if (Character->ForceWingInfo.Exp < GradeLevelData->Exp) return false;
	if (ItemStackCount1 < GradeLevelData->RequiredMaterialItemCount) return false;
	if (ItemStackCount2 < GradeLevelData->RequiredExtraMaterialItemCount) return false;

	if (!RTInventoryCanConsumeStackableItems(
		Runtime,
		&Character->InventoryInfo,
		GradeInfoData->MaterialItemID,
		GradeLevelData->RequiredMaterialItemCount,
		InventorySlotCount1,
		InventorySlotIndex1
	)) return false;

	if (!RTInventoryCanConsumeStackableItems(
		Runtime,
		&Character->InventoryInfo,
		GradeLevelData->ExtraMaterialItemID,
		GradeLevelData->RequiredExtraMaterialItemCount,
		InventorySlotCount2,
		InventorySlotIndex2
	)) return false;

	RTInventoryConsumeStackableItems(
		Runtime,
		&Character->InventoryInfo,
		GradeInfoData->MaterialItemID,
		GradeLevelData->RequiredMaterialItemCount,
		InventorySlotCount1,
		InventorySlotIndex1
	);

	RTInventoryConsumeStackableItems(
		Runtime,
		&Character->InventoryInfo,
		GradeLevelData->ExtraMaterialItemID,
		GradeLevelData->RequiredExtraMaterialItemCount,
		InventorySlotCount2,
		InventorySlotIndex2
	);

	Character->ForceWingInfo.Exp -= GradeLevelData->Exp;
	Character->ForceWingInfo.Level += 1;

	for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT; Index += 1) {
		Character->ForceWingInfo.PresetTrainingPointCount[Index] += RUNTIME_CHARACTER_FORCE_WING_LEVEL_TRAINING_POINT_COUNT;
	}

	Character->SyncMask.InventoryInfo = true;
	Character->SyncMask.ForceWingInfo = true;
	Character->SyncPriority.High = true;

	RTRuntimeBroadcastEvent(
		Runtime,
		RUNTIME_EVENT_CHARACTER_UPDATE_FORCE_WING_LEVEL_UP,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		kEntityIDNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y
	);

	return true;
}

Bool RTCharacterForceWingSetActivePreset(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 PresetIndex
) {
	if (Character->ForceWingInfo.Grade < 1) return false;
	if (PresetIndex < 0 || PresetIndex >= RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT) return false;
	if (!Character->ForceWingInfo.PresetEnabled[PresetIndex]) return false;

	// TODO: Apply changed force effects to character

	Character->ForceWingInfo.ActivePresetIndex = PresetIndex;
	Character->SyncMask.ForceWingInfo = true;
	Character->SyncPriority.Low = true;
}

Bool RTCharacterForceWingRollArrivalSkill(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt8 SkillSlotIndex,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
) {
	if (Character->ForceWingInfo.Grade < 1) return false;

	RTDataForceWingSkillRef SkillData = RTRuntimeDataForceWingSkillGetBySlotIndex(Runtime->Context, SkillSlotIndex);
	if (!SkillData) return false;

	if (!RTInventoryCanConsumeStackableItems(
		Runtime,
		&Character->InventoryInfo,
		SkillData->MaterialItemID,
		SkillData->MaterialItemCount,
		InventorySlotCount,
		InventorySlotIndex
	)) return false;

	Int32 ForceCodeGroups[RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_STATS_COUNT] = { 
		SkillData->ForceCodeGroup1,
		SkillData->ForceCodeGroup2,
	};

	RTForceWingArrivalSkillSlotRef RestoreSkillSlot = &Character->ForceWingInfo.ArrivalSkillRestoreSlot;

	Int32 Seed = GetTickCount();
	for (Index SkillIndex = 0; SkillIndex < RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_SKILL_COUNT; SkillIndex += 1) {
		RTForceWingArrivalSkillSlotRef ArrivalSkillSlot = &Character->ForceWingInfo.ArrivalSkillSlots[SkillIndex];
		if (ArrivalSkillSlot->SlotIndex != SkillSlotIndex) continue;

		memcpy(RestoreSkillSlot, ArrivalSkillSlot, sizeof(struct _RTForceWingArrivalSkillSlot));

		for (Index GroupIndex = 0; GroupIndex < RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_STATS_COUNT; GroupIndex += 1) {
			Int32 DropRateLimit = RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_STATS_RATE;
			Int32 DropRate = RandomRange(&Seed, 0, DropRateLimit);
			Int32 DropRateOffset = 0;
			Bool Found = false;

			RTDataForceWingSkillGroupRef SkillGroupData = RTRuntimeDataForceWingSkillGroupGet(Runtime->Context, ForceCodeGroups[GroupIndex]);
			for (Int32 SkillGroupDetailIndex = 0; SkillGroupDetailIndex < SkillGroupData->ForceWingSkillGroupDetailCount; SkillGroupDetailIndex += 1) {
				RTDataForceWingSkillGroupDetailRef SkillGroupDetailData = &SkillGroupData->ForceWingSkillGroupDetailList[SkillGroupDetailIndex];

				for (Int32 SkillGroupDetailOptionIndex = 0; SkillGroupDetailOptionIndex < SkillGroupDetailData->ForceWingSkillGroupDetailOptionCount; SkillGroupDetailOptionIndex += 1) {
					RTDataForceWingSkillGroupDetailOptionRef SkillGroupDetailOptionData = &SkillGroupDetailData->ForceWingSkillGroupDetailOptionList[SkillGroupDetailOptionIndex];

					if (DropRate < SkillGroupDetailOptionData->Rate + DropRateOffset) {
						ArrivalSkillSlot->ForceEffectIndex[GroupIndex] = SkillGroupDetailData->ForceEffectIndex;
						ArrivalSkillSlot->ForceEffectGrade[GroupIndex] = SkillGroupDetailOptionData->ForceEffectGrade;
						Found = true;
						break;
					}
					else {
						DropRateOffset += SkillGroupDetailOptionData->Rate;
					}
				}

				if (Found) break;
			}
		}

		RTInventoryConsumeStackableItems(
			Runtime,
			&Character->InventoryInfo,
			SkillData->MaterialItemID,
			SkillData->MaterialItemCount,
			InventorySlotCount,
			InventorySlotIndex
		);

		Character->SyncMask.ForceWingInfo = true;
		Character->SyncMask.InventoryInfo = true;
		Character->SyncPriority.High = true;
		return true;
	}

	return false;
}

Bool RTCharacterForceWingChangeArrivalSkill(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (Character->ForceWingInfo.Grade < 1) return false;

	RTForceWingArrivalSkillSlotRef RestoreSkillSlot = &Character->ForceWingInfo.ArrivalSkillRestoreSlot;

	for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_SKILL_COUNT; Index += 1) {
		RTForceWingArrivalSkillSlotRef ArrivalSkillSlot = &Character->ForceWingInfo.ArrivalSkillSlots[Index];
		if (ArrivalSkillSlot->SlotIndex != RestoreSkillSlot->SlotIndex) continue;

		memcpy(ArrivalSkillSlot, RestoreSkillSlot, sizeof(struct _RTForceWingArrivalSkillSlot));
		memset(RestoreSkillSlot, 0, sizeof(struct _RTForceWingArrivalSkillSlot));
		break;
	}

	Character->SyncMask.ForceWingInfo = true;
	Character->SyncPriority.High = true;
	return true;
}