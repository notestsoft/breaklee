#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTQuickSlot {
    UInt16 SkillIndex;
    UInt16 SlotIndex;
};

struct _RTQuickSlotInfo {
    UInt16 SlotCount;
};

struct _RTCharacterQuickSlotInfo {
    struct _RTQuickSlotInfo Info;
    struct _RTQuickSlot Slots[RUNTIME_CHARACTER_MAX_QUICK_SLOT_COUNT];
};

#pragma pack(pop)

Bool RTCharacterAddQuickSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillIndex,
    Int32 SlotIndex
);

RTQuickSlotRef RTCharacterGetQuickSlotByIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
);

Bool RTCharacterSwapQuickSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SourceSlotIndex,
    Int32 TargetSlotIndex
);

Bool RTCharacterRemoveQuickSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
);

EXTERN_C_END