#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTWarpServiceInfo {
    Int32 SlotCount;
};

struct _RTWarpServiceSlot {
    Int32 Type;

    union {
        // Warp Service Type 2
        struct { UInt32 Serial; };
        // Warp Service Type 5
        struct { UInt8 X; UInt8 Y; UInt8 WorldIndex; UInt8 _0; };
    } Data;
};

struct _RTCharacterWarpServiceInfo {
    struct _RTWarpServiceInfo Info;
    struct _RTWarpServiceSlot Slots[RUNTIME_CHARACTER_MAX_WARP_SERVICE_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END