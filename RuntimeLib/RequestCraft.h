#pragma once

#include "Base.h"
#include "Constants.h"

#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTRequestCraftSlot {
    UInt8 SlotIndex;
    UInt32 RequestCode;
    Timestamp Timestamp;
    UInt8 Result;
};

struct _RTRequestCraftInfo {
    UInt8 SlotCount;
    UInt16 Exp;
    UInt8 Unknown1;
    UInt8 RegisteredFlags[1024];
    UInt8 FavoriteFlags[1024];
    UInt16 SortingOrder;
};

struct _RTCharacterRequestCraftInfo {
    struct _RTRequestCraftInfo Info;
    struct _RTRequestCraftSlot Slots[RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END