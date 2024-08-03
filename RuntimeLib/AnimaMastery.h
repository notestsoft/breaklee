#pragma once

#include "Base.h"
#include "Constants.h"

// TODO: Add support for new consumable items for reset, ...

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTAnimaMasteryInfo {
    Int32 PresetCount;
    Int32 StorageCount;
    UInt32 UnlockedCategoryFlags;
    Int32 Unknown2;
};

union _RTAnimaMasteryPresetSlot {
    struct {
        UInt8 CategoryIndex : 4;
        UInt8 StorageIndex : 4;
    };
};

typedef union _RTAnimaMasteryPresetSlot RTAnimaMasteryPresetSlot;

struct _RTAnimaMasteryPresetData {
    union _RTAnimaMasteryPresetSlot CategoryOrder[RUNTIME_MAX_ANIMA_MASTERY_CATEGORY_COUNT];
};

struct _RTAnimaMasteryCategoryData {
    UInt8 CategoryIndex;
    UInt8 StorageIndex;
    UInt8 MasterySlots[RUNTIME_MAX_ANIMA_MASTERY_SLOT_COUNT];
};

struct _RTCharacterAnimaMasteryInfo {
    struct _RTAnimaMasteryInfo Info;
    struct _RTAnimaMasteryPresetData PresetData[RUNTIME_MAX_ANIMA_MASTERY_PRESET_COUNT];
    struct _RTAnimaMasteryCategoryData CategoryData[RUNTIME_MAX_ANIMA_MASTERY_STORAGE_COUNT * RUNTIME_MAX_ANIMA_MASTERY_CATEGORY_COUNT];
};

#pragma pack(pop)

Void RTCharacterInitializeAnimaMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

UInt8 RTCharacterAnimaMasteryTrainSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 MasterySlotIndex,
    Int32 ConversionKitSlotIndex,
    Int32 MaterialSlotCount,
    UInt16* MaterialSlotIndex
);

Bool RTCharacterAnimaMasteryResetSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 StorageSlotIndex,
    Int32 InventorySlotIndex
);

Bool RTCharacterAnimaMasteryIsCategoryUnlocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex
);

Bool RTCharacterAnimaMasteryUnlockCategory(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex
);

RTAnimaMasteryCategoryDataRef RTCharacterAnimaMasteryGetCategoryData(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 StorageIndex,
    Int32 CategoryIndex
);

Bool RTCharacterAnimaMasterySetActiveStorageIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 StorageSlotIndex
);

Bool RTCharacterAnimaMasterySetActivePresetIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetIndex
);

EXTERN_C_END