#pragma once

#include "Base.h"
#include "Socket.h"

EXTERN_C_BEGIN

typedef struct _Server* ServerRef;

typedef Void (*ServerUpdateCallback)(
    ServerRef Server,
    Void *ServerContext
);

typedef Void (*ServerConnectionCallback)(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
);

typedef Void (*ServerPacketCallback)(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext,
    Void *Packet
);

ServerRef ServerCreate(
    AllocatorRef Allocator,
    ServerUpdateCallback OnUpdate,
    Void *ServerContext
);

Void ServerDestroy(
    ServerRef Server
);

Timestamp ServerGetTimestamp(
    ServerRef Server
);

SocketRef ServerCreateSocket(
    ServerRef Server,
    UInt32 SocketFlags,
    CString SocketHost,
    UInt16 SocketPort,
    Index ConnectionContextSize,
    Index MaxConnectionCount,
    ServerConnectionCallback OnConnect,
    ServerConnectionCallback OnDisconnect
);

Void ServerSocketRegisterPacketCallback(
    ServerRef Server,
    SocketRef Socket,
    Index Command,
    ServerPacketCallback Callback
);

Void ServerRun(
	ServerRef Server
);

EXTERN_C_END
