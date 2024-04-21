#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, UPDATE_ACCOUNT_SESSION) {
	Account->AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Account)) return;

	memcpy(Account->SessionIP, Packet->SessionIP, MAX_ADDRESSIP_LENGTH);
	Account->SessionTimeout = Packet->SessionTimeout;

	MasterDBUpdateAccountSession(Context->Database, Account);
}