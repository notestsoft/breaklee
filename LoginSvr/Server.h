#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Context,
    UInt32 AuthKey,
    UInt16 EntityID
);

Void ServerLoadMigrationData(
    ServerConfig Config,
    ServerContextRef Context
);

EXTERN_C_END
