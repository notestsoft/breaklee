#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

CLIENT_PROCEDURE_BINDING(VIEW_EQUIPMENT_REQUEST)
{
    if (!Client) goto error;

	Client->WorldServerIndex = Packet->WorldServerIndex;

	S2C_DATA_VIEW_EQUIPMENT_RESPONSE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, VIEW_EQUIPMENT_RESPONSE);
	Response->Result = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
    SocketDisconnect(Socket, Connection);
}