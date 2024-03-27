#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(UNKNOWN_5383) {
	S2C_DATA_UNKNOWN_5383* Response = PacketBufferInit(Connection->PacketBuffer, S2C, UNKNOWN_5383);
	SocketSend(Socket, Connection, Response);
}
