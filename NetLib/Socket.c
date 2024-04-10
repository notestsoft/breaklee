#include "Socket.h"

SocketRef SocketCreate(
    AllocatorRef Allocator,
    UInt32 Flags,
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Index ReadBufferSize,
    Index WriteBufferSize,
    Index MaxConnectionCount,
    SocketConnectionCallback OnConnect,
    SocketConnectionCallback OnDisconnect,
    SocketPacketCallback OnSend,
    SocketPacketCallback OnReceived,
    Void* Userdata
) {
    if (MaxConnectionCount < 1) {
        FatalError("[SOCKET] MaxConnection count has to be greater than 0");
    }

    SocketRef Socket = (SocketRef)AllocatorAllocate(Allocator, sizeof(struct _Socket));
    if (!Socket) FatalError("Memory allocation failed!");
    memset(Socket, 0, sizeof(struct _Socket));
    Socket->Allocator = Allocator;
    Socket->Handle = -1;
    Socket->Flags = Flags;
    Socket->ProtocolIdentifier = ProtocolIdentifier;
    Socket->ProtocolVersion = ProtocolVersion;
    Socket->ProtocolExtension = ProtocolExtension;
    Socket->ReadBufferSize = ReadBufferSize;
    Socket->WriteBufferSize = WriteBufferSize;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = 0;
    Socket->PacketBuffer = PacketBufferCreate(Allocator, ProtocolIdentifier, ProtocolVersion, ProtocolExtension, 4, WriteBufferSize);
    Socket->OnConnect = OnConnect;
    Socket->OnDisconnect = OnDisconnect;
    Socket->OnSend = OnSend;
    Socket->OnReceived = OnReceived;
    Socket->ConnectionIndices = IndexSetCreate(Allocator, MaxConnectionCount);
    Socket->ConnectionPool = MemoryPoolCreate(Allocator, sizeof(struct _SocketConnection), MaxConnectionCount);
    Socket->Userdata = Userdata;
    return Socket;
}

Void SocketDestroy(
    SocketRef Socket
) {
    assert(Socket);
    PacketBufferDestroy(Socket->PacketBuffer);
    IndexSetDestroy(Socket->ConnectionIndices);
    MemoryPoolDestroy(Socket->ConnectionPool);
    AllocatorDeallocate(Socket->Allocator, Socket);
}

SocketConnectionRef SocketReserveConnection(
    SocketRef Socket
) {
    Index ConnectionPoolIndex = 0;
    SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolReserveNext(Socket->ConnectionPool, &ConnectionPoolIndex);
    IndexSetInsert(Socket->ConnectionIndices, ConnectionPoolIndex);
    memset(Connection, 0, sizeof(struct _SocketConnection));
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

Void SocketReleaseConnection(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    PacketBufferDestroy(Connection->PacketBuffer);
    MemoryBufferDestroy(Connection->ReadBuffer);
    MemoryBufferDestroy(Connection->WriteBuffer);
    IndexSetRemove(Socket->ConnectionIndices, Connection->ConnectionPoolIndex);
    MemoryPoolRelease(Socket->ConnectionPool, Connection->ConnectionPoolIndex);
}

Void SocketConnect(
    SocketRef Socket,
    CString Host,
    UInt16 Port,
    Timestamp Timeout
) {
    assert(!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTING | SOCKET_FLAGS_CONNECTED)));

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

    Socket->Flags |= SOCKET_FLAGS_CONNECTING;

    LogMessageFormat(LOG_LEVEL_INFO, "Socket connecting to port: %d", Port);

    Int32 Result = PlatformSocketConnect(Socket->Handle, (struct sockaddr*)&Socket->Address, sizeof(Socket->Address));
    if (Result == 0) {
        // TODO: Add remote address to `Connection`!
        SocketConnectionRef Connection = SocketReserveConnection(Socket);
        Connection->ID = Socket->NextConnectionID;
        Connection->Handle = Socket->Handle;
        Socket->NextConnectionID += 1;
        Socket->Flags &= ~SOCKET_FLAGS_CONNECTING;
        Socket->Flags |= SOCKET_FLAGS_CONNECTED;
        Socket->OnConnect(Socket, Connection);
        LogMessage(LOG_LEVEL_INFO, "Socket connection established");
    } else if (Result == -1) {
        FatalError("Socket connection failed");
    }
}

Void SocketListen(
    SocketRef Socket,
    UInt16 Port
) {
    assert(!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTING | SOCKET_FLAGS_CONNECTED)));

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

    Socket->Flags |= SOCKET_FLAGS_LISTENING;

    LogMessageFormat(LOG_LEVEL_INFO, "Socket started listening on port: %d", Port);
}

Void SocketSendRaw(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt8 *Data,
    Int32 Length
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTED))) return;

    MemoryBufferAppendCopy(Connection->WriteBuffer, Data, Length);
}

Void SocketSendAllRaw(
    SocketRef Socket,
    UInt8* Data,
    Int32 Length
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTED))) return;

    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        SocketSendRaw(Socket, Connection, Data, Length);
    }
}

Void SocketSend(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *Packet
) {
    UInt16 PacketMagic = *((UInt16*)Packet);
    PacketMagic -= Socket->ProtocolIdentifier;
    PacketMagic -= Socket->ProtocolVersion;

    UInt32 PacketLength = 0;
    if (PacketMagic == Socket->ProtocolExtension) {
        PacketLength = *((UInt32*)((UInt8*)Packet + sizeof(UInt16)));
    } else {
        PacketLength = *((UInt16*)((UInt8*)Packet + sizeof(UInt16)));
    }

    SocketSendRaw(Socket, Connection, Packet, PacketLength);
}

Void SocketSendAll(
    SocketRef Socket,
    Void *Packet
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTED))) return;

    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        SocketSend(Socket, Connection, Packet);
    }
}

Bool SocketFetchReadBuffer(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    while (MemoryBufferGetOffset(Connection->ReadBuffer) >= 6) {
        UInt32 PacketLength = 0;
        
        if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
            PacketLength = KeychainGetPacketLength(
                &Connection->Keychain,
                MemoryBufferGetMemory(Connection->ReadBuffer, 0),
                MemoryBufferGetOffset(Connection->ReadBuffer)
            );
        }
        else {
            Void* Packet = MemoryBufferGetMemory(Connection->ReadBuffer, 0);
            UInt16 PacketMagic = *((UInt16*)Packet);
            PacketMagic -= Socket->ProtocolIdentifier;
            PacketMagic -= Socket->ProtocolVersion;
            
            if (PacketMagic == Socket->ProtocolExtension) {
                PacketLength = *((UInt32*)((UInt8*)Packet + sizeof(UInt16)));
            } else if (PacketMagic == 0) {
                PacketLength = *((UInt16*)((UInt8*)Packet + sizeof(UInt16)));
            } else {
                SocketDisconnect(Socket, Connection);
                break;
            }
        }
        
        // TODO: Add error handling when packet is dropped or not fully received to meet the desired PacketLength
        if (MemoryBufferGetOffset(Connection->ReadBuffer) >= PacketLength) {
            if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                KeychainDecryptPacket(
                    &Connection->Keychain,
                    MemoryBufferGetMemory(Connection->ReadBuffer, 0),
                    PacketLength
                );
            }

            Void* Packet = MemoryBufferGetMemory(Connection->ReadBuffer, 0);
            UInt16 PacketMagic = *((UInt16*)Packet);
            PacketMagic -= Socket->ProtocolIdentifier;
            PacketMagic -= Socket->ProtocolVersion;
            if (PacketMagic != 0 && PacketMagic != Socket->ProtocolExtension) {
                SocketDisconnect(Socket, Connection);
                break;
            }

            if (Socket->OnReceived) Socket->OnReceived(Socket, Connection, Packet);

            MemoryBufferPopFront(Connection->ReadBuffer, PacketLength);
        }
        else {
            break;
        }
    }

    return true;
}

Bool SocketFlushWriteBuffer(
    SocketRef Socket,
    SocketConnectionRef Connection
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

        if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
            KeychainEncryptPacket(&Connection->Keychain, Packet, PacketLength);
        }

        if (send(Connection->Handle, Packet, PacketLength, 0) == -1) {
            SocketDisconnect(Socket, Connection);
            break;
        }
        
        MemoryBufferPopFront(Connection->WriteBuffer, PacketLength);
    }

    return true;
}

Void SocketReleaseConnections(
    SocketRef Socket
) {
    IndexSetIteratorRef Iterator = IndexSetGetInverseIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetInverseIteratorNext(Socket->ConnectionIndices, Iterator);

        // TODO: Sometimes Connection can become null!!!
        SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        if (!Connection) continue;

        if (Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED && !(Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED_END)) {
            Connection->Flags &= ~SOCKET_CONNECTION_FLAGS_DISCONNECTED_END;
            SocketFlushWriteBuffer(Socket, Connection);
            Socket->OnDisconnect(Socket, Connection);
            Socket->Flags &= ~SOCKET_FLAGS_CONNECTED;
            PlatformSocketClose(Connection->Handle);
            SocketReleaseConnection(Socket, Connection);
        }
    }
}

Void SocketUpdate(
    SocketRef Socket
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTING | SOCKET_FLAGS_CONNECTED))) return;

    if (Socket->Flags & SOCKET_FLAGS_LISTENING) {
        if (!MemoryPoolIsFull(Socket->ConnectionPool)) {
            SocketAddress ClientAddress;
            SocketHandle Client;
            if (PlatformSocketAccept(Socket->Handle, (struct sockaddr*)&ClientAddress, sizeof(ClientAddress), &Client)) {
                SocketConnectionRef Connection = SocketReserveConnection(Socket);
                Connection->ID = Socket->NextConnectionID;
                Connection->Handle = Client;
                Connection->Address = ClientAddress;
                PlatformSocketAddressToString(ClientAddress, sizeof(Connection->AddressIP), Connection->AddressIP);

                if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                    KeychainInit(&Connection->Keychain, (Socket->Flags & SOCKET_FLAGS_CLIENT) > 0);
                }

                Socket->NextConnectionID += 1;
                Socket->OnConnect(Socket, Connection);
            }
        }
    }

    if ((Socket->Flags & SOCKET_FLAGS_CONNECTING) && !(Socket->Flags & SOCKET_FLAGS_CONNECTED)) {
        if (PlatformSocketSelect(Socket->Handle, Socket->Timeout)) {
            SocketConnectionRef Connection = SocketReserveConnection(Socket);
            Connection->ID = Socket->NextConnectionID;
            Connection->Handle = Socket->Handle;

            if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                KeychainInit(&Connection->Keychain, (Socket->Flags & SOCKET_FLAGS_CLIENT) > 0);
            }

            Socket->NextConnectionID += 1;
            Socket->Flags &= ~SOCKET_FLAGS_CONNECTING;
            Socket->Flags |= SOCKET_FLAGS_CONNECTED;
            Socket->OnConnect(Socket, Connection);
            LogMessage(LOG_LEVEL_INFO, "Socket connection established");
        } else {
            PlatformSocketConnect(Socket->Handle, (struct sockaddr*)&Socket->Address, sizeof(Socket->Address));
        }

        return;
    }

    Int64 RecvLength = 0;
    UInt8 RecvBuffer[SOCKET_RECV_BUFFER_SIZE] = { 0 };
    
    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);        
        SocketFlushWriteBuffer(Socket, Connection);

        Bool Success = PlatformSocketRecv(
            Connection->Handle,
            RecvBuffer,
            SOCKET_RECV_BUFFER_SIZE,
            &RecvLength
        );
        if (!Success) {
            SocketDisconnect(Socket, Connection);
            continue;
        }
        
        MemoryBufferAppendCopy(Connection->ReadBuffer, RecvBuffer, RecvLength);
        SocketFetchReadBuffer(Socket, Connection);
    }

    SocketReleaseConnections(Socket);
}

Void SocketDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    Connection->Flags |= SOCKET_CONNECTION_FLAGS_DISCONNECTED;
}

Void SocketClose(
    SocketRef Socket
) {
    SocketReleaseConnections(Socket);

    if (Socket->Handle >= 0) {
        PlatformSocketClose(Socket->Handle);
        Socket->Handle = -1;
    }

    Socket->Flags = 0;
}

Index SocketGetConnectionCount(
    SocketRef Socket
) {
    return IndexSetGetElementCount(Socket->ConnectionIndices);
}

SocketConnectionRef SocketGetConnection(
    SocketRef Socket,
    Int32 ConnectionID
) {
    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        if (Connection->ID == ConnectionID) return Connection;
    }

    return NULL;
}

Void SocketConnectionKeychainSeed(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt32 Key,
    UInt32 Step
) {
    assert(Socket->Flags & SOCKET_FLAGS_ENCRYPTED);
    KeychainSeed(&Connection->Keychain, Key, Step);
}

SocketConnectionIteratorRef SocketGetConnectionIterator(
    SocketRef Socket
) {
    return (SocketConnectionIteratorRef)IndexSetGetIterator(Socket->ConnectionIndices);
}

SocketConnectionRef SocketConnectionIteratorFetch(
    SocketRef Socket,
    SocketConnectionIteratorRef Iterator
) {
    IndexSetIteratorRef IndexSetIterator = (IndexSetIteratorRef)Iterator;
    return (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, IndexSetIterator->Value);
}

SocketConnectionIteratorRef SocketConnectionIteratorNext(
    SocketRef Socket,
    SocketConnectionIteratorRef Iterator
) {
    return (SocketConnectionIteratorRef)IndexSetIteratorNext(Socket->ConnectionIndices, (IndexSetIteratorRef)Iterator);
}
