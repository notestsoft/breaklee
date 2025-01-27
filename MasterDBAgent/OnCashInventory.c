#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_CASH_INVENTORY) {
	IPC_D2W_DATA_GET_CASH_INVENTORY* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_CASH_INVENTORY);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
		Context->Database,
		"GetCashInventory",
		DB_INPUT_INT32(Packet->AccountID),
		DB_PARAM_END
	);
	Response->Result = (Handle) ? 5 : 1;

	IPC_DATA_GET_CASH_INVENTORY_SLOT ItemSlot = { 0 };
	while (DatabaseHandleReadNext(
		Context->Database,
		Handle,
		DB_TYPE_INT32, &ItemSlot.TransactionID,
		DB_TYPE_UINT64, &ItemSlot.ItemID,
		DB_TYPE_UINT64, &ItemSlot.ItemOptions,
		DB_TYPE_UINT8, &ItemSlot.ItemDurationIndex,
		DB_PARAM_END
	)) {
		IPCPacketBufferAppendCopy(Connection->PacketBuffer, &ItemSlot, sizeof(ItemSlot));
		Response->ItemCount += 1;
	}

	IPCSocketUnicast(Socket, Response);
}

IPC_PROCEDURE_BINDING(W2D, CASH_INVENTORY_RECEIVE_ITEM) {
	IPC_D2W_DATA_CASH_INVENTORY_RECEIVE_ITEM* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, CASH_INVENTORY_RECEIVE_ITEM);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->TransactionID = Packet->TransactionID;

	Bool Result = 0;
	UInt8 ItemDurationIndex = 0;
	DatabaseCallProcedure(
		Context->Database,
		"DeleteCashInventoryItem",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT32(Packet->TransactionID),
		DB_OUTPUT_BOOL(Result),
		DB_OUTPUT_UINT64(Response->ItemID),
		DB_OUTPUT_UINT64(Response->ItemOptions),
		DB_OUTPUT_UINT8(ItemDurationIndex),
		DB_PARAM_END
	);
	Response->InventorySlotIndex = Packet->InventorySlotIndex;
	Response->ItemDuration.DurationIndex = ItemDurationIndex;
	Response->Unknown1 = 0;

	IPCSocketUnicast(Socket, Response);
}
