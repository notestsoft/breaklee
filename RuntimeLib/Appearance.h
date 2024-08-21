#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTAppearanceInfo {
    Int8 EquipmentAppearanceCount;
    Int16 InventoryAppearanceCount;
};

struct _RTCharacterAppearanceData {
    struct _RTAppearanceInfo Info;
    struct _RTItemSlotAppearance EquipmentSlots[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT];
    struct _RTItemSlotAppearance InventorySlots[RUNTIME_INVENTORY_TOTAL_SIZE];
};

#pragma pack(pop)

EXTERN_C_END