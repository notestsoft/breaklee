#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTGoldMeritMasterySlot {
    UInt32 Index;
    UInt32 Level;
};

struct _RTCharacterGoldMeritMasteryInfo {
    Int32 Count;
    struct _RTGoldMeritMasterySlot Slots[RUNTIME_CHARACTER_MAX_MERIT_MASTERY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END
