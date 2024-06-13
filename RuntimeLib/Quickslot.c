#include "Character.h"
#include "Quickslot.h"

Bool RTCharacterAddQuickSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex,
	Int32 SlotIndex
) {
	if (Character->Data.QuickSlotInfo.Count >= RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) return false;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, SkillIndex);
	if (!SkillSlot) return false;

	RTQuickSlotRef QuickSlot = RTCharacterGetQuickSlotByIndex(Runtime, Character, SlotIndex);
	if (QuickSlot) {
		QuickSlot->SkillIndex = SkillIndex;
		QuickSlot->SlotIndex = SlotIndex;
		return true;
	}

	QuickSlot = &Character->Data.QuickSlotInfo.QuickSlots[Character->Data.QuickSlotInfo.Count];
	QuickSlot->SkillIndex = SkillIndex;
	QuickSlot->SlotIndex = SlotIndex;
	Character->Data.QuickSlotInfo.Count += 1;

	Character->SyncMask.QuickSlotInfo = true;

	return true;
}

RTQuickSlotRef RTCharacterGetQuickSlotByIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Character->Data.QuickSlotInfo.Count; Index++) {
		if (Character->Data.QuickSlotInfo.QuickSlots[Index].SlotIndex == SlotIndex) {
			return &Character->Data.QuickSlotInfo.QuickSlots[Index];
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
	for (Int32 Index = 0; Index < Character->Data.QuickSlotInfo.Count; Index++) {
		if (Character->Data.QuickSlotInfo.QuickSlots[Index].SlotIndex == SlotIndex) {
			Int32 TailLength = Character->Data.QuickSlotInfo.Count - Index - 1;
			if (TailLength > 0) {
				memmove(
					&Character->Data.QuickSlotInfo.QuickSlots[Index],
					&Character->Data.QuickSlotInfo.QuickSlots[Index + 1],
					sizeof(struct _RTQuickSlot)
				);
			}

			Character->Data.QuickSlotInfo.Count -= 1;
			Character->SyncMask.QuickSlotInfo = true;

			return true;
		}
	}

	return false;
}
