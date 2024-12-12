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

Void RTCharacterInitializeCooldowns(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterUpdateCooldowns(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Bool ForceUpdate
);

Bool RTCharacterHasCooldown(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int CooldownIndex
);

Timestamp RTCharacterGetCooldownInterval(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int CooldownIndex
);

Void RTCharacterSetCooldown(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int CooldownIndex,
    Timestamp CooldownInterval,
    Bool IsNationWar
);

Bool RTCharacterHasSpiritRaiseCooldown(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterSetSpiritRaiseCooldown(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CooldownInterval
);

EXTERN_C_END