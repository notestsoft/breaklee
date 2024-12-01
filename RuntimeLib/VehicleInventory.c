#include "Runtime.h"
#include "VehicleInventory.h"

Int32 RTVehicleInventoryGetSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	for (Int Index = 0; Index < Inventory->Info.SlotCount; Index += 1) {
		RTItemSlotRef Slot = &Inventory->Slots[Index];
		if (Slot->SlotIndex == SlotIndex) {
			return Index;
		}
	}

	return -1;
}

RTItemSlotRef RTVehicleInventoryGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	Int Index = RTVehicleInventoryGetSlotIndex(Runtime, Inventory, SlotIndex);
	if (Index < 0) return NULL;
	return &Inventory->Slots[Index];
}

Bool RTVehicleInventorySetSlot(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	RTItemSlotRef Slot
) {
	Info("VehicleInventorySetSlot(%llu, %llu, %d, %d)", Slot->Item.Serial, Slot->ItemOptions, Slot->ItemDuration, Slot->SlotIndex);
	assert(Inventory);
	assert(0 <= Slot->SlotIndex && Slot->SlotIndex < RUNTIME_CHARACTER_MAX_VEHICLE_INVENTORY_SLOT_COUNT);

	assert(RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID));

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID);
	assert(ItemData);

	RTItemSlotRef InventorySlot = RTVehicleInventoryGetSlot(Runtime, Inventory, Slot->SlotIndex);
	if (InventorySlot) {
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

	InventorySlot = &Inventory->Slots[Inventory->Info.SlotCount];
	memcpy(InventorySlot, Slot, sizeof(struct _RTItemSlot));
	Inventory->Info.SlotCount += 1;
	return true;
}

Bool RTVehicleInventoryRemoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	Int32 SlotIndex,
	RTItemSlotRef Result
) {
	Info("VehicleInventoryRemoveSlot(%d)", SlotIndex);
	RTItemSlotRef Slot = RTVehicleInventoryGetSlot(Runtime, Inventory, SlotIndex);
	if (!Slot)
		return false;

	memcpy(Result, Slot, sizeof(struct _RTItemSlot));

	return RTVehicleInventoryClearSlot(Runtime, Inventory, SlotIndex);
}

Bool RTVehicleInventoryClearSlot(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	Int32 SlotIndex
) {
	Info("VehicleInventoryClearSlot(%d)", SlotIndex);
	assert(0 <= SlotIndex && SlotIndex < RUNTIME_INVENTORY_TOTAL_SIZE);

	Int32 InventoryIndex = RTVehicleInventoryGetSlotIndex(
		Runtime,
		Inventory,
		SlotIndex
	);
	if (InventoryIndex < 0) return false;

	Int32 TailLength = Inventory->Info.SlotCount - InventoryIndex - 1;
	if (TailLength > 0) {
		memmove(
			&Inventory->Slots[InventoryIndex],
			&Inventory->Slots[InventoryIndex + 1],
			TailLength * sizeof(struct _RTItemSlot)
		);
	}

	Inventory->Info.SlotCount -= 1;
	return true;
}
