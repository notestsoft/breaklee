#include "Ability.h"
#include "Character.h"
#include "Inventory.h"
#include "Runtime.h"

Void RTCharacterDataEncodeAbility(
    RTCharacterDataRef CharacterData,
    MemoryBufferRef MemoryBuffer
) {
	MemoryBufferAppendCopy(MemoryBuffer, &CharacterData->AbilityInfo.Info, sizeof(struct _RTAbilityInfo));
	MemoryBufferAppendCopy(MemoryBuffer, CharacterData->AbilityInfo.EssenceAbilitySlots, sizeof(struct _RTEssenceAbilitySlot) * CharacterData->AbilityInfo.Info.EssenceAbilityCount);
	MemoryBufferAppendCopy(MemoryBuffer, CharacterData->AbilityInfo.BlendedAbilitySlots, sizeof(struct _RTBlendedAbilitySlot) * CharacterData->AbilityInfo.Info.BlendedAbilityCount);
	MemoryBufferAppendCopy(MemoryBuffer, CharacterData->AbilityInfo.KarmaAbilitySlots, sizeof(struct _RTKarmaAbilitySlot) * CharacterData->AbilityInfo.Info.KarmaAbilityCount);
}

Void RTCharacterDataDecodeAbility(
    RTCharacterDataRef CharacterData,
    MemoryBufferRef MemoryBuffer
) {
	MemoryBufferReadBytesCopy(MemoryBuffer, &CharacterData->AbilityInfo.Info, sizeof(struct _RTAbilityInfo));
	MemoryBufferReadBytesCopy(MemoryBuffer, CharacterData->AbilityInfo.EssenceAbilitySlots, sizeof(struct _RTEssenceAbilitySlot) * CharacterData->AbilityInfo.Info.EssenceAbilityCount);
	MemoryBufferReadBytesCopy(MemoryBuffer, CharacterData->AbilityInfo.BlendedAbilitySlots, sizeof(struct _RTBlendedAbilitySlot) * CharacterData->AbilityInfo.Info.BlendedAbilityCount);
	MemoryBufferReadBytesCopy(MemoryBuffer, CharacterData->AbilityInfo.KarmaAbilitySlots, sizeof(struct _RTKarmaAbilitySlot) * CharacterData->AbilityInfo.Info.KarmaAbilityCount);
}

RTEssenceAbilitySlotRef RTCharacterGetEssenceAbilitySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	for (Int32 Index = 0; Index < Character->Data.AbilityInfo.Info.EssenceAbilityCount; Index += 1) {
		RTEssenceAbilitySlotRef AbilitySlot = &Character->Data.AbilityInfo.EssenceAbilitySlots[Index];
		if (AbilitySlot->AbilityID != AbilityID) continue;

		return AbilitySlot;
	}

	return NULL;
}

Bool RTCharacterAddEssenceAbility(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 InventorySlotIndex,
	UInt16* MaterialSlotIndex,
	Int32 MaterialSlotCount
) {
	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
	if (!ItemSlot) return false;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX);
	if (!ItemData) return false;

	if (ItemData->ItemType != RUNTIME_ITEM_TYPE_ABILITY_RUNE_ESSENCE) return false;
	if (Character->Data.Info.Level < ItemData->MinLevel) return false;

	Int32 AbilitySlotCount = RUNTIME_CHARACTER_ESSENCE_ABILITY_SLOT_COUNT + Character->Data.AbilityInfo.Info.ExtendedEssenceAbilityCount;
	if (Character->Data.AbilityInfo.Info.EssenceAbilityCount >= AbilitySlotCount) return false;

	RTDataPassiveAbilityCostRef AbilityCost = RTRuntimeDataPassiveAbilityCostGet(Runtime->Context, ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX);
	if (!AbilityCost) return false;

	Int32 AbilityLevel = 1;
	RTDataPassiveAbilityCostLevelRef AbilityCostLevel = RTRuntimeDataPassiveAbilityCostLevelGet(AbilityCost, AbilityLevel);
	if (!AbilityCostLevel) return false;

	if (Character->Data.AbilityInfo.Info.AP < AbilityCostLevel->AP) return false;

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

	RTEssenceAbilitySlotRef AbilitySlot = &Character->Data.AbilityInfo.EssenceAbilitySlots[Character->Data.AbilityInfo.Info.EssenceAbilityCount];
	Character->Data.AbilityInfo.Info.EssenceAbilityCount += 1;

	AbilitySlot->AbilityID = ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX;
	AbilitySlot->Level = AbilityCostLevel->Level;
	AbilitySlot->Unknown1 = 0;

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
	Character->Data.AbilityInfo.Info.AP -= AbilityCostLevel->AP;
	Character->SyncMask.AbilityInfo = true;
	Character->SyncMask.InventoryInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
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

	RTDataPassiveAbilityCostRef AbilityCost = RTRuntimeDataPassiveAbilityCostGet(Runtime->Context, AbilityID);
	if (!AbilityCost) return false;

	RTDataPassiveAbilityCostLevelRef AbilityCostLevel = RTRuntimeDataPassiveAbilityCostLevelGet(AbilityCost, AbilitySlot->Level + 1);
	if (!AbilityCostLevel) return false;

	if (Character->Data.AbilityInfo.Info.AP < AbilityCostLevel->AP) return false;

	Int32 CostCount = 2;
	struct { Int64 CostItem[3]; Int64 ConsumableCount; Int64 RemainingCount; } Cost[] = {
		{ { AbilityCostLevel->Item1[0], AbilityCostLevel->Item1[1], AbilityCostLevel->Item1[2] }, 0, AbilityCostLevel->Item1[2] },
		{ { AbilityCostLevel->Item2[0], AbilityCostLevel->Item2[1], AbilityCostLevel->Item2[2] }, 0, AbilityCostLevel->Item2[2] },
	};

	for (Int32 Index = 0; Index < InventorySlotCount; Index += 1) {
		for (Int32 CostIndex = 0; CostIndex < CostCount; CostIndex += 1) {
			if (!Cost[CostIndex].CostItem[0]) continue;

			RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndices[Index]);
			if (!ItemSlot) return false;
			if (ItemSlot->Item.ID != Cost[CostIndex].CostItem[0]) return false;
			if (Cost[CostIndex].CostItem[1] && ItemSlot->ItemOptions != Cost[CostIndex].CostItem[1]) return false;

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

			RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndices[Index]);
			assert(ItemSlot);
			assert(ItemSlot->Item.ID == Cost[CostIndex].CostItem[0]);

			RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
			assert(ItemData);

			Bool DeleteItem = false;
			if (ItemData->MaxStackSize > 0) {
				Int64 ConsumedCount = MIN(Cost[CostIndex].RemainingCount, (Int32)ItemSlot->ItemOptions);
				Cost[CostIndex].RemainingCount -= ConsumedCount;
				ItemSlot->ItemOptions -= ConsumedCount;
				DeleteItem = (ItemSlot->ItemOptions < 1);
			}
			else {
				Int64 ConsumedCount = MIN(Cost[CostIndex].RemainingCount, 1);
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

	Character->Data.AbilityInfo.Info.AP -= AbilityCostLevel->AP;
	Character->SyncMask.AbilityInfo = true;
	Character->SyncMask.InventoryInfo = true;
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

	Int32 SlotIndex = (Int32)(AbilitySlot - &Character->Data.AbilityInfo.EssenceAbilitySlots[0]) / sizeof(struct _RTEssenceAbilitySlot);

	Character->Data.AbilityInfo.Info.EssenceAbilityCount -= 1;
	
	Int32 TailLength = Character->Data.AbilityInfo.Info.EssenceAbilityCount - SlotIndex;
	if (TailLength > 0) {
		memmove(
			&Character->Data.AbilityInfo.EssenceAbilitySlots[SlotIndex],
			&Character->Data.AbilityInfo.EssenceAbilitySlots[SlotIndex + 1],
			TailLength * sizeof(struct _RTEssenceAbilitySlot)
		);
	}

	Character->SyncMask.AbilityInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

RTBlendedAbilitySlotRef RTCharacterGetBlendedAbilitySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	for (Int32 Index = 0; Index < Character->Data.AbilityInfo.Info.BlendedAbilityCount; Index += 1) {
		RTBlendedAbilitySlotRef AbilitySlot = &Character->Data.AbilityInfo.BlendedAbilitySlots[Index];
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
	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
	if (!ItemSlot) return false;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) return false;

	if (ItemData->ItemType != RUNTIME_ITEM_TYPE_ABILITY_RUNE_BLENDED) return false;
	if (Character->Data.Info.Level < ItemData->MinLevel) return false;

	Int32 AbilitySlotCount = RUNTIME_CHARACTER_BLENDED_ABILITY_SLOT_COUNT + Character->Data.AbilityInfo.Info.ExtendedBlendedAbilityCount;
	if (Character->Data.AbilityInfo.Info.BlendedAbilityCount >= AbilitySlotCount) return false;

	RTDataBlendedAbilityCostRef AbilityCost = RTRuntimeDataBlendedAbilityCostGet(Runtime->Context, ItemSlot->Item.ID);
	if (!AbilityCost) return false;

	if (Character->Data.AbilityInfo.Info.AP < AbilityCost->AP) return false;

	Int32 RequiredMaterialSlotCount = 0;
	if (AbilityCost->ItemCost[0] > 0) RequiredMaterialSlotCount += AbilityCost->ItemCost[1];
	if (MaterialSlotCount < RequiredMaterialSlotCount) return false;

	Int32 MaterialSlotOffset = 0;
	for (Int32 Index = 0; Index < AbilityCost->ItemCost[1]; Index += 1) {
		RTItemSlotRef MaterialSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index]);
		if (!MaterialSlot) return false;

		RTItemDataRef MaterialData = RTRuntimeGetItemDataByIndex(Runtime, MaterialSlot->Item.ID);
		if (!MaterialData) return false;

		if (AbilityCost->ItemCost[0] != MaterialData->ItemID) return false;
	}

	for (Int32 Index = 0; Index < AbilityCost->ItemCost[1]; Index += 1) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, MaterialSlotIndex[Index]);
	}
	
	RTBlendedAbilitySlotRef AbilitySlot = &Character->Data.AbilityInfo.BlendedAbilitySlots[Character->Data.AbilityInfo.Info.BlendedAbilityCount];
	Character->Data.AbilityInfo.Info.BlendedAbilityCount += 1;

	AbilitySlot->AbilityID = ItemSlot->Item.ID;
	AbilitySlot->Level = 0;
	AbilitySlot->Unknown1 = 0;

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
	Character->Data.AbilityInfo.Info.AP -= AbilityCost->AP;
	Character->SyncMask.AbilityInfo = true;
	Character->SyncMask.InventoryInfo = true;
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

	Int32 SlotIndex = (Int32)(AbilitySlot - &Character->Data.AbilityInfo.BlendedAbilitySlots[0]) / sizeof(struct _RTBlendedAbilitySlot);

	Character->Data.AbilityInfo.Info.BlendedAbilityCount -= 1;

	Int32 TailLength = Character->Data.AbilityInfo.Info.BlendedAbilityCount - SlotIndex;
	if (TailLength > 0) {
		memmove(
			&Character->Data.AbilityInfo.BlendedAbilitySlots[SlotIndex],
			&Character->Data.AbilityInfo.BlendedAbilitySlots[SlotIndex + 1],
			TailLength * sizeof(struct _RTBlendedAbilitySlot)
		);
	}

	Character->SyncMask.AbilityInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}

RTKarmaAbilitySlotRef RTCharacterGetKarmaAbilitySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt32 AbilityID
) {
	for (Int32 Index = 0; Index < Character->Data.AbilityInfo.Info.KarmaAbilityCount; Index += 1) {
		RTKarmaAbilitySlotRef AbilitySlot = &Character->Data.AbilityInfo.KarmaAbilitySlots[Index];
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
	if (Character->Data.Info.Level < ItemData->MinLevel) return false;

	Int32 AbilitySlotCount = RUNTIME_CHARACTER_KARMA_ABILITY_SLOT_COUNT + Character->Data.AbilityInfo.Info.ExtendedKarmaAbilityCount;
	if (Character->Data.AbilityInfo.Info.KarmaAbilityCount >= AbilitySlotCount) return false;

	RTDataKarmaAbilityCostRef AbilityCost = RTRuntimeDataKarmaAbilityCostGet(Runtime->Context, ItemSlot->Item.ID);
	if (!AbilityCost) return false;

	Int32 AbilityLevel = 1;
	RTDataKarmaAbilityCostLevelRef AbilityCostLevel = RTRuntimeDataKarmaAbilityCostLevelGet(AbilityCost, AbilityLevel);
	if (!AbilityCostLevel) return false;

	if (Character->Data.AbilityInfo.Info.AP < AbilityCostLevel->AP) return false;

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

	RTKarmaAbilitySlotRef AbilitySlot = &Character->Data.AbilityInfo.KarmaAbilitySlots[Character->Data.AbilityInfo.Info.KarmaAbilityCount];
	Character->Data.AbilityInfo.Info.KarmaAbilityCount += 1;

	AbilitySlot->AbilityID = ItemSlot->Item.ID;
	AbilitySlot->Level = AbilityCostLevel->Level;

	RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
	Character->Data.AbilityInfo.Info.AP -= AbilityCostLevel->AP;
	Character->SyncMask.AbilityInfo = true;
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

	if (Character->Data.AbilityInfo.Info.AP < AbilityCostLevel->AP) return false;

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

	Character->Data.AbilityInfo.Info.AP -= AbilityCostLevel->AP;
	Character->SyncMask.AbilityInfo = true;
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

	Int32 SlotIndex = (Int32)(AbilitySlot - &Character->Data.AbilityInfo.KarmaAbilitySlots[0]) / sizeof(struct _RTKarmaAbilitySlot);

	Character->Data.AbilityInfo.Info.KarmaAbilityCount -= 1;

	Int32 TailLength = Character->Data.AbilityInfo.Info.KarmaAbilityCount - SlotIndex;
	if (TailLength > 0) {
		memmove(
			&Character->Data.AbilityInfo.KarmaAbilitySlots[SlotIndex],
			&Character->Data.AbilityInfo.KarmaAbilitySlots[SlotIndex + 1],
			TailLength * sizeof(struct _RTKarmaAbilitySlot)
		);
	}

	Character->SyncMask.AbilityInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);

	return true;
}
