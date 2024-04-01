#include "Constants.h"
#include "Character.h"
#include "Inventory.h"
#include "Runtime.h"

static struct _RTCharacterInventoryInfo kInventorySnapshot = { 0 };

// TODO: Move rollback logic to here...

Int32 RTInventoryGetInsertionIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Inventory->Count; Index += 1) {
		RTItemSlotRef InventorySlot = &Inventory->Slots[Index];
		if (InventorySlot->SlotIndex > SlotIndex) {
			return Index;
		}
	}

	return Inventory->Count;
}

Int32 RTInventoryGetNextFreeSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory
) {
	for (Int32 Index = 0; Index < Inventory->Count - 1; Index += 1) {
		RTItemSlotRef InventorySlot = &Inventory->Slots[Index];
		RTItemSlotRef NextInventorySlot = &Inventory->Slots[Index + 1];
		Int32 SlotOffset = NextInventorySlot->SlotIndex - InventorySlot->SlotIndex;
		if (SlotOffset > 1) {
			return InventorySlot->SlotIndex + 1;
		}
	}

	return Inventory->Count;
}

Bool RTInventoryIsSlotEmpty(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	return RTInventoryGetSlot(Runtime, Inventory, SlotIndex) == NULL;
}

Int32 RTInventoryGetSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Inventory->Count; Index++) {
		RTItemSlotRef Slot = &Inventory->Slots[Index];
		if (Slot->SlotIndex == SlotIndex) {
			return Index;
		}
	}

	return -1;
}

RTItemSlotRef RTInventoryGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	Int32 Index = RTInventoryGetSlotIndex(Runtime, Inventory, SlotIndex);
	if (Index < 0) return NULL;
	return &Inventory->Slots[Index];
}

Bool RTInventorySetSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	RTItemSlotRef Slot
) {
    assert(Inventory);
	assert(0 <= Slot->SlotIndex && Slot->SlotIndex < RUNTIME_INVENTORY_TOTAL_SIZE);

	assert(RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID));

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID);
	assert(ItemData);

	Int32 Index = RTInventoryGetSlotIndex(
		Runtime, 
		Inventory, 
		Slot->SlotIndex
	);
	if (Index >= 0) {
		RTItemSlotRef InventorySlot = RTInventoryGetSlot(Runtime, Inventory, Slot->SlotIndex);
		assert(InventorySlot);

		if (Slot->Item.ID != InventorySlot->Item.ID) return false;

		if (ItemData->ItemType == RUNTIME_ITEM_TYPE_QUEST_S) {
			UInt64 ItemOptions = RTQuestItemGetOptions(Slot->ItemOptions);
			UInt64 InventoryItemOptions = RTQuestItemGetOptions(InventorySlot->ItemOptions);
			
			if (ItemOptions != InventoryItemOptions) return false;

			UInt64 ItemCount = RTQuestItemGetCount(Slot->ItemOptions);
			UInt64 InventoryItemCount = RTQuestItemGetCount(InventorySlot->ItemOptions);
			// if (ItemCount + InventoryItemCount > ItemData->MaxStackSize) return false;

			InventorySlot->ItemOptions = RTQuestItemOptions(ItemOptions, (ItemCount + InventoryItemCount));
			memcpy(Slot, InventorySlot, sizeof(struct _RTItemSlot));
			return true;
		}
		
		return false;
	}

	Int32 InsertionIndex = RTInventoryGetInsertionIndex(Runtime, Inventory, Slot->SlotIndex);
	Int32 InsertionTailLength = Inventory->Count - InsertionIndex;
	if (InsertionTailLength > 0) {
		memmove(
			&Inventory->Slots[InsertionIndex + 1],
			&Inventory->Slots[InsertionIndex],
			sizeof(struct _RTItemSlot) * InsertionTailLength
		);
	}

	RTItemSlotRef InventorySlot = &Inventory->Slots[InsertionIndex];
	memcpy(InventorySlot, Slot, sizeof(struct _RTItemSlot));
	Inventory->Count += 1;
	return true;
}

// TODO: RTInventoryClearSlot is moving the memory and causes pointer corruptions on stack while referencing slots..
//		 instead of moving the memory all inventory slots could just stay in their static locations to reduce complexity.
Bool RTInventoryClearSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	assert(0 <= SlotIndex && SlotIndex < RUNTIME_INVENTORY_TOTAL_SIZE);

	Int32 InventoryIndex = RTInventoryGetSlotIndex(
		Runtime,
		Inventory,
		SlotIndex
	);
	if (InventoryIndex < 0) return false;

	Int32 TailLength = Inventory->Count - InventoryIndex - 1;
	if (TailLength > 0) {
		memmove(
			&Inventory->Slots[InventoryIndex],
			&Inventory->Slots[InventoryIndex + 1],
			sizeof(struct _RTItemSlot) * TailLength
		);
	}

	Inventory->Count -= 1;
	return true;
}

Bool RTInventoryRemoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex,
	RTItemSlotRef Result
) {
	RTItemSlotRef Slot = RTInventoryGetSlot(Runtime, Inventory, SlotIndex);
	if (!Slot) return false;

	memcpy(Result, Slot, sizeof(struct _RTItemSlot));

	return RTInventoryClearSlot(Runtime, Inventory, SlotIndex);
}

Bool RTInventoryMoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef SourceInventory,
	RTCharacterInventoryInfoRef TargetInventory,
	Int32 SourceSlotIndex,
	Int32 TargetSlotIndex
) {
	RTItemSlotRef SourceSlot = RTInventoryGetSlot(Runtime, SourceInventory, SourceSlotIndex);
	if (!SourceSlot) return false;

	RTItemSlotRef TargetSlot = RTInventoryGetSlot(Runtime, TargetInventory, TargetSlotIndex);
	if (TargetSlot && SourceSlot->Item.Serial == TargetSlot->Item.Serial) {
		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, SourceSlot->Item.ID);
		if (!ItemData || ItemData->MaxStackSize <= 1) return false;
		
		// TODO: Check if item is quest item and calculate count accordingly

		Int32 TotalStackSize = (Int32)(SourceSlot->ItemOptions + TargetSlot->ItemOptions);
		TargetSlot->ItemOptions = MIN(ItemData->MaxStackSize, TotalStackSize);
		SourceSlot->ItemOptions = MAX(0, TotalStackSize - TargetSlot->ItemOptions);

		if (SourceSlot->ItemOptions < 1) {
			if (!RTInventoryClearSlot(Runtime, SourceInventory, SourceSlotIndex)) return false;
		}
	}
	else if (SourceSlot && !TargetSlot) {
        struct _RTItemSlot SlotCopy = *SourceSlot;
        if (!RTInventoryClearSlot(Runtime, SourceInventory, SourceSlot->SlotIndex)) return false;

        SlotCopy.SlotIndex = TargetSlotIndex;
        if (!RTInventorySetSlot(Runtime, TargetInventory, &SlotCopy)) return false;
	} else {
        return false;
    }

    return true;
}

Void RTInventoryFindItems(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 ItemID,
	Int32 Length,
	Int32* Count,
	RTItemSlotRef Results
) {
	assert(Length > 0);

	*Count = 0;

	for (Int32 Index = 0; Index < Inventory->Count; Index++) {
		RTItemSlotRef InventorySlot = &Inventory->Slots[Index];
		if (InventorySlot->Item.ID == ItemID) {
			memcpy(&Results[*Count], InventorySlot, sizeof(struct _RTItemSlot));
			*Count += 1;
			
			if (*Count >= Length) {
				return;
			}
		}
	}
}

Bool RTInventoryCanConsumeStackableItems(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	UInt32 RequiredItemID,
	Int64 RequiredItemCount,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
) {
	Int64 ConsumableItemCount = 0;
	for (Index Index = 0; Index < InventorySlotCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, Inventory, InventorySlotIndex[Index]);
		if (!ItemSlot) return false;
		if (ItemSlot->Item.ID != RequiredItemID) return false;

		ConsumableItemCount += ItemSlot->ItemOptions;
	}

	return ConsumableItemCount >= RequiredItemCount;
}

Void RTInventoryConsumeStackableItems(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	UInt32 RequiredItemID,
	Int64 RequiredItemCount,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
) {
	Int64 RemainingItemCount = RequiredItemCount;
	for (Index Index = 0; Index < InventorySlotCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, Inventory, InventorySlotIndex[Index]);
		assert(ItemSlot);
		assert(ItemSlot->Item.ID == RequiredItemID);

		Int64 ConsumeItemCount = MIN(RemainingItemCount, ItemSlot->ItemOptions);
		ItemSlot->ItemOptions -= ConsumeItemCount;
		if (ItemSlot->ItemOptions < 1) {
			RTInventoryClearSlot(Runtime, Inventory, ItemSlot->SlotIndex);
		}

		RemainingItemCount -= ConsumeItemCount;
	}

	assert(RemainingItemCount < 1);
}
