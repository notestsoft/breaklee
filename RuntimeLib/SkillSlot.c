#include "Character.h"
#include "SkillSlot.h"
#include "Runtime.h"

Int32 RTCharacterGetEmptySkillSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 SkillSlotIndex = -1;
	Int32 SkillSlotIndices[RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT] = { 0 };
	Int32 SkillSlotOffset = 0;
	Int32 MaxSkillSlotIndex = -1;

	for (Int Index = 0; Index < Character->Data.SkillSlotInfo.Info.SlotCount; Index += 1) {
		SkillSlotIndices[SkillSlotOffset++] = Character->Data.SkillSlotInfo.Slots[Index].Index;
		MaxSkillSlotIndex = MAX(MaxSkillSlotIndex, Character->Data.SkillSlotInfo.Slots[Index].Index);
	}

	// Quick and dirty bubble sort
	for (Int i = 0; i < SkillSlotOffset - 1; i++) {
		for (Int j = 0; j < SkillSlotOffset - i - 1; j++) {
			if (SkillSlotIndices[j] > SkillSlotIndices[j + 1]) {
				UInt16 temp = SkillSlotIndices[j];
				SkillSlotIndices[j] = SkillSlotIndices[j + 1];
				SkillSlotIndices[j + 1] = temp;
			}
		}
	}

	for (Int Index = 0; Index < MAX(0, Character->Data.SkillSlotInfo.Info.SlotCount - 1); Index += 1) {
		if (ABS(SkillSlotIndices[Index] - SkillSlotIndices[Index + 1]) > 1) {
			return SkillSlotIndices[Index] + 1;
		}
	}

	return MaxSkillSlotIndex + 1;
}

// TODO: Add check for slot count limit based on skill rank!!
RTSkillSlotRef RTCharacterAddSkillSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillID,
	Int32 Level,
	Int32 SlotIndex
) {
	if (Character->Data.SkillSlotInfo.Info.SlotCount >= RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) return NULL;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillID);
	if (!SkillData) return NULL;

	RTSkillSlotRef SkillSlot = &Character->Data.SkillSlotInfo.Slots[Character->Data.SkillSlotInfo.Info.SlotCount];
	SkillSlot->ID = SkillID;
	SkillSlot->Level = Level;
	SkillSlot->Index = SlotIndex;
	Character->Data.SkillSlotInfo.Info.SlotCount += 1;
	Character->SyncMask.SkillSlotInfo = true;

	if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_PASSIVE) {
		RTCharacterInitializeAttributes(Runtime, Character);
	}

	return SkillSlot;
}

RTSkillSlotRef RTCharacterGetSkillSlotBySkillIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex
) {
	for (Int Index = 0; Index < Character->Data.SkillSlotInfo.Info.SlotCount; Index += 1) {
		if (Character->Data.SkillSlotInfo.Slots[Index].ID == SkillIndex) {
			return &Character->Data.SkillSlotInfo.Slots[Index];
		}
	}

	return NULL;
}

RTSkillSlotRef RTCharacterGetSkillSlotBySlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
) {
	for (Int Index = 0; Index < Character->Data.SkillSlotInfo.Info.SlotCount; Index += 1) {
		if (Character->Data.SkillSlotInfo.Slots[Index].Index == SlotIndex) {
			return &Character->Data.SkillSlotInfo.Slots[Index];
		}
	}

	return NULL;
}

Void RTCharacterRemoveSkillSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillID,
	Int32 SlotIndex
) {
	for (Int Index = 0; Index < Character->Data.SkillSlotInfo.Info.SlotCount; Index += 1) {
		if (Character->Data.SkillSlotInfo.Slots[Index].Index == SlotIndex) {
			Int32 TailLength = Character->Data.SkillSlotInfo.Info.SlotCount - Index - 1;
			if (TailLength > 0) {
				memmove(
					&Character->Data.SkillSlotInfo.Slots[Index],
					&Character->Data.SkillSlotInfo.Slots[Index + 1],
					TailLength * sizeof(struct _RTSkillSlot)
				);
			}

			Character->Data.SkillSlotInfo.Info.SlotCount -= 1;
			Character->SyncMask.SkillSlotInfo = true;
			RTCharacterInitializeAttributes(Runtime, Character);
		}
	}
}

// TODO: Somehow the character can stuck being in infinite train mode?????
Bool RTCharacterChangeSkillLevel(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillID,
	Int32 SlotIndex,
	Int32 CurrentSkillLevel,
	Int32 TargetSkillLevel
) {
	if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) return false;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySlotIndex(Runtime, Character, SlotIndex);
	if (!SkillSlot || SkillSlot->ID != SkillID || SkillSlot->Level != CurrentSkillLevel) return false;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillID);
	if (!SkillData) return false;

	Int32 LevelDiff = TargetSkillLevel - CurrentSkillLevel;
	Int32 Increment = LevelDiff < 0 ? -1 : 1;
	Int32 Sign = Increment;
	Int32 RequiredSkillPointCount = 0;
	Int64 RequiredCurrencyAlz = 0;

	if (TargetSkillLevel > 0) {
		for (Int NextLevel = CurrentSkillLevel + Increment; NextLevel <= Sign * TargetSkillLevel; NextLevel += Increment) {
			RTSkillLevelDataRef SkillLevelData = RTRuntimeGetSkillLevelDataByID(Runtime, SkillID, NextLevel);
			if (!SkillLevelData) return false;

			if (Character->Data.Info.SkillRank < SkillLevelData->SkillRank) return false;

			RequiredSkillPointCount += SkillLevelData->SkillPoint;
			RequiredCurrencyAlz += LevelDiff < 0 ? SkillLevelData->UntrainPrice : SkillLevelData->TrainPrice;

			// TODO: Check SkillLevelData->StyleMastery

			Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);
			if (SkillLevelData->BattleStyles[BattleStyleIndex - 1] <= 0) return false;
		}
	}

	if (Character->Data.Info.SkillPoint < RequiredSkillPointCount) return false;
	if (Character->Data.Info.Alz < RequiredCurrencyAlz) return false;

	Character->Data.Info.SkillPoint -= RequiredSkillPointCount;
	Character->Data.Info.Alz -= RequiredCurrencyAlz;
	SkillSlot->Level = TargetSkillLevel;

	if (TargetSkillLevel <= 0) {
		RTCharacterRemoveSkillSlot(Runtime, Character, SkillID, SlotIndex);
	}

	Character->SyncMask.Info = true;
	Character->SyncMask.SkillSlotInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}
