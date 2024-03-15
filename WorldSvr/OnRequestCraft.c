#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(REQUEST_CRAFT) {
	// TODO: Implementation missing

	PacketLogBytes(Packet);

	S2C_DATA_REQUEST_CRAFT *Response = PacketInit(S2C_DATA_REQUEST_CRAFT);
	Response->Command = S2C_REQUEST_CRAFT;
	Response->Success = 0;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}