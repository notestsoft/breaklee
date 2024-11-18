#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"

CLIENT_PROCEDURE_BINDING(CHECK_VERSION) {
    if (!(Client->Flags & CLIENT_FLAGS_CONNECTED)) {
        SocketDisconnect(Socket, Connection);
        return;
    }

    S2C_DATA_CHECK_VERSION* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHECK_VERSION);

    Info("Client Version: %d", Packet->ClientVersion);

    if (Context->Config.Login.CheckVersion) {
        Response->ClientVersion = Context->Config.Login.ClientVersion;
        Response->ServerMagicKey = Context->Config.Login.ServerMagicKey;
        
        if (Response->ClientVersion != Context->Config.Login.ClientVersion) {
            SocketDisconnect(Socket, Connection);
            return;
        }
    }
    else {
        Response->ClientVersion = Packet->ClientVersion;
        Response->ServerMagicKey = Context->Config.Login.ServerMagicKey;
    }

    Client->ClientVersion = Packet->ClientVersion;
    Client->Flags |= CLIENT_FLAGS_VERSION_CHECKED;
    SocketSend(Socket, Connection, Response);
}
