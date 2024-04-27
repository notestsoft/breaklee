#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTBuffSlot {
    UInt16 Unknown1;
    UInt32 Unknown2;
};

struct _RTCharacterBuffInfo {
    Int32 Count;
    struct _RTBuffSlot Slots[RUNTIME_CHARACTER_MAX_BUFF_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END