#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
    RUNTIME_MERIT_MASTERY_GRADE_GOLD        = 0,
    RUNTIME_MERIT_MASTERY_GRADE_PLATINUM    = 1,
    RUNTIME_MERIT_MASTERY_GRADE_DIAMOND     = 2,
};

struct _RTGoldMeritMasterySlot {
    Int32 Index;
    Int32 Level;
};

struct _RTGoldMeritMasteryInfo {
    Int32 SlotCount;
    Int32 Exp;
    Int32 Points;
};

struct _RTCharacterGoldMeritMasteryData {
    struct _RTGoldMeritMasteryInfo Info;
    struct _RTGoldMeritMasterySlot Slots[RUNTIME_CHARACTER_MAX_GOLD_MERIT_MASTERY_SLOT_COUNT];
};

struct _RTPlatinumMeritExtendedMemorizeSlot {
    Int8 MemorizeIndex;
    Int32 Points;
};

struct _RTPlatinumMeritUnlockedSlot {
    Int32 SlotIndex;
};

struct _RTPlatinumMeritMasterySlot {
    Int8 MemorizeIndex;
    Int32 Index;
    Int32 Level;
};

struct _RTPlatinumMeritSpecialMasterySlot {
    Int8 MemorizeIndex;
    Int32 Category;
    Int32 Index;
    Int32 Grade;
};

struct _RTPlatinumMeritMasteryInfo {
    UInt8 IsEnabled;
    Int32 Exp;
    Int32 Points;
    UInt8 ActiveMemorizeIndex;
    Int32 OpenSlotMasteryIndex;
    Timestamp OpenSlotUnlockTime;
    Int16 ExtendedMemorizeCount;
    Int16 UnlockedSlotCount;
    Int16 MasterySlotCount;
    Int16 SpecialMasterySlotCount;
    UInt8 Unknown1;
};

struct _RTCharacterPlatinumMeritMasteryData {
    struct _RTPlatinumMeritMasteryInfo Info;
    struct _RTPlatinumMeritExtendedMemorizeSlot ExtendedMemorizeSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MEMORIZE_COUNT];
    struct _RTPlatinumMeritUnlockedSlot UnlockedSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT];
    struct _RTPlatinumMeritMasterySlot MasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT];
    struct _RTPlatinumMeritSpecialMasterySlot SpecialMasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_SPECIAL_MASTERY_SLOT_COUNT];
};

struct _RTDiamondMeritExtendedMemorizeSlot {
    Int8 MemorizeIndex;
    Int32 Points;
};

struct _RTDiamondMeritUnlockedSlot {
    Int32 SlotIndex;
};

struct _RTDiamondMeritMasterySlot {
    Int8 MemorizeIndex;
    Int32 Index;
    Int32 Level;
};

struct _RTDiamondMeritSpecialMasterySlot {
    Int8 MemorizeIndex;
    Int32 Category;
    Int32 Index;
    Int32 Grade;
};

struct _RTDiamondMeritMasteryInfo {
    UInt8 IsEnabled;
    Int32 Exp;
    Int32 Points;
    UInt8 ActiveMemorizeIndex;
    Int32 OpenSlotMasteryIndex;
    Timestamp OpenSlotUnlockTime;
    Int16 ExtendedMemorizeCount;
    Int16 UnlockedSlotCount;
    Int16 MasterySlotCount;
    Int16 SpecialMasterySlotCount;
    Int32 ExtendedMasterySlotCount;
    UInt8 Unknown1;
};

struct _RTCharacterDiamondMeritMasteryData {
    struct _RTDiamondMeritMasteryInfo Info;
    struct _RTDiamondMeritExtendedMemorizeSlot ExtendedMemorizeSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MEMORIZE_COUNT];
    struct _RTDiamondMeritUnlockedSlot UnlockedSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT];
    struct _RTDiamondMeritMasterySlot MasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT];
    struct _RTDiamondMeritSpecialMasterySlot SpecialMasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_SPECIAL_MASTERY_SLOT_COUNT];
};

#pragma pack(pop)

RTGoldMeritMasterySlotRef RTCharacterGoldMeritGetMasterySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int MasteryIndex
);

Void RTCharacterGoldMeritMasteryAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 Exp
);

RTPlatinumMeritMasterySlotRef RTCharacterPlatinumMeritGetMasterySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int MemorizeIndex,
    Int MasteryIndex
);

Void RTCharacterPlatinumMeritMasteryAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 Exp
);

RTDiamondMeritMasterySlotRef RTCharacterDiamondMeritGetMasterySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int MemorizeIndex,
    Int MasteryIndex
);

Void RTCharacterDiamondMeritMasteryAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 Exp
);

Bool RTCharacterRegisterMeritMedals(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int InventorySlotCount,
    UInt32* InventorySlotIndices
);

Bool RTCharacterMeritMasteryTrain(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    Int32 TargetLevel,
    Int32* RemainingMeritPoints
);

EXTERN_C_END
