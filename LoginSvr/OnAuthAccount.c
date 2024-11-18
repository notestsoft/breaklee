#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(AUTH_ACCOUNT) {
    if (!(Client->Flags & CLIENT_FLAGS_PUBLICKEY_INITIALIZED)) {
        SocketDisconnect(Socket, Connection);
        return;
    }

    UInt32 ServerStatus = SERVER_STATUS_ONLINE;
    if (Context->Config.Login.Maintenance) {
        ServerStatus = SERVER_STATUS_MAINTENANCE;
    }
    
    if (ServerStatus == SERVER_STATUS_ONLINE) {
        Client->Flags |= CLIENT_FLAGS_AUTHORIZED;
    }

    S2C_DATA_AUTH_ACCOUNT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, AUTH_ACCOUNT);
    Response->ServerStatus = ServerStatus;
    Response->Unknown1 = 3;
    SocketSend(Socket, Connection, Response);

    if (ServerStatus == SERVER_STATUS_ONLINE) {
        if (Context->Config.Login.AutoDisconnect) {
            StartDisconnectTimer(
                Server,
                Socket,
                Client,
                Connection,
                Context->Config.Login.AutoDisconnectDelay,
                0
            );
        }
    } else {
        SocketDisconnect(Socket, Connection);
    }
}
