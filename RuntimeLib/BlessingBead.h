#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTBlessingBeadSlot {
    UInt32 Index;
    UInt32 Value;
    RTItemDuration Duration;
};

struct _RTBlessingBeadInfo {
    UInt8 SlotCount;
};

struct _RTCharacterBlessingBeadInfo {
    struct _RTBlessingBeadInfo Info;
    struct _RTBlessingBeadSlot Slots[RUNTIME_CHARACTER_MAX_BLESSING_BEAD_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END