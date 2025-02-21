#include "Character.h"
#include "Craft.h"
#include "Runtime.h"

Bool RTCharacterIsCraftSlotEmpty(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 SlotIndex
) {
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_CRAFT_SLOT_COUNT) return false;

    Int32 ArrayIndex = RTCharacterGetCraftArrayIndex(Runtime, Character, SlotIndex);
    if (ArrayIndex >= RUNTIME_CHARACTER_MAX_CRAFT_SLOT_COUNT) return false;

    if (ArrayIndex < 0)
    {
        return true;
    }
	else if (ArrayIndex >= 0 && Character->Data.CraftInfo.Slots[ArrayIndex].CraftIndex <= 0) {
		return true;
	}
    else {
		return false;
    }
}

Bool RTCharacterIsCraftCategoryRegistered(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 Category
) {
    if (Character->Data.CraftInfo.Info.SlotCount >= RUNTIME_CHARACTER_MAX_CRAFT_SLOT_COUNT || Character->Data.CraftInfo.Info.SlotCount < 0) return false;

    for (int i = 0; i < Character->Data.CraftInfo.Info.SlotCount; i++) {
        if (Character->Data.CraftInfo.Slots[i].CraftIndex == Category) return true;
    }

    return false;
}

Int RTCharacterGetCraftArrayIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 SlotIndex
) {
    if (Character->Data.CraftInfo.Info.SlotCount >= RUNTIME_CHARACTER_MAX_CRAFT_SLOT_COUNT || Character->Data.CraftInfo.Info.SlotCount < 0) return -1;

    for (int i = 0; i < Character->Data.CraftInfo.Info.SlotCount; i++) {
		if (Character->Data.CraftInfo.Slots[i].SlotIndex == SlotIndex) return i;
	}
    return -1;
}

Bool RTCharacterSetCraftSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt8 SlotIndex,
	UInt8 Category,
	UInt8 ItemInventorySlot
) {
	if (RTCharacterIsCraftCategoryRegistered(Runtime, Character, Category)) return false;

	RTDataProfessionalCraftFormulaRef CraftFormula = RTRuntimeDataProfessionalCraftFormulaGet(Runtime->Context, Category);
	if (!CraftFormula) return false;

	if (!RTInventoryCanConsumeItem(
		Runtime,
		&Character->Data.InventoryInfo,
		CraftFormula->ExpansionItemIndex,
		1,
		ItemInventorySlot
	)) return false;

	RTInventoryConsumeItem(
		Runtime,
		&Character->Data.InventoryInfo,
		CraftFormula->ExpansionItemIndex,
		0,
		1,
		ItemInventorySlot
	);

	Character->SyncMask.InventoryInfo = true;

	Character->Data.CraftInfo.Slots[SlotIndex].CraftIndex = Category;
	Character->Data.CraftInfo.Slots[SlotIndex].CraftLevel = 1;
	Character->Data.CraftInfo.Slots[SlotIndex].SlotIndex = SlotIndex;
	Character->Data.CraftInfo.Slots[SlotIndex].Unknown1 = 0;   // I think this is Craft EXP for the slot.
	Character->Data.CraftInfo.Info.SlotCount += 1;

	Character->SyncMask.CraftInfo = true;

	return true;
}