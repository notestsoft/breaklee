#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(HEART_BEAT) {
	S2C_DATA_HEART_BEAT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, HEART_BEAT);
	SocketSend(Socket, Connection, Response);
}
