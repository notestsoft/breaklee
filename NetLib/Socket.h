#pragma once

#include "Base.h"
#include "Keychain.h"
#include "PacketBuffer.h"

EXTERN_C_BEGIN

#define SOCKET_RECV_BUFFER_SIZE     4096
#define MAX_ADDRESSIP_LENGTH        INET6_ADDRSTRLEN
#define SOCKET_KEEP_ALIVE_TIMEOUT   10

typedef struct _Socket* SocketRef;
typedef struct _SocketConnection* SocketConnectionRef;

enum {
    SOCKET_FLAGS_LISTENER   = 1 << 0,
    SOCKET_FLAGS_CLIENT     = 1 << 1,
    SOCKET_FLAGS_IPC        = 1 << 2,
    SOCKET_FLAGS_ENCRYPTED  = 1 << 3,

    SOCKET_FLAGS_LISTENING  = 1 << 4,
    SOCKET_FLAGS_CONNECTING = 1 << 5,
    SOCKET_FLAGS_CONNECTED  = 1 << 6,
};

enum {
    SOCKET_STATE_DISCONNECTED,
    SOCKET_STATE_CONNECTING,
    SOCKET_STATE_CONNECTED,
    SOCKET_STATE_DISCONNECTING,
};

enum {
    SOCKET_CONNECTION_FLAGS_DISCONNECTED     = 1 << 0,
    SOCKET_CONNECTION_FLAGS_DISCONNECTED_END = 1 << 1,
    SOCKET_CONNECTION_FLAGS_DISCONNECT_DELAY = 1 << 2,
};

typedef Void (*SocketConnectionCallback)(
    SocketRef Socket,
    SocketConnectionRef Connection
);

typedef Void (*SocketPacketCallback)(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *Packet
);

typedef Void* SocketConnectionIteratorRef;

struct _Socket {
    AllocatorRef Allocator;
    uv_loop_t* Loop;
    uv_tcp_t Handle;
    uv_connect_t Connect;
    SocketAddress Address;
    UInt32 Flags;
    UInt16 ProtocolIdentifier;
    UInt16 ProtocolVersion;
    UInt16 ProtocolExtension;
    Bool LogPackets;
    Int32 ReadBufferSize;
    Int32 WriteBufferSize;
    Int32 MaxConnectionCount;
    Int NextConnectionID;
    Timestamp Timeout; 
    Int32 State;
    Int32 PacketBufferIndex;
    ArrayRef PacketBufferBacklog;
    SocketConnectionCallback OnConnect;
    SocketConnectionCallback OnDisconnect;
    SocketPacketCallback OnSend;
    SocketPacketCallback OnReceived;
    IndexSetRef ConnectionIndices;
    MemoryPoolRef ConnectionPool;
    ArrayRef DeferredWriteRequests;
    Void* Userdata;
};

struct _SocketConnection {
    SocketRef Socket;
    Int ConnectionPoolIndex;
    uv_tcp_t HandleMemory;
    uv_tcp_t* Handle;
    uv_connect_t* ConnectRequest;
    SocketAddress Address;
    Char AddressIP[MAX_ADDRESSIP_LENGTH];
    Int ID;
    UInt32 Flags;
    struct _Keychain Keychain;
    MemoryRef RecvBuffer;
    Int32 RecvBufferLength;
    MemoryBufferRef ReadBuffer;
    Void* Userdata;
};

SocketRef SocketCreate(
    AllocatorRef Allocator,
    UInt32 Flags,
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Int32 ReadBufferSize,
    Int32 WriteBufferSize,
    Int32 MaxConnectionCount,
    Int32 PacketBufferBacklogSize,
    Bool LogPackets,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketPacketCallback OnSend,
    SocketPacketCallback OnReceived,
    Void* Userdata
);

Void SocketDestroy(
    SocketRef Socket
);

Void SocketConnect(
    SocketRef Socket,
    CString Host,
    UInt16 Port,
    Timestamp Timeout
);

Void SocketListen(
    SocketRef Socket,
    UInt16 Port
);

Void SocketProcessDeferred(
    SocketRef Socket
);

PacketBufferRef SocketGetNextPacketBuffer(
    SocketRef Socket
);

UInt16 SocketGetPacketMagic(
    SocketRef Socket,
    Bool Extended
);

Void SocketSendRaw(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt8* Data,
    Int32 Length,
    Bool IsDeferred
);

Void SocketSendAllRaw(
    SocketRef Socket,
    UInt8* Data,
    Int32 Length,
    Bool IsDeferred
);

Void SocketSend(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *Packet
);

Void SocketSendDeferred(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void* Packet
);

Void SocketSendAll(
    SocketRef Socket,
    Void *Packet
);

Void SocketUpdate(
    SocketRef Socket
);

Void SocketDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Int SocketGetConnectionCount(
    SocketRef Socket
);

SocketConnectionRef SocketGetConnection(
    SocketRef Socket,
    Int ConnectionID
);

Void SocketConnectionKeychainSeed(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt32 Key,
    UInt32 Step
);

SocketConnectionIteratorRef SocketGetConnectionIterator(
    SocketRef Socket
);

SocketConnectionRef SocketConnectionIteratorFetch(
    SocketRef Socket,
    SocketConnectionIteratorRef Iterator
);

SocketConnectionIteratorRef SocketConnectionIteratorNext(
    SocketRef Socket,
    SocketConnectionIteratorRef Iterator
);

EXTERN_C_END
