#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, SET_CHARACTER_SLOT_ORDER) {
	IPC_D2W_DATA_SET_CHARACTER_SLOT_ORDER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, SET_CHARACTER_SLOT_ORDER);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = 0;

	if (!DatabaseCallProcedure(
		Context->Database,
		"SetCharacterSlotOrder",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_UINT64(Packet->CharacterSlotOrder),
		DB_PARAM_END
	)) {
		Response->Result = 1;
	}

	IPCSocketUnicast(Socket, Response);
}