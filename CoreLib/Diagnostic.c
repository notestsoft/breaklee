#include "Diagnostic.h"
#include "String.h"
#include "FileIO.h"
#include "Util.h"

static FILE* kDiagnosticOutputStream = NULL;

Void _DefaultDiagnosticHandler(
    UInt32 Level,
    CString Message,
    Void* Context
) {
    FILE* Stream = kDiagnosticOutputStream;
    if (!Stream) Stream = stdout;

    switch (Level) {
        case LOG_LEVEL_INFO:     fprintf(Stream, "\033[92m[INFO] %s\033[0m\n", Message); break;
        case LOG_LEVEL_WARNING:  fprintf(Stream, "\033[33m[WARNING] %s\033[0m\n", Message); break;
        case LOG_LEVEL_ERROR:    fprintf(Stream, "\033[31m[ERROR] %s\033[0m\n", Message); break;
        default:                 fprintf(Stream, "\033[37m%s\033[0m\n", Message); break;
    }

    if (Stream != stdout) {
        switch (Level) {
        case LOG_LEVEL_INFO:     fprintf(stdout, "\033[92m[INFO] %s\033[0m\n", Message); break;
        case LOG_LEVEL_WARNING:  fprintf(stdout, "\033[33m[WARNING] %s\033[0m\n", Message); break;
        case LOG_LEVEL_ERROR:    fprintf(stdout, "\033[31m[ERROR] %s\033[0m\n", Message); break;
        default:                 fprintf(stdout, "\033[37m%s\033[0m\n", Message); break;
        }
    }
}

static struct {
    UInt32 LevelMask;
    DiagnosticHandler Handler;
    Void* Context;
    Char FormatBuffer[0xFFFF];
} kDiagnosticEngine = { 0xFFFFFFFF, &_DefaultDiagnosticHandler, NULL, 0 };

Void DiagnosticSetDefaultHandler(
    DiagnosticHandler Handler,
    Void* Context
) {
    if (Handler) {
        kDiagnosticEngine.Handler = Handler;
        kDiagnosticEngine.Context = NULL;
    } else {
        kDiagnosticEngine.Handler = &_DefaultDiagnosticHandler;
        kDiagnosticEngine.Context = Context;
    }
}

Void DiagnosticSetOutputStream(
    FILE* Stream
) {
    kDiagnosticOutputStream = Stream;
}

Void DiagnosticCreateLogFile(
    CString Namespace
) {
    Char Buffer[MAX_PATH] = { 0 };
    CString WorkingDirectory = PathGetCurrentDirectory(Buffer, MAX_PATH);
    CString FilePath = CStringFormat("%s\\Logs\\%s_%d.log", WorkingDirectory, Namespace, PlatformGetTickCount());

    if (!DirectoryCreate("Logs")) {
        printf("Error creating directory!\n");
        return EXIT_FAILURE;
    }

    FILE* FileStream = fopen(FilePath, "w+");
    DiagnosticSetOutputStream(FileStream);
}

Void DiagnosticSetLevelFilter(
    UInt32 LevelMask
) {
    kDiagnosticEngine.LevelMask = LevelMask;
}

Void DiagnosticTeardown() {
    if (kDiagnosticOutputStream) fclose(kDiagnosticOutputStream);
}

Void LogMessage(
    UInt32 Level,
    CString Message
) {
    if (!(Level & kDiagnosticEngine.LevelMask)) return;

    kDiagnosticEngine.Handler(Level, Message, kDiagnosticEngine.Context);
}

Void LogMessageFormat(
    UInt32 Level,
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(&kDiagnosticEngine.FormatBuffer[0], Format, ArgumentPointer);
    va_end(ArgumentPointer);

    LogMessage(Level, kDiagnosticEngine.FormatBuffer);
}

Void FatalError(
    CString Message
) {
    kDiagnosticEngine.Handler(LOG_LEVEL_ERROR, Message, kDiagnosticEngine.Context);
    exit(EXIT_FAILURE);
}

Void FatalErrorFormat(
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(&kDiagnosticEngine.FormatBuffer[0], Format, ArgumentPointer);
    va_end(ArgumentPointer);

    FatalError(kDiagnosticEngine.FormatBuffer);
}
