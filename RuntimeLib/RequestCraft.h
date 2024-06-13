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

struct _RTCharacterRequestCraftInfo {
    Int32 RequestCraftCount;
    UInt16 RequestCraftExp;
    UInt8 RequestCraftFlags[1024];
    UInt8 RequestCraftFavoriteFlags[1024];
    UInt16 RequestCraftSortOrder;
    struct _RTRequestCraftSlot RequestCraftSlot[RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END