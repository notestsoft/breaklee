#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(UNKNOWN_3383) {
	S2C_DATA_UNKNOWN_3383* Response = PacketInit(S2C_DATA_UNKNOWN_3383);
	Response->Command = S2C_UNKNOWN_3383;
	SocketSend(Socket, Connection, Response);
}
