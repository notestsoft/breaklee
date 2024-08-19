#pragma once

#include "Base.h"
#include "Constants.h"


EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
    RUNTIME_STELLAR_MASTERY_SLOT_LINK_GRADE_YEARNING = 1,
    RUNTIME_STELLAR_MASTERY_SLOT_LINK_GRADE_SORROW = 2,
    RUNTIME_STELLAR_MASTERY_SLOT_LINK_GRADE_WRATH = 3,
    RUNTIME_STELLAR_MASTERY_SLOT_LINK_GRADE_OBLIVION = 4,
    RUNTIME_STELLAR_MASTERY_SLOT_LINK_GRADE_VOID = 5,
};

struct _RTStellarMasterySlot {
    UInt8 GroupID;
    UInt8 SlotLine;
    UInt8 SlotIndex;
    UInt8 StellarLinkGrade;
    UInt32 StellarForceEffect; // See RuntimeLib/Force.h
    UInt32 StellarForceValue;
    UInt32 StellarForceValueType;
};

struct _RTStellarMasteryInfo {
    UInt8 SlotCount;
};

struct _RTCharacterStellarMasteryInfo {
    struct _RTStellarMasteryInfo Info;
    struct _RTStellarMasterySlot Slots[RUNTIME_CHARACTER_MAX_STELLAR_SLOT_COUNT];
};

RTDataStellarLineGradeRef RTDataStellarMasteryGetLineGrade(
	RTRuntimeRef Runtime,
    RTDataStellarLineRef StellarLine,
    UInt32 MaterialCount
);

#pragma pack(pop)

EXTERN_C_END