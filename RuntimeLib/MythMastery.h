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
    UInt32 ForceEffectIndex;
    UInt32 ForceValue;
    UInt32 ForceValueType;
};

struct _RTMythMasteryInfo {
    Int32 Rebirth;
    Int32 HolyPower;
    Int32 Level;
    UInt64 Exp;
    Int32 Points;
    Int32 UnlockedPageCount;
    UInt8 Unknown[13];
    UInt8 MasterySlotCount;
    Int32 StigmaGrade;
    Int32 StigmaExp;
};

struct _RTCharacterMythMasteryInfo {
    struct _RTMythMasteryInfo Info;
    struct _RTMythMasterySlot Slots[RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT];
    struct _RTMythMasterySlot TemporarySlot; // This must be in the unknown[13] but idk
    Int32 BonusHolyPower;
};

#pragma pack(pop)

Void RTCharacterMythMasteryFinishQuest(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 QuestIndex
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

Void RTCharacterMythMasteryAddExpPercent(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 ExpPercent
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
    RTCharacterRef Character,
    UInt16 InventorySlotIndex
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

RTMythMasterySlotRef RTCharacterMythMasteryGetOrCreateSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 SlotIndex
);

RTMythMasterySlotRef RTCharacterMythMasterySetSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 SlotIndex,
    Int32 Tier,
    Int32 Grade,
    Int32 ForceEffectIndex,
    Int32 ForceValue,
    Int32 ForceValueType
);

Bool RTCharacterMythMasteryRollSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTMythMasterySlotRef MasterySlot
);

Bool RTCharacterMythMasteryCanOpenLockGroup(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 LockGroup
);

Bool RTCharacterMythMasteryCanOpenLockGroup(
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

Int32 RTCharacterMythMasteryGetRealHolyPoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterMythMasteryAssertHolyPoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterMythMasteryAddStigmaExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 ExpAmount
);

static Bool RTCharacterMythMasteryGetPrerequisiteMetForSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 SlotIndex1,
    Int32 SlotIndex2
);

Bool RTCharacterMythMasteryRollback(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTMythMasterySlotRef Slot
);

UInt64 RTCharacterMythMasteryGetRestoreItemID(
    RTRuntimeRef Runtime
);

EXTERN_C_END