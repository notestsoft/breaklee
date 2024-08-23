#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(VERIFY_LINKS) {
	if (!(Client->Flags & CLIENT_FLAGS_VERIFIED)) goto error;

	if (Packet->GroupIndex == MAX_SERVER_COUNT) {
		IPC_W2L_DATA_WORLD_VERIFY_LINKS* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2L, WORLD_VERIFY_LINKS);
		Request->Header.SourceConnectionID = Client->Connection->ID;
		Request->Header.Source = Server->IPCSocket->NodeID;
		Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
		Request->Header.Target.Type = IPC_TYPE_LOGIN;
		Request->AccountID = (Int32)Client->AccountID;
		Request->AuthKey = Packet->AuthKey;
		Request->EntityID = Packet->EntityID;
		Request->GroupIndex = Packet->GroupIndex;
		Request->NodeIndex = Packet->NodeIndex;
		Request->ClientMagicKey = Packet->ClientMagicKey;
		memcpy(Request->SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
		IPCSocketUnicast(Server->IPCSocket, Request);
	}
	else {
		IPC_W2W_DATA_REQUEST_VERIFY_LINKS* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2W, REQUEST_VERIFY_LINKS);
		Request->Header.SourceConnectionID = Client->Connection->ID;
		Request->Header.Source = Server->IPCSocket->NodeID;
		Request->Header.Target.Group = Packet->GroupIndex;
		Request->Header.Target.Index = Packet->NodeIndex;
		Request->Header.Target.Type = IPC_TYPE_WORLD;
		Request->AccountID = (Int32)Client->AccountID;
		Request->AuthKey = Packet->AuthKey;
		Request->EntityID = Packet->EntityID;
		Request->GroupIndex = Packet->GroupIndex;
		Request->NodeIndex = Packet->NodeIndex;
		memcpy(Request->SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
		Request->IsSubpasswordSet = Client->IsSubpasswordSet;
		Request->AccountInfo = Client->AccountInfo;
		memcpy(&Request->Characters, &Client->Characters, sizeof(IPC_DATA_CHARACTER_INFO) * MAX_CHARACTER_COUNT);
		IPCSocketUnicast(Server->IPCSocket, Request);
	}

	SocketDisconnect(Socket, Connection);
	return;

error:
	{
		S2C_DATA_VERIFY_LINKS* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_LINKS);
		Response->ServerID = Packet->GroupIndex;
		Response->WorldID = Packet->NodeIndex;
		Response->Status = 0;
		SocketSend(Socket, Connection, Response);
	}
}

IPC_PROCEDURE_BINDING(L2W, WORLD_VERIFY_LINKS) {
	if (!ClientConnection) return;

	S2C_DATA_VERIFY_LINKS* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_LINKS);
	Response->ServerID = Context->Config.WorldSvr.GroupIndex;
	Response->WorldID = Context->Config.WorldSvr.NodeIndex;
	Response->Status = Packet->Success;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
	SocketDisconnect(Context->ClientSocket, ClientConnection);
}

IPC_PROCEDURE_BINDING(L2W, VERIFY_LINKS) {
	/*
	if (Master->ServerID != Packet->ServerID ||
		Context->Config.WorldSvr.WorldID != Packet->WorldID) {
		return SocketSend(Socket, Connection, Response);
	}
	*/

	// TODO: AddressIP of connection is not set!
	// if (memcmp(Connection->AddressIP, Packet->SessionIP, MAX_ADDRESSIP_LENGTH) != 0)  goto error;

	// TODO: Check IP whitelist for WorldType == WORLD_TYPE_RESTRICTED
	// TODO: Check War status for WorldType == WORLD_TYPE_WAR_...
	// TODO: Store premium service info in client context

	Client = ServerGetClientByAuthKey(Context, Packet->AuthKey, Packet->EntityID);
	if (!Client || Client->Flags & CLIENT_FLAGS_VERIFIED) goto error;
	
	IPC_W2D_DATA_AUTHENTICATE* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, AUTHENTICATE);
	Request->Header.SourceConnectionID = Client->Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Packet->AccountID;
	Request->GroupIndex = Packet->GroupIndex;
	Request->NodeIndex = Packet->NodeIndex;
	Request->LinkConnectionID = Packet->Header.SourceConnectionID;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	{
		IPC_W2L_DATA_VERIFY_LINKS* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2L, VERIFY_LINKS);
		Response->Header.SourceConnectionID = Connection->ID;
		Response->Header.Source = Server->IPCSocket->NodeID;
		Response->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
		Response->Header.Target.Type = IPC_TYPE_LOGIN;
		Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
		Response->AccountID = Packet->AccountID;
		Response->GroupIndex = Packet->GroupIndex;
		Response->NodeIndex = Packet->NodeIndex;
		Response->Status = 0;
		IPCSocketUnicast(Server->IPCSocket, Response); 
	}
}

IPC_PROCEDURE_BINDING(D2W, AUTHENTICATE) {
	IPC_W2L_DATA_VERIFY_LINKS* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2L, VERIFY_LINKS);
	Response->Header.SourceConnectionID = ClientConnection->ID;
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Response->Header.Target.Type = IPC_TYPE_LOGIN;
	Response->Header.TargetConnectionID = Packet->LinkConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->GroupIndex = Packet->GroupIndex;
	Response->NodeIndex = Packet->NodeIndex;
	Response->Status = 0;

	if (Client && Packet->Success) {
		Response->Status = 1;
		Client->Flags |= CLIENT_FLAGS_VERIFIED;
		Client->AccountID = Packet->AccountID;
		Client->IsSubpasswordSet = Packet->IsSubpasswordSet;
		Client->AccountInfo = Packet->AccountInfo;
	}

	IPCSocketUnicast(Server->IPCSocket, Response);
}

IPC_PROCEDURE_BINDING(W2W, REQUEST_VERIFY_LINKS) {
	Client = ServerGetClientByAuthKey(Context, Packet->AuthKey, Packet->EntityID);
	if (!Client) goto error;

	// TODO: AddressIP of connection is not set!
	// if (memcmp(Connection->AddressIP, Packet->SessionIP, MAX_ADDRESSIP_LENGTH) != 0)  goto error;

	// TODO: Check IP whitelist for WorldType == WORLD_TYPE_RESTRICTED
	// TODO: Check War status for WorldType == WORLD_TYPE_WAR_...
	// TODO: Store premium service info in client context

	Client->Flags |= CLIENT_FLAGS_VERIFIED;
	Client->Flags |= CLIENT_FLAGS_CHARACTER_INDEX_LOADED;
	Client->AccountID = Packet->AccountID;
	Client->IsSubpasswordSet = Packet->IsSubpasswordSet;
	Client->AccountInfo = Packet->AccountInfo;
	memcpy(&Client->Characters, &Packet->Characters, sizeof(IPC_DATA_CHARACTER_INFO) * MAX_CHARACTER_COUNT);

	IPC_W2W_DATA_RESPONSE_VERIFY_LINKS* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2W, RESPONSE_VERIFY_LINKS);
	Response->Header.SourceConnectionID = Connection->ID;
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->GroupIndex = Packet->GroupIndex;
	Response->NodeIndex = Packet->NodeIndex;
	Response->Status = 1;
	IPCSocketUnicast(Server->IPCSocket, Response);
	return;

error:
	{
		IPC_W2W_DATA_RESPONSE_VERIFY_LINKS* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2W, RESPONSE_VERIFY_LINKS);
		Response->Header.SourceConnectionID = Connection->ID;
		Response->Header.Source = Server->IPCSocket->NodeID;
		Response->Header.Target = Packet->Header.Source;
		Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
		Response->AccountID = Packet->AccountID;
		Response->GroupIndex = Packet->GroupIndex;
		Response->NodeIndex = Packet->NodeIndex;
		Response->Status = 0;
		IPCSocketUnicast(Server->IPCSocket, Response);
	}
}

IPC_PROCEDURE_BINDING(W2W, RESPONSE_VERIFY_LINKS) {
	if (!ClientConnection) return;

	S2C_DATA_VERIFY_LINKS* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_LINKS);
	Response->ServerID = Packet->GroupIndex;
	Response->WorldID = Packet->NodeIndex;
	Response->Status = Packet->Status;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
	SocketDisconnect(Context->ClientSocket, ClientConnection);
}
