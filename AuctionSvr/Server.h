#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void SendEnvironmentNotification(
    ServerContextRef Context,
    SocketConnectionRef Connection
);

ClientContextRef ServerGetClientByIndex(
    ServerContextRef Context,
    UInt32 CharacterIndex,
    Int32 WorldServerIndex
);

EXTERN_C_END
