#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTHonorMedalSlot {
    UInt8 Unknown1;
    UInt8 GroupIndex;
    UInt32 SlotIndex;
    UInt32 ForceEffectIndex;
    UInt8 Unknown2;
};

struct _RTCharacterHonorMedalInfo {
    Int32 Grade;
    Int32 Score;
    Int32 SlotCount;
    struct _RTHonorMedalSlot Slots[RUNTIME_CHARACTER_MAX_HONOR_MEDAL_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END