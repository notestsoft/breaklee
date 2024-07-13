#include "ClientCommands.h"
#include "ClientSocket.h"
#include "ClientProtocol.h"
#include "ClientProcedures.h"

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
    Client->Flags = 0;
    Client->DisconnectTimestamp = 0;
    Client->RSA = NULL;
}

Void ClientSocketOnDisconnect(
    ServerRef Server,
    Void *ServerContext,
    SocketRef Socket,
    SocketConnectionRef Connection,
    Void *ConnectionContext
) {
    ClientContextRef Client = (ClientContextRef)ConnectionContext;
    if (Client->RSA) {
        RSA_free(Client->RSA);
    }
}
