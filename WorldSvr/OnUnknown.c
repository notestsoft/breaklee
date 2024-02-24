#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(UNKNOWN_428) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_428* Response = PacketInit(S2C_DATA_UNKNOWN_428);
	Response->Command = S2C_UNKNOWN_428;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2559) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2559* Response = PacketInit(S2C_DATA_UNKNOWN_2559);
	Response->Command = S2C_UNKNOWN_2559;
	Response->Unknown1 = Packet->Unknown1;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2566) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2566* Response = PacketInit(S2C_DATA_UNKNOWN_2566);
	Response->Command = S2C_UNKNOWN_2566;
	Response->Unknown1 = 0;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2828) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2828* Response = PacketInit(S2C_DATA_UNKNOWN_2828);
	Response->Command = S2C_UNKNOWN_2828;
    Response->Unknown1 = 1;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_UNKNOWN_USER_LIST) {
	if (!Character) goto error;

	S2C_DATA_GET_UNKNOWN_USER_LIST* Response = PacketInit(S2C_DATA_GET_UNKNOWN_USER_LIST);
	Response->Command = S2C_GET_UNKNOWN_USER_LIST;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_985) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_985* Response = PacketInit(S2C_DATA_UNKNOWN_985);
	Response->Command = S2C_UNKNOWN_985;
	Response->Result = 0;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_2173) {
	if (!Character) goto error;

	S2C_DATA_UNKNOWN_2173* Response = PacketInit(S2C_DATA_UNKNOWN_2173);
	Response->Command = S2C_UNKNOWN_2173;
	Response->Result = 0;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

