#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTTransformSlot {
    UInt8 TransformIndex;
};

struct _RTCharacterTransformInfo {
    Int32 Count;
    struct _RTTransformSlot Slots[RUNTIME_CHARACTER_MAX_TRANSFORM_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END