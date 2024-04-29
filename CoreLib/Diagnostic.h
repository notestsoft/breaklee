#pragma once

#include "Base.h"

EXTERN_C_BEGIN

enum {
    LOG_LEVEL_NONE  = 0,
    LOG_LEVEL_FATAL = 1,
    LOG_LEVEL_ERROR = 2,
    LOG_LEVEL_WARN  = 3,
    LOG_LEVEL_INFO  = 4,
    LOG_LEVEL_TRACE = 5,

    LOG_LEVEL_COUNT,
};

typedef Void (*DiagnosticHandler)(
    FILE* Output,
    Int32 Level,
    CString Message,
    Void* Context
);

Void DiagnosticSetup(
    FILE* Output,
    Int32 Level,
    DiagnosticHandler Handler,
    Void* Context
);

Void DiagnosticSetupLogFile(
    CString Namespace,
    Int32 Level,
    DiagnosticHandler Handler,
    Void* Context
);

Void DiagnosticTeardown();

ATTRIBUTE_NORETURN Void Fatal(
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

Void Error(
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

Void Warn(
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

Void Info(
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

Void Trace(
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

EXTERN_C_END
