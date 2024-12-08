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
    Void *ServerContext
) {
    ServerRef Server = (ServerRef)AllocatorAllocate(Allocator, sizeof(struct _Server));
    if (!Server) Fatal("Memory allocation failed!");

    Server->Allocator = Allocator;
    Server->Sockets = ArrayCreateEmpty(Allocator, sizeof(struct _ServerSocketContext), 8);
    Server->IPCSocket = IPCSocketCreate(
        Allocator,
        NodeID,
        (Host) ? 0 : IPC_SOCKET_FLAGS_LISTENER,
        ReadBufferSize,
        WriteBufferSize,
        Host,
        Port,
        Timeout,
        (Host) ? 1 : IPC_SOCKET_MAX_CONNECTION_COUNT,
        LogPackets,
        Server
    );
    Server->OnUpdate = OnUpdate;
    Server->Userdata = ServerContext;
    return Server;
}

Void ServerDestroy(
    ServerRef Server
) {
    for (Int Index = 0; Index < ArrayGetElementCount(Server->Sockets); Index += 1) {
        ServerSocketContextRef SocketContext = (ServerSocketContextRef)ArrayGetElementAtIndex(Server->Sockets, Index);
        SocketDestroy(SocketContext->Socket);
        MemoryPoolDestroy(SocketContext->ConnectionContextPool);
        DictionaryDestroy(SocketContext->CommandRegistry);
        PacketManagerDestroy(SocketContext->PacketManager);
    }
    
    ArrayDestroy(Server->Sockets);
    AllocatorDeallocate(Server->Allocator, (Void*)Server);
}

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
        PacketBufferBacklogSize,
        LogPackets,
        _ServerSocketOnConnect,
        _ServerSocketOnDisconnect,
        NULL,
        _ServerSocketOnReceived,
        SocketContext
    );
    
    SocketContext->SocketHost = SocketHost;
    SocketContext->SocketPort = SocketPort;
    SocketContext->ConnectionContextPool = MemoryPoolCreate(Server->Allocator, ConnectionContextSize, MaxConnectionCount);
    SocketContext->PacketManager = PacketManagerCreate(Server->Allocator);
    SocketContext->CommandRegistry = IndexDictionaryCreate(Server->Allocator, 8);
    SocketContext->OnConnect = OnConnect;
    SocketContext->OnDisconnect = OnDisconnect;
    SocketContext->PacketGetCommand = &ClientPacketGetCommand;
    SocketContext->PacketGetLength = &PacketGetLength;
    SocketContext->PacketGetHeaderLength = &ClientPacketGetHeaderLength;
    
    if (SocketFlags & SOCKET_FLAGS_CLIENT) {
        SocketContext->PacketGetCommand = &ServerPacketGetCommand;
        SocketContext->PacketGetHeaderLength = &ServerPacketGetHeaderLength;
    }
    
    if (SocketFlags & SOCKET_FLAGS_IPC) {
        SocketContext->PacketGetCommand = &ServerPacketGetCommand;
        SocketContext->PacketGetHeaderLength = &ServerPacketGetHeaderLength;
    }

    return SocketContext->Socket;
}

Void ServerSocketRegisterPacketCallback(
    ServerRef Server,
    SocketRef Socket,
    Int Command,
    ServerPacketCallback Callback
) {
    ServerSocketContextRef SocketContext = (ServerSocketContextRef)Socket->Userdata;
    assert(!DictionaryLookup(SocketContext->CommandRegistry, &Command));
    DictionaryInsert(SocketContext->CommandRegistry, &Command, &Callback, sizeof(ServerPacketCallback));
}

Void ServerSocketLoadScript(
    ServerRef Server,
    SocketRef Socket,
    CString FilePath
) {
    ServerSocketContextRef SocketContext = (ServerSocketContextRef)Socket->Userdata;
    PacketManagerLoadScript(SocketContext->PacketManager, FilePath);
}

Void ServerRun(
	ServerRef Server
) {
    for (Int Index = 0; Index < ArrayGetElementCount(Server->Sockets); Index += 1) {
        ServerSocketContextRef SocketContext = (ServerSocketContextRef)ArrayGetElementAtIndex(Server->Sockets, Index);
        if (!(SocketContext->Socket->Flags & SOCKET_FLAGS_LISTENER)) continue;
        
        SocketListen(SocketContext->Socket, SocketContext->SocketPort);
    }

    while (!ApplicationIsShuttingDown()) {
        if (Server->OnUpdate) Server->OnUpdate(Server, Server->Userdata);

        for (Int Index = 0; Index < ArrayGetElementCount(Server->Sockets); Index += 1) {
            ServerSocketContextRef SocketContext = (ServerSocketContextRef)ArrayGetElementAtIndex(Server->Sockets, Index);
            
            Bool IsListener = (SocketContext->Socket->Flags & SOCKET_FLAGS_LISTENER);
            Bool IsConnected = (SocketContext->Socket->Flags & (SOCKET_FLAGS_CONNECTING | SOCKET_FLAGS_CONNECTED));
            if (!IsListener && !IsConnected) {
                SocketConnect(SocketContext->Socket, SocketContext->SocketHost, SocketContext->SocketPort, 0);
            }
            
            SocketUpdate(SocketContext->Socket);
        }

        IPCSocketUpdate(Server->IPCSocket);
        PlatformSleep(1);
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
    Int Command = (Int)SocketContext->PacketGetCommand(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Packet
    );

    Int32 PacketLength = SocketContext->PacketGetLength(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Packet
    );

    Int32 HeaderLength = SocketContext->PacketGetHeaderLength(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Packet
    );

    UInt8 * Buffer = ((UInt8*)Packet) + HeaderLength;
    Int32 BufferLength = PacketLength - HeaderLength;
    Int32 Result = PacketManagerHandle(
        SocketContext->PacketManager,
        Socket,
        Connection,
        Command,
        Buffer,
        BufferLength
    );
    if (Result < 0) {
        SocketDisconnect(Socket, Connection);
    }

    if (Result == 0) {
        ServerPacketCallback* CommandCallback = (ServerPacketCallback*)DictionaryLookup(SocketContext->CommandRegistry, &Command);
        if (CommandCallback) (*CommandCallback)(
            SocketContext->Server,
            SocketContext->Server->Userdata,
            Socket,
            Connection,
            Connection->Userdata,
            Packet
        );
        else {
            Warn("Received unknown packet: %d", (Int32)Command);

            PacketLogBytes(
                Socket->ProtocolIdentifier,
                Socket->ProtocolVersion,
                Socket->ProtocolExtension,
                Packet
            );
        }
    }
}
