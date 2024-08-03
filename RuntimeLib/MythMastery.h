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

EXTERN_C_END