#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTBlendedAbilitySlot {
    UInt32 AbilityID;
    UInt32 Level;
    UInt32 Unknown1;
};

struct _RTCharacterBlendedAbilityInfo {
    Int32 Count;
    struct _RTBlendedAbilitySlot Slots[RUNTIME_CHARACTER_MAX_BLENDED_ABILITY_SLOT_COUNT];
};

#pragma pack(pop)

Bool RTCharacterAddBlendedAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 InventorySlotIndex,
    UInt16* MaterialSlotIndex,
    Int32 MaterialSlotCount
);

Bool RTCharacterUpgradeBlendedAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 AbilityID,
    UInt32 InventorySlotCount,
    UInt16* InventorySlotIndices
);

Bool RTCharacterRemoveBlendedAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 AbilityID
);

EXTERN_C_END
