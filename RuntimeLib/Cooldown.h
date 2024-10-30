#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCooldownSlot {
    UInt16 CooldownIndex;
    UInt32 Interval;
};

struct _RTCooldownInfo {
    UInt16 SlotCount;
    Int32 SpiritRaiseCooldown;
};

struct _RTCharacterCooldownInfo {
    struct _RTCooldownInfo Info;
    struct _RTCooldownSlot Slots[RUNTIME_CHARACTER_MAX_COOLDOWN_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END