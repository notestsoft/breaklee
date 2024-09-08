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
    RTRuntimeRef Runtime,                               \
    RTCharacterRef Character,                           \
    IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__* Packet \
)
#endif

#define IPC_A2W_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(A2W, __NAME__);
#include "IPCCommands.h"

#define IPC_C2W_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(C2W, __NAME__);
#include "IPCCommands.h"

#define IPC_D2W_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(D2W, __NAME__);
#include "IPCCommands.h"

#define IPC_L2W_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(L2W, __NAME__);
#include "IPCCommands.h"

#define IPC_M2W_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(M2W, __NAME__);
#include "IPCCommands.h"

#define IPC_W2W_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(W2W, __NAME__);
#include "IPCCommands.h"

#define IPC_P2W_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(P2W, __NAME__);
#include "IPCCommands.h"

#define IPC_M2N_COMMAND(__NAME__) \
IPC_PROCEDURE_BINDING(M2N, __NAME__);
#include "IPCCommands.h"

EXTERN_C_END
