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

struct _RTCostumeWarehouseInfo {
    Int32 SlotCount;
};

struct _RTCharacterCostumeWarehouseInfo {
    struct _RTCostumeWarehouseInfo Info;
    struct _RTAccountCostumeSlot Slots[RUNTIME_CHARACTER_MAX_ACCOUNT_COSTUME_SLOT_COUNT];
};

struct _RTCostumeInfo {
    Int32 Unknown1;
    Int32 PageCount;
    Int32 AppliedSlotCount;
    Int32 ActivePageIndex;
};

struct _RTCharacterCostumeInfo {
    struct _RTCostumeInfo Info;
    struct _RTCostumePage Pages[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_COUNT];
    struct _RTAppliedCostumeSlot AppliedSlots[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END