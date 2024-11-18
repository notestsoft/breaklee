#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_CASH_BALANCE) {
    if (!Character) goto error;

    S2C_DATA_GET_CASH_BALANCE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_CASH_BALANCE);
    Response->Amount = 999;
	SocketSend(Socket, Connection, Response);
    return;

error:
	SocketDisconnect(Socket, Connection);
}
