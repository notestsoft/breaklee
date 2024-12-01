#pragma once

#include "Base.h"
#include "IPCNodeID.h"
#include "IPCPacketBuffer.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define IPC_SOCKET_MAX_CONNECTION_COUNT 512
#define IPC_SOCKET_RECONNECT_DELAY      1000
#define IPC_SOCKET_RECV_BUFFER_SIZE     4096
#define IPC_SOCKET_KEEP_ALIVE_TIMEOUT   10

enum {
    IPC_TYPE_ALL        = 0,
    IPC_TYPE_AUCTION    = 1,
    IPC_TYPE_LOGIN      = 2,
    IPC_TYPE_CHAT       = 3,
    IPC_TYPE_MASTER     = 4,
    IPC_TYPE_PARTY      = 5,
    IPC_TYPE_WORLD      = 6,
    IPC_TYPE_MASTERDB   = 7,

    IPC_TYPE_COUNT,
};

enum {
    IPC_COMMAND_REGISTER    = 0,
    IPC_COMMAND_ROUTE       = 1,
};

enum {
    IPC_ROUTE_TYPE_UNICAST   = 0,
    IPC_ROUTE_TYPE_BROADCAST = 1,
};

struct _IPCNodeContext {
    IPCNodeID NodeID;
    Int ConnectionID;
};
typedef struct _IPCNodeContext* IPCNodeContextRef;

struct _IPCPacket {
    UInt32 Length;
    UInt16 Command;
    UInt16 SubCommand;
    UInt8 RouteType;
    IPCNodeID Source;
    Int SourceConnectionID;
    IPCNodeID Target;
    Int TargetConnectionID;
    // UInt8 Data[0];
};
typedef struct _IPCPacket* IPCPacketRef;
typedef struct _IPCSocket* IPCSocketRef;
typedef struct _IPCSocketConnection* IPCSocketConnectionRef;

enum {
    IPC_SOCKET_FLAGS_LISTENER   = 1 << 0,

    IPC_SOCKET_FLAGS_LISTENING  = 1 << 4,
    IPC_SOCKET_FLAGS_CONNECTING = 1 << 5,
    IPC_SOCKET_FLAGS_CONNECTED  = 1 << 6,
};

enum {
    IPC_SOCKET_STATE_DISCONNECTED,
    IPC_SOCKET_STATE_CONNECTING,
    IPC_SOCKET_STATE_CONNECTED,
    IPC_SOCKET_STATE_DISCONNECTING,
};

enum {
    IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED     = 1 << 0,
    IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED_END = 1 << 1,
};

typedef Void (*IPCSocketConnectionCallback)(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
);

typedef Void (*IPCSocketPacketCallback)(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    IPCPacketRef Packet
);

typedef Void (*IPCSocketCommandCallback)(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    IPCPacketRef Packet
);

typedef Void* IPCSocketConnectionIteratorRef;

struct _IPCSocket {
    AllocatorRef Allocator;
    IPCNodeID NodeID;
    uv_loop_t* Loop;
    uv_tcp_t Handle;
    uv_connect_t Connect;
    SocketAddress Address;
    UInt16 ProtocolIdentifier;
    UInt16 ProtocolVersion;
    UInt16 ProtocolExtension;
    Int32 ReadBufferSize;
    Int32 WriteBufferSize;
    Int32 MaxConnectionCount;
    Int NextConnectionID;
    CString Host;
    UInt16 Port;
    Timestamp Timeout;
    Int32 State;
    Bool LogPackets;
    uv_timer_t* ReconnectTimer;
    IPCPacketBufferRef PacketBuffer;
    IndexSetRef ConnectionIndices;
    MemoryPoolRef ConnectionPool; 
    MemoryPoolRef ConnectionContextPool;
    DictionaryRef CommandRegistry;
    DictionaryRef NodeTable;
    Void* Userdata;
};

struct _IPCSocketConnection {
    IPCSocketRef Socket;
    Int ConnectionPoolIndex;
    uv_tcp_t HandleMemory;
    uv_tcp_t* Handle;
    uv_connect_t* ConnectRequest; 
    Char AddressIP[INET6_ADDRSTRLEN];
    Int ID;
    UInt32 Flags;
    IPCPacketBufferRef PacketBuffer;
    MemoryRef RecvBuffer;
    Int32 RecvBufferLength;
    MemoryBufferRef ReadBuffer;
    Void* Userdata;
};

#pragma pack(pop)

IPCSocketRef IPCSocketCreate(
    AllocatorRef Allocator,
    IPCNodeID NodeID,
    UInt32 Flags,
    Int32 ReadBufferSize,
    Int32 WriteBufferSize,
    CString Host,
    UInt16 Port,
    Timestamp Timeout,
    Int32 MaxConnectionCount,
    Bool LogPackets,
    Void* Userdata
);

Void IPCSocketDestroy(
    IPCSocketRef Socket
);

Void IPCSocketRegisterCommandCallback(
    IPCSocketRef Socket,
    Int Command,
    IPCSocketCommandCallback Callback
);

Void IPCSocketSend(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    IPCPacketRef Packet
);

Void IPCSocketUnicast(
    IPCSocketRef Socket,
    Void* Packet
);

Void IPCSocketBroadcast(
    IPCSocketRef Socket,
    Void* Packet
);

Void IPCSocketUpdate(
    IPCSocketRef Socket
);

Void IPCSocketDisconnect(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
);

Int IPCSocketGetConnectionCount(
    IPCSocketRef Socket
);

IPCSocketConnectionRef IPCSocketGetConnection(
    IPCSocketRef Socket,
    Int ConnectionID
);

IPCSocketConnectionIteratorRef IPCSocketGetConnectionIterator(
    IPCSocketRef Socket
);

IPCSocketConnectionRef IPCSocketConnectionIteratorFetch(
    IPCSocketRef Socket,
    IPCSocketConnectionIteratorRef Iterator
);

IPCSocketConnectionIteratorRef IPCSocketConnectionIteratorNext(
    IPCSocketRef Socket,
    IPCSocketConnectionIteratorRef Iterator
);

EXTERN_C_END
