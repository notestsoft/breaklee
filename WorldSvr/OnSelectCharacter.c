#include "ClientProtocol.h"
#include "ClientProcedures.h"

CLIENT_PROCEDURE_BINDING(SELECT_CHARACTER_IN_GAME) {
	if (!Character) goto error;

	S2C_DATA_SELECT_CHARACTER_IN_GAME* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, SELECT_CHARACTER_IN_GAME);
	Response->Result = 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}