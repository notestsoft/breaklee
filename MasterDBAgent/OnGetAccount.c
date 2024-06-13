#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_ACCOUNT) {
	IPC_D2W_DATA_GET_ACCOUNT* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_ACCOUNT);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->GroupIndex = Packet->GroupIndex;
	Response->NodeIndex = Packet->NodeIndex;
	Response->LinkConnectionID = Packet->LinkConnectionID;

	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (MasterDBGetOrCreateAccount(Context->Database, Packet->AccountID, &Account)) {
		Response->Success = true;
		Response->Account.AccountID = Account.AccountID;
		memcpy(Response->Account.SessionIP, Account.SessionIP, MAX_ADDRESSIP_LENGTH);
		Response->Account.SessionTimeout = Account.SessionTimeout;
		Response->Account.CharacterSlotID = Account.CharacterSlotID;
		Response->Account.CharacterSlotOrder = Account.CharacterSlotOrder;
		Response->Account.CharacterSlotFlags = Account.CharacterSlotFlags;
		CStringCopySafe(Response->Account.CharacterPassword, MAX_SUBPASSWORD_LENGTH + 1, Account.CharacterPassword);
		Response->Account.CharacterQuestion = Account.CharacterQuestion;
		CStringCopySafe(Response->Account.CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH + 1, Account.CharacterAnswer);
	}

	IPCSocketUnicast(Socket, Response);
}