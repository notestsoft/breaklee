#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void ServerSyncDB(
    ServerRef Server,
    ServerContextRef Context,
    Bool Force
);

Void ServerRuntimeOnEvent(
    RTRuntimeRef Runtime,
    RTEventRef Event,
    Void* UserData
);

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Context,
    UInt32 AuthKey,
    UInt16 EntityID
);

ClientContextRef ServerGetClientByEntity(
    ServerContextRef Context,
    RTEntityID Entity
);

EXTERN_C_END
