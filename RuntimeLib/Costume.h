#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTAccountCostumeSlot {
    UInt32 ItemID;
    UInt32 Unknown1;
};

struct _RTCostumePage {
    Int32 PageIndex;
    UInt32 CostumeSlots[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT];
};

struct _RTAppliedCostumeSlot {
    UInt32 ItemID;
};

struct _RTCharacterCostumeWarehouseInfo {
    Int32 AccountCostumeSlotCount;
    struct _RTAccountCostumeSlot AccountCostumeSlots[RUNTIME_CHARACTER_MAX_ACCOUNT_COSTUME_SLOT_COUNT];
};

struct _RTCharacterCostumeInfo {
    Int32 ActivePageIndex;
    Int32 PageCount;
    struct _RTCostumePage Pages[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_COUNT];
    Int32 AppliedSlotCount;
    struct _RTAppliedCostumeSlot AppliedSlots[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT];
};


#pragma pack(pop)

EXTERN_C_END