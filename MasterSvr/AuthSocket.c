#include "AuthSocket.h"
#include "IPCProtocol.h"
#include "IPCProcs.h"

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
