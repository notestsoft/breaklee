#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(L2W, VERIFY_LINKS) {
	/*
	if (Master->ServerID != Packet->ServerID ||
		Context->Config.WorldSvr.WorldID != Packet->WorldID) {
		return SocketSend(Socket, Connection, Response);
	}
	*/

	Client = ServerGetClientByAuthKey(Context, Packet->AuthKey, Packet->EntityID);
	if (!Client || Client->Flags & CLIENT_FLAGS_VERIFIED) goto error;
	if (memcmp(Connection->AddressIP, Packet->SessionIP, MAX_ADDRESSIP_LENGTH) != 0)  goto error;

	// TODO: Check IP whitelist for WorldType == WORLD_TYPE_RESTRICTED
	// TODO: Check War status for WorldType == WORLD_TYPE_WAR_...
	// TODO: Store premium service info in client context

	IPC_W2M_DATA_GET_ACCOUNT* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2M, GET_ACCOUNT);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTER;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	{
		IPC_W2L_DATA_VERIFY_LINKS* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2L, VERIFY_LINKS);
		Response->Header.SourceConnectionID = Connection->ID;
		Response->Header.Source = Server->IPCSocket->NodeID;
		Response->Header.Target.Type = IPC_TYPE_LOGIN;
		Response->AccountID = Packet->AccountID;
		Response->Status = 0;
		IPCSocketUnicast(Server->IPCSocket, Response); 
	}
}

IPC_PROCEDURE_BINDING(M2W, GET_ACCOUNT) {
	IPC_W2L_DATA_VERIFY_LINKS* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2L, VERIFY_LINKS);
	Response->Header.SourceConnectionID = Connection->ID;
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target.Type = IPC_TYPE_LOGIN;
	Response->AccountID = Packet->AccountID;
	Response->Status = 0;

	if (Client) {
		Response->Status = 1;
		memcpy(&Client->Account, &Packet->Account, sizeof(GAME_DATA_ACCOUNT));
		Client->Flags |= CLIENT_FLAGS_VERIFIED;
	}

	IPCSocketUnicast(Server->IPCSocket, Response);
}
