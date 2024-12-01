#include "Application.h"
#include "Diagnostic.h"
#include "String.h"
#include "FileIO.h"
#include "Util.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#else
#include <execinfo.h>
#include <pthread.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_init.h>
#endif
#endif

Void _DefaultDiagnosticHandler(
    FILE* Output,
    Int32 Level,
    CString Message,
    Void* Context
);

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
    .Handler = _DefaultDiagnosticHandler,
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

#ifdef _WIN32
    gmtime_s(&NowTm, &NowTime);
    DWORD ThreadID = GetCurrentThreadId();
#elif __APPLE__
    gmtime_r(&NowTime, &NowTm);
    mach_port_t ThreadID = mach_thread_self();
    mach_port_deallocate(mach_task_self(), ThreadID);
#else
    gmtime_r(&NowTime, &NowTm);
    pthread_t ThreadID = pthread_self();
#endif

    if (Output) {
        fprintf(
            Output,
            "\033[%dm[%d-%02d-%02d %02d:%02d:%02d][Thread: %lu] [%s] : %s\033[0m\n",
            kDiagnosticEngine.Colors[Level],
            NowTm.tm_year + 1900,
            NowTm.tm_mon + 1,
            NowTm.tm_mday,
            NowTm.tm_hour,
            NowTm.tm_min,
            NowTm.tm_sec,
            (unsigned long)ThreadID,
            kDiagnosticEngine.Labels[Level],
            Message
        );
    }

    if (Output != stdout && Output != stderr) {
        FILE* Stdout = (Level <= LOG_LEVEL_ERROR) ? stderr : stdout;

        fprintf(
            Stdout,
            "\033[%dm[%d-%02d-%02d %02d:%02d:%02d][Thread: %lu] [%s] : %s\033[0m\n",
            kDiagnosticEngine.Colors[Level],
            NowTm.tm_year + 1900,
            NowTm.tm_mon + 1,
            NowTm.tm_mday,
            NowTm.tm_hour,
            NowTm.tm_min,
            NowTm.tm_sec,
            (unsigned long)ThreadID,
            kDiagnosticEngine.Labels[Level],
            Message
        );
    }
}


Void _OnExitDefault(
    Int32 Signal,
    Void* Context
) {
    DiagnosticTeardown();
}

Void _OnCrashDefault(
    Int32 Signal,
    Void* Context
) {
    CString SignalDescription = "Unknown Signal";
    switch (Signal) {
    case SIGSEGV: SignalDescription = "SIGSEGV (Segmentation Fault)"; break;
    case SIGABRT: SignalDescription = "SIGABRT (Aborted)"; break;
    case SIGFPE:  SignalDescription = "SIGFPE (Floating Point Exception)"; break;
    case SIGILL:  SignalDescription = "SIGILL (Illegal Instruction)"; break;
    default: break;
    }

    Error("Caught signal: %s (%d)", SignalDescription, Signal);

#ifdef _WIN32
    Void* BackTrace[30];
    HANDLE Process = GetCurrentProcess();
    SymInitialize(Process, NULL, TRUE);

    USHORT Frames = CaptureStackBackTrace(0, 30, BackTrace, NULL);
    Error("Stack trace (%d frames):", Frames);

    SYMBOL_INFO_PACKAGE SymbolInfo;
    SYMBOL_INFO* Symbol = &SymbolInfo.si;
    Symbol->MaxNameLen = MAX_SYM_NAME;
    Symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (USHORT Index = 0; Index < Frames; Index += 1) {
        if (SymFromAddr(Process, (DWORD64)(BackTrace[Index]), 0, Symbol)) {
            Error("%u: %s - 0x%0llX", Index, Symbol->Name, Symbol->Address);
        }
        else {
            Error("%u: [unknown]", Index);
        }
    }

    SymCleanup(Process);
#else
    Void* BackTrace[30];
    Int32 Frames = backtrace(BackTrace, 30);
    Error("Stack trace (%d frames):", Frames);

    CString* Symbols = backtrace_symbols(BackTrace, Frames);
    if (Symbols) {
        for (Int Index = 0; Index < Frames; Index += 1) {
            Error("%s", Symbols[Index]);
        }

        free(Symbols);
    }
#endif

    DiagnosticTeardown();

    exit(Signal);
}

Void DiagnosticSetup(
    FILE* Output,
    Int32 Level,
    DiagnosticHandler Handler,
    Void* Context
) {
    kDiagnosticEngine.Output = (Output) ? Output : stdout;
    kDiagnosticEngine.Error = (Output) ? Output : stderr;
    kDiagnosticEngine.Level = Level;
    kDiagnosticEngine.Handler = (Handler) ? Handler : _DefaultDiagnosticHandler;
    kDiagnosticEngine.Context = Context;
    kDiagnosticEngine.Handle = NULL;

    ApplicationRegisterExitCallback(_OnExitDefault, NULL);
    ApplicationRegisterCrashCallback(_OnCrashDefault, NULL);
}

Void DiagnosticSetupLogFile(
    CString Namespace,
    Int32 Level,
    DiagnosticHandler Handler,
    Void* Context
) {
    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString FilePath = CStringFormat(
        "%s%cLogs%c%s_%d.log", 
        WorkingDirectory, 
        PLATFORM_PATH_SEPARATOR, 
        PLATFORM_PATH_SEPARATOR, 
        Namespace, 
        (Int32)PlatformGetTickCount()
    );

    if (!DirectoryCreate("Logs")) {
        Error("Error creating directory!\n");
        return;
    }

    FILE* Output = fopen(FilePath, "w+");
    DiagnosticSetup(Output, Level, Handler, Context);
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
