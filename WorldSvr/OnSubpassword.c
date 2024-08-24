#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CREATE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

	IPC_W2D_DATA_CREATE_SUBPASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, CREATE_SUBPASSWORD);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	memcpy(Request->Password, Packet->Password, MAX_SUBPASSWORD_LENGTH);
	Request->Type = Packet->Type;
	Request->Question = Packet->Question;
	memcpy(Request->Answer, Packet->Answer, MAX_SUBPASSWORD_ANSWER_LENGTH);
	Request->IsChange = Packet->IsChange;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, CREATE_SUBPASSWORD) {
	if (!ClientConnection) return;

	S2C_DATA_CREATE_SUBPASSWORD* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, CREATE_SUBPASSWORD);
	Response->Success = Packet->Success;
	Response->IsChange = Packet->IsChange;
	Response->Type = Packet->Type;
	Response->IsLocked = Packet->IsLocked;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(DELETE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

	Bool IsDeletionVerified = Client->Flags & CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION;
	if (!IsDeletionVerified) goto error;

	IPC_W2D_DATA_DELETE_SUBPASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, DELETE_SUBPASSWORD);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->Type = Packet->Type;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	{
		S2C_DATA_DELETE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DELETE_SUBPASSWORD);
		Response->Success = 0;
		Response->Type = Packet->Type;
		SocketSend(Socket, Connection, Response);
	}
}

IPC_PROCEDURE_BINDING(D2W, DELETE_SUBPASSWORD) {
	if (!ClientConnection) return;

	Client->Flags &= ~CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION;

	S2C_DATA_DELETE_SUBPASSWORD* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, DELETE_SUBPASSWORD);
	Response->Success = Packet->Success;
	Response->Type = Packet->Type;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(VERIFY_DELETE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

	Bool IsDeletionVerified = Client->Flags & CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION;
	if (!IsDeletionVerified) goto error;

	IPC_W2D_DATA_VERIFY_DELETE_SUBPASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, VERIFY_DELETE_SUBPASSWORD);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->Type = Packet->Type;
	memcpy(Request->Password, Packet->Password, MAX_SUBPASSWORD_LENGTH);
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, VERIFY_DELETE_SUBPASSWORD) {
	if (!ClientConnection) return;

	if (Packet->Success) {
		Client->Flags |= CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION;
	}

	S2C_DATA_VERIFY_DELETE_SUBPASSWORD* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_DELETE_SUBPASSWORD);
	Response->Success = Packet->Success;
	Response->FailureCount = Packet->FailureCount;
	Response->Type = Packet->Type;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(CHECK_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

	IPC_W2D_DATA_CHECK_SUBPASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, CHECK_SUBPASSWORD);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, CHECK_SUBPASSWORD) {
	if (!ClientConnection) return;

	S2C_DATA_CHECK_SUBPASSWORD* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, CHECK_SUBPASSWORD);
	Response->IsVerificationRequired = Packet->IsVerificationRequired;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}
