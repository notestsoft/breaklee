#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

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

EXTERN_C_END
