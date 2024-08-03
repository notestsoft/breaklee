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

struct _RTTranscendenceInfo {
    Int32 Points;
    Int32 Unknown1;
    Int32 Unknown2;
    Int32 SlotCount;
};

struct _RTCharacterTranscendenceInfo {
    struct _RTTranscendenceInfo Info;
    struct _RTTranscendenceSlot Slots[RUNTIME_CHARACTER_MAX_TRANSCENDENCE_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END