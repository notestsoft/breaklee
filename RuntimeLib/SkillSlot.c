#include "Character.h"
#include "SkillSlot.h"
#include "Runtime.h"

// TODO: Add check for slot count limit based on skill rank!!
RTSkillSlotRef RTCharacterAddSkillSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillID,
	Int32 Level,
	Int32 SlotIndex
) {
	if (Character->SkillSlotInfo.Count >= RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) return NULL;

	RTSkillSlotRef SkillSlot = &Character->SkillSlotInfo.Skills[Character->SkillSlotInfo.Count];
	SkillSlot->ID = SkillID;
	SkillSlot->Level = Level;
	SkillSlot->Index = SlotIndex;
	Character->SkillSlotInfo.Count += 1;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return SkillSlot;
}

RTSkillSlotRef RTCharacterGetSkillSlotByIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Character->SkillSlotInfo.Count; Index++) {
		if (Character->SkillSlotInfo.Skills[Index].Index == SlotIndex) {
			return &Character->SkillSlotInfo.Skills[Index];
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
	for (Int32 Index = 0; Index < Character->SkillSlotInfo.Count; Index++) {
		if (Character->SkillSlotInfo.Skills[Index].Index == SlotIndex) {
			Int32 TailLength = Character->SkillSlotInfo.Count - Index - 1;
			if (TailLength > 0) {
				memmove(
					&Character->SkillSlotInfo.Skills[Index],
					&Character->SkillSlotInfo.Skills[Index + 1],
					sizeof(struct _RTSkillSlot)
				);
			}

			Character->SkillSlotInfo.Count -= 1;

			Character->SyncMask |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
			Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;
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

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, SlotIndex);
	if (!SkillSlot || SkillSlot->ID != SkillID || SkillSlot->Level != CurrentSkillLevel) return false;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillID);
	if (!SkillData) return false;

	Int32 LevelDiff = TargetSkillLevel - CurrentSkillLevel;
	Int32 Increment = LevelDiff < 0 ? -1 : 1;
	Int32 Sign = Increment;
	Int32 RequiredSkillPointCount = 0;
	Int64 RequiredCurrencyAlz = 0;

	if (TargetSkillLevel > 0)
		for (Int32 NextLevel = CurrentSkillLevel + Increment; NextLevel <= Sign * TargetSkillLevel; NextLevel += Increment) {
			RTSkillLevelDataRef SkillLevelData = RTRuntimeGetSkillLevelDataByID(Runtime, SkillID, NextLevel);
			if (!SkillLevelData) return false;

			if (Character->Info.Skill.Rank < SkillLevelData->SkillRank) return false;
			if (Character->Info.Skill.Level < SkillLevelData->SkillRankPoint) return false;

			RequiredSkillPointCount += SkillLevelData->SkillPoint;
			RequiredCurrencyAlz += LevelDiff < 0 ? SkillLevelData->UntrainPrice : SkillLevelData->TrainPrice;

			// TODO: Check SkillLevelData->StyleMastery

			Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
			if (SkillLevelData->BattleStyles[BattleStyleIndex - 1] <= 0) return false;
		}

	if (Character->Info.Skill.Point < RequiredSkillPointCount) return false;
	if (Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < RequiredCurrencyAlz) return false;

	Character->Info.Skill.Point -= RequiredSkillPointCount;
	Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= RequiredCurrencyAlz;
	SkillSlot->Level = TargetSkillLevel;

	if (TargetSkillLevel <= 0) {
		RTCharacterRemoveSkillSlot(Runtime, Character, SkillID, SlotIndex);
	}

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_SKILLSLOT;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

	return true;
}
