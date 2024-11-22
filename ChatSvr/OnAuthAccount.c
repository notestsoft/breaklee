#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(AUTH_ACCOUNT) {
	if (!Client) goto error;

	// TODO: Check if connection ip is same as worldsvr ip
	Client->CharacterIndex = Packet->CharacterIndex;

	S2C_DATA_AUTH_ACCOUNT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, AUTH_ACCOUNT);
	Response->CharacterIndex = Client->CharacterIndex;
	Response->GuildIndex = 0;
	Response->Result = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
