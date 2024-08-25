#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTWarehouseInfo {
    UInt16 SlotCount;
    UInt64 Currency;
};

struct _RTCharacterWarehouseInfo {
    struct _RTWarehouseInfo Info;
	struct _RTItemSlot Slots[RUNTIME_WAREHOUSE_TOTAL_SIZE];
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