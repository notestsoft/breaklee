#include "Socket.h"

SocketRef SocketCreate(
    AllocatorRef Allocator,
    UInt32 Flags,
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
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->ConnectionIndices = IndexSetCreate(Allocator, MaxConnectionCount);
    Socket->ConnectionPool = MemoryPoolCreate(Allocator, sizeof(struct _SocketConnection), MaxConnectionCount);
    Socket->Handle = -1;
    Socket->Flags = Flags;
    Socket->NextConnectionID = 1;
    Socket->Timeout = 0;
    Socket->OnConnect = OnConnect;
    Socket->OnDisconnect = OnDisconnect;
    Socket->OnSend = OnSend;
    Socket->OnReceived = OnReceived;
    Socket->Userdata = Userdata;
    return Socket;
}

Void SocketDestroy(
    SocketRef Socket
) {
    assert(Socket);
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
    return Connection;
}

Void SocketReleaseConnection(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
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

    assert(Connection->WriteBufferOffset + Length <= SOCKET_MAX_PACKET_SIZE);
    memcpy(&Connection->WriteBuffer[Connection->WriteBufferOffset], Data, Length);
    Connection->WriteBufferOffset += Length;
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
    SocketSendRaw(Socket, Connection, Packet, ((PacketRef)Packet)->Length);
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
    while (Connection->ReadBufferOffset >= sizeof(struct _PacketSignature)) {
        Int32 PacketLength = 0;

        if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
            PacketLength = KeychainGetPacketLength(
                &Connection->Keychain,
                Connection->ReadBuffer,
                Connection->ReadBufferOffset
            );
        }
        else {
            PacketRef Signature = (PacketRef)Connection->ReadBuffer;
            PacketLength = Signature->Length;
        }

        if (Connection->ReadBufferOffset >= PacketLength) {
            if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
                KeychainDecryptPacket(
                    &Connection->Keychain,
                    Connection->ReadBuffer,
                    PacketLength
                );
            }

            if (Socket->OnReceived) Socket->OnReceived(Socket, Connection, (PacketRef)Connection->ReadBuffer);

            Connection->ReadBufferOffset -= PacketLength;
            if (Connection->ReadBufferOffset > 0) {
                memmove(&Connection->ReadBuffer[0], &Connection->ReadBuffer[PacketLength], Connection->ReadBufferOffset);
            }
        }
    }
}

Bool SocketFlushWriteBuffer(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    while (Connection->WriteBufferOffset > 0) {
        PacketRef Packet = (PacketRef)Connection->WriteBuffer;
        Int32 PacketLength = Packet->Length;

        if (Socket->OnSend)
            Socket->OnSend(Socket, Connection, Packet);

        if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
            KeychainEncryptPacket(&Connection->Keychain, (UInt8 *)Packet, PacketLength);
        }

        if (send(Connection->Handle, (Char*)Packet, PacketLength, 0) == -1) {
            SocketDisconnect(Socket, Connection);
            break;
        }
        else {
            Connection->WriteBufferOffset -= PacketLength;
            if (Connection->WriteBufferOffset > 0) {
                memmove(
                    &Connection->WriteBuffer[0],
                    &Connection->WriteBuffer[PacketLength],
                    Connection->WriteBufferOffset
                );
            }
        }
    }
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

    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);        
        SocketFlushWriteBuffer(Socket, Connection);

        Int64 ReadLength;
        Bool Success = PlatformSocketRecv(
            Connection->Handle,
            &Connection->ReadBuffer[Connection->ReadBufferOffset],
            SOCKET_MAX_PACKET_SIZE - Connection->ReadBufferOffset,
            &ReadLength
        );
        if (!Success) {
            SocketDisconnect(Socket, Connection);
            continue;
        }

        Connection->ReadBufferOffset += ReadLength;
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
