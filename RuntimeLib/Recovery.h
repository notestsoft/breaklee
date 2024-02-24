#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterRecoveryInfo {
    UInt32 Count;
    UInt64 Prices[RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT];
    struct _RTItemSlot Slots[RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END