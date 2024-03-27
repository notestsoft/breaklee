#include "Server.h"
#include "IPCProcs.h"

Void MasterSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    MasterContextRef Master = (MasterContextRef)ConnectionContext;
    
    Master->ServerID = 0;

    IPC_DATA_WORLD_REQCONNECT* Request = PacketBufferInitExtended(Connection->PacketBuffer, IPC, WORLD_REQCONNECT);
    Request->WorldID = Context->Config.WorldSvr.WorldID;
    memcpy(Request->WorldHost, Context->Config.WorldSvr.Host, sizeof(Request->WorldHost));
    Request->WorldPort = Context->Config.WorldSvr.Port;
    Request->WorldType = Context->Config.WorldSvr.WorldType;
    Request->MaxPlayerCount = Context->Config.WorldSvr.MaxConnectionCount;
    SocketSend(Socket, Connection, Request);
}

Void MasterSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    MasterContextRef Master = (MasterContextRef)ConnectionContext;

}
