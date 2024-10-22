#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

ClientContextRef ServerGetClientByIndex(
    ServerContextRef Context,
    UInt32 CharacterIndex
);

EXTERN_C_END
