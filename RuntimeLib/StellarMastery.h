#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTStellarMasterySlot {
    UInt8 Data[16];
};

struct _RTStellarMasteryInfo {
    UInt8 SlotCount;
};

struct _RTCharacterStellarMasteryInfo {
    struct _RTStellarMasteryInfo Info;
    struct _RTStellarMasterySlot Slots[RUNTIME_CHARACTER_MAX_STELLAR_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END