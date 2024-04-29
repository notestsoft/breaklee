#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(USE_ITEM) {
	S2C_DATA_USE_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, USE_ITEM);
	Response->Result = S2C_DATA_USE_ITEM_RESULT_FAILED;

	if (!Character) goto error;

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

		Info("Use Item ItemType: %d", ItemData->ItemType);
		// TODO: Check if it is a general option of an item that it been consumed from the inventory

		#define RUNTIME_ITEM_PROCEDURE(__NAME__, __TYPE__, __INTERNAL__)	\
		if (ItemData->ItemType == __TYPE__) {								\
		if ((__INTERNAL__)) goto error;										\
			Response->Result = __NAME__(									\
				Runtime,													\
				Character,													\
				ItemSlot,													\
				ItemData,													\
				Packet->Data												\
			);																\
		}
		#include <RuntimeLib/ItemProcDefinition.h>
	}

	return SocketSend(Socket, Connection, Response);

error:
	Response->Result = S2C_DATA_USE_ITEM_RESULT_FAILED;
	return SocketSend(Socket, Connection, Response);
}

// TODO: Because the item slots can be deleted the source & target slots will lead to dangling pointers!!!
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

	S2C_DATA_CONVERT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CONVERT_ITEM);
	Response->Result = RUNTIME_ITEM_USE_RESULT_FAILED;

	if (SourceItemData->ItemType == RUNTIME_ITEM_TYPE_COATING_KIT) {
		if (TargetItemData->ItemType != RUNTIME_ITEM_TYPE_VEHICLE_BIKE) goto error;

		if (SourceItemData->ItemID == 636) {
			RTDataUpgradeLimitRef UpgradeLimit = RTRuntimeDataUpgradeLimitGet(Runtime->Context, TargetItemData->ItemGrade);
			if (!UpgradeLimit) goto error;
			if (TargetItemSlot->Item.UpgradeLevel >= UpgradeLimit->MaxItemLevel) goto error;
			
            RTInventoryClearSlot(Runtime, &Character->InventoryInfo, SourceItemSlot->SlotIndex);

            // TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
			TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->TargetSlotIndex);
			assert(TargetItemSlot);
			TargetItemSlot->Item.UpgradeLevel += 1;
		}
		else {
			RTInventoryClearSlot(Runtime, &Character->InventoryInfo, SourceItemSlot->SlotIndex);

            // TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
			TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->TargetSlotIndex);
			assert(TargetItemSlot);
			TargetItemSlot->Item.VehicleColor = SourceItemData->CoatingKit.VehicleColor;
		}

		Response->Result = RUNTIME_ITEM_USE_RESULT_SUCCESS;
	}
	else if (
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_SLOT_EXTENDER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_SLOT_CONVERTER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_EPIC_CONVERTER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_DIVINE_CONVERTER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_CHAOS_CONVERTER
	) {
		struct _RTItemConverterPayload Payload = { 0 };
		Payload.TargetSlotIndex = Packet->TargetSlotIndex;
		Response->Result = RTItemUseInternal(Runtime, Character, SourceItemSlot, SourceItemData, &Payload);
	}
	else {
		goto error;
	}

    // TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
	TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, Packet->TargetSlotIndex);
	if (TargetItemSlot) {
		Response->Item = TargetItemSlot->Item;
		Response->ItemOptions = TargetItemSlot->ItemOptions;
		Response->InventorySlotIndex = TargetItemSlot->SlotIndex;
	}

	Character->SyncMask.InventoryInfo = true;
	Character->SyncPriority.High = true;

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
