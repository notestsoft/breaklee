#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTTransformSlot {
    UInt8 TransformIndex;
};

struct _RTTransformInfo {
    UInt16 SlotCount;
};

struct _RTCharacterTransformInfo {
    struct _RTTransformInfo Info;
    struct _RTTransformSlot Slots[RUNTIME_CHARACTER_MAX_TRANSFORM_SLOT_COUNT];
};

#pragma pack(pop)

Bool RTCharacterTransformIsLocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 TransformIndex
);

Bool RTCharacterTransformUnlock(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 TransformIndex
);

EXTERN_C_END