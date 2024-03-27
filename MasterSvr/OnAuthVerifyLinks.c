#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"

IPC_PROCEDURE_BINDING(OnAuthVerifyLinks, IPC_AUTH_VERIFYLINKS, IPC_DATA_AUTH_VERIFYLINKS) {
	if (Context->Config.MasterSvr.ServerID != Packet->ServerID) goto error;

	ClientContextRef WorldClient = ServerGetWorldClient(Server, Context, Packet->WorldID);
	if (!WorldClient) goto error;

	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;

	if (!MasterDBGetOrCreateAccount(Context->Database, &Account)) goto error;

	IPC_DATA_WORLD_VERIFYLINKS* Request = PacketBufferInitExtended(WorldClient->World.Connection->PacketBuffer, IPC, WORLD_VERIFYLINKS);
	Request->ConnectionID = Packet->ConnectionID;
	Request->ServerID = Packet->ServerID;
	Request->WorldID = Packet->WorldID;
	Request->AuthKey = Packet->AuthKey;
	Request->EntityID = Packet->EntityID;
	Request->Account.AccountID = Account.AccountID;
	memcpy(Request->Account.SessionIP, Account.SessionIP, MAX_ADDRESSIP_LENGTH);
	Request->Account.SessionTimeout = Account.SessionTimeout;
	Request->Account.CharacterSlotID = Account.CharacterSlotID;
	Request->Account.CharacterSlotOrder = Account.CharacterSlotOrder;
	Request->Account.CharacterSlotFlags = Account.CharacterSlotFlags;
	memcpy(Request->Account.CharacterPassword, Account.CharacterPassword, MAX_SUBPASSWORD_LENGTH);
	Request->Account.CharacterQuestion = Account.CharacterQuestion;
	memcpy(Request->Account.CharacterAnswer, Account.CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH);
	return SocketSend(Context->WorldSocket, WorldClient->World.Connection, Request);

error:
	{
		IPC_DATA_AUTH_VERIFYRESULT* Response = PacketBufferInitExtended(Connection->PacketBuffer, IPC, AUTH_VERIFYRESULT);
		Response->ConnectionID = Packet->ConnectionID;
		Response->ServerID = Packet->ServerID;
		Response->WorldID = Packet->WorldID;
		Response->Status = 0;
		SocketSend(Socket, Connection, Response);
	}
}
