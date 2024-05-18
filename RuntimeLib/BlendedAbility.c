#include "BlendedAbility.h"
#include "Character.h"
#include "Inventory.h"
#include "Runtime.h"

RTBlendedAbilitySlotRef RTCharacterGetBlendedAbilitySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	for (Int32 Index = 0; Index < Character->BlendedAbilityInfo.Count; Index += 1) {
		RTBlendedAbilitySlotRef AbilitySlot = &Character->BlendedAbilityInfo.Slots[Index];
		if (AbilitySlot->AbilityID != AbilityID) continue;

		return AbilitySlot;
	}

	return NULL;
}

Bool RTCharacterAddBlendedAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 InventorySlotIndex,
	UInt16* MaterialSlotIndex,
	Int32 MaterialSlotCount
) {
	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, InventorySlotIndex);
	if (!ItemSlot) return false;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) return false;

	if (ItemData->ItemType != RUNTIME_ITEM_TYPE_ABILITY_RUNE_BLENDED) return false;
	if (Character->Info.Basic.Level < ItemData->MinLevel) return false;
	if (Character->BlendedAbilityInfo.Count >= Character->Info.Ability.MaxBlendedAbilitySlotCount) return false;

	RTDataBlendedAbilityCostRef AbilityCost = RTRuntimeDataBlendedAbilityCostGet(Runtime->Context, ItemSlot->Item.ID);
	if (!AbilityCost) return false;

	if (Character->Info.Ability.Point < AbilityCost->AP) return false;

	Int32 RequiredMaterialSlotCount = 0;
	if (AbilityCost->ItemCost[0] > 0) RequiredMaterialSlotCount += AbilityCost->ItemCost[1];
	if (MaterialSlotCount < RequiredMaterialSlotCount) return false;

	Int32 MaterialSlotOffset = 0;
	for (Int32 Index = 0; Index < AbilityCost->ItemCost[1]; Index += 1) {
		RTItemSlotRef MaterialSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, MaterialSlotIndex[Index]);
		if (!MaterialSlot) return false;

		RTItemDataRef MaterialData = RTRuntimeGetItemDataByIndex(Runtime, MaterialSlot->Item.ID);
		if (!MaterialData) return false;

		if (AbilityCost->ItemCost[0] != MaterialData->ItemID) return false;
	}

	for (Int32 Index = 0; Index < AbilityCost->ItemCost[1]; Index += 1) {
		RTInventoryClearSlot(Runtime, &Character->InventoryInfo, MaterialSlotIndex[Index]);
	}

	RTBlendedAbilitySlotRef AbilitySlot = &Character->BlendedAbilityInfo.Slots[Character->BlendedAbilityInfo.Count];
	Character->BlendedAbilityInfo.Count += 1;

	AbilitySlot->AbilityID = ItemSlot->Item.ID;
	AbilitySlot->Level = 0;
	AbilitySlot->Unknown1 = 0;

	RTInventoryClearSlot(Runtime, &Character->InventoryInfo, InventorySlotIndex);
	Character->Info.Ability.Point -= AbilityCost->AP;
	Character->SyncMask.Info = true;
	Character->SyncMask.BlendedAbilityInfo = true;
	Character->SyncMask.InventoryInfo = true;
	Character->SyncPriority.High = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

Bool RTCharacterUpgradeBlendedAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID,
	UInt32 InventorySlotCount,
	UInt16* InventorySlotIndices
) {
	return false;
}

Bool RTCharacterRemoveBlendedAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	RTBlendedAbilitySlotRef AbilitySlot = RTCharacterGetBlendedAbilitySlot(Runtime, Character, AbilityID);
	if (!AbilitySlot) return false;

	Int32 SlotIndex = (Int32)(AbilitySlot - &Character->BlendedAbilityInfo.Slots[0]) / sizeof(struct _RTBlendedAbilitySlot);

	Character->BlendedAbilityInfo.Count -= 1;

	Int32 TailLength = Character->BlendedAbilityInfo.Count - SlotIndex;
	if (TailLength > 0) {
		memmove(
			&Character->BlendedAbilityInfo.Slots[SlotIndex],
			&Character->BlendedAbilityInfo.Slots[SlotIndex + 1],
			TailLength * sizeof(struct _RTBlendedAbilitySlot)
		);
	}

	Character->SyncMask.BlendedAbilityInfo = true;
	Character->SyncPriority.High = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}