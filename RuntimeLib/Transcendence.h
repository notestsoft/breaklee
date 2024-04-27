#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTTranscendenceSlot {
    UInt16 SlotIndex;
    UInt16 Unknown1;
    UInt16 Level;
};

struct _RTCharacterTranscendenceInfo {
    Int32 Count;
    struct _RTTranscendenceSlot Slots[RUNTIME_CHARACTER_MAX_TRANSCENDENCE_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END