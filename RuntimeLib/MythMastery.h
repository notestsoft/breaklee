#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTMythMasterySlot {
    UInt8 MasteryIndex;
    UInt8 SlotIndex;
    UInt8 Tier; //related to group index in UI
    UInt8 Grade;
    UInt32 StatOption;
    UInt32 StatValue;
    UInt32 ValueType;
};

struct _RTMythMasteryInfo {
    Int32 Rebirth;
    Int32 HolyPower;
    Int32 Level;
    UInt64 Exp;
    Int32 Points;
    Int32 UnlockedPageCount;
    UInt8 Unknown1[13];
    UInt8 PropertySlotCount;
    Int32 StigmaGrade;
    Int32 StigmaExp;
};

struct _RTCharacterMythMasteryInfo {
    struct _RTMythMasteryInfo Info;
    struct _RTMythMasterySlot Slots[RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT];
};

#pragma pack(pop)

Float32 GetMythSlotValueChance(
    RTDataMythMasterySlotRef MythSlot
);

Void RTCharacterMythMasteryEnable(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterMythMasteryAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt64 Exp
);

Void RTCharacterMythMasteryAddMythLevel(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 LevelsGiven
);

Void RTCharacterMythMasteryAddMythPoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PointsGiven
);

Float RTCharacterMythMasteryGetExpPenaltyMultiplier(
    RTRuntimeRef Runtime,
    Int32 CharacterRebirthCount
);

Int32 RTCharacterMythMasteryGetRepeatBonus(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterMythMasteryCheckEligibleForRepeatBonus(
    RTRuntimeRef Runtime,
    Int32 MythLevel
);

Int32 RTCharacterMythMasteryGetMaximumLevel(
    RTRuntimeRef Runtime
);

UInt64 RTCharacterMythMasteryGetMaximumExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

UInt64 RTCharacterMythMasteryGetCumulativeLevelUpExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterMythMasteryRebirth(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterMythMasteryGetCanRebirth(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

UInt32 RTCharacterMythMasteryGetRebirthGemCost(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

RTMythMasterySlotRef RTCharacterMythMasteryGetSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 SlotIndex
);

Void RTCharacterMythMasterySetSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 SlotIndex,
    RTDataMythMasterySlotRef NewSlotValues
);

RTDataMythMasterySlotRef RTCharacterMythMasteryRollSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 SlotIndex
);

Bool RTCharacterMythMasteryGetCanOpenLockGroup(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 LockGroup
);

Bool RTCharacterMythMasteryGetSlotOccupied(
    RTRuntimeRef Runtime,
    RTCharacterRef,
    Int32 MasteryIndex,
    Int32 LockGroup
);

EXTERN_C_END