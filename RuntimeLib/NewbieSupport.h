#pragma once

#include "Base.h"

#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTNewbieSupportSlot {
    UInt8 CategoryType;
    UInt8 ConditionValue1;
    UInt8 ConditionValue2;
    UInt8 RewardIndex;
};

struct _RTNewbieSupportInfo {
    Timestamp Timestamp;
    Int32 SlotCount;
};

struct _RTCharacterNewbieSupportInfo {
    struct _RTNewbieSupportInfo Info;
    struct _RTNewbieSupportSlot Slots[RUNTIME_CHARACTER_MAX_NEWBIE_SUPPORT_SLOT_COUNT];
};

#pragma pack(pop)

Bool RTCharacterCanTakenNewbieSupportReward(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 CategoryType,
    UInt8 RewardIndex,
    UInt8 ConditionValue1,
    UInt8 ConditionValue2
);

Bool RTCharacterTakeNewbieSupportReward(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 CategoryType,
    UInt8 RewardIndex,
    UInt8 ConditionValue1,
    UInt8 ConditionValue2,
    Int32 InventorySlotCount,
    UInt16* InventorySlotIndex
);

EXTERN_C_END