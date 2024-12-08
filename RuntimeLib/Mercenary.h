#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTMercenarySlot {
    Int32 Index;
    UInt8 Rank;
    UInt8 Level;
};

struct _RTMercenaryInfo {
    Int32 SlotCount;
};

struct _RTCharacterMercenaryInfo {
    struct _RTMercenaryInfo Info;
    struct _RTMercenarySlot Slots[RUNTIME_CHARACTER_MAX_MERCENARY_SLOT_COUNT];
};

#pragma pack(pop)

Bool RTCharacterRegisterMercenaryCard(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt16 InventorySlotIndex
);

EXTERN_C_END