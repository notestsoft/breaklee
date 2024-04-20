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

	Context->TempAccount.AccountID = Packet->AccountID;
	if (MasterDBGetOrCreateAccount(Context->Database, &Context->TempAccount)) {
		Response->Success = true;
		Response->Account.AccountID = Context->TempAccount.AccountID;
		memcpy(Response->Account.SessionIP, Context->TempAccount.SessionIP, MAX_ADDRESSIP_LENGTH);
		Response->Account.SessionTimeout = Context->TempAccount.SessionTimeout;
		Response->Account.CharacterSlotID = Context->TempAccount.CharacterSlotID;
		Response->Account.CharacterSlotOrder = Context->TempAccount.CharacterSlotOrder;
		Response->Account.CharacterSlotFlags = Context->TempAccount.CharacterSlotFlags;
		memcpy(Response->Account.CharacterPassword, Context->TempAccount.CharacterPassword, MAX_SUBPASSWORD_LENGTH);
		Response->Account.CharacterQuestion = Context->TempAccount.CharacterQuestion;
		memcpy(Response->Account.CharacterAnswer, Context->TempAccount.CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH);
	}

	IPCSocketUnicast(Socket, Response);
}