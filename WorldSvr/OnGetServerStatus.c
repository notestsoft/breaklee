#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SERVER_STATUS) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->AccountID < 1) goto error;

	IPC_W2W_DATA_REQUEST_SERVER_STATUS* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2W, REQUEST_SERVER_STATUS);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Packet->GroupIndex;
	Request->Header.Target.Index = Packet->NodeIndex;
	Request->Header.Target.Type = IPC_TYPE_WORLD;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(W2W, REQUEST_SERVER_STATUS) {
	IPC_W2W_DATA_RESPONSE_SERVER_STATUS* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2W, RESPONSE_SERVER_STATUS);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Status = 1;
	IPCSocketUnicast(Server->IPCSocket, Response);
}

IPC_PROCEDURE_BINDING(W2W, RESPONSE_SERVER_STATUS) {
	if (!ClientConnection) return;

	S2C_DATA_GET_SERVER_STATUS* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, GET_SERVER_STATUS);
	Response->Result = Packet->Status;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}
