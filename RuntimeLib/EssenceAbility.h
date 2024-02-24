#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTEssenceAbilitySlot {
    UInt32 AbilityID;
    UInt16 Level;
    UInt16 Unknown1;
};

struct _RTCharacterEssenceAbilityInfo {
    Int32 Count;
    struct _RTEssenceAbilitySlot Slots[RUNTIME_CHARACTER_MAX_ESSENCE_ABILITY_SLOT_COUNT];
};

#pragma pack(pop)

RTEssenceAbilitySlotRef RTCharacterGetEssenceAbilitySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 AbilityID
);

Bool RTCharacterAddEssenceAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 InventorySlotIndex
);

Bool RTCharacterUpgradeEssenceAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 AbilityID,
    UInt32 InventorySlotCount,
    UInt16* InventorySlotIndices
);

Bool RTCharacterRemoveEssenceAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 AbilityID
);

Void RTCharacterApplyEssenceAbilityAttributes(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

EXTERN_C_END
