#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_CASHSHOP_LIST) {
    if (!Character) goto error;

	S2C_DATA_GET_CASHSHOP_LIST* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_CASHSHOP_LIST);
    Response->IsActive = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
