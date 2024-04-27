#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTMercenarySlot {
    UInt32 SlotIndex;
    UInt8 Unknown1;
    UInt8 Unknown2;
};

struct _RTCharacterMercenaryInfo {
    Int32 Count;
    struct _RTMercenarySlot Slots[RUNTIME_CHARACTER_MAX_MERCENARY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END