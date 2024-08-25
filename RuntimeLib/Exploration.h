#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTExplorationSlot {
    Int32 Unknown1;
    Int8 Unknown2;
    Int32 Unknown3;
    Int8 IsRewardTaken;
};

struct _RTExplorationInfo {
    Timestamp EndDate;
    Int32 Points;
    Int32 Level;
    Int32 SlotCount;
};

struct _RTCharacterExplorationData {
    struct _RTExplorationInfo Info;
    struct _RTExplorationSlot Slots[RUNTIME_CHARACTER_MAX_EXPLORATION_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END