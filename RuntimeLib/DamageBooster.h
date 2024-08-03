#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTDamageBoosterSlot {
    UInt32 ItemID;
    Int32 Count;
};

struct _RTDamageBoosterInfo {
    Int8 Unknown1;
    UInt32 ItemID[RUNTIME_CHARACTER_MAX_DAMAGE_BOOSTER_SLOT_COUNT];
    Int8 SlotCount;
};

struct _RTCharacterDamageBoosterData {
    struct _RTDamageBoosterInfo Info;
    struct _RTDamageBoosterSlot Slots[RUNTIME_CHARACTER_MAX_DAMAGE_BOOSTER_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END