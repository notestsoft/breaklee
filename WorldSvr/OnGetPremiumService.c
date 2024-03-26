#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_PREMIUM_SERVICE) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->Account.AccountID < 1) goto error;

	IPC_DATA_WORLD_REQPREMIUMSERVICE* Request = PacketInitExtended(IPC_DATA_WORLD_REQPREMIUMSERVICE);
	Request->Command = IPC_WORLD_REQPREMIUMSERVICE;
	Request->ConnectionID = Connection->ID;
	Request->AccountID = Client->Account.AccountID;
	return SocketSendAll(Context->MasterSocket, Request);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(PREMIUM_BENEFIT_INFO) {
	if (!Character) goto error;

	S2C_DATA_PREMIUM_BENEFIT_INFO* Response = PacketInitExtended(S2C_DATA_PREMIUM_BENEFIT_INFO);
	Response->Command = S2C_PREMIUM_BENEFIT_INFO;
	return SocketSend(Context->ClientSocket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(OnWorldGetPremiumService, IPC_WORLD_ACKPREMIUMSERVICE, IPC_DATA_WORLD_ACKPREMIUMSERVICE) {
	if (!ClientConnection || !Client) goto error;

    S2C_DATA_GET_PREMIUM_SERVICE* Response = PacketInit(S2C_DATA_GET_PREMIUM_SERVICE);
    Response->Command = S2C_GET_PREMIUM_SERVICE;
    Response->ServiceType = Packet->ServiceType;
    Response->ExpirationDate = Packet->ExpiredAt;
    SocketSend(Context->ClientSocket, ClientConnection, Response);

	S2C_DATA_NFY_PREMIUM_SERVICE* Notification = PacketInit(S2C_DATA_NFY_PREMIUM_SERVICE);
	Notification->Command = S2C_NFY_PREMIUM_SERVICE;
	Notification->ServiceType = Packet->ServiceType;
	Notification->ExpirationDate = Packet->ExpiredAt;
	SocketSend(Context->ClientSocket, ClientConnection, Notification);

	S2C_DATA_NFY_UNKNOWN_2518* UnknownNotification = PacketInit(S2C_DATA_NFY_UNKNOWN_2518);
	UnknownNotification->Command = S2C_NFY_UNKNOWN_2518;
	/*
	UnknownNotification->Unknown1[0] = 4;
	UnknownNotification->Unknown1[1] = 1;
	UnknownNotification->Unknown1[2] = 2;
	UnknownNotification->Unknown1[3] = 0;
	UnknownNotification->Unknown1[4] = 20;
	UnknownNotification->Unknown1[5] = 1;
	*/
	SocketSend(Context->ClientSocket, ClientConnection, UnknownNotification);
	return;

error:
    return SocketDisconnect(Socket, Connection);
}
