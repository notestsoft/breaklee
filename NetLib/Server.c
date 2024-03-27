#include "Server.h"
#include "PacketBuffer.h"

Void _ServerSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void _ServerSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
);

Void _ServerSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *Packet
);

typedef UInt16 (*PacketGetCommandCallback)(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void *Packet
);

struct _ServerSocketContext {
    ServerRef Server;
    SocketRef Socket;
    CString SocketHost;
    UInt16 SocketPort;
    MemoryPoolRef ConnectionContextPool;
    DictionaryRef CommandRegistry;
    ServerConnectionCallback OnConnect;
    ServerConnectionCallback OnDisconnect;
    PacketGetCommandCallback PacketGetCommandCallback;
};
typedef struct _ServerSocketContext* ServerSocketContextRef;

struct _Server {
    AllocatorRef Allocator;
    ArrayRef Sockets;
    ServerUpdateCallback OnUpdate;
    Bool IsRunning;
    Timestamp Timestamp;
    Void *Userdata;
};

ServerRef ServerCreate(
    AllocatorRef Allocator,
    ServerUpdateCallback OnUpdate,
    Void *ServerContext
) {
    PlatformLoadSocketLibrary();

    ServerRef Server = (ServerRef)AllocatorAllocate(Allocator, sizeof(struct _Server));
    if (!Server) FatalError("Memory allocation failed!");

    Server->Allocator = Allocator;
    Server->Sockets = ArrayCreateEmpty(Allocator, sizeof(struct _ServerSocketContext), 8);
    Server->OnUpdate = OnUpdate;
    Server->IsRunning = false;
    Server->Timestamp = GetTimestamp();
    Server->Userdata = ServerContext;
    return Server;
}

Void ServerDestroy(
    ServerRef Server
) {
    for (Index Index = 0; Index < ArrayGetElementCount(Server->Sockets); Index += 1) {
        ServerSocketContextRef SocketContext = (ServerSocketContextRef)ArrayGetElementAtIndex(Server->Sockets, Index);
        SocketDestroy(SocketContext->Socket);
        MemoryPoolDestroy(SocketContext->ConnectionContextPool);
        DictionaryDestroy(SocketContext->CommandRegistry);
    }
    
    ArrayDestroy(Server->Sockets);
    AllocatorDeallocate(Server->Allocator, (Void*)Server);
    
    PlatformUnloadSocketLibrary();
}

Timestamp ServerGetTimestamp(
    ServerRef Server
) {
    return Server->Timestamp;
}

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
) {
    ServerSocketContextRef SocketContext = (ServerSocketContextRef)ArrayAppendUninitializedElement(Server->Sockets);
    SocketContext->Server = Server;
    SocketContext->Socket = SocketCreate(
        Server->Allocator,
        SocketFlags,
        ProtocolIdentifier,
        ProtocolVersion,
        ProtocolExtension,
        ReadBufferSize,
        WriteBufferSize,
        MaxConnectionCount,
        _ServerSocketOnConnect,
        _ServerSocketOnDisconnect,
        NULL,
        _ServerSocketOnReceived,
        SocketContext
    );
    SocketContext->SocketHost = SocketHost;
    SocketContext->SocketPort = SocketPort;
    SocketContext->ConnectionContextPool = MemoryPoolCreate(Server->Allocator, ConnectionContextSize, MaxConnectionCount);
    SocketContext->CommandRegistry = IndexDictionaryCreate(Server->Allocator, 8);
    SocketContext->OnConnect = OnConnect;
    SocketContext->OnDisconnect = OnDisconnect;
    SocketContext->PacketGetCommandCallback = &ClientPacketGetCommand;
    
    if (SocketFlags & SOCKET_FLAGS_CLIENT) {
        SocketContext->PacketGetCommandCallback = &ServerPacketGetCommand;
    }
    
    if (SocketFlags & SOCKET_FLAGS_IPC) {
        SocketContext->PacketGetCommandCallback = &ServerPacketGetCommand;
    }

    return SocketContext->Socket;
}

Void ServerSocketRegisterPacketCallback(
    ServerRef Server,
    SocketRef Socket,
    Index Command,
    ServerPacketCallback Callback
) {
    ServerSocketContextRef SocketContext = (ServerSocketContextRef)Socket->Userdata;
    assert(!DictionaryLookup(SocketContext->CommandRegistry, &Command));
    DictionaryInsert(SocketContext->CommandRegistry, &Command, &Callback, sizeof(ServerPacketCallback));
}

Void ServerRun(
	ServerRef Server
) {
    assert(!Server->IsRunning);
    
    for (Int32 Index = 0; Index < ArrayGetElementCount(Server->Sockets); Index += 1) {
        ServerSocketContextRef SocketContext = (ServerSocketContextRef)ArrayGetElementAtIndex(Server->Sockets, Index);
        if (!(SocketContext->Socket->Flags & SOCKET_FLAGS_LISTENER)) continue;
        
        SocketListen(SocketContext->Socket, SocketContext->SocketPort);
    }

    Server->IsRunning = true;
    while (Server->IsRunning) {
        Server->Timestamp = GetTimestamp();

        if (Server->OnUpdate) Server->OnUpdate(Server, Server->Userdata);
        
        for (Int32 Index = 0; Index < ArrayGetElementCount(Server->Sockets); Index += 1) {
            ServerSocketContextRef SocketContext = (ServerSocketContextRef)ArrayGetElementAtIndex(Server->Sockets, Index);
            
            Bool IsListener = (SocketContext->Socket->Flags & SOCKET_FLAGS_LISTENER);
            Bool IsConnected = (SocketContext->Socket->Flags & (SOCKET_FLAGS_CONNECTING | SOCKET_FLAGS_CONNECTED));
            if (!IsListener && !IsConnected) {
                SocketConnect(SocketContext->Socket, SocketContext->SocketHost, SocketContext->SocketPort, 0);
            }
            
            SocketUpdate(SocketContext->Socket);
        }
    }
}

Void _ServerSocketOnConnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerSocketContextRef SocketContext = (ServerSocketContextRef)Socket->Userdata;
    Connection->Userdata = MemoryPoolReserve(SocketContext->ConnectionContextPool, Connection->ConnectionPoolIndex);
    if (SocketContext->OnConnect) SocketContext->OnConnect(
        SocketContext->Server,
        SocketContext->Server->Userdata,
        Socket,
        Connection,
        Connection->Userdata
    );
}

Void _ServerSocketOnDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    ServerSocketContextRef SocketContext = (ServerSocketContextRef)Socket->Userdata;
    if (SocketContext->OnDisconnect) SocketContext->OnDisconnect(
        SocketContext->Server,
        SocketContext->Server->Userdata,
        Socket,
        Connection,
        Connection->Userdata
    );
    MemoryPoolRelease(SocketContext->ConnectionContextPool, Connection->ConnectionPoolIndex);
    Connection->Userdata = NULL;
}

Void _ServerSocketOnReceived(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *Packet
) {
    ServerSocketContextRef SocketContext = (ServerSocketContextRef)Socket->Userdata;
    Index Command = (Index)SocketContext->PacketGetCommandCallback(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Packet
    );

    ServerPacketCallback *CommandCallback = (ServerPacketCallback*)DictionaryLookup(SocketContext->CommandRegistry, &Command);
    if (CommandCallback) (*CommandCallback)(
        SocketContext->Server,
        SocketContext->Server->Userdata,
        Socket,
        Connection,
        Connection->Userdata,
        Packet
    );
    else {
        LogMessageFormat(LOG_LEVEL_WARNING, "Received unknown packet: %d", (Int32)Command);
        
        PacketLogBytes(
            Socket->ProtocolIdentifier,
            Socket->ProtocolVersion,
            Socket->ProtocolExtension,
            Packet
        );
    }
}
