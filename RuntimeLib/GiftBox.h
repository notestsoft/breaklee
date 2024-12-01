#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTGiftBoxSlot {
    UInt8 Index;
    Int16 ReceivedCount;
    Int16 ResetCount;
    Timestamp ElapsedTime;
    Timestamp CooldownTime;
    UInt8 Active;
};

struct _RTGiftBoxRewardSlot {
    Bool IsRolled;
    RTItem ItemID;
    RTItemOptions ItemOptions;
};

struct _RTGiftBoxInfo {
    Int16 SpecialPoints;
    UInt8 SlotCount;
};

struct _RTCharacterGiftboxInfo {
    struct _RTGiftBoxInfo Info;
    struct _RTGiftBoxSlot Slots[RUNTIME_CHARACTER_MAX_GIFT_BOX_SLOT_COUNT];
    struct _RTGiftBoxRewardSlot RewardSlots[RUNTIME_CHARACTER_MAX_GIFT_BOX_SLOT_COUNT];
};

#pragma pack(pop)

RTGiftBoxSlotRef RTCharacterGetGiftBox(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int Index
);

Void RTCharacterUpdateGiftBox(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Bool RTCharacterRollGiftBox(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int Index
);

Bool RTCharacterReceiveGiftBox(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int Index,
    Int32 InventorySlotIndex,
    Int32 StackSize
);

EXTERN_C_END