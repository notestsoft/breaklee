#pragma once

#include "Base.h"
#include "Context.h"
#include "IPCProtocol.h"

EXTERN_C_BEGIN

typedef Void(*IPCProcedureCallback)(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    SocketConnectionRef ClientConnection,
    ClientContextRef Client,
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Void* Packet
);

#ifndef IPC_PROCEDURE_BINDING
#define IPC_PROCEDURE_BINDING(__NAMESPACE__, __NAME__)  \
Void IPC_ ## __NAMESPACE__ ## _PROC_ ## __NAME__(       \
    ServerRef Server,                                   \
    ServerContextRef Context,                           \
    IPCSocketRef Socket,                                \
    IPCSocketConnectionRef Connection,                  \
    SocketConnectionRef ClientConnection,               \
    ClientContextRef Client,                            \
    IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__* Packet \
)
#endif

#define IPC_W2A_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(W2A, __NAME__);
#include "IPCCommands.h"

#define IPC_D2A_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(D2A, __NAME__);
#include "IPCCommands.h"

EXTERN_C_END
