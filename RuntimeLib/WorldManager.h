#pragma once

#include "Base.h"
#include "World.h"

EXTERN_C_BEGIN

struct _RTWorldManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    Int32 MaxWorldDataCount;
    Int32 MaxGlobalWorldContextCount;
    Int32 MaxPartyWorldContextCount;
    Int32 MaxCharacterCount;
    MemoryPoolRef WorldDataPool;
    MemoryPoolRef GlobalWorldContextPool;
    MemoryPoolRef PartyWorldContextPool;
    MemoryPoolRef CharacterContextPool;
    DictionaryRef IndexToGlobalWorldContextPoolIndex;
    DictionaryRef PartyToWorldContextPoolIndex;
    DictionaryRef IndexToCharacterContextPoolIndex;
};

RTWorldManagerRef RTWorldManagerCreate(
    RTRuntimeRef Runtime,
    Int32 MaxWorldDataCount,
    Int32 MaxGlobalWorldContextCount,
    Int32 MaxPartyWorldContextCount,
    Int32 MaxCharacterCount
);

Void RTWorldManagerDestroy(
    RTWorldManagerRef WorldManager
);

Void RTWorldManagerUpdate(
    RTWorldManagerRef WorldManager
);

RTWorldDataRef RTWorldDataCreate(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
);

Bool RTWorldDataExists(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
);

RTWorldDataRef RTWorldDataGet(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
);

RTWorldContextRef RTWorldContextGet(
    RTWorldManagerRef WorldManager,
    Int WorldIndex,
    Int WorldInstanceIndex
);

RTWorldContextRef RTWorldContextCreateGlobal(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
);

RTWorldContextRef RTWorldContextGetGlobal(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
);

Void RTWorldContextDestroyGlobal(
    RTWorldManagerRef WorldManager,
    Int WorldIndex
);

RTWorldContextRef RTWorldContextCreateParty(
    RTWorldManagerRef WorldManager,
    Int WorldIndex,
    Int64 DungeonIndex,
    RTEntityID Party
);

Bool RTWorldContextPartyIsFull(
    RTWorldManagerRef WorldManager
);

Bool RTWorldContextHasParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
);

RTWorldContextRef RTWorldContextGetParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
);

Void RTWorldContextDestroyParty(
    RTWorldManagerRef WorldManager,
    RTEntityID Party
);

Int RTWorldContextGetPartyInstanceCount(
    RTWorldManagerRef WorldManager
);

RTCharacterRef RTWorldManagerCreateCharacter(
    RTWorldManagerRef WorldManager,
    UInt32 CharacterIndex
);

Bool RTWorldManagerCharacterPoolIsFull(
    RTWorldManagerRef WorldManager
);

Bool RTWorldManagerHasCharacter(
    RTWorldManagerRef WorldManager,
    RTEntityID Entity
);

RTCharacterRef RTWorldManagerGetCharacter(
    RTWorldManagerRef WorldManager,
    RTEntityID Entity
);

RTCharacterRef RTWorldManagerGetCharacterByIndex(
    RTWorldManagerRef WorldManager,
    UInt32 CharacterIndex
);

Void RTWorldManagerDestroyCharacter(
    RTWorldManagerRef WorldManager,
    UInt32 CharacterIndex
);

EXTERN_C_END
