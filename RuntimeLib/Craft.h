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
    UInt8 SlotCount;
    UInt32 Unknown1;
    Int32 Energy;
    UInt64 ChargingTimestamp;
};

struct _RTCharacterCraftData {
    struct _RTCraftInfo Info;
    struct _RTCraftSlot Slots[RUNTIME_CHARACTER_MAX_CRAFT_SLOT_COUNT];
};

#pragma pack(pop)

Bool RTCharacterIsCraftSlotEmpty(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 SlotIndex
);

Bool RTCharacterIsCraftCategoryRegistered(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 Category
);

EXTERN_C_END