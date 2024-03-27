#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SERVER_TIME) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->Account.AccountID < 1) goto error;
	
	S2C_DATA_GET_SERVER_TIME* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_SERVER_TIME);
	Response->Timestamp = ServerGetTimestamp(Server);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
