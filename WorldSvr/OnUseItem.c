#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(USE_ITEM) {
	if (!Character) goto error;

	S2C_DATA_USE_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, USE_ITEM);
	Response->Result = 1;
	
	Bool Success = false;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->InventorySlotIndex);
	if (ItemSlot) {
		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
		if (!ItemData) goto error;

		if (Character->Info.Basic.Level < ItemData->MinLevel) goto error;

		PacketLogBytes(
			Socket->ProtocolIdentifier,
			Socket->ProtocolVersion,
			Socket->ProtocolExtension,
			Packet
		);

		LogMessageFormat(LOG_LEVEL_INFO, "Use Item ItemType: %d", ItemData->ItemType);
		// TODO: Check if it is a general option of an item that it been consumed from the inventory

		#define RUNTIME_ITEM_PROCEDURE(__NAME__, __TYPE__, __INTERNAL__)	\
		if (ItemData->ItemType == __TYPE__) {								\
		if ((__INTERNAL__)) goto error;										\
			Success = __NAME__(												\
				Runtime,													\
				Character,													\
				ItemSlot,													\
				ItemData,													\
				Packet->Data												\
			);																\
		}
		#include <RuntimeLib/ItemProcDefinition.h>
	}

	if (Success) {
		Response->Result = 0;
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CONVERT_ITEM) {
	if (!Character) goto error;

	RTItemSlotRef SourceItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->SourceSlotIndex);
	if (!SourceItemSlot) goto error;

	RTItemDataRef SourceItemData = RTRuntimeGetItemDataByIndex(Runtime, SourceItemSlot->Item.ID);
	if (!SourceItemData) goto error;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->TargetSlotIndex);
	if (!TargetItemSlot) goto error;

	RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
	if (!TargetItemData) goto error;

	if (SourceItemData->ItemType == RUNTIME_ITEM_TYPE_SLOT_EXTENDER) {
		struct _RTItemSlotExtenderPayload Payload;
		Payload.TargetSlotIndex = Packet->TargetSlotIndex;
		if (!RTItemUseInternal(Runtime, Character, SourceItemSlot, SourceItemData, &Payload)) goto error;
	}
	else {
		goto error;
	}

	S2C_DATA_CONVERT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CONVERT_ITEM);
	Response->Result = 0;
	Response->Item = TargetItemSlot->Item;
	Response->ItemOptions = TargetItemSlot->ItemOptions;
	Response->InventorySlotIndex = TargetItemSlot->SlotIndex;
	
	PacketLogBytes(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Response
    );
    
	return SocketSend(Socket, Connection, Response);

error:
	{
		S2C_DATA_CONVERT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CONVERT_ITEM);
		Response->Result = 1;
		return SocketSend(Socket, Connection, Response);
	}
}
