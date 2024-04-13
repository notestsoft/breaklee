#pragma once

#include "Base.h"
#include "IPCSocket.h"
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
    IPCNodeID NodeID,
    CString Host,
    UInt16 Port,
    Timestamp Timeout,
    Index ReadBufferSize,
    Index WriteBufferSize,
    ServerUpdateCallback OnUpdate,
    Void* ServerContext
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
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Index ReadBufferSize,
    Index WriteBufferSize,
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
