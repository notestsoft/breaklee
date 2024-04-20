#pragma once

#include "Base.h"
#include "Entity.h"
#include "Party.h"

EXTERN_C_BEGIN

struct _RTPartyManager {
    AllocatorRef Allocator;
    Index MaxPartyCount;
    MemoryPoolRef PartyPool;
    DictionaryRef CharacterToPartyEntity;
};

RTPartyManagerRef RTPartyManagerCreate(
    AllocatorRef Allocator,
    Index MaxPartyCount
);

Void RTPartyManagerDestroy(
    RTPartyManagerRef PartyManager
);

Void RTPartyManagerUpdate(
    RTPartyManagerRef PartyManager
);

EXTERN_C_END
