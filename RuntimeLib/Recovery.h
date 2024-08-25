#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTRecoveryInfo {
    Int32 SlotCount;
};

struct _RTCharacterRecoveryInfo {
    struct _RTRecoveryInfo Info;
    UInt64 Prices[RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT];
    struct _RTItemSlot Slots[RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END