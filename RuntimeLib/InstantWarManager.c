#include "Runtime.h"
#include "InstantWarManager.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "WorldManager.h"
#include <Shared/Enumerations.h>

RTInstantWarManagerRef RTInstantWarManagerCreate(
    RTRuntimeRef Runtime,
    UInt64 WorldType
) {
    RTInstantWarManagerRef InstantWarManager = AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTInstantWarManager));
    if (!InstantWarManager) Fatal("Memory allocation failed");

    InstantWarManager->Allocator = Runtime->Allocator;
    InstantWarManager->Runtime = Runtime;
    InstantWarManager->WorldType = WorldType;

    RTDataWarMapRef WarMap = RTRuntimeDataWarMapGet(Runtime->Context, WorldType >> 40);

    InstantWarManager->WarMapRef = WarMap;

    InstantWarManager->LobbyTimeAttackSec = WarMap->LobbyTime;
    InstantWarManager->LobbyTimeAttackRemaining = WarMap->LobbyTime;
    InstantWarManager->LobbyIsTimeAttackEnabled = 1;
    InstantWarManager->BfTimeAttackSec = WarMap->BfTime;
    InstantWarManager->BfTimeAttackRemaining = WarMap->BfTime;
    InstantWarManager->BfIsTimeAttackEnabled = 0;
    InstantWarManager->EntryLimitPerNation = WarMap->MaxUser / 2;

    InstantWarManager->CharacterCapellaLobbyPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTCharacterWarEntry),
        InstantWarManager->EntryLimitPerNation
    );

    InstantWarManager->CharacterCapellaBfPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTCharacterWarEntry),
        InstantWarManager->EntryLimitPerNation
    );

    InstantWarManager->CharacterProcyonLobbyPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTCharacterWarEntry),
        InstantWarManager->EntryLimitPerNation
    );

    InstantWarManager->CharacterProcyonBfPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTCharacterWarEntry),
        InstantWarManager->EntryLimitPerNation
    );

    InstantWarManager->CharacterGMPool = MemoryPoolCreate(
        Runtime->Allocator,
        sizeof(struct _RTCharacterWarEntry),
        InstantWarManager->EntryLimitPerNation
    );

    InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        WarMap->MaxUser
    );

    InstantWarManager->IndexToCharacterCapellaBfPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        WarMap->MaxUser
    );

    InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        WarMap->MaxUser
    );

    InstantWarManager->IndexToCharacterProcyonBfPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        WarMap->MaxUser
    );

    InstantWarManager->IndexToCharacterGMPoolIndex = IndexDictionaryCreate(
        Runtime->Allocator,
        WarMap->MaxUser
    );

    Int NullIndex = 0;
    MemoryPoolReserve(InstantWarManager->CharacterCapellaLobbyPool, NullIndex);
    MemoryPoolReserve(InstantWarManager->CharacterCapellaBfPool, NullIndex);
    MemoryPoolReserve(InstantWarManager->CharacterProcyonLobbyPool, NullIndex);
    MemoryPoolReserve(InstantWarManager->CharacterProcyonBfPool, NullIndex);
    MemoryPoolReserve(InstantWarManager->CharacterGMPool, NullIndex);
    
    InstantWarManager->InstantWarTimestamp = GetTimestampMs() + 1000;
    
    return InstantWarManager;
}

Void RTInstantWarUpdate(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager
) {
    if (InstantWarManager->LobbyTimeAttackRemaining > 0) {
        InstantWarManager->LobbyTimeAttackRemaining = InstantWarManager->LobbyTimeAttackRemaining - 1;
    }
    else {
        if (InstantWarManager->BfIsTimeAttackEnabled == 0) {
            InstantWarManager->BfIsTimeAttackEnabled = 1;
            InstantWarManager->BfTimeAttackRemaining = InstantWarManager->BfTimeAttackRemaining - 1;
        }
        else {
            InstantWarManager->BfTimeAttackRemaining = InstantWarManager->BfTimeAttackRemaining - 1;
        }
    }
}

RTCharacterWarEntryRef RTInstantWarCharacterInitialize(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    RTCharacterRef Character
) {
    if (Character->Data.StyleInfo.Nation <= 0) return NULL;

    Character->Data.Info.WorldIndex = InstantWarManager->WarMapRef->LobbyWorldId;

    RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
    RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, WorldContext->WorldData->DeadWarpIndex);

    Character->Data.Info.PositionX = WarpPoint.X;
    Character->Data.Info.PositionY = WarpPoint.Y;
    Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
    Character->Data.Info.DungeonIndex = 0;

    UInt8 n = Character->Data.StyleInfo.Nation;

    assert(!DictionaryLookup(n == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex : 
        (n == 1 ? InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex : 
        InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex), &Character->CharacterIndex));

    Int CharacterPoolIndex = 0;

    RTCharacterWarEntryRef WarEntry = (RTCharacterWarEntryRef)MemoryPoolReserveNext(n == 3 ? InstantWarManager->CharacterGMPool :
        (n == 1 ? InstantWarManager->CharacterCapellaLobbyPool :
        InstantWarManager->CharacterProcyonLobbyPool), &CharacterPoolIndex);

    WarEntry->ID = Character->ID;
    WarEntry->PartyID = Character->PartyID;
    WarEntry->CharacterIndex = Character->CharacterIndex;
    WarEntry->Nation = Character->Data.StyleInfo.Nation;
    WarEntry->WorldIdx = Character->Data.Info.WorldIndex;
    WarEntry->EntryOrder = DictionarySize(n == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex :
        (n == 1 ? InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex :
            InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex))+1;

    DictionaryInsert(n == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex :
        (n == 1 ? InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex :
            InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex), &Character->CharacterIndex, &CharacterPoolIndex, sizeof(Int));
    return WarEntry;
}

RTCharacterWarEntryRef RTInstantWarGetCharacterLobbyByIndex(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    UInt32 CharacterIndex,
    UInt8 nation
) {
    Int* CharacterPoolIndex = DictionaryLookup(nation == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex :
        (nation == 1 ? InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex :
            InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex), &CharacterIndex);
    if (!CharacterPoolIndex) return NULL;

    return (RTCharacterRef)MemoryPoolFetch(nation == 3 ? InstantWarManager->CharacterGMPool :
        (nation == 1 ? InstantWarManager->CharacterCapellaLobbyPool :
            InstantWarManager->CharacterProcyonLobbyPool), *CharacterPoolIndex);
}

RTCharacterWarEntryRef RTInstantWarGetCharacterBfByIndex(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    UInt32 CharacterIndex,
    UInt8 nation
) {
    Int* CharacterPoolIndex = DictionaryLookup(nation == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex :
        (nation == 1 ? InstantWarManager->IndexToCharacterCapellaBfPoolIndex :
            InstantWarManager->IndexToCharacterProcyonBfPoolIndex), &CharacterIndex);
    if (!CharacterPoolIndex) return NULL;

    return (RTCharacterRef)MemoryPoolFetch(nation == 3 ? InstantWarManager->CharacterGMPool :
        (nation == 1 ? InstantWarManager->CharacterCapellaBfPool :
            InstantWarManager->CharacterProcyonBfPool), *CharacterPoolIndex);
}

Void RTInstantWarManagerDestroyCharacter(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    RTCharacterRef Character
) {
    UInt8 n = Character->Data.StyleInfo.Nation;
    if (Character->Data.Info.WorldIndex == InstantWarManager->WarMapRef->LobbyWorldId) {

        Int* CharacterPoolIndex = DictionaryLookup(n == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex :
            (n == 1 ? InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex :
                InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex), &Character->CharacterIndex);
        assert(CharacterPoolIndex);

        MemoryPoolRelease(n == 3 ? InstantWarManager->CharacterGMPool :
            (n == 1 ? InstantWarManager->CharacterCapellaLobbyPool :
                InstantWarManager->CharacterProcyonLobbyPool), *CharacterPoolIndex);
        DictionaryRemove(n == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex :
            (n == 1 ? InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex :
                InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex), &Character->CharacterIndex);
    }
    else {
        Int* CharacterPoolIndex = DictionaryLookup(n == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex :
            (n == 1 ? InstantWarManager->IndexToCharacterCapellaBfPoolIndex :
                InstantWarManager->IndexToCharacterProcyonBfPoolIndex), &Character->CharacterIndex);
        assert(CharacterPoolIndex);

        MemoryPoolRelease(n == 3 ? InstantWarManager->CharacterGMPool :
            (n == 1 ? InstantWarManager->CharacterCapellaBfPool :
                InstantWarManager->CharacterProcyonBfPool), *CharacterPoolIndex);
        DictionaryRemove(n == 3 ? InstantWarManager->IndexToCharacterGMPoolIndex :
            (n == 1 ? InstantWarManager->IndexToCharacterCapellaBfPoolIndex :
                InstantWarManager->IndexToCharacterProcyonBfPoolIndex), &Character->CharacterIndex);
    }

}

Void RTInstantWarNotifyWorldInOutPool(
    RTRuntimeRef Runtime,
    NOTIFICATION_DATA_USERWORLDINOUT* Notification,
    DictionaryRef Dictionary
) {
    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(Dictionary);
    while (Iterator.Key) {
        UInt32 CharacterIndex = *(Int*)Iterator.Key;
        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, CharacterIndex);
        if (Character) {
            RTNotificationDispatchToCharacter(Notification, Character);
        }
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }
}

Void RTInstantWarNotifyWorldInOut(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    RTCharacterRef Character
) {

    Int8 Nation = Character->Data.StyleInfo.Nation;

    NOTIFICATION_DATA_USERWORLDINOUT* Notification = RTNotificationInit(USERWORLDINOUT);
    Notification->countCapellaInLobby = DictionarySize(InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex);
    Notification->countProcyonInLobby = DictionarySize(InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex);
    Notification->countCapellaInBf = DictionarySize(InstantWarManager->IndexToCharacterCapellaBfPoolIndex);
    Notification->countProcyonInBf = DictionarySize(InstantWarManager->IndexToCharacterProcyonBfPoolIndex);
    Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
    Notification->Entity = Character->ID;
    Notification->Level = Character->Data.Info.Level;
    Notification->Exp = Character->Data.Info.Exp;
    Notification->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
    Notification->Nation = Nation;
    Notification->worldIdx = Character->Data.Info.WorldIndex;

    if (Character->Data.Info.WorldIndex == InstantWarManager->WarMapRef->LobbyWorldId) {
        RTCharacterWarEntryRef WarEntry = RTInstantWarGetCharacterLobbyByIndex(Runtime, InstantWarManager, Character->CharacterIndex, Nation);
        if (WarEntry) {
            Notification->entryOrder = WarEntry->EntryOrder;
            Notification->entered = 1;
        }
    }
    else if(Character->Data.Info.WorldIndex == InstantWarManager->WarMapRef->WorldId) {
        Notification->entryOrder = 0;
        Notification->entered = 1;
    }

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(InstantWarManager->IndexToCharacterCapellaLobbyPoolIndex);
    while (Iterator.Key) {
        UInt32 CharacterIndex = *(Int*)Iterator.Key;
        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, CharacterIndex);
        if (Character) {
            RTNotificationDispatchToCharacter(Notification, Character);
        }
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    Iterator = DictionaryGetKeyIterator(InstantWarManager->IndexToCharacterCapellaBfPoolIndex);
    while (Iterator.Key) {
        UInt32 CharacterIndex = *(Int*)Iterator.Key;
        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, CharacterIndex);
        if (Character) {
            RTNotificationDispatchToCharacter(Notification, Character);
        }
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    Iterator = DictionaryGetKeyIterator(InstantWarManager->IndexToCharacterProcyonLobbyPoolIndex);
    while (Iterator.Key) {
        UInt32 CharacterIndex = *(Int*)Iterator.Key;
        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, CharacterIndex);
        if (Character) {
            RTNotificationDispatchToCharacter(Notification, Character);
        }
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    Iterator = DictionaryGetKeyIterator(InstantWarManager->IndexToCharacterProcyonBfPoolIndex);
    while (Iterator.Key) {
        UInt32 CharacterIndex = *(Int*)Iterator.Key;
        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, CharacterIndex);
        if (Character) {
            RTNotificationDispatchToCharacter(Notification, Character);
        }
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

}


Void RTInstantWarManagerDestroy(
    RTInstantWarManagerRef InstantWarManager
) {
    // TODO: Check if all running PvPs have to be cleaned up!!!
    AllocatorDeallocate(InstantWarManager->Allocator, InstantWarManager);
}

Void RTInstantWarManagerUpdate(
    RTInstantWarManagerRef InstantWarManager,
    UInt64 WorldType
) {

    RTRuntimeRef Runtime = InstantWarManager->Runtime;
    
    return 0;

error:
    return 1;

}

