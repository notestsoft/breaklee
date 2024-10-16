#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

RTOptionPoolManagerRef RTOptionPoolManagerCreate(
	AllocatorRef Allocator
);

Void RTOptionPoolManagerDestroy(
	RTOptionPoolManagerRef OptionPoolManager
);

Void RTOptionPoolManagerAddItemLevel(
    RTOptionPoolManagerRef OptionPoolManager,
    Index PoolIndex,
    Int32 Level,
    Float64 Rate
);

Void RTOptionPoolManagerAddEpicLevel(
    RTOptionPoolManagerRef OptionPoolManager,
    Index PoolIndex,
    Int32 Level,
    Float64 Rate
);

Void RTOptionPoolManagerAddEpicOption(
    RTOptionPoolManagerRef OptionPoolManager,
    Index PoolIndex,
    Int32 ItemType,
    Int32 Level,
    Int32 ForceIndex,
    Float64 Rate
);

Void RTOptionPoolManagerAddForceSlot(
    RTOptionPoolManagerRef OptionPoolManager,
    Index PoolIndex,
    Int32 Count,
    Float64 Rate
);

Void RTOptionPoolManagerAddForceOptionSlot(
    RTOptionPoolManagerRef OptionPoolManager,
    Index PoolIndex,
    Int32 Count,
    Float64 Rate
);

Void RTOptionPoolManagerAddForceOption(
    RTOptionPoolManagerRef OptionPoolManager,
    Index PoolIndex,
    Int32 ItemType,
    Int32 ForceIndex,
    Float64 Rate
);

Void RTOptionPoolManagerCalculateOptions(
    RTRuntimeRef Runtime,
    RTOptionPoolManagerRef OptionPoolManager,
    Index OptionPoolIndex,
    RTDropResultRef DropResult
);

EXTERN_C_END
