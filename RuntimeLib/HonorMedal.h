#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTHonorMedalSlot {
    UInt8 CategoryIndex;
    UInt8 GroupIndex;
    UInt32 SlotIndex;
    UInt32 ForceEffectIndex;
    UInt8 IsUnlocked;
};

struct _RTHonorMedalInfo {
    Int32 Grade;
    Int32 Score;
    Int8 SlotCount;
};

struct _RTCharacterHonorMedalInfo {
    struct _RTHonorMedalInfo Info;
    struct _RTHonorMedalSlot Slots[RUNTIME_CHARACTER_MAX_HONOR_MEDAL_SLOT_COUNT];
};

#pragma pack(pop)

Bool RTCharacterIsHonorMedalUnlocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex
);

Bool RTCharacterAddHonorMedalScore(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 Score
);

Int32 RTCharacterGetHonorMedalGrade(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex
);

Int32 RTCharacterGetHonorMedalSlotCount(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 GroupIndex
);

Bool RTCharacterCanAddHonorMedalSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 GroupIndex,
    Int32 SlotIndex
);

Void RTCharacterAddHonorMedalSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 GroupIndex,
    Int32 SlotIndex
);

RTHonorMedalSlotRef RTCharacterGetHonorMedalSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 GroupIndex,
    Int32 SlotIndex
);

EXTERN_C_END