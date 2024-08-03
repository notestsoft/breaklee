#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTVehicleInventoryInfo {
	Int16 SlotCount;
};

struct _RTCharacterVehicleInventoryInfo {
	struct _RTVehicleInventoryInfo Info;
	struct _RTItemSlot Slots[RUNTIME_CHARACTER_MAX_VEHICLE_INVENTORY_SLOT_COUNT];
};

#pragma pack(pop)

Int32 RTVehicleInventoryGetSlotIndex(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	Int32 SlotIndex
);

RTItemSlotRef RTVehicleInventoryGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	Int32 SlotIndex
);

Bool RTVehicleInventorySetSlot(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	RTItemSlotRef Slot
);

Bool RTVehicleInventoryRemoveSlot(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	Int32 SlotIndex,
	RTItemSlotRef Result
);

Bool RTVehicleInventoryClearSlot(
	RTRuntimeRef Runtime,
	RTCharacterVehicleInventoryInfoRef Inventory,
	Int32 SlotIndex
);

EXTERN_C_END
