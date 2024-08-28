#include "IPCSocket.h"

// TODO: Remove usage of malloc for write requests!

struct _IPCSocketConnectionWriteRequest {
    uv_write_t Request;
    uv_buf_t Buffer;
};

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

IPCSocketConnectionRef IPCSocketReserveConnection(
    IPCSocketRef Socket
);

Void IPCSocketReleaseConnection(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
);

Bool IPCSocketFetchReadBuffer(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
);

Void IPCSocketOnConnect(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
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
}

Void OnReconnectTimerClose(
    uv_handle_t* Handle
) {
    IPCSocketRef Socket = (IPCSocketRef)Handle->data;
    AllocatorDeallocate(Socket->Allocator, Handle);
    Socket->ReconnectTimer = NULL;
    IPCSocketConnect(Socket, Socket->Host, Socket->Port, Socket->Timeout);
}

Void OnReconnect(
    uv_timer_t* Timer
) {
    IPCSocketRef Socket = (IPCSocketRef)Timer->data;
    uv_timer_stop(Socket->ReconnectTimer);
    uv_close((uv_handle_t*)Socket->ReconnectTimer, OnReconnectTimerClose);
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
}

Void IPCSocketOnReceived(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    IPCPacketRef Packet
) {
    IPCNodeContextRef NodeContext = (IPCNodeContextRef)Connection->Userdata;
    
    if (Socket->LogPackets) IPCPacketLogBytes(Packet);

    if (Packet->Command == IPC_COMMAND_REGISTER) {
        if (Packet->Source.Group != Socket->NodeID.Group) goto error;

        NodeContext->NodeID = Packet->Source;
        DictionaryInsert(Socket->NodeTable, &Packet->Source.Serial, &Connection->ID, sizeof(Index));
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

    return;

error:
    IPCSocketDisconnect(Socket, Connection);
}

Void AllocateRecvBuffer(
    uv_handle_t* Handle,
    size_t SuggestedSize,
    uv_buf_t* Buffer
) {
    IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)Handle->data;
    MemoryRef RecvBuffer = Connection->RecvBuffer;
    Int32 RecvBufferLength = Connection->RecvBufferLength;
    if (RecvBuffer) SuggestedSize = MAX(SuggestedSize, RecvBufferLength);

    if (!RecvBuffer) {
        RecvBuffer = AllocatorAllocate(Connection->Socket->Allocator, SuggestedSize);
    } else if (RecvBufferLength < SuggestedSize) {
        RecvBuffer = AllocatorReallocate(Connection->Socket->Allocator, RecvBuffer, SuggestedSize);
    }

    if (RecvBuffer) {
        RecvBufferLength = SuggestedSize;
        Connection->RecvBuffer = RecvBuffer;
        Connection->RecvBufferLength = RecvBufferLength;
    }

    Buffer->base = RecvBuffer;
    Buffer->len = RecvBufferLength;
}

Void OnClose(
    uv_handle_t* Handle
) {
    IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)Handle->data;
    IPCSocketRef Socket = Connection->Socket;

    Connection->Flags |= IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED_END;
    IPCSocketOnDisconnect(Connection->Socket, Connection);
    IPCSocketReleaseConnection(Connection->Socket, Connection);

    if (Socket->Host && !Socket->ReconnectTimer) {
        Socket->State = IPC_SOCKET_STATE_DISCONNECTED;
        uv_tcp_close_reset(&Socket->Handle, NULL);
        uv_tcp_init(Socket->Loop, &Socket->Handle);
        uv_tcp_nodelay(&Socket->Handle, 1);
        uv_tcp_keepalive(&Socket->Handle, 1, IPC_SOCKET_KEEP_ALIVE_TIMEOUT);
        uv_tcp_simultaneous_accepts(&Socket->Handle, 1);
        uv_recv_buffer_size((uv_handle_t*)&Socket->Handle, &Socket->ReadBufferSize);
        uv_send_buffer_size((uv_handle_t*)&Socket->Handle, &Socket->WriteBufferSize);
        Socket->Handle.data = Socket;

        Socket->ReconnectTimer = (uv_timer_t*)AllocatorAllocate(Socket->Allocator, sizeof(uv_timer_t));
        uv_timer_init(Socket->Loop, Socket->ReconnectTimer);
        Socket->ReconnectTimer->data = Socket;
        uv_timer_start(Socket->ReconnectTimer, OnReconnect, IPC_SOCKET_RECONNECT_DELAY, IPC_SOCKET_RECONNECT_DELAY);
    }
}

Void OnRead(
    uv_stream_t* Stream,
    ssize_t RecvLength,
    const uv_buf_t* Buffer
) {
    IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)Stream->data;
    assert((uv_stream_t*)Connection->Handle == Stream);

    if (RecvLength < 0) {
        if (RecvLength != UV_EOF) {
            Error("Read error: %s\n", uv_strerror(RecvLength));
        }

        IPCSocketDisconnect(Connection->Socket, Connection);
        return;
    }

    if (RecvLength > 0) {
        MemoryBufferAppendCopy(Connection->ReadBuffer, Buffer->base, RecvLength);
    }

    IPCSocketFetchReadBuffer(Connection->Socket, Connection);
}

Void OnNewConnection(
    uv_stream_t* Stream, 
    Int32 Status
) {
    if (Status < 0) {
        Error("Socket new connection error: %s\n", uv_strerror(Status));
        return;
    }

    IPCSocketRef Socket = (IPCSocketRef)Stream->data;
    if (MemoryPoolIsFull(Socket->ConnectionPool)) {
        return;
    }

    IPCSocketConnectionRef Connection = IPCSocketReserveConnection(Socket);
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

        Connection->ID = Socket->NextConnectionID;
        Socket->NextConnectionID += 1;
        IPCSocketOnConnect(Socket, Connection);
        uv_read_start((uv_stream_t*)Connection->Handle, AllocateRecvBuffer, OnRead);
    }
    else {
        IPCSocketDisconnect(Connection->Socket, Connection);
    }
}

Void OnConnect(
    uv_connect_t* Connect,
    Int32 Status
) {
    IPCSocketRef Socket = (IPCSocketRef)Connect->data;

    if (Status < 0) {
        Socket->State = IPC_SOCKET_STATE_DISCONNECTED;
        Error("Socket connection error: %s\n", uv_strerror(Status));

        if (!Socket->ReconnectTimer) {
            Socket->ReconnectTimer = (uv_timer_t*)AllocatorAllocate(Socket->Allocator, sizeof(uv_timer_t));
            uv_timer_init(Socket->Loop, Socket->ReconnectTimer);
            Socket->ReconnectTimer->data = Socket;
            uv_timer_start(Socket->ReconnectTimer, OnReconnect, IPC_SOCKET_RECONNECT_DELAY, IPC_SOCKET_RECONNECT_DELAY);
        }

        return;
    }

    IPCSocketConnectionRef Connection = IPCSocketReserveConnection(Socket);
    Connection->Handle = (uv_tcp_t*)Connect->handle;
    Connection->Handle->data = Connection;
    Connection->ID = Socket->NextConnectionID;
    Connection->ConnectRequest = Connect;
    Socket->NextConnectionID += 1;
    Socket->State = IPC_SOCKET_STATE_CONNECTED;

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
    IPCSocketOnConnect(Socket, Connection);
    uv_read_start(Connect->handle, AllocateRecvBuffer, OnRead);
}

IPCSocketRef IPCSocketCreate(
    AllocatorRef Allocator, 
    IPCNodeID NodeID,
    UInt32 Flags,
    Int32 ReadBufferSize,
    Int32 WriteBufferSize,
    CString Host,
    UInt16 Port,
    Timestamp Timeout,
    Index MaxConnectionCount,
    Bool LogPackets,
    Void* Userdata
) {
    if (MaxConnectionCount < 1) {
        Fatal("[SOCKET] MaxConnection count has to be greater than 0");
    }

    IPCSocketRef Socket = (IPCSocketRef)AllocatorAllocate(Allocator, sizeof(struct _IPCSocket));
    if (!Socket) Fatal("Memory allocation failed!");
    memset(Socket, 0, sizeof(struct _IPCSocket));
    Socket->Allocator = Allocator;
    Socket->NodeID = NodeID;
    Socket->Loop = uv_default_loop();
    uv_tcp_init(Socket->Loop, &Socket->Handle);
    uv_tcp_nodelay(&Socket->Handle, 1);
    uv_tcp_keepalive(&Socket->Handle, 1, IPC_SOCKET_KEEP_ALIVE_TIMEOUT);
    uv_tcp_simultaneous_accepts(&Socket->Handle, 1);
    uv_recv_buffer_size((uv_handle_t*)&Socket->Handle, &ReadBufferSize);
    uv_send_buffer_size((uv_handle_t*)&Socket->Handle, &WriteBufferSize);
    Socket->Handle.data = Socket;
    Socket->ReadBufferSize = ReadBufferSize;
    Socket->WriteBufferSize = WriteBufferSize;
    Socket->MaxConnectionCount = MaxConnectionCount;
    Socket->NextConnectionID = 1;
    Socket->Timeout = Timeout;
    Socket->State = IPC_SOCKET_STATE_DISCONNECTED;
    Socket->LogPackets = LogPackets;
    Socket->PacketBuffer = IPCPacketBufferCreate(Allocator, 4, WriteBufferSize);
    Socket->ConnectionIndices = IndexSetCreate(Allocator, MaxConnectionCount);
    Socket->ConnectionPool = MemoryPoolCreate(Allocator, sizeof(struct _IPCSocketConnection), MaxConnectionCount);
    Socket->ConnectionContextPool = MemoryPoolCreate(Allocator, sizeof(struct _IPCNodeContext), MaxConnectionCount);
    Socket->CommandRegistry = IndexDictionaryCreate(Allocator, 8);
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
    uv_tcp_close_reset(&Socket->Handle, NULL);
    uv_loop_close(Socket->Loop);
    free(Socket->Loop);
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
    Connection->Socket = Socket;
    Connection->ConnectionPoolIndex = ConnectionPoolIndex;
    Connection->PacketBuffer = IPCPacketBufferCreate(Socket->Allocator, 4, Socket->WriteBufferSize);
    Connection->ReadBuffer = MemoryBufferCreate(Socket->Allocator, 4, Socket->ReadBufferSize);
    return Connection;
}

Void IPCSocketReleaseConnection(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    IPCPacketBufferDestroy(Connection->PacketBuffer);
    MemoryBufferDestroy(Connection->ReadBuffer);
    IndexSetRemove(Socket->ConnectionIndices, Connection->ConnectionPoolIndex);
    MemoryPoolRelease(Socket->ConnectionPool, Connection->ConnectionPoolIndex);
}

Void IPCSocketConnect(
    IPCSocketRef Socket,
    CString Host,
    UInt16 Port,
    Timestamp Timeout
) {
    if (Socket->State != IPC_SOCKET_STATE_DISCONNECTED) return;
    
    Socket->State = IPC_SOCKET_STATE_CONNECTING;
    Socket->Host = Host;
    Socket->Port = Port;
    Socket->Timeout = Timeout;
    uv_ip4_addr(Host, Port, &Socket->Address);

    Info("Socket connecting to port: %d", Port);

    Socket->Connect.data = Socket;
    Int32 Result = uv_tcp_connect(&Socket->Connect, &Socket->Handle, (const struct sockaddr*)&Socket->Address, OnConnect);
    if (Result) {
        Fatal("Socket connection failed: %s\n", uv_strerror(Result));
        return;
    }
}

Void IPCSocketListen(
    IPCSocketRef Socket,
    UInt16 Port
) {
    if (Socket->State != IPC_SOCKET_STATE_DISCONNECTED) return;

    Socket->Port = Port;
    uv_ip4_addr("0.0.0.0", Socket->Port, &Socket->Address);

    Int32 Result = uv_tcp_bind(&Socket->Handle, (const struct sockaddr*)&Socket->Address, 0);
    if (Result) {
        Fatal("Socket binding failed: %s\n", uv_strerror(Result));
    }
    
    Result = uv_listen((uv_stream_t*)&Socket->Handle, Socket->MaxConnectionCount, OnNewConnection);
    if (Result) {
        Fatal("Socket listening failed: %s\n", uv_strerror(Result));
    }

    Info("Socket started listening on port: %d", Port);
    Socket->State = IPC_SOCKET_STATE_CONNECTED;
}

Void OnWrite(
    uv_write_t* WriteRequest,
    Int32 Status
) {
    IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)WriteRequest->data;

    if (Status < 0) {
        Error("Write error: %s\n", uv_strerror(Status));

        if (Status == UV_ECONNRESET || Status == UV_ECONNREFUSED) {
            assert((uv_stream_t*)Connection->Handle == WriteRequest->handle);
            IPCSocketDisconnect(Connection->Socket, Connection);
        }
    }

    AllocatorDeallocate(Connection->Socket->Allocator, WriteRequest);
}

Void IPCSocketSend(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection,
    IPCPacketRef Packet
) {
    assert(Socket->State == IPC_SOCKET_STATE_CONNECTED);
    assert(!(Connection->Flags & IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED));

    if (Socket->LogPackets) IPCPacketLogBytes(Packet);

    Int32 MemoryLength = sizeof(struct _IPCSocketConnectionWriteRequest) + Packet->Length;
    UInt8* Memory = AllocatorAllocate(Socket->Allocator, MemoryLength);
    if (!Memory) {
        Fatal("Memory allocation failed!");
    }

    struct _IPCSocketConnectionWriteRequest* WriteRequest = (struct _IPCSocketConnectionWriteRequest*)Memory;
    WriteRequest->Request.data = Connection;
    WriteRequest->Buffer.base = Memory + sizeof(struct _IPCSocketConnectionWriteRequest);
    WriteRequest->Buffer.len = Packet->Length;
    memcpy(WriteRequest->Buffer.base, Packet, Packet->Length);
    uv_write(&WriteRequest->Request, (uv_stream_t*)Connection->Handle, &WriteRequest->Buffer, 1, OnWrite);
}

Void IPCSocketUnicast(
    IPCSocketRef Socket,
    Void* Packet
) {
    IPCPacketRef IPCPacket = (IPCPacketRef)Packet;
    IPCPacket->RouteType = IPC_ROUTE_TYPE_UNICAST;

    Bool IsHost = Socket->Host == NULL;
    Index* ConnectionID = DictionaryLookup(Socket->NodeTable, &IPCPacket->Target.Serial);
    if (ConnectionID) {
        IPCSocketConnectionRef Connection = IPCSocketGetConnection(Socket, *ConnectionID);
        if (Connection) {
            assert(!(Connection->Flags & IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED));
            IPCSocketSend(Socket, Connection, IPCPacket); 
        }
    }
    else if (!IsHost) {
        IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
        while (Iterator) {
            Index ConnectionPoolIndex = Iterator->Value;
            Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

            IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
            assert(!(Connection->Flags & IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED));
            IPCSocketSend(Socket, Connection, IPCPacket);
        }
    }
}

Void IPCSocketBroadcast(
    IPCSocketRef Socket,
    Void* Packet
) {
    assert(Socket->State == IPC_SOCKET_STATE_CONNECTED);

    IPCPacketRef IPCPacket = (IPCPacketRef)Packet;
    IPCPacket->RouteType = IPC_ROUTE_TYPE_BROADCAST;

    IndexSetIteratorRef Iterator = IndexSetGetIterator(Socket->ConnectionIndices);
    while (Iterator) {
        Index ConnectionPoolIndex = Iterator->Value;
        Iterator = IndexSetIteratorNext(Socket->ConnectionIndices, Iterator);

        IPCSocketConnectionRef Connection = (IPCSocketConnectionRef)MemoryPoolFetch(Socket->ConnectionPool, ConnectionPoolIndex);
        assert(!(Connection->Flags & IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED));
        IPCSocketSend(Socket, Connection, IPCPacket);
    }
}

Bool IPCSocketFetchReadBuffer(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    while (MemoryBufferGetWriteOffset(Connection->ReadBuffer) >= sizeof(struct _IPCPacket)) {
        IPCPacketRef Packet = (IPCPacketRef)MemoryBufferGetMemory(Connection->ReadBuffer, 0);

        // TODO: Add error handling when packet is dropped or not fully received to meet the desired PacketLength
        if (MemoryBufferGetWriteOffset(Connection->ReadBuffer) >= Packet->Length) {
            IPCSocketOnReceived(Socket, Connection, Packet);
            MemoryBufferPopFront(Connection->ReadBuffer, Packet->Length);
        }
        else {
            break;
        }
    }

    return true;
}

Void IPCSocketUpdate(
    IPCSocketRef Socket
) {
    uv_run(Socket->Loop, UV_RUN_NOWAIT);
}

Void IPCSocketDisconnect(
    IPCSocketRef Socket,
    IPCSocketConnectionRef Connection
) {
    if (Connection->Flags & IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED) return;
    Connection->Flags |= IPC_SOCKET_CONNECTION_FLAGS_DISCONNECTED;
    uv_close((uv_handle_t*)Connection->Handle, OnClose);
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
