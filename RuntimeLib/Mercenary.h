#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTMercenarySlot {
    UInt32 Index;
    UInt8 Rank;
    UInt8 Level;
};

struct _RTMercenaryInfo {
    UInt16 SlotCount;
};

struct _RTCharacterMercenaryInfo {
    struct _RTMercenaryInfo Info;
    struct _RTMercenarySlot Slots[RUNTIME_CHARACTER_MAX_MERCENARY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END