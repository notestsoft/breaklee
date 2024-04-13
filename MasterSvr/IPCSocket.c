#include "Server.h"
#include "IPCProcs.h"
#include "IPCSocket.h"

Void AuthSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    ClientContextRef Client = (ClientContextRef)ConnectionContext;

    Client->Auth.Connection = Connection;

    IPC_DATA_AUTH_REQCONNECT* Request = PacketBufferInitExtended(Connection->PacketBuffer, IPC, AUTH_REQCONNECT);
    Request->ServerID = Context->Config.MasterSvr.ServerID;
    SocketSend(Socket, Connection, Request);
}

Void AuthSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    ClientContextRef Client = (ClientContextRef)ConnectionContext;
}

Void WorldSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    ClientContextRef Client = (ClientContextRef)ConnectionContext;
    Client->World.Connection = Connection;
}

Void WorldSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    ClientContextRef Client = (ClientContextRef)ConnectionContext;
    Context->WorldListUpdateTimestamp = ServerGetTimestamp(Server);
}
/*
Void IPCSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    IPCContextRef IPCContext = (IPCContextRef)ConnectionContext;
    IPCContext->ConnectionID = Connection->ID;
}

Void IPCSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    IPCContextRef IPCContext = (IPCContextRef)ConnectionContext;

    DictionaryRemove(Context->RouteTable, &IPCContext->RouteIndex.Serial);
    PrintRouteTable(Context);
}

Void OnIPCConnect(
    ServerRef Server,
    Void* ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void* ConnectionContext,
    Void* Data
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    IPCContextRef IPCContext = (IPCContextRef)ConnectionContext;
    IPC_DATA_CONNECT* Packet = (IPC_DATA_CONNECT*)Data;

    if (Packet->Source.Group != Context->Config.MasterSvr.ServerID) goto error;
    if (DictionaryLookup(Context->RouteTable, &Packet->Source.Serial)) goto error;

    // TODO: Validate Packet->Source for validity...

    IPCContext->RouteIndex = Packet->Source;
    DictionaryInsert(Context->RouteTable, &Packet->Source.Serial, &Connection->ID, sizeof(Index));
    PrintRouteTable(Context);
    return;

error:
    SocketDisconnect(Socket, Connection);
}

Void OnIPCRoute(
    ServerRef Server,
    Void* ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void* ConnectionContext,
    Void* Data
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    IPCContextRef IPCContext = (IPCContextRef)ConnectionContext;
    IPC_DATA_ROUTE* Packet = (IPC_DATA_ROUTE*)Data;

    IPC_DATA_ROUTE* Message = PacketBufferInitExtended(Connection->PacketBuffer, IPC, ROUTE);
    Message->Source = Packet->Source;
    Message->Target = Packet->Target;
    
    if (Packet->DataLength) {
        Void* MessageData = PacketBufferAppend(Connection->PacketBuffer, Packet->DataLength);
        memcpy(MessageData, Packet->Data, Packet->DataLength);
    }

    if (!Packet->Broadcast) {
        Index* TargetConnectionID = DictionaryLookup(Context->RouteTable, &Packet->Target.Serial);
        if (!TargetConnectionID) return;

        SocketConnectionRef TargetConnection = SocketGetConnection(Socket, *TargetConnectionID);
        if (!TargetConnection) return;

        SocketSend(Socket, TargetConnection, Message);
        return;
    }

    SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Socket);
    while (Iterator) {
        SocketConnectionRef TargetConnection = SocketConnectionIteratorFetch(Socket, Iterator);
        if (!TargetConnection->Userdata) goto next;
        
        IPCContextRef IPCContext = (IPCContextRef)TargetConnection->Userdata;
        if (IPCContext->RouteIndex.Type != Packet->Target.Type) goto next;
        if (IPCContext->RouteIndex.Group != Packet->Target.Group) goto next;

        Message->Target = IPCContext->RouteIndex;
        SocketSend(Socket, TargetConnection, Message);
    next:
        Iterator = SocketConnectionIteratorNext(Socket, Iterator);
    }
}*/