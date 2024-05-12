#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_PREMIUM_SERVICE) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->Account.AccountID < 1) goto error;

	IPC_W2M_DATA_GET_PREMIUM_SERVICE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2M, GET_PREMIUM_SERVICE);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTER;
	Request->AccountID = Client->Account.AccountID;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PREMIUM_BENEFIT_INFO) {
	if (!Character) goto error;

	S2C_DATA_PREMIUM_BENEFIT_INFO* Response = PacketBufferInitExtended(Connection->PacketBuffer, S2C, PREMIUM_BENEFIT_INFO);
	return SocketSend(Context->ClientSocket, Connection, Response);

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(M2W, GET_PREMIUM_SERVICE) {
	if (!ClientConnection || !Client) return;

    S2C_DATA_GET_PREMIUM_SERVICE* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, GET_PREMIUM_SERVICE);
    Response->ServiceType = Packet->ServiceType;
    Response->ExpirationDate = Packet->ExpiredAt;
    SocketSend(Context->ClientSocket, ClientConnection, Response);

	S2C_DATA_NFY_PREMIUM_SERVICE* Notification = PacketBufferInit(ClientConnection->PacketBuffer, S2C, NFY_PREMIUM_SERVICE);
	Notification->ServiceType = Packet->ServiceType;
	Notification->ExpirationDate = Packet->ExpiredAt;
	SocketSend(Context->ClientSocket, ClientConnection, Notification);

	S2C_DATA_NFY_UNKNOWN_2518* UnknownNotification = PacketBufferInit(ClientConnection->PacketBuffer, S2C, NFY_UNKNOWN_2518);
	/*
	UnknownNotification->Unknown1[0] = 4;
	UnknownNotification->Unknown1[1] = 1;
	UnknownNotification->Unknown1[2] = 2;
	UnknownNotification->Unknown1[3] = 0;
	UnknownNotification->Unknown1[4] = 20;
	UnknownNotification->Unknown1[5] = 1;
	*/
	SocketSend(Context->ClientSocket, ClientConnection, UnknownNotification);
}
