#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterWarehouseInfo {
	UInt16 Count;
	UInt64 Alz;
	struct _RTItemSlot Slots[RUNTIME_WAREHOUSE_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END

Bool RTWarehouseSetSlot(
    RTRuntimeRef Runtime,
    RTCharacterWarehouseInfoRef Warehouse,
    RTItemSlotRef Slot
);

Int32 RTWarehouseGetSlotIndex(
    RTRuntimeRef Runtime,
    RTCharacterWarehouseInfoRef Warehouse,
    Int32 SlotIndex
);

RTItemSlotRef RTWarehouseGetSlot(
    RTRuntimeRef Runtime,
    RTCharacterWarehouseInfoRef Warehouse,
    Int32 SlotIndex
);

Int32 RTWarehouseGetInsertionIndex(
    RTRuntimeRef Runtime,
    RTCharacterWarehouseInfoRef Warehouse,
    Int32 SlotIndex
);

Bool RTWarehouseRemoveSlot(
    RTRuntimeRef Runtime,
    RTCharacterWarehouseInfoRef Warehouse,
    Int32 SlotIndex,
    RTItemSlotRef Result
);

Bool RTWarehouseClearSlot(
    RTRuntimeRef Runtime,
    RTCharacterWarehouseInfoRef Warehouse,
    Int32 SlotIndex
);