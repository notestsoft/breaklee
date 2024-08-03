#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTAuraMasterySlot {
    Int8 Category;
    Int8 SlotIndex;
    Int8 Unknown1;
    Int8 Level;
};

struct _RTAuraMasteryInfo {
    Int32 Points;
    Int32 AccumulatedTimeInMinutes;
    Int8 SlotCount;
};

struct _RTCharacterAuraMasteryData {
    struct _RTAuraMasteryInfo Info;
    struct _RTAuraMasterySlot Slots[RUNTIME_CHARACTER_MAX_AURA_MASTERY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END