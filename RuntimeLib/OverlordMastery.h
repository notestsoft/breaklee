#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTOverlordMasterySlot {
    UInt8 MasteryIndex;
    UInt8 Level;
};

struct _RTOverlordMasteryInfo {
    Int16 Level;
    Int64 Exp;
    Int16 Point;
    Int8 SlotCount;
};

struct _RTCharacterOverlordMasteryInfo {
    struct _RTOverlordMasteryInfo Info;
    struct _RTOverlordMasterySlot Slots[RUNTIME_CHARACTER_MAX_OVERLORD_MASTERY_SLOT_COUNT];
};

#pragma pack(pop)

Void RTCharacterAddOverlordExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt64 Exp
);

RTOverlordMasterySlotRef RTCharacterGetOverlordMasterySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex
);

EXTERN_C_END