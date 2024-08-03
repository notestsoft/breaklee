#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCollectionSlot {
    UInt8 TypeID;
    UInt8 CollectionID;
    UInt16 MissionID;
    UInt8 ReceivedReward;
    UInt16 MissionItemCounts[RUNTIME_CHARACTER_MAX_COLLECTION_ITEM_COUNT];
};

struct _RTCollectionInfo {
    UInt16 SlotCount;
};

struct _RTCharacterCollectionInfo {
    struct _RTCollectionInfo Info;
    struct _RTCollectionSlot Slots[RUNTIME_CHARACTER_MAX_COLLECTION_SLOT_COUNT];
};

#pragma pack(pop)

RTCollectionSlotRef RTCharacterGetCollectionSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt8 TypeID,
    UInt8 CollectionID,
    UInt16 MissionID
);

EXTERN_C_END