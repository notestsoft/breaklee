#include "Application.h"
#include "Diagnostic.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

struct _ApplicationExitHandler {
    ApplicationExitCallback Callback;
    Void* Context;
    Bool IsSignalActive;
};

static struct _ApplicationExitHandler kApplicationExitHandler = { NULL, NULL };
static struct _ApplicationExitHandler kApplicationCrashHandler = { NULL, NULL };

static volatile Bool kApplicationIsShuttingDown = false;

Void OnExitSignal(
    Int32 Signal
) {
    if (kApplicationIsShuttingDown) return;
    kApplicationIsShuttingDown = true;

    if (kApplicationExitHandler.Callback) {
        kApplicationExitHandler.Callback(Signal, kApplicationExitHandler.Context);
    }

    signal(Signal, SIG_DFL);
    raise(Signal);
}

Void OnCrashSignal(
    Int32 Signal
) {
    if (kApplicationCrashHandler.Callback) {
        kApplicationCrashHandler.Callback(Signal, kApplicationCrashHandler.Context);
    }

    signal(Signal, SIG_DFL);
    raise(Signal);
}

Void ApplicationRegisterExitCallback(
    ApplicationExitCallback Callback, 
    Void* Context
) {
    kApplicationExitHandler.Callback = Callback;
    kApplicationExitHandler.Context = Context;

    if (kApplicationExitHandler.IsSignalActive) return;
    kApplicationExitHandler.IsSignalActive = true;

    signal(SIGINT, OnExitSignal);
    signal(SIGTERM, OnExitSignal);

#ifdef _WIN32
    signal(SIGBREAK, OnExitSignal);
#endif
}

Void ApplicationRegisterCrashCallback(
    ApplicationExitCallback Callback,
    Void* Context
) {
    kApplicationCrashHandler.Callback = Callback;
    kApplicationCrashHandler.Context = Context;

    if (kApplicationCrashHandler.IsSignalActive) return;
    kApplicationCrashHandler.IsSignalActive = true;

    signal(SIGSEGV, OnCrashSignal);
    signal(SIGABRT, OnCrashSignal);
    signal(SIGFPE, OnCrashSignal);
    signal(SIGILL, OnCrashSignal);
}

Bool ApplicationIsShuttingDown() {
    return kApplicationIsShuttingDown;
}
