#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, GET_ACCOUNT) {
	IPC_M2W_DATA_GET_ACCOUNT* Response = IPCPacketBufferInit(Connection->PacketBuffer, M2W, GET_ACCOUNT);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->GroupIndex = Packet->GroupIndex;
	Response->NodeIndex = Packet->NodeIndex;
	Response->LinkConnectionID = Packet->LinkConnectionID;

	Account->AccountID = Packet->AccountID;
	if (MasterDBGetOrCreateAccount(Context->Database, Account)) {
		Response->Success = true;
		Response->Account.AccountID = Account->AccountID;
		memcpy(Response->Account.SessionIP, Account->SessionIP, MAX_ADDRESSIP_LENGTH);
		Response->Account.SessionTimeout = Account->SessionTimeout;
		Response->Account.CharacterSlotID = Account->CharacterSlotID;
		Response->Account.CharacterSlotOrder = Account->CharacterSlotOrder;
		Response->Account.CharacterSlotFlags = Account->CharacterSlotFlags;
		CStringCopySafe(Response->Account.CharacterPassword, MAX_SUBPASSWORD_LENGTH, Account->CharacterPassword);
		Response->Account.CharacterQuestion = Context->TempAccount.CharacterQuestion;
		CStringCopySafe(Response->Account.CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH, Account->CharacterAnswer);
	}

	IPCSocketUnicast(Socket, Response);
}