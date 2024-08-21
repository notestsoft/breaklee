#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

static struct _RTCharacterInventoryInfo kInventoryInfoBackup;

CLIENT_PROCEDURE_BINDING(USE_ITEM) {
	S2C_DATA_USE_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, USE_ITEM);
	Response->Result = S2C_DATA_USE_ITEM_RESULT_FAILED;

	if (!Character) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	if (ItemSlot) {
		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
		if (!ItemData) goto error;

		if (Character->Data.Info.Level < ItemData->MinLevel) goto error;

		PacketLogBytes(
			Socket->ProtocolIdentifier,
			Socket->ProtocolVersion,
			Socket->ProtocolExtension,
			Packet
		);

		Info("Use Item ItemType: %d", ItemData->ItemType);
		// TODO: Check if it is a general option of an item that it been consumed from the inventory
		// TODO: CHeck if Packet->Data length is matching the expected payload length

		#define RUNTIME_ITEM_PROCEDURE(__NAME__, __TYPE__, __INTERNAL__)	\
		if (ItemData->ItemType == __TYPE__) {								\
		if ((__INTERNAL__)) goto error;										\
			Response->Result = __NAME__(									\
				Runtime,													\
				Character,													\
				ItemSlot,													\
				ItemData,													\
				&Packet->Data[0]										    \
			);																\
		}
		#include <RuntimeLib/ItemProcDefinition.h>
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	Response->Result = S2C_DATA_USE_ITEM_RESULT_FAILED;
	SocketSend(Socket, Connection, Response);
}

// TODO: Because the item slots can be deleted the source & target slots will lead to dangling pointers!!!
CLIENT_PROCEDURE_BINDING(CONVERT_ITEM) {
	if (!Character) goto error;

	RTItemSlotRef SourceItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->SourceSlotIndex);
	if (!SourceItemSlot) goto error;

	RTItemDataRef SourceItemData = RTRuntimeGetItemDataByIndex(Runtime, SourceItemSlot->Item.ID);
	if (!SourceItemData) goto error;

	RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
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
			
            RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceItemSlot->SlotIndex);

            // TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
			TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
			assert(TargetItemSlot);
			TargetItemSlot->Item.UpgradeLevel += 1;
		}
		else {
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceItemSlot->SlotIndex);

            // TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
			TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
			assert(TargetItemSlot);
			TargetItemSlot->Item.VehicleColor = SourceItemData->CoatingKit.VehicleColor;
		}

		Response->Result = RUNTIME_ITEM_USE_RESULT_SUCCESS;
	}
	else if (
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_CORE_ENHANCER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_SLOT_EXTENDER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_SLOT_CONVERTER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_EPIC_CONVERTER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_DIVINE_CONVERTER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_CHAOS_CONVERTER ||
		SourceItemData->ItemType == RUNTIME_ITEM_TYPE_EPIC_BOOSTER
	) { // TODO: When item is broken it returns a failure with value 17
		struct _RTItemConverterPayload Payload = { 0 };
		Payload.TargetSlotIndex = Packet->TargetSlotIndex;
		Response->Result = RTItemUseInternal(Runtime, Character, SourceItemSlot, SourceItemData, &Payload);
	}
	else {
		goto error;
	}

    // TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
	TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->TargetSlotIndex);
	if (TargetItemSlot) {
		Response->Item = TargetItemSlot->Item;
		Response->ItemOptions = TargetItemSlot->ItemOptions;
		Response->InventorySlotIndex = TargetItemSlot->SlotIndex;
	}

	Character->SyncMask.InventoryInfo = true;

	PacketLogBytes(
        Socket->ProtocolIdentifier,
        Socket->ProtocolVersion,
        Socket->ProtocolExtension,
        Response
    );
    
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_CONVERT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CONVERT_ITEM);
		Response->Result = 1;
		SocketSend(Socket, Connection, Response);
		return;
	}
}

CLIENT_PROCEDURE_BINDING(MULTI_CONVERT_ITEM) {
	if (!Character) goto error;

	memcpy(&kInventoryInfoBackup, &Character->Data.InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));

	Int32 TailLength = (Packet->SourceItemCount + Packet->TargetItemCount) * sizeof(UInt16);
	if (Packet->Length != sizeof(C2S_DATA_MULTI_CONVERT_ITEM) + TailLength) goto error;
	if (Packet->SourceItemCount < 1 || Packet->TargetItemCount < 1) goto error;

	S2C_DATA_MULTI_CONVERT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, MULTI_CONVERT_ITEM);
	Response->Result = RUNTIME_ITEM_USE_RESULT_FAILED;

	UInt16* SourceInventorySlots = &Packet->SourceInventorySlots[0];
	UInt16* TargetInventorySlots = &Packet->SourceInventorySlots[Packet->SourceItemCount];

	Int32 SourceItemIndex = 0;

	for (Int32 Index = 0; Index < Packet->TargetItemCount; Index += 1) {
		RTItemSlotRef SourceItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SourceInventorySlots[SourceItemIndex]);
		if (!SourceItemSlot && SourceItemIndex < Packet->SourceItemCount) {
			SourceItemIndex += 1;
			SourceItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SourceInventorySlots[SourceItemIndex]);
		}
		if (!SourceItemSlot) goto error;

		RTItemDataRef SourceItemData = RTRuntimeGetItemDataByIndex(Runtime, SourceItemSlot->Item.ID);
		if (!SourceItemData) goto error;

		RTItemSlotRef TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, TargetInventorySlots[Index]);
		if (!TargetItemSlot) goto error;

		RTItemDataRef TargetItemData = RTRuntimeGetItemDataByIndex(Runtime, TargetItemSlot->Item.ID);
		if (!TargetItemData) goto error;

		S2C_DATA_MULTI_CONVERT_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, MULTI_CONVERT_ITEM);
		Response->Result = RUNTIME_ITEM_USE_RESULT_FAILED;

		if (SourceItemData->ItemType == RUNTIME_ITEM_TYPE_COATING_KIT) {
			if (TargetItemData->ItemType != RUNTIME_ITEM_TYPE_VEHICLE_BIKE) goto error;

			if (SourceItemData->ItemID == 636) {
				RTDataUpgradeLimitRef UpgradeLimit = RTRuntimeDataUpgradeLimitGet(Runtime->Context, TargetItemData->ItemGrade);
				if (!UpgradeLimit) goto error;
				if (TargetItemSlot->Item.UpgradeLevel >= UpgradeLimit->MaxItemLevel) goto error;

				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceItemSlot->SlotIndex);

				// TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
				TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, TargetInventorySlots[Index]);
				assert(TargetItemSlot);
				TargetItemSlot->Item.UpgradeLevel += 1;
			}
			else {
				RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SourceItemSlot->SlotIndex);

				// TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
				TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, TargetInventorySlots[Index]);
				assert(TargetItemSlot);
				TargetItemSlot->Item.VehicleColor = SourceItemData->CoatingKit.VehicleColor;
			}

			Response->Result = RUNTIME_ITEM_USE_RESULT_SUCCESS;
		}
		else if (
			SourceItemData->ItemType == RUNTIME_ITEM_TYPE_CORE_ENHANCER ||
			SourceItemData->ItemType == RUNTIME_ITEM_TYPE_SLOT_EXTENDER ||
			SourceItemData->ItemType == RUNTIME_ITEM_TYPE_SLOT_CONVERTER ||
			SourceItemData->ItemType == RUNTIME_ITEM_TYPE_EPIC_CONVERTER ||
			SourceItemData->ItemType == RUNTIME_ITEM_TYPE_DIVINE_CONVERTER ||
			SourceItemData->ItemType == RUNTIME_ITEM_TYPE_CHAOS_CONVERTER ||
			SourceItemData->ItemType == RUNTIME_ITEM_TYPE_EPIC_BOOSTER
		) { // TODO: When item is broken it returns a failure with value 17
			struct _RTItemConverterPayload Payload = { 0 };
			Payload.TargetSlotIndex = TargetInventorySlots[Index];
			Response->Result = RTItemUseInternal(Runtime, Character, SourceItemSlot, SourceItemData, &Payload);
		}
		else {
			goto error;
		}

		// TODO: This is a fallback solution because the inventory pointers are invalidated by RTInventoryClearSlot
		TargetItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, TargetInventorySlots[Index]);
		if (TargetItemSlot) {
			Response->ResultSlotCount += 1;

			S2C_MULTI_CONVERT_ITEM_SLOT_INDEX* ResponseSlot = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_MULTI_CONVERT_ITEM_SLOT_INDEX);
			ResponseSlot->ItemID = TargetItemSlot->Item.Serial;
			ResponseSlot->ItemOptions = TargetItemSlot->ItemOptions;
			ResponseSlot->InventorySlotIndex = TargetItemSlot->SlotIndex;
		}
	}

	Character->SyncMask.InventoryInfo = true;

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(USE_ITEM_SAVER) {
	S2C_DATA_USE_ITEM* Response = PacketBufferInit(Connection->PacketBuffer, S2C, USE_ITEM);
	Response->Result = S2C_DATA_USE_ITEM_RESULT_FAILED;

	Int32 PacketLength = sizeof(C2S_DATA_USE_ITEM_SAVER) + Packet->InventorySlotCount * sizeof(C2S_DATA_USE_ITEM_SAVER_INVENTORY_SLOT);
	if (PacketLength < Packet->Length) goto error;

	if (!Character) goto error;

	if (Packet->InventorySlotCount < 1) goto error;

	RTItemSlotRef FirstItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlots[0].InventorySlotIndex);
	if (!FirstItemSlot) goto error;

	RTItemDataRef FirstItemData = RTRuntimeGetItemDataByIndex(Runtime, FirstItemSlot->Item.ID);
	if (!FirstItemData) goto error;
	if (FirstItemData->ItemType != RUNTIME_ITEM_TYPE_SAVER) goto error;

	Int32 FirstItemAmount = (FirstItemSlot->ItemOptions >> 16) & 0xFFFF;
	Int32 FirstItemLimit = FirstItemSlot->ItemOptions & 0xFFFF;
	Int32 FirstItemSubtype = FirstItemData->Options[0];
	Bool FirstIsSaving = FirstItemAmount == 0;
	Int32 TotalAmount = 0;

	for (Index Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlots[Index].InventorySlotIndex);
		if (!ItemSlot) goto error;

		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
		if (!ItemData) goto error;
		if (ItemData->ItemType != RUNTIME_ITEM_TYPE_SAVER) goto error;

		Int32 ItemAmount = (ItemSlot->ItemOptions >> 16) & 0xFFFF;
		Int32 ItemLimit = ItemSlot->ItemOptions & 0xFFFF;
		Int32 ItemSubtype = ItemData->Options[0];
		Bool IsSaving = ItemAmount == 0;

		if (FirstIsSaving != IsSaving) goto error;
		if (FirstItemSubtype != ItemSubtype) goto error;

		if (!IsSaving && ItemAmount != Packet->InventorySlots[Index].Amount) goto error;
		if (IsSaving && ItemLimit != Packet->InventorySlots[Index].Amount) goto error;

		TotalAmount += Packet->InventorySlots[Index].Amount;
	}

	if (FirstIsSaving) {
		switch (FirstItemSubtype) {
		case RUNTIME_ITEM_SUBTYPE_SAVER_DP:
			if (Character->Data.Info.DP < TotalAmount) goto error;
			Character->Data.Info.DP -= TotalAmount;
			// TODO: Notification for dp is missing!
			break;

		case RUNTIME_ITEM_SUBTYPE_SAVER_AP:
			if (Character->Data.AbilityInfo.Info.AP < TotalAmount) goto error;
			Character->Data.AbilityInfo.Info.AP -= TotalAmount;
			Character->SyncMask.AbilityInfo = true;
			break;

		case RUNTIME_ITEM_SUBTYPE_SAVER_WEXP:
			if (Character->Data.Info.Wexp < TotalAmount) goto error;
			Character->Data.Info.Wexp -= TotalAmount;
			break;

		default:
			goto error;
		}
	}
	else {
		switch (FirstItemSubtype) {
		case RUNTIME_ITEM_SUBTYPE_SAVER_DP:
			Character->Data.Info.DP += TotalAmount;
			break;

		case RUNTIME_ITEM_SUBTYPE_SAVER_AP:
			Character->Data.AbilityInfo.Info.AP += TotalAmount;
			Character->SyncMask.AbilityInfo = true;
			break;

		case RUNTIME_ITEM_SUBTYPE_SAVER_WEXP:
			Character->Data.Info.Wexp += TotalAmount;
			break;

		default:
			goto error;
		}
	}

	Response->Result = S2C_DATA_USE_ITEM_RESULT_SUCCESS;

	if (FirstIsSaving) {
		for (Index Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
			RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlots[Index].InventorySlotIndex);
			assert(ItemSlot);
			ItemSlot->ItemOptions |= (UInt64)Packet->InventorySlots[Index].Amount << 16;
		}
	}
	else {
		for (Index Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
			RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlots[Index].InventorySlotIndex);
			assert(ItemSlot);
			RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlots[Index].InventorySlotIndex);
		}
	}

	Character->SyncMask.Info = true;
	Character->SyncMask.InventoryInfo = true;

	SocketSend(Socket, Connection, Response);
	return;

error:
	Response->Result = S2C_DATA_USE_ITEM_RESULT_FAILED;
	SocketSend(Socket, Connection, Response);
	return;
}