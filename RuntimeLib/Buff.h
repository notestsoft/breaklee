#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTSkillCooldownSlot {
    UInt16 SkillIndex;
    UInt32 Interval;
};

struct _RTBuffSlot {
    UInt16 Unknown1;
    UInt32 Unknown2;
};

struct _RTBuffInfo {
    UInt8 SkillCooldownCount;
    UInt16 Unknown2389;
    UInt16 BuffCount;
    UInt32 SpiritRaiseBuffCooldown;
};

struct _RTCharacterBuffInfo {
    struct _RTBuffInfo Info;
    struct _RTBuffSlot Slots[RUNTIME_CHARACTER_MAX_BUFF_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END