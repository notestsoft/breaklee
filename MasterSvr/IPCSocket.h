#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void AuthSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
);

Void AuthSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
);

Void WorldSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
);

Void WorldSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
);

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

Void OnIPCConnect(
    ServerRef Server,
    Void* ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void* ConnectionContext,
    Void* Data
);

Void OnIPCRoute(
    ServerRef Server,
    Void* ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void* ConnectionContext,
    Void* Data
);

EXTERN_C_END
