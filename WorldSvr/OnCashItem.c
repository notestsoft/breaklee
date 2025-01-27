#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_CASH_INVENTORY) {
	if (!Character) goto error;

	IPC_W2D_DATA_GET_CASH_INVENTORY* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, GET_CASH_INVENTORY);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, GET_CASH_INVENTORY) {
	if (!ClientConnection) return;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);
	S2C_DATA_GET_CASH_INVENTORY* Response = PacketBufferInit(PacketBuffer, S2C, GET_CASH_INVENTORY);
	Response->Result = Packet->Result;
	Response->ItemCount = Packet->ItemCount;

	for (Int Index = 0; Index < Packet->ItemCount; Index += 1) {
		S2C_DATA_GET_CASH_INVENTORY_SLOT* ResponseItem = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_GET_CASH_INVENTORY_SLOT);
		ResponseItem->TransactionID = Packet->ItemSlots[Index].TransactionID;
		ResponseItem->ItemID = Packet->ItemSlots[Index].ItemID;
		ResponseItem->ItemOptions = Packet->ItemSlots[Index].ItemOptions;
		ResponseItem->ItemDurationIndex = Packet->ItemSlots[Index].ItemDurationIndex;
	}

	SocketSend(Context->ClientSocket, ClientConnection, Response);
}

CLIENT_PROCEDURE_BINDING(CASH_INVENTORY_RECEIVE_ITEM) {
	if (!Character) goto error;

	if (!RTInventoryIsSlotEmpty(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex)) goto error;

	IPC_W2D_DATA_CASH_INVENTORY_RECEIVE_ITEM* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, CASH_INVENTORY_RECEIVE_ITEM);
	Request->Header.SourceConnectionID = Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->TransactionID = Packet->TransactionID;
	Request->InventorySlotIndex = Packet->InventorySlotIndex;
	IPCSocketUnicast(Server->IPCSocket, Request);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, CASH_INVENTORY_RECEIVE_ITEM) {
	if (!ClientConnection || !Character) return;

	struct _RTItemSlot ItemSlot = { 0 };
	ItemSlot.Item.Serial = Packet->ItemID;
	ItemSlot.ItemOptions = Packet->ItemOptions;
	ItemSlot.ItemDuration = Packet->ItemDuration;
	ItemSlot.SlotIndex = Packet->InventorySlotIndex;
	if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &ItemSlot)) goto error;

	S2C_DATA_CASH_INVENTORY_RECEIVE_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, CASH_INVENTORY_RECEIVE_ITEM);
	Response->TransactionID = Packet->TransactionID;
	Response->ItemID = Packet->ItemID;
	Response->ItemOptions = Packet->ItemOptions;
	Response->InventorySlotIndex = Packet->InventorySlotIndex;
	Response->ItemDuration = Packet->ItemDuration;
	Response->Unknown1 = Packet->Unknown1;
	SocketSend(Context->ClientSocket, ClientConnection, Response);
	return;

error:
	// TODO: Add error handling and revert item deletion in database
	return;
}

CLIENT_PROCEDURE_BINDING(DESTROY_CASH_ITEM) {
	// TODO: Implementation missing!
}
