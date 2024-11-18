#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"

CLIENT_PROCEDURE_BINDING(FORCE_DISCONNECT) {
    S2C_DATA_FORCE_DISCONNECT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, FORCE_DISCONNECT);
    Response->Result = 1;

    if (Client->AccountID == Packet->AccountID) {

    }

    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(DISCONNECT) {
    SocketDisconnect(Socket, Connection);
}
