#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTBlendedAbilitySlot {
    UInt32 AbilityID;
    UInt16 Level;
    UInt16 Unknown1;
};

struct _RTCharacterBlendedAbilityInfo {
    Int32 Count;
    struct _RTBlendedAbilitySlot Slots[RUNTIME_CHARACTER_MAX_BLENDED_ABILITY_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END
