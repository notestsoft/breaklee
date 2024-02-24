#pragma once

#include "Base.h"
#include "Context.h"
#include "IPCProtocol.h"
#include "Server.h"

EXTERN_C_BEGIN

#ifndef IPC_PROCEDURE_BINDING
#define IPC_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__)  \
Void __NAME__(                                                      \
    ServerRef Server,                                               \
    ServerContextRef Context,                                       \
    SocketRef Socket,                                               \
    SocketConnectionRef Connection,                                 \
    ClientContextRef Client,                                        \
    __PROTOCOL__* Packet                                            \
)
#endif

#define IPC_AUTH_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__) \
IPC_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__);

#define IPC_WORLD_PROCEDURE(__NAME__, __COMMAND__, __PROTOCOL__) \
IPC_PROCEDURE_BINDING(__NAME__, __COMMAND__, __PROTOCOL__);

#include "IPCProcDefinition.h"

EXTERN_C_END
