#pragma once

#include "Base.h"
#include "IPCSocket.h"
#include "PacketLayout.h"
#include "Socket.h"

EXTERN_C_BEGIN

typedef struct _ServerSocketContext* ServerSocketContextRef;
typedef struct _Server* ServerRef;

typedef Void (*ServerUpdateCallback)(
    ServerRef Server,
    Void* UserData
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

typedef Void(*ServerIPCPacketCallback)(
    ServerRef Server,
    Void* ServerContext,
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    Void* ConnectionContext,
    IPCPacketRef Packet
);

typedef UInt16(*PacketGetCommandCallback)(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
);

typedef Int32(*PacketGetLengthCallback)(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
);

struct _ServerSocketContext {
    ServerRef Server;
    SocketRef Socket;
    CString SocketHost;
    UInt16 SocketPort;
    MemoryPoolRef ConnectionContextPool;
    PacketManagerRef PacketManager;
    DictionaryRef CommandRegistry;
    ServerConnectionCallback OnConnect;
    ServerConnectionCallback OnDisconnect;
    PacketGetCommandCallback PacketGetCommand;
    PacketGetLengthCallback PacketGetLength;
    PacketGetLengthCallback PacketGetHeaderLength;
};

struct _Server {
    AllocatorRef Allocator;
    ArrayRef Sockets; // TODO: Replace this with client socket there is no usecase for having many of them..
    SocketRef ClientSocket;
    IPCSocketRef IPCSocket;
    ServerUpdateCallback OnUpdate;
    Void* Userdata;
};

ServerRef ServerCreate(
    AllocatorRef Allocator,
    IPCNodeID NodeID,
    CString Host,
    UInt16 Port,
    Timestamp Timeout,
    Int32 ReadBufferSize,
    Int32 WriteBufferSize,
    Bool LogPackets,
    ServerUpdateCallback OnUpdate,
    Void* ServerContext
);

Void ServerDestroy(
    ServerRef Server
);

SocketRef ServerCreateSocket(
    ServerRef Server,
    UInt32 SocketFlags,
    CString SocketHost,
    UInt16 SocketPort,
    Int ConnectionContextSize,
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Int32 ReadBufferSize,
    Int32 WriteBufferSize,
    Int32 MaxConnectionCount,
    Int32 PacketBufferBacklogSize,
    Bool LogPackets,
    ServerConnectionCallback OnConnect,
    ServerConnectionCallback OnDisconnect
);

Void ServerSocketRegisterPacketCallback(
    ServerRef Server,
    SocketRef Socket,
    Int Command,
    ServerPacketCallback Callback
);

Void ServerSocketLoadScript(
    ServerRef Server,
    SocketRef Socket,
    CString FilePath
);

Void ServerRun(
	ServerRef Server
);

EXTERN_C_END
