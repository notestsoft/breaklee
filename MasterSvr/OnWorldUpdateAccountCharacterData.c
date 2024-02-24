#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"

IPC_PROCEDURE_BINDING(OnWorldUpdateAccountCharacterData, IPC_WORLD_UPDATE_ACCOUNT_CHARACTER_DATA, IPC_DATA_WORLD_UPDATE_ACCOUNT_CHARACTER_DATA) {
	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, &Account)) return;

	Account.CharacterSlotID = Packet->CharacterSlotID;
	Account.CharacterSlotOrder = Packet->CharacterSlotOrder;
	Account.CharacterSlotFlags = Packet->CharacterSlotFlags;

	MasterDBUpdateAccountCharacterSlot(Context->Database, &Account);
}
