#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SERVER_TIME) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->AccountID < 1) goto error;
	
	S2C_DATA_GET_SERVER_TIME* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_SERVER_TIME);
	Response->Timestamp = GetTimestamp();
    Response->Timezone = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
