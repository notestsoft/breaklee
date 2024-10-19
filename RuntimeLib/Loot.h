#pragma once

#include "Base.h"
#include "Constants.h"
#include "Item.h"
#include "Entity.h"

struct _RTLootResult {
    Int32 Result;
    RTItem ItemID;
    UInt64 ItemOptions;
    UInt16 InventorySlotIndex;
    UInt32 Unknown1;
};
typedef struct _RTLootResult RTLootResult;

EXTERN_C_BEGIN

RTLootResult RTCharacterLootItem(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTEntityID ItemEntityID,
    UInt16 UniqueKey,
    UInt16 InventorySlotIndex
);

EXTERN_C_END