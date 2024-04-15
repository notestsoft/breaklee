#include "IPCSocket.h"

Void PrintNodeTable(
    IPCSocketRef Socket
) {
    CString IPCTypeNames[] = {
        "NONE",
        "AUCTION",
        "CHAT",
        "LOGIN",
        "MASTER",
        "PARTY",
        "WORLD"
    };

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    printf("+-------+-------+---------------+\n");
    printf("| Type  | Group |     Index     |\n");
    printf("+-------+-------+---------------+\n");

    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(Socket->NodeTable);
    while (Iterator.Key) {
        IPCNodeID Node = { 0 };
        Node.Serial = *(Index*)Iterator.Key;

        printf("|   %d   |   %d   | %08d      |\n", Node.Type, Node.Group, Node.Index);

        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    printf("+-------+-------+---------------+\n");
}

Void IPCSocketConnect(
    IPCSocketRef Socket,
    CString Host,
    UInt16 Port,
    Timestamp Timeout
);

Void IPCSocketListen(
    IPCSocketRef Socket,
    UInt16 Port
);

Void IPCSocketOnConnect(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    Connection->HeartbeatTimestamp = GetTimestampMs() + IPC_SOCKET_HEARTBEAT_TIMEOUT;
    Connection->Userdata = MemoryPoolReserve(Socket->ConnectionContextPool, Connection->ConnectionPoolIndex);
    
    IPCNodeContextRef NodeContext = (IPCNodeContextRef)Connection->Userdata;
    NodeContext->ConnectionID = Connection->ID;

    struct _IPCPacket Packet = { 0 };
    Packet.Length = sizeof(struct _IPCPacket);
    Packet.Command = IPC_COMMAND_REGISTER;
    Packet.RouteType = IPC_ROUTE_TYPE_UNICAST;
    Packet.Source = Socket->NodeID;
    Packet.SourceConnectionID = 0;
    Packet.Target = NodeContext->NodeID;
    Packet.TargetConnectionID = 0;
    IPCSocketSend(Socket, Connection, &Packet);

    // PrintNodeTable(Socket);
}

Void IPCSocketOnDisconnect(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    IPCNodeContextRef NodeContext = (IPCNodeContextRef)Connection->Userdata;

    if (NodeContext->NodeID.Serial) {
        DictionaryRemove(Socket->NodeTable, &NodeContext->NodeID.Serial);
    }

    MemoryPoolRelease(Socket->ConnectionContextPool, Connection->ConnectionPoolIndex);
    Connection->Userdata = NULL;

    // PrintNodeTable(Socket);
}

Void IPCSocketOnReceived(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    IPCPacketRef Packet
) {
    IPCNodeContextRef NodeContext = (IPCNodeContextRef)Connection->Userdata;

    if (Packet->Command == IPC_COMMAND_REGISTER) {
        if (Packet->Source.Group != Socket->NodeID.Group) goto error;
        if (DictionaryContains(Socket->NodeTable, &Packet->Source.Serial)) goto error;
        // TODO: Validate Packet->Source for integrity..

        NodeContext->NodeID = Packet->Source;
        DictionaryInsert(Socket->NodeTable, &Packet->Source.Serial, &Connection->ID, sizeof(Index));
        // PrintNodeTable(Socket);
    }

    if (Packet->Command == IPC_COMMAND_ROUTE) {
        if (Packet->Target.Serial == Socket->NodeID.Serial) {
            Index Command = Packet->SubCommand;
            IPCSocketCommandCallback* CommandCallback = (IPCSocketCommandCallback*)DictionaryLookup(Socket->CommandRegistry, &Command);
            if (CommandCallback) (*CommandCallback)(
                Socket,
                Connection,
                Packet
            );
        }
        else if (Packet->RouteType == IPC_ROUTE_TYPE_UNICAST) {
            Index* TargetConnectionID = DictionaryLookup(Socket->NodeTable, &Packet->Target.Serial);
            if (!TargetConnectionID) return;

            IPCSocketConnectionRef TargetConnection = IPCSocketGetConnection(Socket, *TargetConnectionID);
            if (!TargetConnection) return;

            IPCSocketSend(Socket, TargetConnection, Packet);
        }
        else if (Packet->RouteType == IPC_ROUTE_TYPE_BROADCAST) {
            IPCSocketConnectionIteratorRef Iterator = IPCSocketGetConnectionIterator(Socket);
            while (Iterator) {
                IPCSocketConnectionRef TargetConnection = IPCSocketConnectionIteratorFetch(Socket, Iterator);
                if (!TargetConnection->Userdata) goto next;

                IPCNodeContextRef NodeContext = (IPCNodeContextRef)TargetConnection->Userdata;
                if (NodeContext->NodeID.Serial < 1) goto next;
                if (NodeContext->NodeID.Type != Packet->Target.Type) goto next;
                if (NodeContext->NodeID.Group != Packet->Target.Group) goto next;

                Packet->Target.Index = NodeContext->NodeID.Index;
                IPCSocketSend(Socket, TargetConnection, Packet);
            
            next:
                Iterator = IPCSocketConnectionIteratorNext(Socket, Iterator);
            }
        }
    }

    if (Packet->Command == IPC_COMMAND_HEARTBEAT) {
        Socket->HeartbeatPacket.Target = NodeContext->NodeID;
        IPCSocketSend(Socket, Connection, &Socket->HeartbeatPacket);
    }

    return;

error:
    IPCSocketDisconnect(Socket, Connection);
}

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
) {
    if (MaxConnectionCount < 1) {
        FatalError("[SOCKET] MaxConnection count has to be greater than 0");
    }

    IPCSocketRef Socket = (IPCSocketRef)AllocatorAllocate(Allocator, sizeof(struct _IPCSocket));
    if (!Socket) FatalError("Memory allocation failed!");
    memset(Socket, 0, sizeof(struct _IPCSocket));
    Socket->Allocator = Allocator;
    Socket->NodeID = NodeID;
    Socket->Handle = -1;
    Socket->Flags = Flags;
    Socket->ReadBufferSize = ReadBufferSize;
    Socket->WriteBufferSize = WriteBufferSize;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = Timeout;
    Socket->PacketBuffer = IPCPacketBufferCreate(Allocator, 4, WriteBufferSize);
    Socket->ConnectionIndices = IndexSetCreate(Allocator, MaxConnectionCount);
    Socket->ConnectionPool = MemoryPoolCreate(Allocator, sizeof(struct _IPCSocketConnection), MaxConnectionCount);
    Socket->ConnectionContextPool = MemoryPoolCreate(Allocator, sizeof(struct _IPCNodeContext), MaxConnectionCount);
    Socket->CommandRegistry = IndexDictionaryCreate(Allocator, 8);
    Socket->NodeTable = IndexDictionaryCreate(Allocator, MaxConnectionCount);
    Socket->Userdata = Userdata;

    Socket->HeartbeatPacket.Length = sizeof(struct _IPCPacket);
    Socket->HeartbeatPacket.Command = IPC_COMMAND_HEARTBEAT;
    Socket->HeartbeatPacket.RouteType = IPC_ROUTE_TYPE_UNICAST;
    Socket->HeartbeatPacket.Source = NodeID;
    Socket->HeartbeatPacket.SourceConnectionID = 0;
    Socket->HeartbeatPacket.Target = kIPCNodeIDNull;
    Socket->HeartbeatPacket.TargetConnectionID = 0;

    if (Host) {
        IPCSocketConnect(Socket, Host, Port, Timeout);
    }
    else {
        IPCSocketListen(Socket, Port);
    }

    return Socket;
}

Void IPCSocketDestroy(
    IPCSocketRef Socket
) {
    assert(Socket);
    DictionaryDestroy(Socket->CommandRegistry);
    DictionaryDestroy(Socket->NodeTable);
    IPCPacketBufferDestroy(Socket->PacketBuffer);
    IndexSetDestroy(Socket->ConnectionIndices);
    MemoryPoolDestroy(Socket->ConnectionPool);
    AllocatorDeallocate(Socket->Allocator, Socket);
}

Void IPCSocketRegisterCommandCallback(
    IPCSocketRef Socket,
    Index Command,
    IPCSocketCommandCallback Callback
) {
    assert(!DictionaryLookup(Socket->CommandRegistry, &Command));
    DictionaryInsert(Socket->CommandRegistry, &Command, &Callback, sizeof(IPCSocketCommandCallback));
}

IPCSocketConnectionRef IPCSocketReserveConnection(
    IPCSocketRef Socket
) {
    Index ConnectionPoolIndex = 0;
    IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)MemoryPoolReserveNext(Socket->ConnectionPool, &ConnectionPoolIndex);
    IndexSetInsert(Socket->ConnectionIndices, ConnectionPoolIndex);
    memset(Connection, 0, sizeof(struct _IPCSocketConnection));
    Connection->ConnectionPoolIndex = ConnectionPoolIndex;
    Connection->PacketBuffer = IPCPacketBufferCreate(
        Socket->Allocator,
        4,
        Socket->WriteBufferSize
    );
    Connection->ReadBuffer = MemoryBufferCreate(Socket->Allocator, 4, Socket->ReadBufferSize);
    Connection->WriteBuffer = MemoryBufferCreate(Socket->Allocator, 4, Socket->WriteBufferSize);
    return Connection;
}

Void IPCSocketReleaseConnection(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    IPCPacketBufferDestroy(Connection->PacketBuffer);
    MemoryBufferDestroy(Connection->ReadBuffer);
    MemoryBufferDestroy(Connection->WriteBuffer);
    IndexSetRemove(Socket->ConnectionIndices, Connection->ConnectionPoolIndex);
    MemoryPoolRelease(Socket->ConnectionPool, Connection->ConnectionPoolIndex);
}

Void IPCSocketConnect(
    IPCSocketRef Socket,
    CString Host,
    UInt16 Port,
    Timestamp Timeout
) {
    assert(!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTING | IPC_SOCKET_FLAGS_CONNECTED)));

    Socket->Host = Host;
    Socket->Port = Port;
    Socket->Timeout = Timeout;
    Socket->Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket->Handle < 0) FatalError("Socket creation failed");

    PlatformSetSocketIOBlocking(Socket->Handle, false);

    struct hostent* Server = (struct hostent*)gethostbyname(Host);
    if (!Server) FatalError("Socket creation failed");

    memset(&Socket->Address, 0, sizeof(Socket->Address));
    Socket->Address.sin_family = AF_INET;

    memmove(&Socket->Address.sin_addr.s_addr, Server->h_addr, Server->h_length);
    Socket->Address.sin_port = htons(Port);

    Socket->Flags |= IPC_SOCKET_FLAGS_CONNECTING;

    LogMessageFormat(LOG_LEVEL_INFO, "Socket connecting to port: %d", Port);

    Int32 Result = PlatformSocketConnect(Socket->Handle, (struct sockaddr*)&Socket->Address, sizeof(Socket->Address));
    if (Result == 0) {
        // TODO: Add remote address to `Connection`!
        IPCSocketConnectionRef Connection = IPCSocketReserveConnection(Socket);
        Connection->ID = Socket->NextConnectionID;
        Connection->Handle = Socket->Handle;
        Socket->NextConnectionID += 1;
        Socket->Flags &= ~IPC_SOCKET_FLAGS_CONNECTING;
        Socket->Flags |= IPC_SOCKET_FLAGS_CONNECTED;
        IPCSocketOnConnect(Socket, Connection);
        LogMessage(LOG_LEVEL_INFO, "Socket connection established");
    } else if (Result == -1) {
        FatalError("Socket connection failed");
    }
}

Void IPCSocketListen(
    IPCSocketRef Socket,
    UInt16 Port
) {
    assert(!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTING | IPC_SOCKET_FLAGS_CONNECTED)));

    Socket->Host = NULL;
    Socket->Port = Port;
    Socket->Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket->Handle < 0) FatalError("Socket creation failed");

    PlatformSetSocketIOBlocking(Socket->Handle, false);

    Socket->Address.sin_family = AF_INET;
    Socket->Address.sin_addr.s_addr = htonl(INADDR_ANY);
    Socket->Address.sin_port = htons(Port);

    Int32 Enabled = 1;
    if (setsockopt(Socket->Handle, SOL_SOCKET, SO_REUSEADDR, (Char*)&Enabled, sizeof(Enabled)) != 0)
        FatalError("Socket re-use address set failed");

    if ((bind(Socket->Handle, (struct sockaddr*)&Socket->Address, sizeof(Socket->Address))) != 0)
        FatalError("Socket binding failed");

    if ((listen(Socket->Handle, (Int32)Socket->MaxConnectionCount)) != 0)
        FatalError("Socket listening failed");

    Socket->Flags |= IPC_SOCKET_FLAGS_LISTENING;

    LogMessageFormat(LOG_LEVEL_INFO, "Socket started listening on port: %d", Port);
}

Void IPCSocketSend(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    IPCPacketRef Packet
) {
    if (!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTED))) return;

    MemoryBufferAppendCopy(Connection->WriteBuffer, Packet, Packet->Length);
}

Void IPCSocketUnicast(
    IPCSocketRef Socket,
    Void* Packet
) {
    if (!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTED))) return;

    IPCPacketRef IPCPacket = (IPCPacketRef)Packet;
    IPCPacket->RouteType = IPC_ROUTE_TYPE_UNICAST;

    Index* ConnectionID = DictionaryLookup(Socket->NodeTable, &IPCPacket->Target.Serial);
    if (ConnectionID) {
        IPCSocketConnectionRef Connection = IPCSocketGetConnection(Socket, *ConnectionID);
        if (Connection) {
            MemoryBufferAppendCopy(Connection->WriteBuffer, IPCPacket, IPCPacket->Length);
        }
        else if (!(Socket->Flags & IPC_SOCKET_FLAGS_LISTENER)) {
            IPCSocketBroadcast(Socket, IPCPacket);
        }
    }
}

Void IPCSocketBroadcast(
    IPCSocketRef Socket,
    Void* Packet
) {
    if (!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTED))) return;

    IPCPacketRef IPCPacket = (IPCPacketRef)Packet;
    IPCPacket->RouteType = IPC_ROUTE_TYPE_BROADCAST;

    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        IPCSocketSend(Socket, Connection, Packet);
    }
}

Bool IPCSocketFetchReadBuffer(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    while (MemoryBufferGetOffset(Connection->ReadBuffer) >= sizeof(struct _IPCPacket)) {
        IPCPacketRef Packet = (IPCPacketRef)MemoryBufferGetMemory(Connection->ReadBuffer, 0);

        // TODO: Add error handling when packet is dropped or not fully received to meet the desired PacketLength
        if (MemoryBufferGetOffset(Connection->ReadBuffer) >= Packet->Length) {
            IPCSocketOnReceived(Socket, Connection, Packet);
            MemoryBufferPopFront(Connection->ReadBuffer, Packet->Length);
        }
        else {
            break;
        }
    }

    return true;
}

Bool IPCSocketFlushWriteBuffer(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    while (MemoryBufferGetOffset(Connection->WriteBuffer) > 0) {
        IPCPacketRef Packet = (IPCPacketRef)MemoryBufferGetMemory(Connection->WriteBuffer, 0);
       
        if (send(Connection->Handle, (Char*)Packet, Packet->Length, 0) == -1) {
            IPCSocketDisconnect(Socket, Connection);
            break;
        }
        
        MemoryBufferPopFront(Connection->WriteBuffer, Packet->Length);
    }

    return true;
}

Void IPCSocketReleaseConnections(
    IPCSocketRef Socket
) {
    IndexSetIteratorRef Iterator = IndexSetGetInverseIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetInverseIteratorNext(Socket->ConnectionIndices, Iterator);

        // TODO: Sometimes Connection can become null!!!
        IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        if (!Connection) continue;

        if (Connection->Flags & IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED && !(Connection->Flags & IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED_END)) {
            Connection->Flags &= ~IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED_END;
            IPCSocketFlushWriteBuffer(Socket, Connection);
            IPCSocketOnDisconnect(Socket, Connection);
            Socket->Flags &= ~IPC_SOCKET_FLAGS_CONNECTED;
            PlatformSocketClose(Connection->Handle);
            IPCSocketReleaseConnection(Socket, Connection);
        }
    }
}

Void IPCSocketUpdate(
    IPCSocketRef Socket
) {
    Bool IsListening = (Socket->Flags & IPC_SOCKET_FLAGS_LISTENING);
    Bool IsConnected = (Socket->Flags & (IPC_SOCKET_FLAGS_CONNECTING | IPC_SOCKET_FLAGS_CONNECTED));
    if (IsListening) {
        if (!MemoryPoolIsFull(Socket->ConnectionPool)) {
            SocketAddress ClientAddress = { 0 };
            SocketHandle Client = 0;
            if (PlatformSocketAccept(Socket->Handle, (struct sockaddr*)&ClientAddress, sizeof(ClientAddress), &Client)) {
                IPCSocketConnectionRef Connection = IPCSocketReserveConnection(Socket);
                Connection->ID = Socket->NextConnectionID;
                Connection->Handle = Client;
                Connection->Address = ClientAddress;
                PlatformSocketAddressToString(ClientAddress, sizeof(Connection->AddressIP), Connection->AddressIP);

                Socket->NextConnectionID += 1;
                IPCSocketOnConnect(Socket, Connection);
            }
        }
    }
    else if (!IsConnected) {
        IPCSocketConnect(Socket, Socket->Host, Socket->Port, Socket->Timeout);
    }

    if ((Socket->Flags & IPC_SOCKET_FLAGS_CONNECTING) && !(Socket->Flags & IPC_SOCKET_FLAGS_CONNECTED)) {
        if (PlatformSocketSelect(Socket->Handle, Socket->Timeout)) {
            IPCSocketConnectionRef Connection = IPCSocketReserveConnection(Socket);
            Connection->ID = Socket->NextConnectionID;
            Connection->Handle = Socket->Handle;

            Socket->NextConnectionID += 1;
            Socket->Flags &= ~IPC_SOCKET_FLAGS_CONNECTING;
            Socket->Flags |= IPC_SOCKET_FLAGS_CONNECTED;
            IPCSocketOnConnect(Socket, Connection);
            LogMessage(LOG_LEVEL_INFO, "Socket connection established");
        } else {
            PlatformSocketConnect(Socket->Handle, (struct sockaddr*)&Socket->Address, sizeof(Socket->Address));
        }

        return;
    }

    Int64 RecvLength = 0;
    UInt8 RecvBuffer[IPC_SOCKET_RECV_BUFFER_SIZE] = { 0 };

    Timestamp CurrentTimestamp = GetTimestampMs();
    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);        
        IPCSocketFlushWriteBuffer(Socket, Connection);

        Bool Success = PlatformSocketRecv(
            Connection->Handle,
            RecvBuffer,
            IPC_SOCKET_RECV_BUFFER_SIZE,
            &RecvLength
        );
        if (!Success) {
            IPCSocketDisconnect(Socket, Connection);
            continue;
        }
        
        MemoryBufferAppendCopy(Connection->ReadBuffer, RecvBuffer, RecvLength);
        IPCSocketFetchReadBuffer(Socket, Connection);

        if (Connection->HeartbeatTimestamp < CurrentTimestamp) {
            Connection->HeartbeatTimestamp = CurrentTimestamp + IPC_SOCKET_HEARTBEAT_TIMEOUT;

            IPCNodeContextRef NodeContext = (IPCNodeContextRef)Connection->Userdata;
            Socket->HeartbeatPacket.Target = NodeContext->NodeID;
            IPCSocketSend(Socket, Connection, &Socket->HeartbeatPacket);
        }
    }

    IPCSocketReleaseConnections(Socket);
}

Void IPCSocketDisconnect(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    Connection->Flags |= IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED;
}

Void IPCSocketClose(
    IPCSocketRef Socket
) {
    IPCSocketReleaseConnections(Socket);

    if (Socket->Handle >= 0) {
        PlatformSocketClose(Socket->Handle);
        Socket->Handle = -1;
    }

    Socket->Flags = 0;
}

Index IPCSocketGetConnectionCount(
    IPCSocketRef Socket
) {
    return IndexSetGetElementCount(Socket->ConnectionIndices);
}

IPCSocketConnectionRef IPCSocketGetConnection(
    IPCSocketRef Socket,
    Index ConnectionID
) {
    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        if (Connection->ID == ConnectionID) return Connection;
    }

    return NULL;
}

IPCSocketConnectionIteratorRef IPCSocketGetConnectionIterator(
    IPCSocketRef Socket
) {
    return (IPCSocketConnectionIteratorRef)IndexSetGetIterator(Socket->ConnectionIndices);
}

IPCSocketConnectionRef IPCSocketConnectionIteratorFetch(
    IPCSocketRef Socket,
    IPCSocketConnectionIteratorRef Iterator
) {
    IndexSetIteratorRef IndexSetIterator = (IndexSetIteratorRef)Iterator;
    return (IPCSocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, IndexSetIterator->Value);
}

IPCSocketConnectionIteratorRef IPCSocketConnectionIteratorNext(
    IPCSocketRef Socket,
    IPCSocketConnectionIteratorRef Iterator
) {
    return (IPCSocketConnectionIteratorRef)IndexSetIteratorNext(Socket->ConnectionIndices, (IndexSetIteratorRef)Iterator);
}
