#pragma once

#include "Base.h"
#include "Platform.h"
#include "IPCPacketBuffer.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define IPC_SOCKET_MAX_CONNECTION_COUNT 512
#define IPC_SOCKET_MAX_RECONNECT_COUNT  3
#define IPC_SOCKET_RECV_BUFFER_SIZE     4096
#define IPC_SOCKET_HEARTBEAT_TIMEOUT    1000

enum {
    IPC_TYPE_ALL        = 0,
    IPC_TYPE_AUCTION    = 1,
    IPC_TYPE_LOGIN      = 2,
    IPC_TYPE_CHAT       = 3,
    IPC_TYPE_MASTER     = 4,
    IPC_TYPE_PARTY      = 5,
    IPC_TYPE_WORLD      = 6,
};

enum {
    IPC_COMMAND_REGISTER    = 0,
    IPC_COMMAND_ROUTE       = 1,
    IPC_COMMAND_HEARTBEAT   = 2,
};

enum {
    IPC_ROUTE_TYPE_UNICAST   = 0,
    IPC_ROUTE_TYPE_BROADCAST = 1,
};

struct _IPCNodeID {
    union {
        struct { UInt32 Index; UInt16 Group; UInt16 Type; };

        UInt64 Serial;
    };
};
typedef struct _IPCNodeID IPCNodeID;

static const IPCNodeID kIPCNodeIDNull = { 0 };

struct _IPCNodeContext {
    IPCNodeID NodeID;
    Index ConnectionID;
};
typedef struct _IPCNodeContext* IPCNodeContextRef;

struct _IPCPacket {
    UInt32 Length;
    UInt16 Command;
    UInt16 SubCommand;
    UInt8 RouteType;
    IPCNodeID Source;
    Index SourceConnectionID;
    IPCNodeID Target;
    Index TargetConnectionID;
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
    SocketHandle Handle;
    SocketAddress Address;
    UInt32 Flags;
    UInt16 ProtocolIdentifier;
    UInt16 ProtocolVersion;
    UInt16 ProtocolExtension;
    Index ReadBufferSize;
    Index WriteBufferSize;
    Index MaxConnectionCount;
    Index NextConnectionID;
    CString Host;
    UInt16 Port;
    Timestamp Timeout;
    struct _IPCPacket HeartbeatPacket;
    IPCPacketBufferRef PacketBuffer;
    IndexSetRef ConnectionIndices;
    MemoryPoolRef ConnectionPool; 
    MemoryPoolRef ConnectionContextPool;
    DictionaryRef CommandRegistry;
    DictionaryRef NodeTable;
    Void* Userdata;
};

struct _IPCSocketConnection {
    Index ConnectionPoolIndex;
    SocketHandle Handle;
    SocketAddress Address;
    Char AddressIP[16];
    Index ID;
    UInt32 Flags;
    IPCPacketBufferRef PacketBuffer;
    MemoryBufferRef ReadBuffer;
    MemoryBufferRef WriteBuffer;
    Timestamp HeartbeatTimestamp;
    Void* Userdata;
};

#pragma pack(pop)

IPCSocketRef IPCSocketCreate(
    AllocatorRef Allocator,
    IPCNodeID NodeID,
    UInt32 Flags,
    Index ReadBufferSize,
    Index WriteBufferSize,
    CString Host,
    UInt16 Port,
    Timestamp Timeout,
    Index MaxConnectionCount,
    Void* Userdata
);

Void IPCSocketDestroy(
    IPCSocketRef Socket
);

Void IPCSocketRegisterCommandCallback(
    IPCSocketRef Socket,
    Index Command,
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

Void IPCSocketClose(
    IPCSocketRef Socket
);

Index IPCSocketGetConnectionCount(
    IPCSocketRef Socket
);

IPCSocketConnectionRef IPCSocketGetConnection(
    IPCSocketRef Socket,
    Index ConnectionID
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
