#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(LOGIN) {
	if (!Client) goto error;

	Client->WorldServerIndex = Packet->WorldServerIndex;

	S2C_DATA_LOGIN* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, LOGIN);
	Response->Result = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
