#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCraftSlot {
    UInt8 SlotIndex;
    UInt8 CraftIndex;
    Int16 CraftLevel;
    Int16 Unknown1;
};

struct _RTCraftInfo {
    Int32 Unknown1;
    Int32 SlotCount;
    UInt8 Unknown2;
    Int32 Energy;
    UInt8 Unknown3[8];
};

struct _RTCharacterCraftData {
    struct _RTCraftInfo Info;
    struct _RTCraftSlot Slots[RUNTIME_CHARACTER_MAX_CRAFT_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END