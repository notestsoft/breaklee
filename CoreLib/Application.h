#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef Void(*ApplicationExitCallback)(
    Int32 Signal,
    Void* Context
);

Void ApplicationRegisterExitCallback(
    ApplicationExitCallback Callback,
    Void* Context
);

Void ApplicationRegisterCrashCallback(
    ApplicationExitCallback Callback,
    Void* Context
);

Bool ApplicationIsShuttingDown();

EXTERN_C_END