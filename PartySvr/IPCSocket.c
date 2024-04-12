#include "IPCProtocol.h"
#include "Server.h"

Void IPCSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    IPCContextRef IPCContext = (IPCContextRef)ConnectionContext;

    Int32 Seed = GetTickCount();
    IPC_DATA_CONNECT* Request = PacketBufferInitExtended(Connection->PacketBuffer, IPC, CONNECT);
    Request->Source.Index = Random(&Seed);
    Request->Source.Group = 1;
    Request->Source.Type = IPC_TYPE_PARTY;
    SocketSend(Socket, Connection, Request);
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

}
