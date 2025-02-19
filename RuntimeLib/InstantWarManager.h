#pragma once
#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

struct _RTInstantWarManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    UInt64 WorldType;
    UInt64 EntryValue;
    RTDataWarMapRef WarMapRef;
};

RTInstantWarManagerRef RTInstantWarManagerCreate(
    RTRuntimeRef Runtime,
    UInt64 WorldType
);

Void RTInstantWarSetPosition(
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
