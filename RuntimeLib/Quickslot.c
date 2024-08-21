#include "Character.h"
#include "Quickslot.h"

Bool RTCharacterAddQuickSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex,
	Int32 SlotIndex
) {
	if (Character->Data.QuickSlotInfo.Info.SlotCount >= RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) return false;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, SkillIndex);
	if (!SkillSlot) return false;

	RTQuickSlotRef QuickSlot = RTCharacterGetQuickSlotByIndex(Runtime, Character, SlotIndex);
	if (QuickSlot) {
		QuickSlot->SkillIndex = SkillIndex;
		QuickSlot->SlotIndex = SlotIndex;
		return true;
	}

	QuickSlot = &Character->Data.QuickSlotInfo.Slots[Character->Data.QuickSlotInfo.Info.SlotCount];
	QuickSlot->SkillIndex = SkillIndex;
	QuickSlot->SlotIndex = SlotIndex;
	Character->Data.QuickSlotInfo.Info.SlotCount += 1;

	Character->SyncMask.QuickSlotInfo = true;

	return true;
}

RTQuickSlotRef RTCharacterGetQuickSlotByIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Character->Data.QuickSlotInfo.Info.SlotCount; Index += 1) {
		if (Character->Data.QuickSlotInfo.Slots[Index].SlotIndex == SlotIndex) {
			return &Character->Data.QuickSlotInfo.Slots[Index];
		}
	}

	return NULL;
}

Bool RTCharacterSwapQuickSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SourceSlotIndex,
	Int32 TargetSlotIndex
) {
	RTQuickSlotRef SourceQuickSlot = RTCharacterGetQuickSlotByIndex(Runtime, Character, SourceSlotIndex);
	RTQuickSlotRef TargetQuickSlot = RTCharacterGetQuickSlotByIndex(Runtime, Character, TargetSlotIndex);

	if (!SourceQuickSlot || !TargetQuickSlot) return false;

	SourceQuickSlot->SlotIndex = TargetSlotIndex;
	TargetQuickSlot->SlotIndex = SourceSlotIndex;

	Character->SyncMask.QuickSlotInfo = true;

	return true;
}

Bool RTCharacterRemoveQuickSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Character->Data.QuickSlotInfo.Info.SlotCount; Index++) {
		if (Character->Data.QuickSlotInfo.Slots[Index].SlotIndex == SlotIndex) {
			Int32 TailLength = Character->Data.QuickSlotInfo.Info.SlotCount - Index - 1;
			if (TailLength > 0) {
				memmove(
					&Character->Data.QuickSlotInfo.Slots[Index],
					&Character->Data.QuickSlotInfo.Slots[Index + 1],
					TailLength * sizeof(struct _RTQuickSlot)
				);
			}

			Character->Data.QuickSlotInfo.Info.SlotCount -= 1;
			Character->SyncMask.QuickSlotInfo = true;

			return true;
		}
	}

	return false;
}
