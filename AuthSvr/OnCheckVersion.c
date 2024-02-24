#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CHECK_VERSION) {
    if (!(Client->Flags & CLIENT_FLAGS_CONNECTED)) {
        return SocketDisconnect(Socket, Connection);
    }

    S2C_DATA_CHECK_VERSION* Response = PacketInit(S2C_DATA_CHECK_VERSION);
    Response->Command = S2C_CHECK_VERSION;

    LogMessageFormat(LOG_LEVEL_INFO, "Client Version: %d", Packet->ClientVersion);

    if (Context->Config.Auth.CheckVersion) {
        Response->ClientVersion = Context->Config.Auth.ClientVersion;
        Response->ServerMagicKey = Context->Config.Auth.ServerMagicKey;
        
        if (Response->ClientVersion != Context->Config.Auth.ClientVersion) {
            return SocketDisconnect(Socket, Connection);
        }
    }
    else {
        Response->ClientVersion = Packet->ClientVersion;
        Response->ServerMagicKey = Context->Config.Auth.ServerMagicKey;
    }

    Client->ClientVersion = Packet->ClientVersion;
    Client->Flags |= CLIENT_FLAGS_VERSION_CHECKED;
    SocketSend(Socket, Connection, Response);
}
