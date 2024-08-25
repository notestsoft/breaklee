#include "Constants.h"
#include "Character.h"
#include "Inventory.h"
#include "Runtime.h"

Bool RTWarehouseSetSlot(
	RTRuntimeRef Runtime,
	RTCharacterWarehouseInfoRef Warehouse,
	RTItemSlotRef Slot
) {
	assert(0 <= Slot->SlotIndex && Slot->SlotIndex < RUNTIME_WAREHOUSE_TOTAL_SIZE);
	assert(RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID));

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID);
	assert(ItemData);

	Int32 Index = RTWarehouseGetSlotIndex(
		Runtime,
		Warehouse,
		Slot->SlotIndex
	);

	if (Index >= 0) {
		RTItemSlotRef InventorySlot = RTWarehouseGetSlot(Runtime, Warehouse, Slot->SlotIndex);
		assert(InventorySlot);

		if (Slot->Item.ID != InventorySlot->Item.ID) return false;

		if (ItemData->ItemType == RUNTIME_ITEM_TYPE_QUEST_S) {
			UInt64 ItemOptions = RTQuestItemGetOptions(Slot->ItemOptions);
			UInt64 InventoryItemOptions = RTQuestItemGetOptions(InventorySlot->ItemOptions);

			if (ItemOptions != InventoryItemOptions) return false;

			UInt64 ItemCount = RTQuestItemGetCount(Slot->ItemOptions);
			UInt64 InventoryItemCount = RTQuestItemGetCount(InventorySlot->ItemOptions);

			InventorySlot->ItemOptions = RTQuestItemOptions(ItemOptions, (ItemCount + InventoryItemCount));
			memcpy(Slot, InventorySlot, sizeof(struct _RTItemSlot));
			return true;
		}

		return false;
	}

	Int32 InsertionIndex = RTWarehouseGetInsertionIndex(Runtime, Warehouse, Slot->SlotIndex);
	Int32 InsertionTailLength = Warehouse->Info.SlotCount - InsertionIndex;
	if (InsertionTailLength > 0) {
		memmove(
			&Warehouse->Slots[InsertionIndex + 1],
			&Warehouse->Slots[InsertionIndex],
			InsertionTailLength * sizeof(struct _RTItemSlot)
		);
	}

	RTItemSlotRef InventorySlot = &Warehouse->Slots[InsertionIndex];
	memcpy(InventorySlot, Slot, sizeof(struct _RTItemSlot));
	Warehouse->Info.SlotCount += 1;
	return true;
}

Int32 RTWarehouseGetSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterWarehouseInfoRef Warehouse,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Warehouse->Info.SlotCount; Index++) {
		RTItemSlotRef Slot = &Warehouse->Slots[Index];
		if (Slot->SlotIndex == SlotIndex) {
			return Index;
		}
	}

	return -1;
}

RTItemSlotRef RTWarehouseGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterWarehouseInfoRef Warehouse,
	Int32 SlotIndex
) {
	Int32 Index = RTWarehouseGetSlotIndex(Runtime, Warehouse, SlotIndex);
	if (Index < 0) return NULL;
	return &Warehouse->Slots[Index];
}

Int32 RTWarehouseGetInsertionIndex(
	RTRuntimeRef Runtime,
	RTCharacterWarehouseInfoRef Warehouse,
	Int32 SlotIndex
) {
	for (Int32 Index = 0; Index < Warehouse->Info.SlotCount; Index += 1) {
		RTItemSlotRef InventorySlot = &Warehouse->Slots[Index];
		if (InventorySlot->SlotIndex > SlotIndex) {
			return Index;
		}
	}

	return Warehouse->Info.SlotCount;
}


Bool RTWarehouseRemoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterWarehouseInfoRef Warehouse,
	Int32 SlotIndex,
	RTItemSlotRef Result
) {
	RTItemSlotRef Slot = RTWarehouseGetSlot(Runtime, Warehouse, SlotIndex);
	if (!Slot) return false;

	memcpy(Result, Slot, sizeof(struct _RTItemSlot));

	return RTWarehouseClearSlot(Runtime, Warehouse, SlotIndex);
}

Bool RTWarehouseClearSlot(
	RTRuntimeRef Runtime,
	RTCharacterWarehouseInfoRef Warehouse,
	Int32 SlotIndex
) {
	assert(0 <= SlotIndex && SlotIndex < RUNTIME_WAREHOUSE_TOTAL_SIZE);

	Int32 WarehouseIndex = RTWarehouseGetSlotIndex(
		Runtime,
		Warehouse,
		SlotIndex
	);
	if (WarehouseIndex < 0) return false;

	Int32 TailLength = Warehouse->Info.SlotCount - WarehouseIndex - 1;
	if (TailLength > 0) {
		memmove(
			&Warehouse->Slots[WarehouseIndex],
			&Warehouse->Slots[WarehouseIndex + 1],
			TailLength * sizeof(struct _RTItemSlot)
		);
	}

	Warehouse->Info.SlotCount -= 1;
	return true;
}