#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, UPDATE_SUBPASSWORD) {
	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Packet->AccountID, &Account)) return;

	memcpy(Account.CharacterPassword, Packet->CharacterPassword, MAX_SUBPASSWORD_LENGTH);
	Account.CharacterQuestion = Packet->CharacterQuestion;
	memcpy(Account.CharacterAnswer, Packet->CharacterAnswer, MAX_SUBPASSWORD_ANSWER_LENGTH);

	MasterDBUpdateAccountCharacterPassword(Context->Database, &Account);
}