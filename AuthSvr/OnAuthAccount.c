#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "Server.h"

Void StartDisconnectTimer(
    ServerRef Server,
    SocketRef Socket,
    ClientContextRef Client,
    SocketConnectionRef Connection,
    UInt64 Timeout
) {
    S2C_DATA_DISCONNECT_TIMER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DISCONNECT_TIMER);
    Response->Timeout = Timeout;
    Client->Flags |= CLIENT_FLAGS_CHECK_DISCONNECT_TIMER;
    Client->DisconnectTimestamp = ServerGetTimestamp(Server) + Timeout;
    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(AUTH_ACCOUNT) {
    if (!(Client->Flags & CLIENT_FLAGS_PUBLICKEY_INITIALIZED)) {
        return SocketDisconnect(Socket, Connection);
    }

    UInt32 ServerStatus = SERVER_STATUS_ONLINE;
    if (Context->Config.Auth.Maintenance) {
        ServerStatus = SERVER_STATUS_MAINTENANCE;
    }
    
    if (ServerStatus == SERVER_STATUS_ONLINE) {
        Client->Flags |= CLIENT_FLAGS_AUTHORIZED;
    }

    S2C_DATA_AUTH_ACCOUNT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, AUTH_ACCOUNT);
    Response->ServerStatus = ServerStatus;
    SocketSend(Socket, Connection, Response);

    if (ServerStatus == SERVER_STATUS_ONLINE) {
        if (Context->Config.Auth.AutoDisconnect) {
            StartDisconnectTimer(
                Server,
                Socket,
                Client,
                Connection,
                Context->Config.Auth.AutoDisconnectDelay
            );
        }
    } else {
        SocketDisconnect(Socket, Connection);
    }
}
