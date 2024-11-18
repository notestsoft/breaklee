#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_CASH_INVENTORY) {
	if (!Character) goto error;

	S2C_DATA_GET_CASH_INVENTORY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_CASH_INVENTORY);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2559) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2559* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UNKNOWN_2559);
	Response->Unknown1 = Packet->Unknown1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2566) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2566* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UNKNOWN_2566);
	Response->Unknown1 = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2828) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2828* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UNKNOWN_2828);
    Response->Unknown1 = 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_ACHIEVEMENT_TIMELINE) {
	if (!Character) goto error;

	S2C_DATA_GET_ACHIEVEMENT_TIMELINE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GET_ACHIEVEMENT_TIMELINE);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPDATE_HELP_INFO) {
	if (!Character) goto error;

	S2C_DATA_UPDATE_HELP_INFO* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPDATE_HELP_INFO);
	Response->Result = Packet->HelpWindow;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

