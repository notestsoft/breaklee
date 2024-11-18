#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"

CLIENT_PROCEDURE_BINDING(UNKNOWN_3383) {
	S2C_DATA_UNKNOWN_3383* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UNKNOWN_3383);
	SocketSend(Socket, Connection, Response);
}
