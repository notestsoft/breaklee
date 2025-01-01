#include "Runtime.h"
#include "TradeManager.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "WorldManager.h"

struct _RTTradeMemberContext {
    UInt32 CharacterIndex;
    UInt64 Currency;
    Bool IsReady;
    Bool IsConfirmed;
    Bool IsInventoryReady;
    UInt8 TradeInventorySlotCount;
    UInt16 TradeInventorySlots[RUNTIME_MAX_TRADE_INVENTORY_SIZE];
    UInt8 InventorySlotCount;
    UInt16 InventorySlots[RUNTIME_MAX_TRADE_INVENTORY_SIZE];
};

struct _RTTradeContext {
    Int PoolIndex;
    Timestamp RequestTimeout;
    Bool IsActive;
    struct _RTTradeMemberContext Source;
    struct _RTTradeMemberContext Target;
};

RTTradeMemberContextRef RTTradeContextGetMemberContext(
    RTTradeContextRef TradeContext,
    UInt32 CharacterIndex
) {
    if (TradeContext->Source.CharacterIndex == CharacterIndex) return &TradeContext->Source;
    if (TradeContext->Target.CharacterIndex == CharacterIndex) return &TradeContext->Target;

    return NULL;
}

RTTradeManagerRef RTTradeManagerCreate(
    RTRuntimeRef Runtime,
    Int32 MaxTradeContextCount,
    Int32 MaxTradeDistance,
    Timestamp TradeRequestTimeout
) {
    RTTradeManagerRef TradeManager = AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTTradeManager));
    if (!TradeManager) Fatal("Memory allocation failed");

    TradeManager->Allocator = Runtime->Allocator;
    TradeManager->Runtime = Runtime;
    TradeManager->MaxTradeContextCount = MaxTradeContextCount;
    TradeManager->MaxTradeDistance = MaxTradeDistance;
    TradeManager->TradeRequestTimeout = TradeRequestTimeout;
    TradeManager->NextRequestTimeout = UINT64_MAX;
    TradeManager->ContextPool = MemoryPoolCreate(Runtime->Allocator, sizeof(struct _RTTradeContext), MaxTradeContextCount);
    TradeManager->ContextPoolIndices = ArrayCreateEmpty(Runtime->Allocator, sizeof(Int), MaxTradeContextCount);
    TradeManager->ContextPoolIndicesToRemove = ArrayCreateEmpty(Runtime->Allocator, sizeof(Int), 8);
    TradeManager->CharacterToTradeContext = IndexDictionaryCreate(Runtime->Allocator, MaxTradeContextCount);

    return TradeManager;
}

Void RTTradeManagerDestroy(
    RTTradeManagerRef TradeManager
) {
    // TODO: Check if all running trades have to be cleaned up!!!

    DictionaryDestroy(TradeManager->CharacterToTradeContext);
    ArrayDestroy(TradeManager->ContextPoolIndices);
    MemoryPoolDestroy(TradeManager->ContextPool);
    AllocatorDeallocate(TradeManager->Allocator, TradeManager);
}

Void RTTradeManagerUpdate(
    RTTradeManagerRef TradeManager
) {
    // TODO: Maybe add position verifications here...
    // TODO: Update request timeouts
    Timestamp CurrentTimestamp = GetTimestampMs();
    if (TradeManager->NextRequestTimeout <= CurrentTimestamp) {
        TradeManager->NextRequestTimeout = UINT64_MAX;

        for (Int Index = 0; Index < ArrayGetElementCount(TradeManager->ContextPoolIndices); Index += 1) {
            Int PoolIndex = *(Int*)ArrayGetElementAtIndex(TradeManager->ContextPoolIndices, Index);
            assert(MemoryPoolIsReserved(TradeManager->ContextPool, PoolIndex));
            RTTradeContextRef TradeContext = (RTTradeContextRef)MemoryPoolFetch(TradeManager->ContextPool, PoolIndex);
            if (TradeContext->IsActive) continue;
            if (TradeContext->RequestTimeout > CurrentTimestamp) {
                TradeManager->NextRequestTimeout = MIN(TradeManager->NextRequestTimeout, TradeContext->RequestTimeout);
                continue;
            }

            ArrayAppendElement(TradeManager->ContextPoolIndicesToRemove, &PoolIndex);
        }

        for (Int Index = 0; Index < ArrayGetElementCount(TradeManager->ContextPoolIndicesToRemove); Index += 1) {
            Int PoolIndex = *(Int*)ArrayGetElementAtIndex(TradeManager->ContextPoolIndicesToRemove, Index);

            assert(MemoryPoolIsReserved(TradeManager->ContextPool, PoolIndex));
            RTTradeContextRef TradeContext = (RTTradeContextRef)MemoryPoolFetch(TradeManager->ContextPool, PoolIndex);
            RTTradeManagerDestroyContext(
                TradeManager, 
                TradeContext->Source.CharacterIndex, 
                NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE
            );
        }

        ArrayRemoveAllElements(TradeManager->ContextPoolIndicesToRemove, true);
    }
}

RTTradeContextRef RTTradeManagerCreateContext(
    RTTradeManagerRef TradeManager,
    UInt32 SourceCharacterIndex,
    UInt32 TargetCharacterIndex
) {
    assert(!RTTradeManagerGetContextByCharacter(TradeManager, SourceCharacterIndex));
    assert(!RTTradeManagerGetContextByCharacter(TradeManager, TargetCharacterIndex));

    Int PoolIndex = 0;
    RTTradeContextRef TradeContext = MemoryPoolReserveNext(TradeManager->ContextPool, &PoolIndex);
    if (!TradeContext) return NULL;

    TradeContext->PoolIndex = PoolIndex;
    TradeContext->Source.CharacterIndex = SourceCharacterIndex;
    TradeContext->Target.CharacterIndex = TargetCharacterIndex;
    TradeContext->RequestTimeout = GetTimestampMs() + TradeManager->TradeRequestTimeout;
    TradeManager->NextRequestTimeout = MIN(TradeManager->NextRequestTimeout, TradeContext->RequestTimeout);

    Int SourceIndex = SourceCharacterIndex;
    Int TargetIndex = TargetCharacterIndex;
    DictionaryInsert(TradeManager->CharacterToTradeContext, &SourceIndex, &PoolIndex, sizeof(Int));
    DictionaryInsert(TradeManager->CharacterToTradeContext, &TargetIndex, &PoolIndex, sizeof(Int));
    ArrayAppendElement(TradeManager->ContextPoolIndices, &PoolIndex);

    return TradeContext;
}

Bool RTTradeManagerDestroyContext(
    RTTradeManagerRef TradeManager,
    UInt32 SourceCharacterIndex,
    UInt8 EventType
) {
    RTRuntimeRef Runtime = TradeManager->Runtime;
    RTTradeContextRef TradeContext = RTTradeManagerGetContextByCharacter(TradeManager, SourceCharacterIndex);
    if (!TradeContext) return false;

    UInt8 TargetEventType = EventType;
    if (TargetEventType != NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE_END &&
        TargetEventType != NOTIFICATION_TRADE_EVENT_TYPE_TRADE_END) {
        TargetEventType = NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE;
    }

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Source.CharacterIndex);
    if (Source) {
        NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
        Notification->EventType = TargetEventType;
        RTNotificationDispatchToCharacter(Notification, Source);
    }

    RTCharacterRef Target = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Target.CharacterIndex);
    if (Target) {
        NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
        Notification->EventType = TargetEventType;
        RTNotificationDispatchToCharacter(Notification, Target);
    }

    Int SourceIndex = TradeContext->Source.CharacterIndex;
    Int TargetIndex = TradeContext->Target.CharacterIndex;
    DictionaryRemove(TradeManager->CharacterToTradeContext, &SourceIndex);
    DictionaryRemove(TradeManager->CharacterToTradeContext, &TargetIndex);
    ArrayRemoveElement(TradeManager->ContextPoolIndices, &TradeContext->PoolIndex);
    MemoryPoolRelease(TradeManager->ContextPool, TradeContext->PoolIndex);
    return true;
}

RTTradeContextRef RTTradeManagerGetContextByCharacter(
    RTTradeManagerRef TradeManager,
    UInt32 CharacterIndex
) {
    Int Index = CharacterIndex;
    Int* PoolIndex = DictionaryLookup(TradeManager->CharacterToTradeContext, &Index);
    if (!PoolIndex) return false;

    return (RTTradeContextRef)MemoryPoolFetch(TradeManager->ContextPool, *PoolIndex);
}

Bool RTTradeManagerRequestTrade(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Source,
    UInt32 TargetCharacterIndex
) {
    RTRuntimeRef Runtime = TradeManager->Runtime;

    if (RTTradeManagerGetContextByCharacter(TradeManager, Source->CharacterIndex)) goto error;
    if (RTTradeManagerGetContextByCharacter(TradeManager, TargetCharacterIndex)) goto error;

    RTCharacterRef Target = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TargetCharacterIndex);
    if (Source->Data.Info.WorldIndex != Target->Data.Info.WorldIndex) goto error;

    Int Distance = RTCalculateDistance(
        Source->Movement.PositionCurrent.X,
        Source->Movement.PositionCurrent.Y,
        Target->Movement.PositionCurrent.X,
        Target->Movement.PositionCurrent.Y
    );
    if (Distance > TradeManager->MaxTradeDistance) goto error;

    RTTradeContextRef TradeContext = RTTradeManagerCreateContext(TradeManager, Source->CharacterIndex, TargetCharacterIndex);
    if (!TradeContext) goto error;
    
    /* Target */ {
        NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
        Notification->CharacterIndex = Source->CharacterIndex;
        Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_REQUEST;
        RTNotificationDispatchToCharacter(Notification, Target);
    }

    return true;

error:
    {
        NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
        Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_CANNOT_REQUEST;
        RTNotificationDispatchToCharacter(Notification, Source);
        return false;
    }
}

UInt8 RTTradeManagerRequestTradeResponse(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Target,
    UInt8 EventType
) {
    RTRuntimeRef Runtime = TradeManager->Runtime;
    Bool IsAccepted = (EventType == NOTIFICATION_TRADE_EVENT_TYPE_REQUEST_ACCEPT);

    RTTradeContextRef TradeContext = RTTradeManagerGetContextByCharacter(TradeManager, Target->CharacterIndex);
    if (!TradeContext) goto error;

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Source.CharacterIndex);
    if (!Source) goto error;

    if (IsAccepted) {
        TradeContext->RequestTimeout = UINT64_MAX;
        TradeContext->IsActive = true;

        /* Source */ {
            NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
            Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_REQUEST_ACK;
            RTNotificationDispatchToCharacter(Notification, Source);
        }
    }
    else {
        // TODO: Notify Target
    }

    return EventType;

error:
    return NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE;
}

UInt8 RTTradeManagerCloseTrade(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Character,
    UInt8 EventType
) {
    RTRuntimeRef Runtime = TradeManager->Runtime;
    RTTradeContextRef TradeContext = RTTradeManagerGetContextByCharacter(TradeManager, Character->CharacterIndex);
    if (!TradeContext) goto error;

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Source.CharacterIndex);
    if (!Source) goto error;

    RTCharacterRef Target = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Target.CharacterIndex);
    if (!Target) goto error;

    if (EventType == NOTIFICATION_TRADE_EVENT_TYPE_READY_TO_TRADE) {
        if (Character->CharacterIndex == Source->CharacterIndex) {
            TradeContext->Source.IsReady = true;

            /* Target */ {
                NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
                Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_READY_TO_TRADE;
                RTNotificationDispatchToCharacter(Notification, Target);
            }
        }
        else if (Character->CharacterIndex == Target->CharacterIndex) {
            TradeContext->Target.IsReady = true;

            /* Source */ {
                NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
                Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_READY_TO_TRADE;
                RTNotificationDispatchToCharacter(Notification, Source);
            }
        }
        else {
            goto error;
        }

        if (TradeContext->Source.IsReady && TradeContext->Target.IsReady) {
            /* Source */ {
                NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
                Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_TRADE_COMPLETE;
                RTNotificationDispatchToCharacter(Notification, Source);
            }

            /* Target */ {
                NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
                Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_TRADE_COMPLETE;
                RTNotificationDispatchToCharacter(Notification, Target);
            }
        }

        return EventType;
    }

    if (EventType == NOTIFICATION_TRADE_EVENT_TYPE_TRADE_COMPLETE_ACK) {
        if (Character->CharacterIndex == Source->CharacterIndex) {
            TradeContext->Source.IsConfirmed = true;

            /* Target */ {
                NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
                Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_TRADE_COMPLETE_ACK;
                RTNotificationDispatchToCharacter(Notification, Target);
            }
        }
        else if (Character->CharacterIndex == Target->CharacterIndex) {
            TradeContext->Target.IsConfirmed = true;

            /* Source */ {
                NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
                Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_TRADE_COMPLETE_ACK;
                RTNotificationDispatchToCharacter(Notification, Source);
            }
        }
        else {
            goto error;
        }

        if (TradeContext->Source.IsConfirmed && TradeContext->Target.IsConfirmed) {
            /* Source */ {
                NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
                Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_REQUEST_ITEM_LIST;
                RTNotificationDispatchToCharacter(Notification, Source);
            }

            /* Target */ {
                NOTIFICATION_DATA_TRADE_EVENT* Notification = RTNotificationInit(TRADE_EVENT);
                Notification->EventType = NOTIFICATION_TRADE_EVENT_TYPE_REQUEST_ITEM_LIST;
                RTNotificationDispatchToCharacter(Notification, Target);
            }
        }

        return EventType;
    }

    if (RTTradeManagerDestroyContext(TradeManager, Character->CharacterIndex, EventType)) {
        return EventType;
    }
    else {
        // TODO: Check which event type to return for blocking the cancellation!
        return NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE;
    }

error:
    // TODO: Check which event type to return for blocking the cancellation!
    return NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE;
}

UInt8 RTTradeManagerAddItems(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Character,
    Int32 InventorySlotCount,
    UInt16* SourceInventorySlotIndex,
    UInt16* TradeInventorySlotIndex
) {
    RTRuntimeRef Runtime = TradeManager->Runtime;
    RTTradeContextRef TradeContext = RTTradeManagerGetContextByCharacter(TradeManager, Character->CharacterIndex);
    if (!TradeContext) goto error;

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Source.CharacterIndex);
    if (!Source) goto error;

    RTCharacterRef Target = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Target.CharacterIndex);
    if (!Target) goto error;

    RTTradeMemberContextRef MemberContext = RTTradeContextGetMemberContext(TradeContext, Character->CharacterIndex);
    if (!MemberContext) goto error;

    NOTIFICATION_DATA_TRADE_ADD_ITEMS* Notification = RTNotificationInit(TRADE_ADD_ITEMS);
    Notification->ItemCount = 0;

    // TODO: Check if source inventory slots are used multiple times!

    for (Int Index = 0; Index < InventorySlotCount; Index += 1) {
        UInt16 InventorySlotIndex = TradeInventorySlotIndex[Index];
        if (InventorySlotIndex >= RUNTIME_MAX_TRADE_INVENTORY_SIZE) goto error;
        if (MemberContext->TradeInventorySlots[InventorySlotIndex]) goto error;

        RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SourceInventorySlotIndex[Index]);
        if (!ItemSlot) goto error;
        if (ItemSlot->Item.IsAccountBinding || ItemSlot->Item.IsCharacterBinding || ItemSlot->Item.IsProtected) goto error;

        MemberContext->TradeInventorySlots[InventorySlotIndex] = SourceInventorySlotIndex[Index];

        Notification->ItemCount += 1;
        NOTIFICATION_DATA_TRADE_ADD_ITEMS_SLOT* NotificationSlot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_TRADE_ADD_ITEMS_SLOT);
        NotificationSlot->ItemID = ItemSlot->Item.Serial;
        NotificationSlot->ItemOptions = ItemSlot->ItemOptions;
        NotificationSlot->SlotIndex = InventorySlotIndex;
        NotificationSlot->ItemDuration = ItemSlot->ItemDuration.Serial;
    }

    MemberContext->TradeInventorySlotCount += InventorySlotCount;

    if (Character->CharacterIndex == Source->CharacterIndex) {
        RTNotificationDispatchToCharacter(Notification, Target);
    }
    else if (Character->CharacterIndex == Target->CharacterIndex) {
        RTNotificationDispatchToCharacter(Notification, Source);
    }

    return NOTIFICATION_TRADE_EVENT_TYPE_ADD_ITEM;

error:
    // TODO: Check which event type to return for blocking the cancellation!
    return NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE;
}

UInt8 RTTradeManagerAddCurrency(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Character,
    UInt64 Currency
) {
    RTRuntimeRef Runtime = TradeManager->Runtime;
    RTTradeContextRef TradeContext = RTTradeManagerGetContextByCharacter(TradeManager, Character->CharacterIndex);
    if (!TradeContext) goto error;

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Source.CharacterIndex);
    if (!Source) goto error;

    RTCharacterRef Target = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Target.CharacterIndex);
    if (!Target) goto error;

    RTTradeMemberContextRef MemberContext = RTTradeContextGetMemberContext(TradeContext, Character->CharacterIndex);
    if (!MemberContext) goto error;

    if (Currency > Character->Data.Info.Alz) goto error;

    MemberContext->Currency = Currency;

    NOTIFICATION_DATA_TRADE_ADD_ITEMS* Notification = RTNotificationInit(TRADE_ADD_ITEMS);
    Notification->ItemCount = 1;

    NOTIFICATION_DATA_TRADE_ADD_ITEMS_SLOT* NotificationSlot = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_TRADE_ADD_ITEMS_SLOT);
    NotificationSlot->ItemID = RUNTIME_ITEM_ID_CURRENCY;
    NotificationSlot->ItemOptions = Currency;
    NotificationSlot->SlotIndex = 0;
    NotificationSlot->ItemDuration = 0;

    if (Character->CharacterIndex == Source->CharacterIndex) {
        RTNotificationDispatchToCharacter(Notification, Target);
    }
    else if (Character->CharacterIndex == Target->CharacterIndex) {
        RTNotificationDispatchToCharacter(Notification, Source);
    }

    return NOTIFICATION_TRADE_EVENT_TYPE_ADD_ITEM;

error:
    // TODO: Check which event type to return for blocking the cancellation!
    return NOTIFICATION_TRADE_EVENT_TYPE_CANCEL_TRADE;
}

Bool RTTradeManagerSetInventorySlots(
    RTTradeManagerRef TradeManager,
    RTCharacterRef Character,
    UInt8 InventorySlotCount,
    UInt16* InventorySlotIndex
) {
    if (InventorySlotCount > RUNTIME_MAX_TRADE_INVENTORY_SIZE) return false;

    RTRuntimeRef Runtime = TradeManager->Runtime;
    RTTradeContextRef TradeContext = RTTradeManagerGetContextByCharacter(TradeManager, Character->CharacterIndex);
    if (!TradeContext) return false;

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Source.CharacterIndex);
    if (!Source) return false;

    RTCharacterRef Target = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TradeContext->Target.CharacterIndex);
    if (!Target) return false;

    RTTradeMemberContextRef MemberContext = RTTradeContextGetMemberContext(TradeContext, Character->CharacterIndex);
    if (!MemberContext) return false;

    for (Int Index = 0; Index < MemberContext->InventorySlotCount; Index += 1) {
        if (!RTInventoryIsSlotEmpty(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex[Index])) return false;
    }

    if (Character->CharacterIndex == Source->CharacterIndex) {
        if (InventorySlotCount != TradeContext->Target.TradeInventorySlotCount) return false;
    }
    else if (Character->CharacterIndex == Target->CharacterIndex) {
        if (InventorySlotCount != TradeContext->Source.TradeInventorySlotCount) return false;
    }

    MemberContext->IsInventoryReady = true;
    MemberContext->InventorySlotCount = InventorySlotCount;
    memcpy(MemberContext->InventorySlots, InventorySlotIndex, sizeof(UInt16) * InventorySlotCount);

    if (TradeContext->Source.IsInventoryReady && TradeContext->Target.IsInventoryReady) {
        Int SourceTempInventorySlotCount = 0;
        Int TargetTempInventorySlotCount = 0;
        struct _RTItemSlot SourceTempInventorySlots[RUNTIME_MAX_TRADE_INVENTORY_SIZE] = { 0 };
        struct _RTItemSlot TargetTempInventorySlots[RUNTIME_MAX_TRADE_INVENTORY_SIZE] = { 0 };

        for (Int Index = 0; Index < RUNTIME_MAX_TRADE_INVENTORY_SIZE; Index += 1) {
            if (TradeContext->Source.TradeInventorySlots[Index]) {
                RTInventoryRemoveSlot(
                    Runtime,
                    &Source->Data.InventoryInfo,
                    TradeContext->Source.TradeInventorySlots[Index],
                    &SourceTempInventorySlots[SourceTempInventorySlotCount]
                );
                SourceTempInventorySlotCount += 1;
            }

            if (TradeContext->Target.TradeInventorySlots[Index]) {
                RTInventoryRemoveSlot(
                    Runtime,
                    &Target->Data.InventoryInfo,
                    TradeContext->Target.TradeInventorySlots[Index],
                    &TargetTempInventorySlots[TargetTempInventorySlotCount]
                );
                TargetTempInventorySlotCount += 1;
            }
        }

        for (Int Index = 0; Index < SourceTempInventorySlotCount; Index += 1) {
            RTInventorySetSlot(Runtime, &Target->Data.InventoryInfo, &SourceTempInventorySlots[Index]);
        }

        for (Int Index = 0; Index < TargetTempInventorySlotCount; Index += 1) {
            RTInventorySetSlot(Runtime, &Source->Data.InventoryInfo, &TargetTempInventorySlots[Index]);
        }

        Source->Data.Info.Alz -= TradeContext->Source.Currency;
        Source->Data.Info.Alz += TradeContext->Target.Currency;
        Source->SyncMask.Info = true;
        Source->SyncMask.InventoryInfo = true;

        Target->Data.Info.Alz -= TradeContext->Target.Currency;
        Target->Data.Info.Alz += TradeContext->Source.Currency;
        Target->SyncMask.Info = true;
        Target->SyncMask.InventoryInfo = true;

        RTTradeManagerDestroyContext(
            TradeManager,
            Character->CharacterIndex,
            NOTIFICATION_TRADE_EVENT_TYPE_TRADE_END
        );
    }

    return true;
}
