#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterEquipmentAppearanceInfo {
    Int32 Count;
    struct _RTItemSlotAppearance Slots[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT];
};

struct _RTCharacterInventoryAppearanceInfo {
    Int32 Count;
    struct _RTItemSlotAppearance Slots[RUNTIME_INVENTORY_TOTAL_SIZE];
};

#pragma pack(pop)

EXTERN_C_END