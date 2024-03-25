#include "Character.h"
#include "EssenceAbility.h"
#include "Inventory.h"
#include "Runtime.h"

// TODO: Calculate character essence ability slot count and slot extension!

RTEssenceAbilitySlotRef RTCharacterGetEssenceAbilitySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	for (Int32 Index = 0; Index < Character->EssenceAbilityInfo.Count; Index += 1) {
		RTEssenceAbilitySlotRef AbilitySlot = &Character->EssenceAbilityInfo.Slots[Index];
		if (AbilitySlot->AbilityID != AbilityID) continue;

		return AbilitySlot;
	}

	return NULL;
}

Bool RTCharacterAddEssenceAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
	UInt32 InventorySlotIndex
) {
	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, InventorySlotIndex);
	if (!ItemSlot) return false;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) return false;

	if (Character->Info.Basic.Level < ItemData->MinLevel) return false;

	if (ItemData->ItemType == RUNTIME_ITEM_TYPE_ABILITY_RUNE_ESSENCE) {
		if (Character->EssenceAbilityInfo.Count >= Character->Info.Ability.MaxEssenceAbilitySlotCount) return false;

		RTDataPassiveAbilityCostRef AbilityCost = RTRuntimeDataPassiveAbilityCostGet(
			Runtime->Context,
			ItemSlot->Item.ID
		);
		assert(AbilityCost);

		RTDataPassiveAbilityCostLevelRef AbilityCostLevel = RTRuntimeDataPassiveAbilityCostLevelGet(
			AbilityCost,
			1
		);
		assert(AbilityCostLevel);

		if (Character->Info.Ability.Point < AbilityCostLevel->AP) return false;

		// TODO: Check how inventory slot indices are passed!
		// TODO: Consume item1, 2, 3 from inventory

		RTEssenceAbilitySlotRef AbilitySlot = &Character->EssenceAbilityInfo.Slots[Character->EssenceAbilityInfo.Count];
		Character->EssenceAbilityInfo.Count += 1;

		AbilitySlot->AbilityID = ItemSlot->Item.ID;
		AbilitySlot->Level = AbilityCostLevel->Level;
		AbilitySlot->Unknown1 = 0;

		RTInventoryClearSlot(Runtime, &Character->InventoryInfo, InventorySlotIndex);
		Character->Info.Ability.Point -= AbilityCostLevel->AP;
		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_ESSENCE_ABILITY;
		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
		Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH;
		RTCharacterInitializeAttributes(Runtime, Character);

		return true;
	}

	return false;
}

Bool RTCharacterUpgradeEssenceAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID,
	UInt32 InventorySlotCount,
	UInt16* InventorySlotIndices
) {
	RTEssenceAbilitySlotRef AbilitySlot = RTCharacterGetEssenceAbilitySlot(Runtime, Character, AbilityID);
	if (!AbilitySlot) return false;

	RTDataPassiveAbilityCostRef AbilityCost = RTRuntimeDataPassiveAbilityCostGet(
		Runtime->Context,
		AbilityID
	);
	assert(AbilityCost);

	RTDataPassiveAbilityCostLevelRef AbilityCostLevel = RTRuntimeDataPassiveAbilityCostLevelGet(
		AbilityCost,
		AbilitySlot->Level + 1
	);
	if (!AbilityCostLevel) return false;

	if (Character->Info.Ability.Point < AbilityCostLevel->AP) return false;

	Int32 CostCount = 2;
	struct { Int32 CostItem[3]; Int32 ConsumableCount; Int32 RemainingCount; } Cost[] = {
		{ { AbilityCostLevel->Item1[0], AbilityCostLevel->Item1[1], AbilityCostLevel->Item1[2] }, 0, AbilityCostLevel->Item1[2]},
		{ { AbilityCostLevel->Item2[0], AbilityCostLevel->Item2[1], AbilityCostLevel->Item2[2] }, 0, AbilityCostLevel->Item2[2] },
	};

	for (Int32 Index = 0; Index < InventorySlotCount; Index += 1) {
		for (Int32 CostIndex = 0; CostIndex < CostCount; CostIndex += 1) {
			if (!Cost[CostIndex].CostItem[0]) continue;

			RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, InventorySlotIndices[Index]);
			if (!ItemSlot) return false;
			if (ItemSlot->Item.ID != Cost[CostIndex].CostItem[0]) return false;
			if (ItemSlot->ItemOptions != Cost[CostIndex].CostItem[1]) return false;

			RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
			if (!ItemData) return false;

			if (ItemData->MaxStackSize > 0) {
				Cost[CostIndex].ConsumableCount += ItemSlot->ItemOptions;
			}
			else {
				Cost[CostIndex].ConsumableCount += 1;
			}
		}
	}

	for (Int32 CostIndex = 0; CostIndex < CostCount; CostIndex += 1) {
		if (!Cost[CostIndex].CostItem[0]) continue;

		if (Cost[CostIndex].CostItem[2] > Cost[CostIndex].ConsumableCount) {
			return false;
		}
	}

	for (Int32 Index = 0; Index < InventorySlotCount; Index += 1) {
		for (Int32 CostIndex = 0; CostIndex < CostCount; CostIndex += 1) {
			if (!Cost[CostIndex].CostItem[0]) continue;
			if (Cost[CostIndex].RemainingCount < 1) continue;

			RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, InventorySlotIndices[Index]);
			assert(ItemSlot);
			assert(ItemSlot->Item.ID == Cost[CostIndex].CostItem[0]);
			assert(ItemSlot->ItemOptions == Cost[CostIndex].CostItem[1]);

			RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
			assert(ItemData);

			Bool DeleteItem = false;
			if (ItemData->MaxStackSize > 0) {
				Int32 ConsumedCount = MIN(Cost[CostIndex].RemainingCount, (Int32)ItemSlot->ItemOptions);
				Cost[CostIndex].RemainingCount -= ConsumedCount;
				ItemSlot->ItemOptions -= ConsumedCount;
				DeleteItem = (ItemSlot->ItemOptions < 1);
			}
			else {
				Int32 ConsumedCount = MIN(Cost[CostIndex].RemainingCount, 1); 
				Cost[CostIndex].RemainingCount -= ConsumedCount;
				DeleteItem = true;
			}

			if (DeleteItem) {
				RTInventoryClearSlot(Runtime, &Character->InventoryInfo, ItemSlot->SlotIndex);
				Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
				Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH;
			}
		}
	}

	AbilitySlot->Level = AbilityCostLevel->Level;

	Character->Info.Ability.Point -= AbilityCostLevel->AP;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_ESSENCE_ABILITY;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

Bool RTCharacterRemoveEssenceAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	RTEssenceAbilitySlotRef AbilitySlot = RTCharacterGetEssenceAbilitySlot(Runtime, Character, AbilityID);
	if (!AbilitySlot) return false;

	Int32 SlotIndex = (Int32)(AbilitySlot - &Character->EssenceAbilityInfo.Slots[0]) / sizeof(struct _RTEssenceAbilitySlot);

	Character->EssenceAbilityInfo.Count -= 1;

	Int32 TailLength = Character->EssenceAbilityInfo.Count - SlotIndex;
	if (TailLength > 0) {
		memmove(
			&Character->EssenceAbilityInfo.Slots[SlotIndex],
			&Character->EssenceAbilityInfo.Slots[SlotIndex + 1],
			TailLength * sizeof(struct _RTEssenceAbilitySlot)
		);
	}

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_ESSENCE_ABILITY;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_HIGH;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

Void RTCharacterApplyEssenceAbilityAttributes(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	for (Int32 Index = 0; Index < Character->EssenceAbilityInfo.Count; Index += 1) {
		RTEssenceAbilitySlotRef AbilitySlot = &Character->EssenceAbilityInfo.Slots[Index];

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

		// TODO: Check if ForceCode is force effect or code..
		/*
		RUNTIME_DATA_PROPERTY(Int32, ForceCode, "forcecode")
		RUNTIME_DATA_PROPERTY(Int32, ValueType, "valuetype")
		AbilityLevel->ForceValue
		*/
	}
}
