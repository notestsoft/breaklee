#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void IPCSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
);

Void IPCSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
);

EXTERN_C_END
