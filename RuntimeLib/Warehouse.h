#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterWarehouseInfo {
	UInt16 Count;
	UInt64 Alz;
	struct _RTWarehouseItemSlot Slots[RUNTIME_WAREHOUSE_SLOT_COUNT];
};;

#pragma pack(pop)

EXTERN_C_END