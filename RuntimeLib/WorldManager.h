#pragma once

#include "Base.h"
#include "World.h"

EXTERN_C_BEGIN

struct _RTWorldManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    Index MaxWorldDataCount;
    Index MaxGlobalWorldContextCount;
    Index MaxPartyWorldContextCount;
    Index MaxCharacterCount;
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
    Index MaxWorldDataCount,
    Index MaxGlobalWorldContextCount,
    Index MaxPartyWorldContextCount,
    Index MaxCharacterCount
);

Void RTWorldManagerDestroy(
    RTWorldManagerRef WorldManager
);

Void RTWorldManagerUpdate(
    RTWorldManagerRef WorldManager
);

RTWorldDataRef RTWorldDataCreate(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
);

Bool RTWorldDataExists(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
);

RTWorldDataRef RTWorldDataGet(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
);

RTWorldContextRef RTWorldContextGet(
    RTWorldManagerRef WorldManager,
    Index WorldIndex,
    Index WorldInstanceIndex
);

RTWorldContextRef RTWorldContextCreateGlobal(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
);

RTWorldContextRef RTWorldContextGetGlobal(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
);

Void RTWorldContextDestroyGlobal(
    RTWorldManagerRef WorldManager,
    Index WorldIndex
);

RTWorldContextRef RTWorldContextCreateParty(
    RTWorldManagerRef WorldManager,
    Index WorldIndex,
    Index DungeonIndex,
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

RTCharacterRef RTWorldManagerCreateCharacter(
    RTWorldManagerRef WorldManager,
    Index CharacterIndex
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
    Index CharacterIndex
);

Void RTWorldManagerDestroyCharacter(
    RTWorldManagerRef WorldManager,
    Index CharacterIndex
);

EXTERN_C_END
