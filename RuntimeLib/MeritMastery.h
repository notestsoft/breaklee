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
    Int32 MasteryIndex;
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
    Int16 TotalMemorizeCount;
    Int16 UnlockedSlotCount;
    Int16 MasterySlotCount;
    Int16 SpecialMasterySlotCount;
    UInt8 ExtendedMemorizeCount;
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
    Int32 MasteryIndex;
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
    Int16 TotalMemorizeCount;
    Int16 UnlockedSlotCount;
    Int16 MasterySlotCount;
    Int16 SpecialMasterySlotCount;
    Int32 SpecialMasteryExp;
    UInt8 ExtendedMemorizeCount;
};

struct _RTCharacterDiamondMeritMasteryData {
    struct _RTDiamondMeritMasteryInfo Info;
    struct _RTDiamondMeritExtendedMemorizeSlot ExtendedMemorizeSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MEMORIZE_COUNT];
    struct _RTDiamondMeritUnlockedSlot UnlockedSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT];
    struct _RTDiamondMeritMasterySlot MasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT];
    struct _RTDiamondMeritSpecialMasterySlot SpecialMasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_SPECIAL_MASTERY_SLOT_COUNT];
};

#pragma pack(pop)

Int32 RTMeritMasteryGetCategoryIndex(
    RTRuntimeRef Runtime,
    Int32 MasteryIndex
);

Void RTCharacterInitializeMeritMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterUpdateMeritMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Bool ForceUpdate
);

RTGoldMeritMasterySlotRef RTCharacterGoldMeritGetMasterySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex
);

Void RTCharacterGoldMeritMasteryAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 Exp
);

Bool RTCharacterPlatinumMeritMasteryGradeUp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt16 InventorySlotCount,
    UInt16* InventorySlotIndex
);

RTPlatinumMeritMasterySlotRef RTCharacterPlatinumMeritGetMasterySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MemorizeIndex,
    Int32 MasteryIndex
);

Void RTCharacterPlatinumMeritMasteryAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 Exp
);

Bool RTCharacterPlatinumMeritMasteryOpenSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    UInt16 InventorySlotCount1,
    UInt16* InventorySlotIndices1,
    UInt16 InventorySlotCount2,
    UInt16* InventorySlotIndices2
);

Bool RTCharacterPlatinumMeritMasterySetActiveMemorizeIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MemorizeIndex
);

Int32 RTCharacterPlatinumMeritMasteryGetPoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterPlatinumMeritMasteryGetSpecialMasterySlots(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int8 MemorizeIndex,
    Int32 CategoryIndex,
    RTPlatinumMeritSpecialMasterySlotRef* Result1,
    RTPlatinumMeritSpecialMasterySlotRef* Result2
);

Bool RTCharacterPlatinumMeritMasteryGrantSpecialMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 Category,
    UInt16 InventorySlotCount,
    UInt16* InventorySlotIndex
);

Bool RTCharacterDiamondMeritMasteryGradeUp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt16 InventorySlotCount,
    UInt16* InventorySlotIndex
);

RTDiamondMeritMasterySlotRef RTCharacterDiamondMeritGetMasterySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MemorizeIndex,
    Int32 MasteryIndex
);

Void RTCharacterDiamondMeritMasteryAddExp(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 Exp
);

Bool RTCharacterDiamondMeritMasteryOpenSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MasteryIndex,
    UInt16 InventorySlotCount1,
    UInt16* InventorySlotIndices1,
    UInt16 InventorySlotCount2,
    UInt16* InventorySlotIndices2
);

Bool RTCharacterDiamondMeritMasterySetActiveMemorizeIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 MemorizeIndex
);

Int32 RTCharacterDiamondMeritMasteryGetPoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterDiamondMeritMasteryGetSpecialMasterySlots(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int8 MemorizeIndex,
    Int32 CategoryIndex,
    RTDiamondMeritSpecialMasterySlotRef* Result1,
    RTDiamondMeritSpecialMasterySlotRef* Result2,
    RTDiamondMeritSpecialMasterySlotRef* Result3
);

Bool RTCharacterDiamondMeritMasteryGrantSpecialMastery(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    UInt16 InventorySlotCount,
    UInt16* InventorySlotIndex
);

Bool RTCharacterRegisterMeritMedals(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 InventorySlotCount,
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
