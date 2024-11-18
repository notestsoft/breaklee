#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_DUNGEON_EXPIRATION_TIME) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

	S2C_DATA_GET_DUNGEON_EXPIRATION_TIME* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_DUNGEON_EXPIRATION_TIME);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
