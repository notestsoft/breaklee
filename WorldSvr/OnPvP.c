#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"


CLIENT_PROCEDURE_BINDING(PVP_REQUEST) {
    if (!Character) goto error;

   

    S2C_DATA_PVP_REQUEST* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PVP_REQUEST);
    Response->Result = RTPvPManagerRequestPvP(Runtime->PvPManager, Character, Packet->TargetIndex, Packet->PvpType);
    SocketSend(Socket, Connection, Response);

    return;

error:
    SocketDisconnect(Socket, Connection);
    return;
}

CLIENT_PROCEDURE_BINDING(PVP_RESPONSE) {
    if (!Character) goto error;
    if (!Character) return;

    S2C_DATA_PVP_RESPONSE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, PVP_RESPONSE);
    Response->Result = RTPvPManagerRequestPvPResponse(Runtime->PvPManager, Character, Packet->PvpType, Packet->PvpResult);
    SocketSend(Socket, Connection, Response);

    return;

error:
    SocketDisconnect(Socket, Connection);
    return;
}


CLIENT_PROCEDURE_BINDING(PVP_REQUEST_END) {
    if (!Character) goto error;

    return;

error:
    SocketDisconnect(Socket, Connection);
    return;
}
