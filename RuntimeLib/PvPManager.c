#include "Runtime.h"
#include "PvPManager.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "WorldManager.h"
#include <Shared/Enumerations.h>

struct _RTPvPMemberContext {
    UInt32 CharacterIndex;
    UInt64 Hp;
    UInt64 HpMax;
    UInt64 Score;
    UInt64 ScoreMax;
};

struct _RTPvPContext {
    Int PoolIndex;
    Timestamp RequestTimeout;
    UInt8 PvpType;
    Bool IsActive;
    struct _RTPvPMemberContext Source;
    struct _RTPvPMemberContext Target;
};

RTPvPMemberContextRef RTPvPContextGetMemberContext(
    RTPvPContextRef PvPContext,
    UInt32 CharacterIndex
) {
    if (PvPContext->Source.CharacterIndex == CharacterIndex) return &PvPContext->Source;
    if (PvPContext->Target.CharacterIndex == CharacterIndex) return &PvPContext->Target;

    return NULL;
}

RTPvPManagerRef RTPvPManagerCreate(
    RTRuntimeRef Runtime,
    Int32 MaxPvPContextCount,
    Int32 MaxPvPDistance,
    Timestamp PvPRequestTimeout
) {
    RTPvPManagerRef PvPManager = AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTPvPManager));
    if (!PvPManager) Fatal("Memory allocation failed");

    PvPManager->Allocator = Runtime->Allocator;
    PvPManager->Runtime = Runtime;
    PvPManager->MaxPvPContextCount = MaxPvPContextCount;
    PvPManager->MaxPvPDistance = MaxPvPDistance;
    PvPManager->PvPRequestTimeout = PvPRequestTimeout;
    PvPManager->NextRequestTimeout = UINT64_MAX;
    PvPManager->ContextPool = MemoryPoolCreate(Runtime->Allocator, sizeof(struct _RTPvPContext), MaxPvPContextCount);
    PvPManager->ContextPoolIndices = ArrayCreateEmpty(Runtime->Allocator, sizeof(Int), MaxPvPContextCount);
    PvPManager->ContextPoolIndicesToRemove = ArrayCreateEmpty(Runtime->Allocator, sizeof(Int), 8);
    PvPManager->CharacterToPvPContext = IndexDictionaryCreate(Runtime->Allocator, MaxPvPContextCount);

    return PvPManager;
}

Void RTPvPManagerDestroy(
    RTPvPManagerRef PvPManager
) {
    // TODO: Check if all running PvPs have to be cleaned up!!!
    DictionaryDestroy(PvPManager->CharacterToPvPContext);
    ArrayDestroy(PvPManager->ContextPoolIndices);
    MemoryPoolDestroy(PvPManager->ContextPool);
    AllocatorDeallocate(PvPManager->Allocator, PvPManager);
}

Void RTPvPManagerUpdate(
    RTPvPManagerRef PvPManager,
    RTPvPContextRef PvPContext,
    RTCharacterRef Character
) {

    RTRuntimeRef Runtime = PvPManager->Runtime;

    UInt32 OtherCharacterIndex = PvPContext->Source.CharacterIndex == Character->CharacterIndex ? PvPContext->Target.CharacterIndex : PvPContext->Source.CharacterIndex;

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, OtherCharacterIndex);
    if (!Source) goto error;

    if (PvPContext->PvpType == PVP_TYPE_PERSON) {

        NOTIFICATION_DATA_PVP_HP_INFO* NotificationHp = RTNotificationInit(PVP_HP_INFO);
        NotificationHp->CharacterIndex = Character->CharacterIndex;
        NotificationHp->CurrentHP = Character->Data.Info.CurrentHP;
        NotificationHp->MaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        RTNotificationDispatchToCharacter(NotificationHp, Source);
    }

    return 0;

error:
    return 1;
   
}

RTPvPContextRef RTPvPManagerCreateContext(
    RTPvPManagerRef PvPManager,
    UInt32 SourceCharacterIndex,
    UInt32 TargetCharacterIndex,
    UInt8 PvpType
) {
    assert(!RTPvPManagerGetContextByCharacter(PvPManager, SourceCharacterIndex));
    assert(!RTPvPManagerGetContextByCharacter(PvPManager, TargetCharacterIndex));

    Int PoolIndex = 0;
    RTPvPContextRef PvPContext = MemoryPoolReserveNext(PvPManager->ContextPool, &PoolIndex);
    if (!PvPContext) return NULL;

    PvPContext->PoolIndex = PoolIndex;
    PvPContext->Source.CharacterIndex = SourceCharacterIndex;
    PvPContext->Target.CharacterIndex = TargetCharacterIndex;
    PvPContext->RequestTimeout = GetTimestampMs() + PvPManager->PvPRequestTimeout;
    PvPContext->PvpType = PvpType;
    PvPManager->NextRequestTimeout = MIN(PvPManager->NextRequestTimeout, PvPContext->RequestTimeout);

    PvPContext->Source.Hp = 0;
    PvPContext->Source.HpMax = 0;
    PvPContext->Source.Score = 0;
    PvPContext->Source.ScoreMax = 0;

    PvPContext->Target.Hp = 0;
    PvPContext->Target.HpMax = 0;
    PvPContext->Target.Score = 0;
    PvPContext->Target.ScoreMax = 0;


    Int SourceIndex = SourceCharacterIndex;
    Int TargetIndex = TargetCharacterIndex;
    DictionaryInsert(PvPManager->CharacterToPvPContext, &SourceIndex, &PoolIndex, sizeof(Int));
    DictionaryInsert(PvPManager->CharacterToPvPContext, &TargetIndex, &PoolIndex, sizeof(Int));
    ArrayAppendElement(PvPManager->ContextPoolIndices, &PoolIndex);

    return PvPContext;
}

Bool RTPvPManagerDestroyContext(
    RTPvPManagerRef PvPManager,
    UInt32 SourceCharacterIndex
) {
    RTRuntimeRef Runtime = PvPManager->Runtime;
    RTPvPContextRef PvPContext = RTPvPManagerGetContextByCharacter(PvPManager, SourceCharacterIndex);
    if (!PvPContext) return false;

    Int SourceIndex = PvPContext->Source.CharacterIndex;
    Int TargetIndex = PvPContext->Target.CharacterIndex;
    DictionaryRemove(PvPManager->CharacterToPvPContext, &SourceIndex);
    DictionaryRemove(PvPManager->CharacterToPvPContext, &TargetIndex);
    ArrayRemoveElement(PvPManager->ContextPoolIndices, &PvPContext->PoolIndex);
    MemoryPoolRelease(PvPManager->ContextPool, PvPContext->PoolIndex);
    return true;
}

RTPvPContextRef RTPvPManagerGetContextByCharacter(
    RTPvPManagerRef PvPManager,
    UInt32 CharacterIndex
) {
    Int Index = CharacterIndex;
    Int* PoolIndex = DictionaryLookup(PvPManager->CharacterToPvPContext, &Index);
    if (!PoolIndex) return false;

    return (RTPvPContextRef)MemoryPoolFetch(PvPManager->ContextPool, *PoolIndex);
}

UInt8 RTPvPManagerRequestPvP(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Source,
    UInt32 TargetCharacterIndex,
    UInt8 PvpType
) {
    RTRuntimeRef Runtime = PvPManager->Runtime;

    if (RTPvPManagerGetContextByCharacter(PvPManager, Source->CharacterIndex)) goto error;
    if (RTPvPManagerGetContextByCharacter(PvPManager, TargetCharacterIndex)) goto error;

    RTCharacterRef Target = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, TargetCharacterIndex);
    if (Source->Data.Info.WorldIndex != Target->Data.Info.WorldIndex) goto error;

    Int Distance = RTCalculateDistance(
        Source->Movement.PositionCurrent.X,
        Source->Movement.PositionCurrent.Y,
        Target->Movement.PositionCurrent.X,
        Target->Movement.PositionCurrent.Y
    );
    if (Distance > PvPManager->MaxPvPDistance) goto error;

    RTPvPContextRef PvPContext = RTPvPManagerCreateContext(PvPManager, Source->CharacterIndex, TargetCharacterIndex, PvpType);
    if (!PvPContext) goto error;

        {
            NOTIFICATION_DATA_PVP_REQUEST* Notification = RTNotificationInit(PVP_REQUEST);
            Notification->CharacterIndex = Source->CharacterIndex;
            Notification->PvpType = PvpType;
            RTNotificationDispatchToCharacter(Notification, Target);
        }

    return 0;

error:
    return 1;
    
}

UInt8 RTPvPManagerRequestPvPResponse(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Target,
    UInt8 PvpType,
    UInt8 ResultType
) {
    RTRuntimeRef Runtime = PvPManager->Runtime;
  //  Bool IsAccepted = (EventType == NOTIFICATION_PvP_EVENT_TYPE_REQUEST_ACCEPT);

    RTPvPContextRef PvPContext = RTPvPManagerGetContextByCharacter(PvPManager, Target->CharacterIndex);
    if (!PvPContext) goto error;

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, PvPContext->Source.CharacterIndex);
    if (!Source) goto error;


    if (PvPContext->PvpType == PVP_TYPE_WAITING) {

        if (ResultType == PVP_REQUEST_RESULT_AGREE) {

            PvPContext->PvpType = PvpType;

            if (PvpType == PVP_TYPE_PERSON) {
                PvPContext->Source.Hp = 0;
                PvPContext->Source.HpMax = 0;
                PvPContext->Source.Score = 0;
                PvPContext->Source.ScoreMax = 0;

                PvPContext->Target.Hp = 0;
                PvPContext->Target.HpMax = 0;
                PvPContext->Target.Score = 0;
                PvPContext->Target.ScoreMax = 0;
            }

        }
    }

    if (PvpType == PVP_TYPE_PERSON) {
        {
            NOTIFICATION_DATA_PVP_RESPONSE* Notification = RTNotificationInit(PVP_RESPONSE);
            Notification->CharacterIndex = Target->CharacterIndex;
            Notification->PvpType = PvpType;
            Notification->PvpResult = ResultType;
            RTNotificationDispatchToCharacter(Notification, Source);
          
        }
    }
    else {
        goto error;
    }

    return 0;

error:
    return 1;
 //   return NOTIFICATION_PvP_EVENT_TYPE_CANCEL_PvP;
}

UInt8 RTPvPManagerStartPvP(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Target
) {
    RTRuntimeRef Runtime = PvPManager->Runtime;
    //  Bool IsAccepted = (EventType == NOTIFICATION_PvP_EVENT_TYPE_REQUEST_ACCEPT);

    RTPvPContextRef PvPContext = RTPvPManagerGetContextByCharacter(PvPManager, Target->CharacterIndex);
    if (!PvPContext) goto error;

    RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, PvPContext->Source.CharacterIndex);
    if (!Source) goto error;

    if (PvPContext->PvpType == PVP_TYPE_PERSON) {

        NOTIFICATION_DATA_PVP_HP_INFO* NotificationHp = RTNotificationInit(PVP_HP_INFO);
        NotificationHp->CharacterIndex = Target->CharacterIndex;
        NotificationHp->CurrentHP = Target->Data.Info.CurrentHP;
        NotificationHp->MaxHP = Target->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        RTNotificationDispatchToCharacter(NotificationHp, Source);


        NOTIFICATION_DATA_PVP_HP_INFO* NotificationHpOther = RTNotificationInit(PVP_HP_INFO);
        NotificationHpOther->CharacterIndex = Source->CharacterIndex;
        NotificationHpOther->CurrentHP = Source->Data.Info.CurrentHP;
        NotificationHpOther->MaxHP = Source->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        RTNotificationDispatchToCharacter(NotificationHpOther, Target);
    }

    return 0;

error:
    return 1;
}

UInt8 RTPvPManagerEndPvP(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Character,
    UInt8 PvpType,
    UInt8 ResultType
) {
    RTRuntimeRef Runtime = PvPManager->Runtime;
    //  Bool IsAccepted = (EventType == NOTIFICATION_PvP_EVENT_TYPE_REQUEST_ACCEPT);

    RTPvPContextRef PvPContext = RTPvPManagerGetContextByCharacter(PvPManager, Character->CharacterIndex);
    if (!PvPContext) goto error;


    if (PvpType == PVP_TYPE_PERSON) {

        UInt32 OtherCharacterIndex = PvPContext->Source.CharacterIndex == Character->CharacterIndex ? PvPContext->Target.CharacterIndex : PvPContext->Source.CharacterIndex;

        RTCharacterRef Source = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, OtherCharacterIndex);
        if (!Source) goto error;


        NOTIFICATION_DATA_PVP_END* Notification = RTNotificationInit(PVP_END);
        Notification->PvpResult = ResultType;
        Notification->PvpType = PvpType;
        RTNotificationDispatchToCharacter(Notification, Source);

    }

    RTPvPManagerDestroyContext(PvPManager, PvPContext->Source.CharacterIndex);
    return 0;

error:
    return 1;
}

UInt8 RTPvPManagerClosePvP(
    RTPvPManagerRef PvPManager,
    RTCharacterRef Character,
    UInt8 EventType
) {
    return 1;
}
