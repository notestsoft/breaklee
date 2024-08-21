#pragma once

#include "Base.h"
#include "Keychain.h"
#include "Platform.h"
#include "PacketBuffer.h"

EXTERN_C_BEGIN

#define SOCKET_RECV_BUFFER_SIZE 4096
#define MAX_ADDRESSIP_LENGTH 44

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
    SocketHandle Handle;
    SocketAddress Address;
    UInt32 Flags;
    UInt16 ProtocolIdentifier;
    UInt16 ProtocolVersion;
    UInt16 ProtocolExtension;
    Bool LogPackets;
    Index ReadBufferSize;
    Index WriteBufferSize;
    Index MaxConnectionCount;
    Index NextConnectionID;
    Timestamp Timeout;
    PacketBufferRef PacketBuffer;
    SocketConnectionCallback OnConnect;
    SocketConnectionCallback OnDisconnect;
    SocketPacketCallback OnSend;
    SocketPacketCallback OnReceived;
    IndexSetRef ConnectionIndices;
    MemoryPoolRef ConnectionPool;
    Void* Userdata;
};

struct _SocketConnection {
    Index ConnectionPoolIndex;
    SocketHandle Handle;
    SocketAddress Address;
    Char AddressIP[MAX_ADDRESSIP_LENGTH + 1];
    Index ID;
    UInt32 Flags;
    Timestamp Timestamp;
    struct _Keychain Keychain;
    PacketBufferRef PacketBuffer;
    MemoryBufferRef ReadBuffer;
    MemoryBufferRef WriteBuffer;
    Void* Userdata;
};

SocketRef SocketCreate(
    AllocatorRef Allocator,
    UInt32 Flags,
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Index ReadBufferSize,
    Index WriteBufferSize,
    Index MaxConnectionCount,
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

Void SocketSendRaw(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt8* Data,
    Int32 Length
);

Void SocketSendAllRaw(
    SocketRef Socket,
    UInt8* Data,
    Int32 Length
);

Void SocketSend(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *Packet
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

Void SocketDisconnectDelay(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Timestamp Delay
);

Void SocketClose(
    SocketRef Socket
);

Index SocketGetConnectionCount(
    SocketRef Socket
);

SocketConnectionRef SocketGetConnection(
    SocketRef Socket,
    Index ConnectionID
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
