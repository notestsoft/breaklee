#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Context,
    UInt32 AuthKey,
    UInt16 EntityID
);

EXTERN_C_END
