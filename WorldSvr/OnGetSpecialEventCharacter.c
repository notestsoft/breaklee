#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SPECIAL_EVENT_CHARACTER) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

	S2C_DATA_GET_SPECIAL_EVENT_CHARACTER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_SPECIAL_EVENT_CHARACTER);
	if (Context->Config.WorldSvr.DebugCharacter) {
		Response->IsSet = 1;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
