#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_CASHSHOP_LIST) {
    if (!Character) goto error;

	S2C_DATA_GET_CASHSHOP_LIST* Response = PacketInit(S2C_DATA_GET_CASHSHOP_LIST);
	Response->Command = S2C_GET_CASHSHOP_LIST;
    Response->IsActive = 0;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
