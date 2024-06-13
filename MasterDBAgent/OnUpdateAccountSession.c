#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, UPDATE_ACCOUNT_SESSION) {
	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Packet->AccountID, &Account)) return;

	memcpy(Account.SessionIP, Packet->SessionIP, MAX_ADDRESSIP_LENGTH);
	Account.SessionTimeout = Packet->SessionTimeout;

	MasterDBUpdateAccountSession(Context->Database, &Account);
}