#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterVehicleInventoryInfo {
	UInt32 Count;
	struct _RTItemSlot Slots[RUNTIME_CHARACTER_MAX_VEHICLE_INVENTORY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END
