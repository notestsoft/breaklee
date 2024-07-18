#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

Void OnCreateCharacterSubpassword(
	ServerRef Server,
    ServerContextRef Context,
	SocketRef Socket,
	ClientContextRef Client,
	SocketConnectionRef Connection,
	C2S_DATA_CREATE_SUBPASSWORD* Packet
) {
	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	if (IsSubpasswordSet ||
		strlen(Packet->Password) < MIN_SUBPASSWORD_LENGTH ||
		strlen(Packet->Answer) < MIN_SUBPASSWORD_ANSWER_LENGTH) {
		goto error;
	}

	memcpy(Client->Account.CharacterPassword, Packet->Password, MAX_SUBPASSWORD_LENGTH);
	Client->Account.CharacterQuestion = Packet->Question;
	memcpy(Client->Account.CharacterAnswer, Packet->Answer, MAX_SUBPASSWORD_ANSWER_LENGTH);

	IPC_W2D_DATA_UPDATE_SUBPASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, UPDATE_SUBPASSWORD);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->Account.AccountID;
	memcpy(Request->CharacterPassword, Client->Account.CharacterPassword, MAX_SUBPASSWORD_LENGTH);
	Request->CharacterQuestion = Client->Account.CharacterQuestion;
	memcpy(Request->CharacterAnswer, Client->Account.CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH);
	IPCSocketUnicast(Server->IPCSocket, Request);

	S2C_DATA_CREATE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CREATE_SUBPASSWORD);
	Response->Success = 1;
	Response->Mode = Packet->Mode;
	Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CREATE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	if (Packet->Type == CHARACTER_SUBPASSWORD_TYPE_CHARACTER) {
		OnCreateCharacterSubpassword(
			Server,
            Context,
			Socket,
			Client,
			Connection,
			Packet
		);
		return;
	}

	S2C_DATA_CREATE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CREATE_SUBPASSWORD);
	Response->Success = 0;
	Response->Mode = Packet->Mode;
	Response->Type = Packet->Type;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

Void OnDeleteCharacterSubpassword(
	ServerRef Server,
	ServerContextRef Context,
	SocketRef Socket,
	ClientContextRef Client,
	SocketConnectionRef Connection,
	C2S_DATA_DELETE_SUBPASSWORD* Packet
) {
	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	Bool IsDeletionVerified = Client->Flags & CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION;
	Bool IsSessionExpired = Client->Account.SessionTimeout <= time(NULL);
	if (!IsSubpasswordSet || !IsDeletionVerified || IsSessionExpired) {
		S2C_DATA_DELETE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DELETE_SUBPASSWORD);
		Response->Success = 0;
		Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
		SocketSend(Socket, Connection, Response);
		return;
	}

	memset(Client->Account.CharacterPassword, 0, MAX_SUBPASSWORD_LENGTH);
	Client->Account.CharacterQuestion = 0;
	memset(Client->Account.CharacterAnswer, 0, MAX_SUBPASSWORD_ANSWER_LENGTH);

	IPC_W2D_DATA_UPDATE_SUBPASSWORD* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, UPDATE_SUBPASSWORD);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->Account.AccountID;
	memcpy(Request->CharacterPassword, Client->Account.CharacterPassword, MAX_SUBPASSWORD_LENGTH);
	Request->CharacterQuestion = Client->Account.CharacterQuestion;
	memcpy(Request->CharacterAnswer, Client->Account.CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH);
	IPCSocketUnicast(Server->IPCSocket, Request);

	S2C_DATA_DELETE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DELETE_SUBPASSWORD);
	Response->Success = 1;
	Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(DELETE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	if (Packet->Type == CHARACTER_SUBPASSWORD_TYPE_CHARACTER) {
		OnDeleteCharacterSubpassword(
			Server,
			Context,
			Socket,
			Client,
			Connection,
			Packet
		);
		return;
	}

	S2C_DATA_DELETE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, DELETE_SUBPASSWORD);
	Response->Success = 0;
	Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

Void OnVerifyDeleteCharacterSubpassword(
	ServerRef Server,
    ServerContextRef Context,
	SocketRef Socket,
	ClientContextRef Client,
	SocketConnectionRef Connection,
	C2S_DATA_VERIFY_DELETE_SUBPASSWORD* Packet
) {
	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	if (!IsSubpasswordSet || strlen(Packet->Password) < MIN_SUBPASSWORD_LENGTH ||
		memcmp(Client->Account.CharacterPassword, Packet->Password, MAX_SUBPASSWORD_LENGTH) != 0) {
		Client->SubpasswordFailureCount += 1;
		if (Client->SubpasswordFailureCount >= Context->Config.WorldSvr.MaxSubpasswordFailureCount) {
			// TODO: Ban account based on configuration due to max failure count reach!
		}

		S2C_DATA_VERIFY_DELETE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_DELETE_SUBPASSWORD);
		Response->Success = 0;
		Response->FailureCount = Client->SubpasswordFailureCount;
		Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
		SocketSend(Socket, Connection, Response);
		return;
	}

	Client->SubpasswordFailureCount = 0;
	Client->Flags |= CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION;
	Client->Account.SessionTimeout = MAX(Client->Account.SessionTimeout, time(NULL) + 60);

	S2C_DATA_VERIFY_DELETE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_DELETE_SUBPASSWORD);
	Response->Success = 1;
	Response->FailureCount = Client->SubpasswordFailureCount;
	Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(VERIFY_DELETE_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	if (Packet->Type == CHARACTER_SUBPASSWORD_TYPE_CHARACTER) {
		OnVerifyDeleteCharacterSubpassword(
			Server,
            Context,
			Socket,
			Client,
			Connection,
			Packet
		);
		return;
	}

	S2C_DATA_VERIFY_DELETE_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_DELETE_SUBPASSWORD);
	Response->Success = 0;
	Response->FailureCount =0;
	Response->Type = Packet->Type;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CHECK_SUBPASSWORD) {
	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;
	
	Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
	Bool IsVerifiedSession = time(NULL) < Client->Account.SessionTimeout;

	S2C_DATA_CHECK_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CHECK_SUBPASSWORD);
	Response->IsVerificationRequired = !IsVerifiedSession && IsSubpasswordSet;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
