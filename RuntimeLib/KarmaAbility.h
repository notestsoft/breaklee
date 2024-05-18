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

Bool RTCharacterAddKarmaAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 InventorySlotIndex,
    UInt16* MaterialSlotIndex,
    Int32 MaterialSlotCount
);

Bool RTCharacterUpgradeKarmaAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 AbilityID,
    UInt32 InventorySlotCount,
    UInt16* InventorySlotIndices
);

Bool RTCharacterRemoveKarmaAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 AbilityID
);

EXTERN_C_END
