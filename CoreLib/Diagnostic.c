#include "Diagnostic.h"
#include "String.h"
#include "FileIO.h"
#include "Util.h"

struct _DiagnosticEngine {
    FILE* Output;
    FILE* Error;
    Int32 Level;
    DiagnosticHandler Handler;
    Void* Context;
    FILE* Handle;
    Char Colors[LOG_LEVEL_COUNT];
    CString Labels[LOG_LEVEL_COUNT];
    Char Buffer[4096];
};

static struct _DiagnosticEngine kDiagnosticEngine = {
    .Output = NULL,
    .Level = LOG_LEVEL_TRACE,
    .Handler = NULL,
    .Context = NULL,
    .Handle = NULL,
    .Colors = { 30, 41, 31, 33, 90, 46 },
    .Labels = { "", "FATAL", "ERROR", "WARN", "INFO", "TRACE" },
    .Buffer = { 0 }
};

Void _DefaultDiagnosticHandler(
    FILE* Output,
    Int32 Level,
    CString Message,
    Void* Context
) {
    time_t NowTime = time(NULL);
    struct tm NowTm = { 0 };
    gmtime_s(&NowTm, &NowTime);

    fprintf(
        Output,
        "\033[%dm[%d-%02d-%02d %02d:%02d:%02d] [%s] : %s\033[0m\n",
        kDiagnosticEngine.Colors[Level],
        NowTm.tm_year + 1900, 
        NowTm.tm_mon + 1, 
        NowTm.tm_mday,
        NowTm.tm_hour, 
        NowTm.tm_min,
        NowTm.tm_sec,
        kDiagnosticEngine.Labels[Level],
        Message
    );

    if (Output != stdout && Output != stderr) {
        FILE* Stdout = (Level <= LOG_LEVEL_ERROR) ? stderr : stdout;

        fprintf(
            Stdout,
            "\033[%dm[%d-%02d-%02d %02d:%02d:%02d] [%s] : %s\033[0m\n",
            kDiagnosticEngine.Colors[Level],
            NowTm.tm_year + 1900,
            NowTm.tm_mon + 1,
            NowTm.tm_mday,
            NowTm.tm_hour,
            NowTm.tm_min,
            NowTm.tm_sec,
            kDiagnosticEngine.Labels[Level],
            Message
        );
    }
}

Void DiagnosticSetup(
    Int32 Level,
    FILE* Output,
    DiagnosticHandler Handler,
    Void* Context
) {
    kDiagnosticEngine.Output = (Output) ? Output : stdout;
    kDiagnosticEngine.Error = (Output) ? Output : stderr;
    kDiagnosticEngine.Level = Level;
    kDiagnosticEngine.Handler = (Handler) ? Handler : _DefaultDiagnosticHandler;
    kDiagnosticEngine.Context = Context;
    kDiagnosticEngine.Handle = NULL;
}

Void DiagnosticSetupLogFile(
    CString Namespace,
    Int32 Level,
    DiagnosticHandler Handler,
    Void* Context
) {
    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString FilePath = CStringFormat("%s\\Logs\\%s_%d.log", WorkingDirectory, Namespace, PlatformGetTickCount());

    if (!DirectoryCreate("Logs")) {
        Error("Error creating directory!\n");
        return;
    }

    FILE* Output = fopen(FilePath, "w+");
    kDiagnosticEngine.Output = (Output) ? Output : stdout;
    kDiagnosticEngine.Error = (Output) ? Output : stderr;
    kDiagnosticEngine.Level = Level;
    kDiagnosticEngine.Handler = (Handler) ? Handler : _DefaultDiagnosticHandler;
    kDiagnosticEngine.Context = Context;
    kDiagnosticEngine.Handle = Output;
}

Void DiagnosticTeardown() {
    if (!kDiagnosticEngine.Handle) return;

    fclose(kDiagnosticEngine.Handle);
    kDiagnosticEngine.Output = stdout;
    kDiagnosticEngine.Handle = NULL;
}

static inline Void _Log(
    FILE* Output,
    Int32 Level,
    CString Message
) {
    if (kDiagnosticEngine.Level < Level) return;

    kDiagnosticEngine.Handler(
        Output,
        Level,
        Message,
        kDiagnosticEngine.Context
    );
}

Void Fatal(
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(kDiagnosticEngine.Buffer, Format, ArgumentPointer);
    va_end(ArgumentPointer);

    _Log(kDiagnosticEngine.Error, LOG_LEVEL_FATAL, kDiagnosticEngine.Buffer);
    exit(EXIT_FAILURE);
}

Void Error(
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(kDiagnosticEngine.Buffer, Format, ArgumentPointer);
    va_end(ArgumentPointer);

    _Log(kDiagnosticEngine.Error, LOG_LEVEL_ERROR, kDiagnosticEngine.Buffer);
}

Void Warn(
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(kDiagnosticEngine.Buffer, Format, ArgumentPointer);
    va_end(ArgumentPointer);

    _Log(kDiagnosticEngine.Output, LOG_LEVEL_WARN, kDiagnosticEngine.Buffer);
}

Void Info(
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(kDiagnosticEngine.Buffer, Format, ArgumentPointer);
    va_end(ArgumentPointer);

    _Log(kDiagnosticEngine.Output, LOG_LEVEL_INFO, kDiagnosticEngine.Buffer);
}

Void Trace(
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(kDiagnosticEngine.Buffer, Format, ArgumentPointer);
    va_end(ArgumentPointer);

    _Log(kDiagnosticEngine.Output, LOG_LEVEL_TRACE, kDiagnosticEngine.Buffer);
}
