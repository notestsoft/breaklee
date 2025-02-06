#pragma once
#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

struct _RTPvPManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    Int32 MaxPvPContextCount;
    Int32 MaxPvPDistance;
    Timestamp PvPRequestTimeout;
    Timestamp NextRequestTimeout;
    MemoryPoolRef ContextPool;
    ArrayRef ContextPoolIndices;
    ArrayRef ContextPoolIndicesToRemove;
    DictionaryRef CharacterToPvPContext;
};

RTPvPManagerRef RTPvPManagerCreate(
    RTRuntimeRef Runtime,
    Int32 MaxPvPContextCount,
    Int32 MaxPvPDistance,
    Timestamp PvPRequestTimeout
);

Void RTPvPManagerDestroy(
    RTPvPManagerRef PvPManager
);

Void RTPvPManagerUpdate(
    RTPvPManagerRef PvPManager
);

RTPvPContextRef RTPvPManagerCreateContext(
    RTPvPManagerRef PvPManager,
    UInt8 PvpType,
    UInt32 SourceCharacterIndex,
    UInt32 TargetCharacterIndex
);

Bool RTPvPManagerDestroyContext(
    RTPvPManagerRef PvPManager,
    UInt32 SourceCharacterIndex,
    UInt8 EventType
);

RTPvPContextRef RTPvPManagerGetContextByCharacter(
    RTPvPManagerRef PvPManager,
    UInt32 CharacterIndex
);

UInt8 RTPvPManagerRequestPvP(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Source,
    UInt32 TargetCharacterIndex,
    UInt8 PvpType
);

UInt8 RTPvPManagerRequestPvPResponse(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Target,
    UInt8 PvpType,
    UInt8 ResultType
);

UInt8 RTPvPManagerClosePvP(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Character,
    UInt8 EventType
);

UInt8 RTPvPManagerAddItems(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Character,
    Int32 InventorySlotCount,
    UInt16* SourceInventorySlotIndex,
    UInt16* PvPInventorySlotIndex
);

UInt8 RTPvPManagerAddCurrency(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Character,
    UInt64 Currency
);

Bool RTPvPManagerSetInventorySlots(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Character,
    UInt8 InventorySlotCount,
    UInt16* InventorySlotIndex
);

EXTERN_C_END
