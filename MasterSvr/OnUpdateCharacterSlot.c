#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, UPDATE_CHARACTER_SLOT) {
	Account->AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Account)) return;

	Account->CharacterSlotID = Packet->CharacterSlotID;
	Account->CharacterSlotOrder = Packet->CharacterSlotOrder;
	Account->CharacterSlotFlags = Packet->CharacterSlotFlags;

	MasterDBUpdateAccountCharacterSlot(Context->Database, Account);
}