#pragma once

#include "Base.h"

EXTERN_C_BEGIN

enum {
    LOG_LEVEL_INFO       = 1 << 0,
    LOG_LEVEL_WARNING    = 1 << 1,
    LOG_LEVEL_ERROR      = 1 << 2,
};

typedef Void (*DiagnosticHandler)(
    UInt32 Level,
    CString Message,
    Void* Context
);

Void DiagnosticSetDefaultHandler(
    DiagnosticHandler Handler,
    Void* Context
);

Void DiagnosticSetOutputStream(
    FILE *Stream
);

Void DiagnosticCreateLogFile(
    CString Namespace
);

Void DiagnosticSetLevelFilter(
    UInt32 LevelMask
);

Void DiagnosticTeardown();

Void LogMessage(
    UInt32 Level,
    CString Message
);

Void LogMessageFormat(
    UInt32 Level,
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

ATTRIBUTE_NORETURN Void FatalError(
    CString Message
);

Void FatalErrorFormat(
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

EXTERN_C_END
