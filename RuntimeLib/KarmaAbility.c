#include "KarmaAbility.h"
#include "Character.h"
#include "Inventory.h"
#include "Runtime.h"

RTKarmaAbilitySlotRef RTCharacterGetKarmaAbilitySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	for (Int32 Index = 0; Index < Character->Data.KarmaAbilityInfo.Count; Index += 1) {
		RTKarmaAbilitySlotRef AbilitySlot = &Character->Data.KarmaAbilityInfo.Slots[Index];
		if (AbilitySlot->AbilityID != AbilityID) continue;

		return AbilitySlot;
	}

	return NULL;
}

Bool RTCharacterAddKarmaAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 InventorySlotIndex,
	UInt16* MaterialSlotIndex,
	Int32 MaterialSlotCount
) {
	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
	if (!ItemSlot) return false;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) return false;

	if (ItemData->ItemType != RUNTIME_ITEM_TYPE_ABILITY_RUNE_KARMA) return false;
	if (Character->Data.Info.Basic.Level < ItemData->MinLevel) return false;
	if (Character->Data.KarmaAbilityInfo.Count >= Character->Data.Info.Ability.MaxKarmaAbilitySlotCount) return false;

	RTDataKarmaAbilityCostRef AbilityCost = RTRuntimeDataKarmaAbilityCostGet(Runtime->Context, ItemSlot->Item.ID);
	if (!AbilityCost) return false;

	Int32 AbilityLevel = 1;
	RTDataKarmaAbilityCostLevelRef AbilityCostLevel = RTRuntimeDataKarmaAbilityCostLevelGet(AbilityCost, AbilityLevel);
	if (!AbilityCostLevel) return false;

	if (Character->Data.Info.Ability.Point < AbilityCostLevel->AP) return false;

	Int32 RequiredMaterialSlotCount = 0;
	if (AbilityCostLevel->Item1[0] > 0) RequiredMaterialSlotCount += AbilityCostLevel->Item1[2];
	if (AbilityCostLevel->Item2[0] > 0) RequiredMaterialSlotCount += AbilityCostLevel->Item2[2];
	if (MaterialSlotCount < RequiredMaterialSlotCount) return false;

	Int32 MaterialSlotOffset = 0;
	for (Int32 Index = 0; Index < AbilityCostLevel->Item1[2]; Index += 1) {
		RTItemSlotRef MaterialSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index]);
		if (!MaterialSlot) return false;

		RTItemDataRef MaterialData = RTRuntimeGetItemDataByIndex(Runtime, MaterialSlot->Item.ID);
		if (!MaterialData) return false;

		if (AbilityCostLevel->Item1[0] != MaterialData->ItemID) return false;
		if (AbilityCostLevel->Item1[1] != MaterialSlot->ItemOptions) return false;
	}

	for (Int32 Index = 0; Index < AbilityCostLevel->Item2[2]; Index += 1) {
		RTItemSlotRef MaterialSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index + AbilityCostLevel->Item1[2]]);
		if (!MaterialSlot) return false;

		RTItemDataRef MaterialData = RTRuntimeGetItemDataByIndex(Runtime, MaterialSlot->Item.ID);
		if (!MaterialData) return false;

		if (AbilityCostLevel->Item1[0] != MaterialData->ItemID) return false;
		if (AbilityCostLevel->Item1[1] != MaterialSlot->ItemOptions) return false;
	}

	for (Int32 Index = 0; Index < AbilityCostLevel->Item1[2]; Index += 1) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index]);
	}

	for (Int32 Index = 0; Index < AbilityCostLevel->Item2[2]; Index += 1) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index + AbilityCostLevel->Item1[2]]);
	}

	RTKarmaAbilitySlotRef AbilitySlot = &Character->Data.KarmaAbilityInfo.Slots[Character->Data.KarmaAbilityInfo.Count];
	Character->Data.KarmaAbilityInfo.Count += 1;

	AbilitySlot->AbilityID = ItemSlot->Item.ID;
	AbilitySlot->Level = AbilityCostLevel->Level;
	AbilitySlot->Unknown1 = 0;

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
	Character->Data.Info.Ability.Point -= AbilityCostLevel->AP;
	Character->SyncMask.Info = true;
	Character->SyncMask.KarmaAbilityInfo = true;
	Character->SyncMask.InventoryInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

Bool RTCharacterUpgradeKarmaAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID,
	UInt32 InventorySlotCount,
	UInt16* InventorySlotIndices
) {
	RTKarmaAbilitySlotRef AbilitySlot = RTCharacterGetKarmaAbilitySlot(Runtime, Character, AbilityID);
	if (!AbilitySlot) return false;

	RTDataKarmaAbilityCostRef AbilityCost = RTRuntimeDataKarmaAbilityCostGet(Runtime->Context, AbilityID);
	if (!AbilityCost) return false;

	RTDataKarmaAbilityCostLevelRef AbilityCostLevel = RTRuntimeDataKarmaAbilityCostLevelGet(AbilityCost, AbilitySlot->Level + 1);
	if (!AbilityCostLevel) return false;

	if (Character->Data.Info.Ability.Point < AbilityCostLevel->AP) return false;

	Int32 CostCount = 2;
	struct { Int32 CostItem[3]; Int32 ConsumableCount; Int32 RemainingCount; } Cost[] = {
		{ { AbilityCostLevel->Item1[0], AbilityCostLevel->Item1[1], AbilityCostLevel->Item1[2] }, 0, AbilityCostLevel->Item1[2]},
		{ { AbilityCostLevel->Item2[0], AbilityCostLevel->Item2[1], AbilityCostLevel->Item2[2] }, 0, AbilityCostLevel->Item2[2] },
	};

	for (Int32 Index = 0; Index < InventorySlotCount; Index += 1) {
		for (Int32 CostIndex = 0; CostIndex < CostCount; CostIndex += 1) {
			if (!Cost[CostIndex].CostItem[0]) continue;

			RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndices[Index]);
			if (!ItemSlot) return false;
			if (ItemSlot->Item.ID != Cost[CostIndex].CostItem[0]) return false;

			RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
			if (!ItemData) return false;

			if (Cost[CostIndex].CostItem[1] && ItemSlot->ItemOptions != Cost[CostIndex].CostItem[1]) return false;

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

			RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndices[Index]);
			assert(ItemSlot);
			assert(ItemSlot->Item.ID == Cost[CostIndex].CostItem[0]);

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
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
				Character->SyncMask.InventoryInfo = true;
			}
		}
	}

	AbilitySlot->Level = AbilityCostLevel->Level;

	Character->Data.Info.Ability.Point -= AbilityCostLevel->AP;
	Character->SyncMask.Info = true;
	Character->SyncMask.KarmaAbilityInfo = true;
	Character->SyncMask.InventoryInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

Bool RTCharacterRemoveKarmaAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	RTKarmaAbilitySlotRef AbilitySlot = RTCharacterGetKarmaAbilitySlot(Runtime, Character, AbilityID);
	if (!AbilitySlot) return false;

	Int32 SlotIndex = (Int32)(AbilitySlot - &Character->Data.KarmaAbilityInfo.Slots[0]) / sizeof(struct _RTKarmaAbilitySlot);

	Character->Data.KarmaAbilityInfo.Count -= 1;

	Int32 TailLength = Character->Data.KarmaAbilityInfo.Count - SlotIndex;
	if (TailLength > 0) {
		memmove(
			&Character->Data.KarmaAbilityInfo.Slots[SlotIndex],
			&Character->Data.KarmaAbilityInfo.Slots[SlotIndex + 1],
			TailLength * sizeof(struct _RTKarmaAbilitySlot)
		);
	}

	Character->SyncMask.KarmaAbilityInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}
