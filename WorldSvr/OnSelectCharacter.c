#include "ClientProtocol.h"
#include "ClientProcedures.h"

CLIENT_PROCEDURE_BINDING(SELECT_CHARACTER_IN_GAME) {
	if (!Character) goto error;

	S2C_DATA_SELECT_CHARACTER_IN_GAME* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SELECT_CHARACTER_IN_GAME);
	Response->Result = 1;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}