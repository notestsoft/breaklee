#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTGiftBoxSlot {
    UInt8 Index;
    UInt32 ReceivedCount;
    Timestamp ElapsedTime;
    Timestamp CooldownTime;
    UInt8 Active;
};

struct _RTCharacterGiftboxInfo {
    Int32 Count;
    struct _RTGiftBoxSlot Slots[RUNTIME_CHARACTER_MAX_GIFT_BOX_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END