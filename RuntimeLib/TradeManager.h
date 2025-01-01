#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

struct _RTTradeManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    Int32 MaxTradeContextCount;
    Int32 MaxTradeDistance;
    Timestamp TradeRequestTimeout;
    Timestamp NextRequestTimeout;
    MemoryPoolRef ContextPool;
    ArrayRef ContextPoolIndices;
    ArrayRef ContextPoolIndicesToRemove;
    DictionaryRef CharacterToTradeContext;
};

RTTradeManagerRef RTTradeManagerCreate(
    RTRuntimeRef Runtime,
    Int32 MaxTradeContextCount,
    Int32 MaxTradeDistance,
    Timestamp TradeRequestTimeout
);

Void RTTradeManagerDestroy(
    RTTradeManagerRef TradeManager
);

Void RTTradeManagerUpdate(
    RTTradeManagerRef TradeManager
);

RTTradeContextRef RTTradeManagerCreateContext(
    RTTradeManagerRef TradeManager,
    UInt32 SourceCharacterIndex,
    UInt32 TargetCharacterIndex
);

Bool RTTradeManagerDestroyContext(
    RTTradeManagerRef TradeManager,
    UInt32 SourceCharacterIndex,
    UInt8 EventType
);

RTTradeContextRef RTTradeManagerGetContextByCharacter(
    RTTradeManagerRef TradeManager,
    UInt32 CharacterIndex
);

Bool RTTradeManagerRequestTrade(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Source,
    UInt32 TargetCharacterIndex
);

UInt8 RTTradeManagerRequestTradeResponse(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Target,
    UInt8 EventType
);

UInt8 RTTradeManagerCloseTrade(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Character,
    UInt8 EventType
);

UInt8 RTTradeManagerAddItems(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Character,
    Int32 InventorySlotCount,
    UInt16* SourceInventorySlotIndex,
    UInt16* TradeInventorySlotIndex
);

UInt8 RTTradeManagerAddCurrency(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Character,
    UInt64 Currency
);

Bool RTTradeManagerSetInventorySlots(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Character,
    UInt8 InventorySlotCount,
    UInt16* InventorySlotIndex
);

EXTERN_C_END
