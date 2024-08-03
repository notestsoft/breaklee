#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

UInt8 RTRuntimeGetLevelByExp(
    RTRuntimeRef Runtime,
    UInt8 Level,
    UInt64* Exp
);

UInt64 RTRuntimeGetExpByLevel(
    RTRuntimeRef Runtime,
    UInt8 Level
);

EXTERN_C_END