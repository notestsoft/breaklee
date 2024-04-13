#include "IPCSocket.h"

enum {
    IPC_COMMAND_REGISTER = 0,
    IPC_COMMAND_ROUTE   = 1,
};

Void PrintNodeTable(
    IPCSocketRef Socket
) {
    CString IPCTypeNames[] = {
        "NONE",
        "AUCTION",
        "AUTH",
        "CHAT",
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
    Connection->Userdata = MemoryPoolReserve(Socket->ConnectionContextPool, Connection->ConnectionPoolIndex);
    
    IPCNodeContextRef NodeContext = (IPCNodeContextRef)Connection->Userdata;
    NodeContext->ConnectionID = Connection->ID;

    IPCPacketRef Packet = _PacketBufferInit(Connection->PacketBuffer, true, sizeof(struct _IPCPacket), IPC_COMMAND_REGISTER);
    Packet->Source = Socket->NodeID;
    IPCSocketSend(Socket, Connection, Packet);

    if (Socket->OnConnect) Socket->OnConnect(Socket, Connection);

    PrintNodeTable(Socket);
}

Void IPCSocketOnDisconnect(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    if (Socket->OnDisconnect) Socket->OnDisconnect(Socket, Connection);

    IPCNodeContextRef NodeContext = (IPCNodeContextRef)Connection->Userdata;

    if (NodeContext->NodeID.Serial) {
        DictionaryRemove(Socket->NodeTable, &NodeContext->NodeID.Serial);
    }

    MemoryPoolRelease(Socket->ConnectionContextPool, Connection->ConnectionPoolIndex);
    Connection->Userdata = NULL;

    PrintNodeTable(Socket);
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
        PrintNodeTable(Socket);
    }

    if (Packet->Command == IPC_COMMAND_ROUTE) {
        if (Packet->Target.Serial == Socket->NodeID.Serial) {
            if (Socket->OnReceived) Socket->OnReceived(Socket, Connection, Packet);
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
    IPCSocketConnectionCallback OnConnect,
    IPCSocketConnectionCallback OnDisconnect,
    IPCSocketPacketCallback OnSend,
    IPCSocketPacketCallback OnReceived,
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
    Socket->ProtocolIdentifier = IPC_PROTOCOL_IDENTIFIER;
    Socket->ProtocolVersion = IPC_PROTOCOL_VERSION;
    Socket->ProtocolExtension = IPC_PROTOCOL_EXTENSION;
    Socket->ReadBufferSize = ReadBufferSize;
    Socket->WriteBufferSize = WriteBufferSize;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = Timeout;
    Socket->PacketBuffer = PacketBufferCreate(Allocator, IPC_PROTOCOL_IDENTIFIER, IPC_PROTOCOL_VERSION, IPC_PROTOCOL_EXTENSION, 4, WriteBufferSize);
    Socket->OnConnect = OnConnect;
    Socket->OnDisconnect = OnDisconnect;
    Socket->OnSend = OnSend;
    Socket->OnReceived = OnReceived;
    Socket->ConnectionIndices = IndexSetCreate(Allocator, MaxConnectionCount);
    Socket->ConnectionPool = MemoryPoolCreate(Allocator, sizeof(struct _IPCSocketConnection), MaxConnectionCount);
    Socket->ConnectionContextPool = MemoryPoolCreate(Allocator, sizeof(struct _IPCNodeContext), MaxConnectionCount);
    Socket->NodeTable = IndexDictionaryCreate(Allocator, MaxConnectionCount);
    Socket->Userdata = Userdata;

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
    DictionaryDestroy(Socket->NodeTable);
    PacketBufferDestroy(Socket->PacketBuffer);
    IndexSetDestroy(Socket->ConnectionIndices);
    MemoryPoolDestroy(Socket->ConnectionPool);
    AllocatorDeallocate(Socket->Allocator, Socket);
}

IPCSocketConnectionRef IPCSocketReserveConnection(
    IPCSocketRef Socket
) {
    Index ConnectionPoolIndex = 0;
    IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)MemoryPoolReserveNext(Socket->ConnectionPool, &ConnectionPoolIndex);
    IndexSetInsert(Socket->ConnectionIndices, ConnectionPoolIndex);
    memset(Connection, 0, sizeof(struct _IPCSocketConnection));
    Connection->ConnectionPoolIndex = ConnectionPoolIndex;
    Connection->PacketBuffer = PacketBufferCreate(
        Socket->Allocator,
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
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
    PacketBufferDestroy(Connection->PacketBuffer);
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

    Socket->Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket->Handle < 0) FatalError("Socket creation failed");

    PlatformSetSocketIOBlocking(Socket->Handle, false);

    Socket->Address.sin_family = AF_INET;
    Socket->Address.sin_addr.s_addr = htonl(INADDR_ANY);
    Socket->Address.sin_port = htons(Port);

    Int32 enable = 1;
    if (setsockopt(Socket->Handle, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) != 0)
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
    UInt16 PacketMagic = *((UInt16*)Packet);
    PacketMagic -= Socket->ProtocolIdentifier;
    PacketMagic -= Socket->ProtocolVersion;

    UInt32 PacketLength = 0;
    if (PacketMagic == Socket->ProtocolExtension) {
        PacketLength = *((UInt32*)((UInt8*)Packet + sizeof(UInt16)));
    }
    else {
        PacketLength = *((UInt16*)((UInt8*)Packet + sizeof(UInt16)));
    }

    if (!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTED))) return;

    MemoryBufferAppendCopy(Connection->WriteBuffer, Packet, Packet->Length);
}

Void IPCSocketUnicast(
    IPCSocketRef Socket,
    IPCPacketRef Packet
) {
    if (!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTED))) return;
    
    UInt16 PacketMagic = *((UInt16*)Packet);
    PacketMagic -= Socket->ProtocolIdentifier;
    PacketMagic -= Socket->ProtocolVersion;

    UInt32 PacketLength = 0;
    if (PacketMagic == Socket->ProtocolExtension) {
        PacketLength = *((UInt32*)((UInt8*)Packet + sizeof(UInt16)));
    } else {
        PacketLength = *((UInt16*)((UInt8*)Packet + sizeof(UInt16)));
    }

    Index* ConnectionID = DictionaryLookup(Socket->NodeTable, &Packet->Target.Serial);
    if (ConnectionID) {
        IPCSocketConnectionRef Connection = IPCSocketGetConnection(Socket, *ConnectionID);
        if (Connection) {
            MemoryBufferAppendCopy(Connection->WriteBuffer, Packet, Packet->Length);
        }
        else if (!(Socket->Flags & IPC_SOCKET_FLAGS_LISTENER)) {
            IPCSocketBroadcast(Socket, Packet);
        }
    }
}

Void IPCSocketBroadcast(
    IPCSocketRef Socket,
    IPCPacketRef Packet
) {
    if (!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTED))) return;

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
    while (MemoryBufferGetOffset(Connection->ReadBuffer) >= 6) {
        UInt32 PacketLength = 0;
    
        Void* Packet = MemoryBufferGetMemory(Connection->ReadBuffer, 0);
        UInt16 PacketMagic = *((UInt16*)Packet);
        PacketMagic -= Socket->ProtocolIdentifier;
        PacketMagic -= Socket->ProtocolVersion;
        
        if (PacketMagic == Socket->ProtocolExtension) {
            PacketLength = *((UInt32*)((UInt8*)Packet + sizeof(UInt16)));
        } else if (PacketMagic == 0) {
            PacketLength = *((UInt16*)((UInt8*)Packet + sizeof(UInt16)));
        } else {
            IPCSocketDisconnect(Socket, Connection);
            break;
        }
        
        // TODO: Add error handling when packet is dropped or not fully received to meet the desired PacketLength
        if (MemoryBufferGetOffset(Connection->ReadBuffer) >= PacketLength) {
            Void* Packet = MemoryBufferGetMemory(Connection->ReadBuffer, 0);
            UInt16 PacketMagic = *((UInt16*)Packet);
            PacketMagic -= Socket->ProtocolIdentifier;
            PacketMagic -= Socket->ProtocolVersion;
            if (PacketMagic != 0 && PacketMagic != Socket->ProtocolExtension) {
                IPCSocketDisconnect(Socket, Connection);
                break;
            }

            IPCSocketOnReceived(Socket, Connection, (IPCSocketRef)Packet);
            MemoryBufferPopFront(Connection->ReadBuffer, PacketLength);
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
        Void* Packet = MemoryBufferGetMemory(Connection->WriteBuffer, 0);
        UInt16 PacketMagic = *((UInt16*)Packet);
        PacketMagic -= Socket->ProtocolIdentifier;
        PacketMagic -= Socket->ProtocolVersion;
        
        UInt32 PacketLength = 0;
        if (PacketMagic == Socket->ProtocolExtension) {
            PacketLength = *((UInt32*)((UInt8*)Packet + sizeof(UInt16)));
        } else {
            PacketLength = *((UInt16*)((UInt8*)Packet + sizeof(UInt16)));
        }
        
        if (Socket->OnSend) Socket->OnSend(Socket, Connection, Packet);

        if (send(Connection->Handle, Packet, PacketLength, 0) == -1) {
            IPCSocketDisconnect(Socket, Connection);
            break;
        }
        
        MemoryBufferPopFront(Connection->WriteBuffer, PacketLength);
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
    if (!(Socket->Flags & (IPC_SOCKET_FLAGS_LISTENING | IPC_SOCKET_FLAGS_CONNECTING | IPC_SOCKET_FLAGS_CONNECTED))) return;

    if (Socket->Flags & IPC_SOCKET_FLAGS_LISTENING) {
        if (!MemoryPoolIsFull(Socket->ConnectionPool)) {
            SocketAddress ClientAddress;
            SocketHandle Client;
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
