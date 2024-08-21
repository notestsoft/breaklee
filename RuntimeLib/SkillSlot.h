#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTSkillSlot {
    UInt16 ID;
    UInt8 Level;
    UInt16 Index;
};

struct _RTSkillSlotInfo {
    UInt16 SlotCount;
};

struct _RTCharacterSkillSlotInfo {
    struct _RTSkillSlotInfo Info;
    struct _RTSkillSlot Slots[RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT];
};

#pragma pack(pop)

RTSkillSlotRef RTCharacterAddSkillSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillID,
    Int32 Level,
    Int32 SlotIndex
);

RTSkillSlotRef RTCharacterGetSkillSlotByIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
);

Void RTCharacterRemoveSkillSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillID,
    Int32 SlotIndex
);

Bool RTCharacterChangeSkillLevel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillID,
    Int32 SlotIndex,
    Int32 CurrentSkillLevel,
    Int32 TargetSkillLevel
);

EXTERN_C_END