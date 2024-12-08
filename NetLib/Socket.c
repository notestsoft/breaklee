#include "Socket.h"

// TODO: Remove usage of malloc for write requests!

struct _SocketConnectionWriteRequest {
    uv_write_t Request;
    uv_buf_t Buffer;
};

SocketConnectionRef SocketReserveConnection(
    SocketRef Socket
) {
    Int ConnectionPoolIndex = 0;
    SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolReserveNext(Socket->ConnectionPool, &ConnectionPoolIndex);
    IndexSetInsert(Socket->ConnectionIndices, ConnectionPoolIndex);
    memset(Connection, 0, sizeof(struct _SocketConnection));
    Connection->ConnectionPoolIndex = ConnectionPoolIndex;
    Connection->ReadBuffer = MemoryBufferCreate(Socket->Allocator, 4, Socket->ReadBufferSize);
    return Connection;
}

Void SocketReleaseConnection(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    MemoryBufferDestroy(Connection->ReadBuffer);
    IndexSetRemove(Socket->ConnectionIndices, Connection->ConnectionPoolIndex);
    MemoryPoolRelease(Socket->ConnectionPool, Connection->ConnectionPoolIndex);
}

Bool SocketFetchReadBuffer(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    while (MemoryBufferGetWriteOffset(Connection->ReadBuffer) >= 6) {
        UInt32 PacketLength = 0;

        if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
            PacketLength = KeychainGetPacketLength(
                &Connection->Keychain,
                MemoryBufferGetMemory(Connection->ReadBuffer, 0),
                MemoryBufferGetWriteOffset(Connection->ReadBuffer)
            );
        }
        else {
            Void* Packet = MemoryBufferGetMemory(Connection->ReadBuffer, 0);
            UInt16 PacketMagic = *((UInt16*)Packet);
            PacketMagic -= Socket->ProtocolIdentifier;
            PacketMagic -= Socket->ProtocolVersion;

            if (PacketMagic == Socket->ProtocolExtension) {
                PacketLength = *((UInt32*)((UInt8*)Packet + sizeof(UInt16)));
            }
            else if (PacketMagic == 0) {
                PacketLength = *((UInt16*)((UInt8*)Packet + sizeof(UInt16)));
            }
            else {
                Error("Invalid packet header magic: %d", PacketMagic);
                SocketDisconnect(Socket, Connection);
                break;
            }
        }

        // TODO: Add error handling when packet is dropped or not fully received to meet the desired PacketLength
        if (MemoryBufferGetWriteOffset(Connection->ReadBuffer) >= PacketLength) {
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
                Error("Invalid packet header magic: %d", PacketMagic);
                SocketDisconnect(Socket, Connection);
                break;
            }

            Socket->PacketBufferIndex = 0;

            if (Socket->OnReceived) Socket->OnReceived(Socket, Connection, Packet);
            if (Socket->LogPackets) PacketLogBytes(Socket->ProtocolIdentifier, Socket->ProtocolVersion, Socket->ProtocolExtension, Packet);

            MemoryBufferPopFront(Connection->ReadBuffer, PacketLength);
        }
        else {
            break;
        }
    }

    return true;
}

Void _AllocateRecvBuffer(
    uv_handle_t* Handle,
    size_t SuggestedSize,
    uv_buf_t* Buffer
) {
    SocketConnectionRef Connection = (SocketConnectionRef)Handle->data;
    MemoryRef RecvBuffer = Connection->RecvBuffer;
    Int RecvBufferLength = Connection->RecvBufferLength;
    if (RecvBuffer) SuggestedSize = MAX(SuggestedSize, RecvBufferLength);

    if (!RecvBuffer) {
        RecvBuffer = AllocatorAllocate(Connection->Socket->Allocator, SuggestedSize);
    }
    else if (RecvBufferLength < SuggestedSize) {
        RecvBuffer = AllocatorReallocate(Connection->Socket->Allocator, RecvBuffer, SuggestedSize);
    }

    if (RecvBuffer) {
        RecvBufferLength = SuggestedSize;
        Connection->RecvBuffer = RecvBuffer;
        Connection->RecvBufferLength = (Int32)RecvBufferLength;
    }

    Buffer->base = RecvBuffer;
    Buffer->len = RecvBufferLength;
}

Void _OnClose(
    uv_handle_t* Handle
) {
    SocketConnectionRef Connection = (SocketConnectionRef)Handle->data;
    SocketRef Socket = Connection->Socket;

    Connection->Flags |= SOCKET_CONNECTION_FLAGS_DISCONNECTED_END;
    if (Socket->OnDisconnect) Socket->OnDisconnect(Socket, Connection);
    SocketReleaseConnection(Connection->Socket, Connection);
}

Void _OnRead(
    uv_stream_t* Stream,
    ssize_t RecvLength,
    const uv_buf_t* Buffer
) {
    SocketConnectionRef Connection = (SocketConnectionRef)Stream->data;
    assert((uv_stream_t*)Connection->Handle == Stream);

    if (RecvLength < 0) {
        if (RecvLength != UV_EOF) {
            Error("Read error: %s\n", uv_strerror((Int32)RecvLength));
        }

        SocketDisconnect(Connection->Socket, Connection);
        return;
    }

    if (RecvLength > 0) {
        MemoryBufferAppendCopy(Connection->ReadBuffer, Buffer->base, RecvLength);
    }

    SocketFetchReadBuffer(Connection->Socket, Connection);
    SocketProcessDeferred(Connection->Socket);
}

Void _OnNewConnection(
    uv_stream_t* Stream,
    Int32 Status
) {
    if (Status < 0) {
        Error("Socket new connection error: %s\n", uv_strerror(Status));
        return;
    }

    SocketRef Socket = (SocketRef)Stream->data;
    if (MemoryPoolIsFull(Socket->ConnectionPool)) {
        return;
    }

    SocketConnectionRef Connection = SocketReserveConnection(Socket);
    Connection->Socket = Socket;
    Connection->Handle = &Connection->HandleMemory;
    uv_tcp_init(Socket->Loop, Connection->Handle);
    Connection->Handle->data = Connection;

    Int32 Result = uv_accept((uv_stream_t*)&Socket->Handle, (uv_stream_t*)Connection->Handle);
    if (Result == 0) {
        struct sockaddr_storage ClientAddress = { 0 };
        Int32 ClientAddressSize = sizeof(ClientAddress);
        if (uv_tcp_getpeername(Connection->Handle, (struct sockaddr*)&ClientAddress, &ClientAddressSize) == 0) {
            if (ClientAddress.ss_family == AF_INET) {
                uv_ip4_name((struct sockaddr_in*)&ClientAddress, Connection->AddressIP, sizeof(Connection->AddressIP));
            }
            else if (ClientAddress.ss_family == AF_INET6) {
                uv_ip6_name((struct sockaddr_in6*)&ClientAddress, Connection->AddressIP, sizeof(Connection->AddressIP));
            }
        }

        if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
            KeychainInit(&Connection->Keychain, (Socket->Flags & SOCKET_FLAGS_CLIENT) > 0);
        }

        Connection->ID = Socket->NextConnectionID;
        Socket->NextConnectionID += 1;
        if (Socket->OnConnect) Socket->OnConnect(Socket, Connection);
        uv_read_start((uv_stream_t*)Connection->Handle, _AllocateRecvBuffer, _OnRead);
    }
    else {
        SocketDisconnect(Connection->Socket, Connection);
    }
}

Void _OnConnect(
    uv_connect_t* Connect,
    Int32 Status
) {
    SocketRef Socket = (SocketRef)Connect->data;

    if (Status < 0) {
        Socket->State = SOCKET_STATE_DISCONNECTED;
        Error("Socket connection error: %s\n", uv_strerror(Status));
        return;
    }

    SocketConnectionRef Connection = SocketReserveConnection(Socket);
    Connection->Handle = (uv_tcp_t*)Connect->handle;
    Connection->Handle->data = Connection;
    Connection->ID = Socket->NextConnectionID;
    Connection->ConnectRequest = Connect;
    Socket->NextConnectionID += 1;
    Socket->State = SOCKET_STATE_CONNECTED;

    if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
        KeychainInit(&Connection->Keychain, (Socket->Flags & SOCKET_FLAGS_CLIENT) > 0);
    }

    struct sockaddr_storage ClientAddress = { 0 };
    Int32 ClientAddressSize = sizeof(ClientAddress);
    if (uv_tcp_getpeername(Connection->Handle, (struct sockaddr*)&ClientAddress, &ClientAddressSize) == 0) {
        if (ClientAddress.ss_family == AF_INET) {
            uv_ip4_name((struct sockaddr_in*)&ClientAddress, Connection->AddressIP, sizeof(Connection->AddressIP));
        }
        else if (ClientAddress.ss_family == AF_INET6) {
            uv_ip6_name((struct sockaddr_in6*)&ClientAddress, Connection->AddressIP, sizeof(Connection->AddressIP));
        }
    }

    Info("Socket connection established");
    if (Socket->OnConnect) Socket->OnConnect(Socket, Connection);
    uv_read_start(Connect->handle, _AllocateRecvBuffer, _OnRead);
}

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
) {
    if (MaxConnectionCount < 1) {
        Fatal("[SOCKET] MaxConnection count has to be greater than 0");
    }

    SocketRef Socket = (SocketRef)AllocatorAllocate(Allocator, sizeof(struct _Socket));
    if (!Socket) Fatal("Memory allocation failed!");
    memset(Socket, 0, sizeof(struct _Socket));
    Socket->Allocator = Allocator;
    Socket->Loop = uv_default_loop();
    uv_tcp_init(Socket->Loop, &Socket->Handle);
    uv_tcp_nodelay(&Socket->Handle, 1);
    uv_tcp_keepalive(&Socket->Handle, 1, SOCKET_KEEP_ALIVE_TIMEOUT);
    uv_tcp_simultaneous_accepts(&Socket->Handle, 1);
    uv_recv_buffer_size((uv_handle_t*)&Socket->Handle, &ReadBufferSize);
    uv_send_buffer_size((uv_handle_t*)&Socket->Handle, &WriteBufferSize);
    Socket->Handle.data = Socket;
    Socket->Flags = Flags;
    Socket->ProtocolIdentifier = ProtocolIdentifier;
    Socket->ProtocolVersion = ProtocolVersion;
    Socket->ProtocolExtension = ProtocolExtension;
    Socket->LogPackets = LogPackets;
    Socket->ReadBufferSize = ReadBufferSize;
    Socket->WriteBufferSize = WriteBufferSize;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = 0;
    Socket->State = SOCKET_STATE_DISCONNECTED;
    Socket->PacketBufferBacklog = ArrayCreateEmpty(Allocator, sizeof(struct _PacketBuffer), PacketBufferBacklogSize);
    Socket->OnConnect = OnConnect;
    Socket->OnDisconnect = OnDisconnect;
    Socket->OnSend = OnSend;
    Socket->OnReceived = OnReceived;
    Socket->ConnectionIndices = IndexSetCreate(Allocator, MaxConnectionCount);
    Socket->ConnectionPool = MemoryPoolCreate(Allocator, sizeof(struct _SocketConnection), MaxConnectionCount);
    Socket->DeferredWriteRequests = ArrayCreateEmpty(Allocator, sizeof(struct _SocketConnectionWriteRequest*), 8);
    Socket->Userdata = Userdata;

    for (Int Index = 0; Index < PacketBufferBacklogSize; Index += 1) {
        PacketBufferRef PacketBuffer = (PacketBufferRef)ArrayAppendUninitializedElement(Socket->PacketBufferBacklog);
        PacketBufferInitialize(PacketBuffer, Allocator, ProtocolIdentifier, ProtocolVersion, ProtocolExtension, 4, WriteBufferSize, Flags & SOCKET_FLAGS_CLIENT);
    }

    return Socket;
}

Void SocketDestroy(
    SocketRef Socket
) {
    for (Int Index = 0; Index < ArrayGetElementCount(Socket->PacketBufferBacklog); Index += 1) {
        PacketBufferRef PacketBuffer = (PacketBufferRef)ArrayGetElementAtIndex(Socket->PacketBufferBacklog, Index);
        PacketBufferFree(PacketBuffer);
    }

    assert(Socket);
    uv_tcp_close_reset(&Socket->Handle, NULL);
    uv_loop_close(Socket->Loop);
    free(Socket->Loop);
    ArrayDestroy(Socket->PacketBufferBacklog);
    IndexSetDestroy(Socket->ConnectionIndices);
    MemoryPoolDestroy(Socket->ConnectionPool);
    AllocatorDeallocate(Socket->Allocator, Socket);
}

Void SocketConnect(
    SocketRef Socket,
    CString Host,
    UInt16 Port,
    Timestamp Timeout
) {
    if (Socket->State != SOCKET_STATE_DISCONNECTED) return;

    Socket->State = SOCKET_STATE_CONNECTING;
    Socket->Timeout = Timeout;
    uv_ip4_addr(Host, Port, &Socket->Address);

    Info("Socket connecting to port: %d", Port);

    Socket->Connect.data = Socket;
    Int32 Result = uv_tcp_connect(&Socket->Connect, &Socket->Handle, (const struct sockaddr*)&Socket->Address, _OnConnect);
    if (Result) {
        Fatal("Socket connection failed: %s\n", uv_strerror(Result));
        return;
    }
}

Void SocketListen(
    SocketRef Socket,
    UInt16 Port
) {
    if (Socket->State != SOCKET_STATE_DISCONNECTED) return;

    uv_ip4_addr("0.0.0.0", Port, &Socket->Address);

    Int32 Result = uv_tcp_bind(&Socket->Handle, (const struct sockaddr*)&Socket->Address, 0);
    if (Result) {
        Fatal("Socket binding failed: %s\n", uv_strerror(Result));
    }

    Result = uv_listen((uv_stream_t*)&Socket->Handle, Socket->MaxConnectionCount, _OnNewConnection);
    if (Result) {
        Fatal("Socket listening failed: %s\n", uv_strerror(Result));
    }

    Info("Socket started listening on port: %d", Port);
    Socket->State = SOCKET_STATE_CONNECTED;
}

Void _OnWrite(
    uv_write_t* WriteRequest,
    Int32 Status
) {
    SocketConnectionRef Connection = (SocketConnectionRef)WriteRequest->data;

    if (Status < 0) {
        Error("Write error: %s\n", uv_strerror(Status));

        if (Status == UV_ECONNRESET || Status == UV_ECONNREFUSED) {
            SocketDisconnect(Connection->Socket, Connection);
        }
    }

    AllocatorDeallocate(Connection->Socket->Allocator, WriteRequest);
}

Void SocketProcessDeferred(
    SocketRef Socket
) {
    for (Int Index = 0; Index < ArrayGetElementCount(Socket->DeferredWriteRequests); Index += 1) {
        struct _SocketConnectionWriteRequest* WriteRequest = *(struct _SocketConnectionWriteRequest**)ArrayGetElementAtIndex(Socket->DeferredWriteRequests, Index);
        SocketConnectionRef Connection = (SocketConnectionRef)WriteRequest->Request.data;
        uv_write(&WriteRequest->Request, (uv_stream_t*)Connection->Handle, &WriteRequest->Buffer, 1, _OnWrite);
    }

    ArrayRemoveAllElements(Socket->DeferredWriteRequests, true);
}

PacketBufferRef SocketGetNextPacketBuffer(
    SocketRef Socket
) {
    // TODO: If this is reallocating while containing packets its causing dangling pointers!
    if (Socket->PacketBufferIndex >= ArrayGetElementCount(Socket->PacketBufferBacklog)) {
        PacketBufferRef PacketBuffer = (PacketBufferRef)ArrayAppendUninitializedElement(Socket->PacketBufferBacklog);
        PacketBufferInitialize(
            PacketBuffer, 
            Socket->Allocator, 
            Socket->ProtocolIdentifier, 
            Socket->ProtocolVersion,
            Socket->ProtocolExtension,
            4, 
            Socket->WriteBufferSize,
            Socket->Flags & SOCKET_FLAGS_CLIENT
        );
    }

    PacketBufferRef PacketBuffer = (PacketBufferRef)ArrayGetElementAtIndex(Socket->PacketBufferBacklog, Socket->PacketBufferIndex);
    Socket->PacketBufferIndex += 1;
    return PacketBuffer;
}

UInt16 SocketGetPacketMagic(
    SocketRef Socket,
    Bool Extended
) {
    if (Socket->Flags & SOCKET_FLAGS_CLIENT) {
        return Socket->ProtocolIdentifier + Socket->ProtocolVersion;
    }
    else {
        if (Extended) {
            return Socket->ProtocolIdentifier + Socket->ProtocolVersion + Socket->ProtocolExtension;
        }

        return Socket->ProtocolIdentifier + Socket->ProtocolVersion;
    }
}

Void SocketSendRaw(
    SocketRef Socket,
    SocketConnectionRef Connection,
    UInt8 *Data,
    Int32 Length,
    Bool IsDeferred
) {
    if (Socket->State != SOCKET_STATE_CONNECTED) return;
    if (Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED) return;

    UInt16 PacketMagic = *((UInt16*)Data);
    PacketMagic -= Socket->ProtocolIdentifier;
    PacketMagic -= Socket->ProtocolVersion;

    UInt32 PacketLength = 0;
    if (PacketMagic == Socket->ProtocolExtension) {
        PacketLength = *((UInt32*)((UInt8*)Data + sizeof(UInt16)));
    }
    else {
        PacketLength = *((UInt16*)((UInt8*)Data + sizeof(UInt16)));
    }

    assert(Length == PacketLength);

    if (Socket->LogPackets) PacketLogBytes(Socket->ProtocolIdentifier, Socket->ProtocolVersion, Socket->ProtocolExtension, Data);
    if (Socket->OnSend) Socket->OnSend(Socket, Connection, Data);

    Int32 MemoryLength = sizeof(struct _SocketConnectionWriteRequest) + PacketLength;
    UInt8* Memory = AllocatorAllocate(Socket->Allocator, MemoryLength);
    if (!Memory) {
        Fatal("Memory allocation failed!");
    }

    struct _SocketConnectionWriteRequest* WriteRequest = (struct _SocketConnectionWriteRequest*)Memory;
    WriteRequest->Request.data = Connection;
    WriteRequest->Buffer.base = (CString)(Memory + sizeof(struct _SocketConnectionWriteRequest));
    WriteRequest->Buffer.len = Length;
    memcpy(WriteRequest->Buffer.base, Data, Length);

    if (Socket->Flags & SOCKET_FLAGS_ENCRYPTED) {
        KeychainEncryptPacket(&Connection->Keychain, (UInt8*)WriteRequest->Buffer.base, PacketLength);
    }

    if (IsDeferred) {
        ArrayAppendElement(Socket->DeferredWriteRequests, &WriteRequest);
    }
    else {
        uv_write(&WriteRequest->Request, (uv_stream_t*)Connection->Handle, &WriteRequest->Buffer, 1, _OnWrite);
    }
}

Void SocketSendAllRaw(
    SocketRef Socket,
    UInt8* Data,
    Int32 Length,
    Bool IsDeferred
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTED))) return;

    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Int ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        SocketSendRaw(Socket, Connection, Data, Length, IsDeferred);
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

    SocketSendRaw(Socket, Connection, Packet, PacketLength, false);
}

Void SocketSendDeferred(
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void* Packet
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

    SocketSendRaw(Socket, Connection, Packet, PacketLength, true);
}

Void SocketSendAll(
    SocketRef Socket,
    Void *Packet
) {
    if (!(Socket->Flags & (SOCKET_FLAGS_LISTENING | SOCKET_FLAGS_CONNECTED))) return;

    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Int ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        SocketConnectionRef Connection = (SocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        SocketSend(Socket, Connection, Packet);
    }
}

Void SocketUpdate(
    SocketRef Socket
) {
    uv_run(Socket->Loop, UV_RUN_NOWAIT);
    Socket->PacketBufferIndex = 0;
}

Void SocketDisconnect(
    SocketRef Socket,
    SocketConnectionRef Connection
) {
    if (Connection->Flags & SOCKET_CONNECTION_FLAGS_DISCONNECTED) return;
    Connection->Flags |= SOCKET_CONNECTION_FLAGS_DISCONNECTED;
    uv_close((uv_handle_t*)Connection->Handle, _OnClose);
}

Int SocketGetConnectionCount(
    SocketRef Socket
) {
    return IndexSetGetElementCount(Socket->ConnectionIndices);
}

SocketConnectionRef SocketGetConnection(
    SocketRef Socket,
    Int ConnectionID
) {
    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Int ConnectionPoolIndex = Iterator->Value;
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
