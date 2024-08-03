#pragma once

#include "Base.h"
#include "Constants.h"

#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTInventoryInfo {
	UInt16 SlotCount;
};

struct _RTCharacterInventoryInfo {
	struct _RTInventoryInfo Info;
	struct _RTItemSlot Slots[RUNTIME_INVENTORY_PAGE_SIZE * RUNTIME_INVENTORY_PAGE_COUNT];
};

#pragma pack(pop)

Int32 RTInventoryGetNextFreeSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory
);

Void RTInventorySort(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory
);

// NOTE: This function assumes that the inventory is already sorted by slot index!
Bool RTInventoryInsertSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	RTItemSlotRef Slot
);

Bool RTInventoryIsSlotEmpty(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
);

Int32 RTInventoryGetSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
);

RTItemSlotRef RTInventoryGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
);

Bool RTInventorySetSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	RTItemSlotRef Slot
);

Bool RTInventoryClearSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex
);

Bool RTInventoryRemoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 SlotIndex,
	RTItemSlotRef Result
);

Bool RTInventoryMoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef SourceInventory,
	RTCharacterInventoryInfoRef TargetInventory,
	Int32 SourceSlotIndex,
	Int32 TargetSlotIndex
);

Void RTInventoryFindItems(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	Int32 ItemID,
	Int32 Length,
	Int32* Count,
	RTItemSlotRef Results
);

Bool RTInventoryCanConsumeStackableItems(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	UInt64 RequiredItemID,
	Int64 RequiredItemCount,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
);

Void RTInventoryConsumeStackableItems(
	RTRuntimeRef Runtime,
	RTCharacterInventoryInfoRef Inventory,
	UInt64 RequiredItemID,
	Int64 RequiredItemCount,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
);

EXTERN_C_END
