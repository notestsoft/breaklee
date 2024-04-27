#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTKarmaAbilitySlot {
    UInt32 AbilityID;
    UInt16 Level;
    UInt16 Unknown1;
};

struct _RTCharacterKarmaAbilityInfo {
    Int32 Count;
    struct _RTKarmaAbilitySlot Slots[RUNTIME_CHARACTER_MAX_KARMA_ABILITY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END
