#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(UNKNOWN_428) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_428* Response = PacketBufferInit(Connection->PacketBuffer, S2C, UNKNOWN_428);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2559) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2559* Response = PacketBufferInit(Connection->PacketBuffer, S2C, UNKNOWN_2559);
	Response->Unknown1 = Packet->Unknown1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2566) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2566* Response = PacketBufferInit(Connection->PacketBuffer, S2C, UNKNOWN_2566);
	Response->Unknown1 = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2828) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2828* Response = PacketBufferInit(Connection->PacketBuffer, S2C, UNKNOWN_2828);
    Response->Unknown1 = 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_UNKNOWN_USER_LIST) {
	if (!Character) goto error;

	S2C_DATA_GET_UNKNOWN_USER_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_UNKNOWN_USER_LIST);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPDATE_HELP_INFO) {
	if (!Character) goto error;

	S2C_DATA_UPDATE_HELP_INFO* Response = PacketBufferInit(Connection->PacketBuffer, S2C, UPDATE_HELP_INFO);
	Response->Result = Packet->HelpWindow;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

