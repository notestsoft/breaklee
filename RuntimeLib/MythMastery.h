#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTMythMasterySlot {
    UInt8 Data[16];
};

struct _RTCharacterMythMasteryInfo {
    Int32 Count;
    struct _RTMythMasterySlot Slots[RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END