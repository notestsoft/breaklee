#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, UPDATE_CHARACTER_SLOT) {
	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Packet->AccountID, &Account)) return;

	Account.CharacterSlotID = Packet->CharacterSlotID;
	Account.CharacterSlotOrder = Packet->CharacterSlotOrder;
	Account.CharacterSlotFlags = Packet->CharacterSlotFlags;

	MasterDBUpdateAccountCharacterSlot(Context->Database, &Account);
}