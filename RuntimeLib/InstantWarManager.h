#pragma once
#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

struct _RTCharacterWarEntry {
    RTEntityID ID;
    RTEntityID PartyID;
    UInt32 CharacterIndex;
    UInt8 Nation;
    UInt32 WorldIdx;
    UInt32 EntryOrder;
};

struct _RTInstantWarManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    Timestamp InstantWarTimestamp;
    UInt64 WorldType;
    RTDataWarMapRef WarMapRef;


    MemoryPoolRef CharacterCapellaLobbyPool;
    DictionaryRef IndexToCharacterCapellaLobbyPoolIndex;

    MemoryPoolRef CharacterCapellaBfPool;
    DictionaryRef IndexToCharacterCapellaBfPoolIndex;

    MemoryPoolRef CharacterProcyonLobbyPool;
    DictionaryRef IndexToCharacterProcyonLobbyPoolIndex;

    MemoryPoolRef CharacterProcyonBfPool;
    DictionaryRef IndexToCharacterProcyonBfPoolIndex;

    MemoryPoolRef CharacterGMPool;
    DictionaryRef IndexToCharacterGMPoolIndex;


    Int32 LobbyTimeAttackSec;
    Int32 LobbyTimeAttackRemaining;
    UInt8 LobbyIsTimeAttackEnabled;
    Int32 BfTimeAttackSec;
    Int32 BfTimeAttackRemaining;
    UInt8 BfIsTimeAttackEnabled;
    Int32 CapellaScore;
    Int32 ProcyonScore;
    Int32 CapellaPoints;
    Int32 ProcyonPoints;
    Int32 EntryLimitPerNation;
    Int32 CapellaBattleFieldTicketCount;
    Int32 ProcyonBattleFieldTicketCount;
};

RTInstantWarManagerRef RTInstantWarManagerCreate(
    RTRuntimeRef Runtime,
    UInt64 WorldType
);

Void RTInstantWarUpdate(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager
);

RTCharacterWarEntryRef RTInstantWarCharacterInitialize(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    RTCharacterRef Character
);

RTCharacterWarEntryRef RTInstantWarGetCharacterLobbyByIndex(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    UInt32 CharacterIndex,
    UInt8 nation
);

RTCharacterWarEntryRef RTInstantWarGetCharacterBfByIndex(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    UInt32 CharacterIndex,
    UInt8 nation
);

Void RTInstantWarManagerDestroyCharacter(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    RTCharacterRef Character
);

Void RTInstantWarNotifyWorldInOut(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    RTCharacterRef Character
);

Void RTInstantWarManagerDestroy(
    RTInstantWarManagerRef InstantWarManager
);

Void RTInstantWarManagerUpdate(
    RTInstantWarManagerRef InstantWarManager,
    UInt64 WorldType
);



EXTERN_C_END
