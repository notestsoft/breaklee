#include "ClientCommands.h"
#include "ClientSocket.h"
#include "ClientProcedures.h"
#include "Server.h"

Void ClientSocketOnConnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ClientContextRef Client = (ClientContextRef)ConnectionContext;
    Client->Connection = Connection;
    Client->AccountID = -1;
    Client->CharacterIndex = -1;
}

Void ClientSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ServerContextRef Context = (ServerContextRef)ServerContext;
    ClientContextRef Client = (ClientContextRef)ConnectionContext;
    Info("Client disconnected...");

    if (Client->AccountID > 0) {
        IPC_N2M_DATA_CLIENT_DISCONNECT* Notification = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, N2M, CLIENT_DISCONNECT);
        Notification->Header.Source = Server->IPCSocket->NodeID;
        Notification->Header.Target.Group = Server->IPCSocket->NodeID.Group;
        Notification->Header.Target.Type = IPC_TYPE_MASTER;
        Notification->AccountID = Client->AccountID;
        IPCSocketUnicast(Server->IPCSocket, Notification);
    }
}
