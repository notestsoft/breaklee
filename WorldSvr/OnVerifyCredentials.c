#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Enumerations.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

Void OnVerifyCharacterSubpassword(
    ServerRef Server,
    ServerContextRef Context,
    SocketRef Socket,
    ClientContextRef Client,
    SocketConnectionRef Connection,
    C2S_DATA_VERIFY_SUBPASSWORD* Packet
) {
	Int32 SubpasswordLength = strlen(Client->Account.CharacterPassword);
    Bool IsSubpasswordSet = SubpasswordLength > 0;
    if (!IsSubpasswordSet || strlen(Packet->Password) != SubpasswordLength ||
        memcmp(Client->Account.CharacterPassword, Packet->Password, SubpasswordLength) != 0) {
        Client->SubpasswordFailureCount += 1;
        if (Client->SubpasswordFailureCount >= Context->Config.WorldSvr.MaxSubpasswordFailureCount) {
            // TODO: Ban account based on configuration due to max failure count reach!
        }

        S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_SUBPASSWORD);
        Response->Success = 0;
        Response->FailureCount = Client->SubpasswordFailureCount;
        Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
        SocketSend(Socket, Connection, Response);
		return;
    }

    Client->SubpasswordFailureCount = 0;
    memcpy(Client->Account.SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
    Client->Account.SessionTimeout = time(NULL) + (time_t)Packet->ExpirationInHours * 60 * 60;

	IPC_W2M_DATA_UPDATE_ACCOUNT_SESSION* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2M, UPDATE_ACCOUNT_SESSION);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTER;
    Request->AccountID = Client->Account.AccountID;
    memcpy(Request->SessionIP, Client->Account.SessionIP, MAX_ADDRESSIP_LENGTH);
    Request->SessionTimeout = Client->Account.SessionTimeout;
	IPCSocketUnicast(Server->IPCSocket, Request);

    S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_SUBPASSWORD);
    Response->Success = 1;
    Response->FailureCount = Client->SubpasswordFailureCount;
    Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
    SocketSend(Socket, Connection, Response);
}

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
		Request->AccountID = Client->Account.AccountID;
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
	if (!ClientConnection || !Client) goto error;

	if (Packet->Success) {
		Client->PasswordVerificationTimestamp = GetTimestampMs();
	}

	S2C_DATA_VERIFY_CREDENTIALS* Response = PacketBufferInit(ClientConnection->PacketBuffer, S2C, VERIFY_CREDENTIALS);
	Response->Success = Packet->Success;
    return SocketSend(Context->ClientSocket, ClientConnection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(VERIFY_CREDENTIALS_SUBPASSWORD) {
	/*
	Void OnVerifyCharacterSubpassword(
		ServerRef Server,
		SocketRef Socket,
		ClientContextRef Client,
		SocketConnectionRef Connection,
		C2S_DATA_VERIFY_SUBPASSWORD * Packet
	) {
		Bool IsSubpasswordSet = strlen(Client->Account.CharacterPassword) > 0;
		if (!IsSubpasswordSet || strlen(Packet->Password) < MIN_SUBPASSWORD_LENGTH ||
			memcmp(Client->Account.CharacterPassword, Packet->Password, MAX_SUBPASSWORD_LENGTH) != 0) {
			Client->SubpasswordFailureCount += 1;
			if (Client->SubpasswordFailureCount >= Context->Config.WorldSvr.MaxSubpasswordFailureCount) {
				// TODO: Ban account based on configuration due to max failure count reach!
			}

			S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_SUBPASSWORD);
			Response->Success = 0;
			Response->FailureCount = Client->SubpasswordFailureCount;
			Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
			return SocketSend(Socket, Connection, Response);
		}

		Client->SubpasswordFailureCount = 0;
		memcpy(Client->Account.SessionIP, Connection->AddressIP, MAX_ADDRESSIP_LENGTH);
		Client->Account.SessionTimeout = time(NULL) + (time_t)Packet->ExpirationInHours * 60 * 60;

		IPC_DATA_WORLD_UPDATE_ACCOUNT_SESSION_DATA* Request = PacketBufferInitExtended(IPC_DATA_WORLD_UPDATE_ACCOUNT_SESSION_DATA);
		Request->ConnectionID = Connection->ID;
		Request->AccountID = Client->Account.AccountID;
		memcpy(Request->SessionIP, Client->Account.SessionIP, MAX_ADDRESSIP_LENGTH);
		Request->SessionTimeout = Client->Account.SessionTimeout;
		SocketSendAll(Server->MasterSocket, Request);

		S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_SUBPASSWORD);
		Response->Success = 1;
		Response->FailureCount = Client->SubpasswordFailureCount;
		Response->Type = CHARACTER_SUBPASSWORD_TYPE_CHARACTER;
		return SocketSend(Socket, Connection, Response);
	}
	*/

	if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

	S2C_DATA_VERIFY_CREDENTIALS_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_CREDENTIALS_SUBPASSWORD);
	Response->Success = 1;

	Int32 SubpasswordLength = strlen(Client->Account.CharacterPassword);
	Bool IsSubpasswordSet = SubpasswordLength > 0;
	if (!IsSubpasswordSet || strlen(Packet->Password) != SubpasswordLength ||
		memcmp(Client->Account.CharacterPassword, Packet->Password, SubpasswordLength) != 0) {
		Client->SubpasswordFailureCount += 1;
		if (Client->SubpasswordFailureCount >= Context->Config.WorldSvr.MaxSubpasswordFailureCount) {
			// TODO: Ban account based on configuration due to max failure count reach!
		}

		Response->Success = 0;
	}

	/*
	return OnVerifyCharacterSubpassword(
		Server,
        Context,
		Socket,
		Client,
		Connection,
		Packet // TODO: Verify wrong packet is begin passed here...
	);
	*/
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(VERIFY_SUBPASSWORD) {
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;
    
    if (Packet->Type == CHARACTER_SUBPASSWORD_TYPE_CHARACTER) {
        return OnVerifyCharacterSubpassword(
            Server,
            Context,
            Socket,
            Client,
            Connection,
            Packet
        );
    }

    S2C_DATA_VERIFY_SUBPASSWORD* Response = PacketBufferInit(Connection->PacketBuffer, S2C, VERIFY_SUBPASSWORD);
    Response->Success = 0;
    Response->FailureCount = 0; 
    Response->Type = Packet->Type;
    return SocketSend(Socket, Connection, Response);

error:
    return SocketDisconnect(Socket, Connection);
}
