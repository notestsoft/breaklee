#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(MERIT_MEDAL_EVALUATION) {
	if (!Character) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	RTItemDataRef ItemData = (ItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID) : NULL;
	if (!ItemData) goto error;

	S2C_DATA_MERIT_MEDAL_EVALUATION* Response = PacketBufferInit(Connection->PacketBuffer, S2C, MERIT_MEDAL_EVALUATION);
	Response->Success = 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}