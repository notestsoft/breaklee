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
    Int32 MillagePoints;
};

struct _RTCharacterCostumeWarehouseInfo {
    struct _RTCostumeWarehouseInfo Info;
    struct _RTAccountCostumeSlot Slots[RUNTIME_CHARACTER_MAX_ACCOUNT_COSTUME_SLOT_COUNT];
};

struct _RTCostumeInfo {
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

Bool RTCharacterIsCostumeUnlocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 CostumeItemID
);

Bool RTCharacterUnlockCostume(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 CostumeItemID
);

RTCostumePageRef RTCharacterGetCostumePage(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetIndex
);

Bool RTCharacterSetCostumePreset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetIndex,
    UInt32* CostumeSlots
);

Bool RTCharacterSetActiveCostumePreset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetIndex
);

EXTERN_C_END