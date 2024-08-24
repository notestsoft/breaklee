#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(VERIFY_CREDENTIALS) {
	Int64 Length = strlen(Packet->Credentials);
	if (Length < 1 || Length >= MAX_CREDENTIALS_LENGTH) {
		goto error;
	}

	if (Packet->CredentialsType == C2S_DATA_VERIFY_CREDENTIALS_TYPE_PASSWORD) {
		IPC_W2L_DATA_VERIFY_PASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2L, VERIFY_PASSWORD);
		Request->Header.SourceConnectionID = Connection->ID;
		Request->Header.Source = Server->IPCSocket->NodeID;
		Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
		Request->Header.Target.Type = IPC_TYPE_LOGIN;
		Request->AccountID = Client->AccountID;
		memcpy(Request->Credentials, Packet->Credentials, MAX_CREDENTIALS_LENGTH);
		IPCSocketUnicast(Server->IPCSocket, Request);
		return;
	}

error: 
	{
		S2C_DATA_VERIFY_CREDENTIALS* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_CREDENTIALS);
		Response->Success = false;
		SocketSend(Socket, Connection, Response);
	}
}

IPC_PROCEDURE_BINDING(L2W, VERIFY_PASSWORD) {
	if (!ClientConnection || !Client) return;

	if (Packet->Success) {
		Client->PasswordVerificationTimestamp = GetTimestampMs();
	}

	S2C_DATA_VERIFY_CREDENTIALS* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_CREDENTIALS);
	Response->Success = Packet->Success;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(VERIFY_CREDENTIALS_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

	IPC_W2D_DATA_VERIFY_CREDENTIALS_SUBPASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, VERIFY_CREDENTIALS_SUBPASSWORD);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	memcpy(Request->Password, Packet->Password, MAX_SUBPASSWORD_LENGTH);
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, VERIFY_CREDENTIALS_SUBPASSWORD) {
	if (!ClientConnection || !Client) return;

	if (Packet->Success) {
		Client->PasswordVerificationTimestamp = GetTimestampMs();
	}

	S2C_DATA_VERIFY_CREDENTIALS_SUBPASSWORD* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_CREDENTIALS_SUBPASSWORD);
	Response->Success = Packet->Success;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(VERIFY_SUBPASSWORD) {
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;
    
	IPC_W2D_DATA_VERIFY_SUBPASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, VERIFY_SUBPASSWORD);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->Type = Packet->Type;
	Request->ExpirationInHours = Packet->ExpirationInHours;
	Request->IsLocked = Packet->IsLocked;
	memcpy(Request->Password, Packet->Password, sizeof(Packet->Password));
	IPCSocketUnicast(Server->IPCSocket, Request);

	return;

error:
    SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, VERIFY_SUBPASSWORD) {
	if (!ClientConnection) return;

	S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_SUBPASSWORD);
	Response->Success = Packet->Success;
	Response->FailureCount = Packet->FailureCount;
	Response->IsLocked = Packet->IsLocked;
	Response->Type = Packet->Type;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
}