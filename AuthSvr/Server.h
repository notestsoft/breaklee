#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

MasterContextRef ServerGetMaster(
    ServerRef Server,
    ServerContextRef Context,
    Int32 ServerID
);

MasterContextWorldRef ServerGetWorld(
    ServerRef Server,
    ServerContextRef Context,
    Int32 ServerID,
    Int32 WorldID
);

EXTERN_C_END
