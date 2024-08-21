#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_SERVER_LIST) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED) || Client->AccountID < 1) goto error;

	IPC_W2M_DATA_GET_WORLD_LIST* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2M, GET_WORLD_LIST);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTER;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(M2W, GET_WORLD_LIST) {
	if (!ClientConnection) return;

	S2C_DATA_GET_SERVER_LIST* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, GET_SERVER_LIST);
	Response->WorldCount = Packet->NodeCount;

	Char LocalHost[] = "127.0.0.1";
	Bool IsLocalHost = strcmp(ClientConnection->AddressIP, LocalHost) == 0;
	
	Index PacketOffset = sizeof(IPC_M2W_DATA_GET_WORLD_LIST);
	for (Index Index = 0; Index < Packet->NodeCount; Index += 1) {
		IPC_M2L_DATA_SERVER_GROUP_NODE* Node = (IPC_M2L_DATA_SERVER_GROUP_NODE*)((UInt8*)Packet + PacketOffset);
		PacketOffset += sizeof(IPC_M2L_DATA_SERVER_GROUP_NODE);

		S2C_DATA_SERVER_LIST_WORLD* ResponseWorld = PacketBufferAppendStruct(ClientConnection->PacketBuffer, S2C_DATA_SERVER_LIST_WORLD);
		ResponseWorld->ServerID = Context->Config.WorldSvr.GroupIndex;
		ResponseWorld->WorldID = Node->NodeIndex;
		ResponseWorld->PlayerCount = Node->PlayerCount;
		ResponseWorld->MaxPlayerCount = Node->MaxPlayerCount;
		memcpy(ResponseWorld->WorldHost, (IsLocalHost) ? LocalHost : Node->Host, (IsLocalHost) ? sizeof(LocalHost) : sizeof(Node->Host));
		ResponseWorld->WorldPort = Node->Port;
		ResponseWorld->WorldType = Node->Type;
	}

	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(UNKNOWN_5383) {
	S2C_DATA_UNKNOWN_5383* Response = PacketBufferInit(Connection->PacketBuffer, S2C, UNKNOWN_5383);
	SocketSend(Socket, Connection, Response);
}