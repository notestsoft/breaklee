#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTMythMasterySlot {
    UInt8 Data[16];
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

Bool RTCharacterMythMasteryGetCanRebirth(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterMythMasteryGetCanOpenLockGroup(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 LockGroup
);

Bool RTCharacterMythMasteryGetIsSlotOccupied(
    RTRuntimeRef Runtime,
    RTCharacterRef,
    Int32 MasteryIndex,
    Int32 LockGroup
);

EXTERN_C_END