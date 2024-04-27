#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTBlessingBeadSlot {
    UInt32 Index;
    UInt32 Unknown1;
    UInt32 Unknown2;
};

struct _RTCharacterBlessingBeadInfo {
    Int32 Count;
    struct _RTBlessingBeadSlot Slots[RUNTIME_CHARACTER_MAX_BLESSING_BEAD_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END