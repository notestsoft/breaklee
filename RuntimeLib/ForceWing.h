#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTForceWingArrivalSkillSlot {
    UInt8 SlotIndex;
    UInt32 ForceEffectIndex[RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_STATS_COUNT];
    UInt8 ForceEffectGrade[RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_STATS_COUNT];
};

struct _RTForceWingPresetSlot {
    UInt8 PresetPageIndex;
    UInt8 SlotIndex;
    UInt8 TrainingIndex;
    UInt8 TrainingLevel;
};

struct _RTForceWingTrainingSlot {
    UInt8 PresetPageIndex;
    UInt8 SlotIndex;
    UInt8 TrainingIndex;
};

struct _RTForceWingInfo {
    UInt8 Grade;
    UInt8 Level;
    Int64 Exp;
    Int32 RegisteredMaterialCount[2];
    UInt8 ActivePresetIndex;
    UInt8 PresetEnabled[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT];
    Int32 PresetTrainingPointCount[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT];
    UInt8 PresetSlotCount;
    UInt8 TrainingSlotCount;
    struct _RTForceWingArrivalSkillSlot ArrivalSkillSlots[RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_SKILL_COUNT];
    struct _RTForceWingArrivalSkillSlot ArrivalSkillRestoreSlot;
    UInt8 TrainingUnlockFlags[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_SIZE];
};

struct _RTCharacterForceWingInfo {
    struct _RTForceWingInfo Info;
    struct _RTForceWingPresetSlot PresetSlots[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_SLOT_COUNT];
    struct _RTForceWingTrainingSlot TrainingSlots[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_SLOT_COUNT];
};

#pragma pack(pop)

Bool RTCharacterEnableForceWing(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterForceWingLevelUp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 ItemStackCount1,
    UInt16 InventorySlotCount1,
    UInt16* InventorySlotIndex1,
    Int32 ItemStackCount2,
    UInt16 InventorySlotCount2,
    UInt16* InventorySlotIndex2
);

Bool RTCharacterForceWingSetActivePreset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetPageIndex
);

Bool RTCharacterForceWingSetPresetTraining(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetPageIndex,
    Int32 PresetSlotIndex,
    Int32 TrainingSlotIndex
);

Bool RTCharacterForceWingAddTrainingLevel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetPageIndex,
    Int32 PresetSlotIndex,
    Int32 TrainingSlotIndex,
    Int32 AddedTrainingLevelCount,
    UInt8* TargetTrainingLevel
);

Bool RTCharacterForceWingRollArrivalSkill(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 SkillSlotIndex,
    UInt16 InventorySlotCount,
    UInt16* InventorySlotIndex
);

Bool RTCharacterForceWingChangeArrivalSkill(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

EXTERN_C_END
