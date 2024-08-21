#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SERVER_TIME) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->AccountID < 1) goto error;
	
	S2C_DATA_GET_SERVER_TIME* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_SERVER_TIME);
	Response->Timestamp = ServerGetTimestamp(Server);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
