#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTStellarMasterySlot {
    UInt8 Data[16];
};

struct _RTCharacterStellarMasteryInfo {
    Int32 Count;
    struct _RTStellarMasterySlot Slots[RUNTIME_CHARACTER_MAX_STELLAR_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END