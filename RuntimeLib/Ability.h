#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
    RUNTIME_ABILITY_TYPE_ESSENCE = 1,
    RUNTIME_ABILITY_TYPE_BLENDED = 2,
    RUNTIME_ABILITY_TYPE_KARMA = 3,
};

struct _RTEssenceAbilitySlot {
    UInt32 AbilityID;
    UInt16 Level;
    UInt16 Unknown1;
};

struct _RTCharacterEssenceAbilityInfo {
    Int32 Count;
};

struct _RTBlendedAbilitySlot {
    UInt32 AbilityID;
    UInt32 Level;
    UInt32 Unknown1;
};

struct _RTKarmaAbilitySlot {
    UInt32 AbilityID;
    Int16 Level;
};

struct _RTAbilityInfo {
    UInt16 APTotal;
    UInt16 AP;
    UInt32 Axp;
    UInt8 EssenceAbilityCount;
    UInt8 ExtendedEssenceAbilityCount;
    UInt8 BlendedAbilityCount;
    UInt8 ExtendedBlendedAbilityCount;
    UInt8 KarmaAbilityCount;
    UInt8 ExtendedKarmaAbilityCount;
};

struct _RTCharacterAbilityInfo {
    struct _RTAbilityInfo Info;
    struct _RTEssenceAbilitySlot EssenceAbilitySlots[RUNTIME_CHARACTER_MAX_ESSENCE_ABILITY_SLOT_COUNT];
    struct _RTBlendedAbilitySlot BlendedAbilitySlots[RUNTIME_CHARACTER_MAX_BLENDED_ABILITY_SLOT_COUNT];
    struct _RTKarmaAbilitySlot KarmaAbilitySlots[RUNTIME_CHARACTER_MAX_KARMA_ABILITY_SLOT_COUNT];
};

#pragma pack(pop)

Void RTCharacterDataEncodeAbility(
    RTCharacterDataRef CharacterData,
    MemoryBufferRef MemoryBuffer
);

Void RTCharacterDataDecodeAbility(
    RTCharacterDataRef CharacterData,
    MemoryBufferRef MemoryBuffer
);

RTEssenceAbilitySlotRef RTCharacterGetEssenceAbilitySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 AbilityID
);

Bool RTCharacterAddEssenceAbility(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 InventorySlotIndex,
    UInt16* MaterialSlotIndex,
    Int32 MaterialSlotCount
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
