#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SPECIAL_EVENT_CHARACTER) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

	S2C_DATA_GET_SPECIAL_EVENT_CHARACTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_SPECIAL_EVENT_CHARACTER);
	Response->IsSet = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
