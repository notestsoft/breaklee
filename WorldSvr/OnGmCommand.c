#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GM_COMMAND) {
    if (!Character) goto error;

    if (Packet->Command == 4) {

    }

    if (Packet->GmCommand == 7) {
        Int32 Index = 0;
        S2C_DATA_NFY_CHARACTER_PARAMETERS* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_CHARACTER_PARAMETERS);
        
        SocketSend(Socket, Connection, Notification);
    }

    S2C_DATA_GM_COMMAND* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GM_COMMAND);
    Response->Result = 0;
    Response->State = 0;
    SocketSend(Socket, Connection, Response);
    return;

error:
    SocketDisconnect(Socket, Connection);
}