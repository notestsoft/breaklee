#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_REGISTER) {
	if (!Character) goto error;

	Int32 PacketLength = sizeof(C2S_DATA_COSTUME_WAREHOUSE_REGISTER) + sizeof(UInt16) * Packet->InventorySlotCount;
	if (Packet->Length < PacketLength) goto error;

	for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
		if (!ItemSlot) goto error;

		RTDataCostumeWarehousePriceRef ItemPrice = RTRuntimeDataCostumeWarehousePriceGet(Runtime->Context, ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX);
		if (!ItemPrice) goto error;
	}

	Int32 TotalMillagePointReward = 0;
	for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex[Index]);
		assert(ItemSlot);
		
		RTDataCostumeWarehousePriceRef ItemPrice = RTRuntimeDataCostumeWarehousePriceGet(Runtime->Context, ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX);
		assert(ItemPrice);
		
		UInt32 ItemID = ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX;
		if (!RTCharacterIsCostumeUnlocked(Runtime, Character, ItemID)) {
			RTCharacterUnlockCostume(Runtime, Character, ItemID);
		}
		else {
			TotalMillagePointReward += ItemPrice->RewardMillage;
		}

		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, ItemSlot->SlotIndex);
		Character->SyncMask.InventoryInfo = true;
	}

	Character->Data.CostumeWarehouseInfo.Info.MillagePoints += TotalMillagePointReward;
	Character->SyncMask.CostumeWarehouseInfo = true;

	S2C_DATA_COSTUME_WAREHOUSE_REGISTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_REGISTER);
	Response->Success = 1;
	Response->MillagePoints = Character->Data.CostumeWarehouseInfo.Info.MillagePoints;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
} 

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_LIST) {
	if (!Character) goto error;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_COSTUME_WAREHOUSE_LIST* Response = PacketBufferInit(PacketBuffer, S2C, COSTUME_WAREHOUSE_LIST);

	if (Context->Config.Environment.IsCostumeWarehouseEnabled) {
		Response->IsActive = 1;
		Response->ItemCount = Runtime->Context->CostumeWarehousePriceCount;

		for (Int Index = 0; Index < Runtime->Context->CostumeWarehousePriceCount; Index += 1) {
			RTDataCostumeWarehousePriceRef Item = &Runtime->Context->CostumeWarehousePriceList[Index];

			S2C_DATA_COSTUME_WAREHOUSE_ITEM* ResponseItem = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_COSTUME_WAREHOUSE_ITEM);
			ResponseItem->ItemID = Item->ItemID;
			ResponseItem->PriceMillage = Item->PriceMillage;
			ResponseItem->PriceForceGem = Item->PriceForceGem;
			ResponseItem->RewardMillage = Item->RewardMillage;
		}
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_NAME_LIST) {
	if (!Character) goto error;

	S2C_DATA_COSTUME_WAREHOUSE_NAME_LIST* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_NAME_LIST);
	Response->ItemCount = 0;

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_SHOPPING) {
	if (!Character) goto error;

	S2C_DATA_COSTUME_WAREHOUSE_SHOPPING* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_SHOPPING);
	Response->Result = 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_PURCHASE) {
	if (!Character) goto error;

	Int32 PacketLength = sizeof(C2S_DATA_COSTUME_WAREHOUSE_PURCHASE) + sizeof(C2S_DATA_COSTUME_WAREHOUSE_PURCHASE_SLOT) * Packet->ItemCount;
	if (Packet->Length < PacketLength) goto error;

	S2C_DATA_COSTUME_WAREHOUSE_PURCHASE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_PURCHASE);
	Response->Success = 1;

	UInt32 TotalMillagePrice = 0;
	UInt32 TotalForceGemPrice = 0;
	for (Int Index = 0; Index < Packet->ItemCount; Index += 1) {
		RTDataCostumeWarehousePriceRef CostumePrice = RTRuntimeDataCostumeWarehousePriceGet(Runtime->Context, Packet->ItemSlots[Index].ItemID);
		if (Packet->ItemSlots[Index].PriceType == C2S_DATA_COSTUME_WAREHOUSE_PURCHASE_PRICE_TYPE_MILLAGE) {
			TotalMillagePrice += CostumePrice->PriceMillage;
		}
		else if (Packet->ItemSlots[Index].PriceType == C2S_DATA_COSTUME_WAREHOUSE_PURCHASE_PRICE_TYPE_FORCEGEM) {
			TotalForceGemPrice += CostumePrice->PriceForceGem;
		}
		else {
			goto error;
		}		
	}
	
	if (Character->Data.CostumeWarehouseInfo.Info.MillagePoints < TotalMillagePrice) goto error;
	if (Character->Data.AccountInfo.ForceGem < TotalForceGemPrice) goto error;

	for (Int Index = 0; Index < Packet->ItemCount; Index += 1) {
		if (!RTCharacterUnlockCostume(Runtime, Character, Packet->ItemSlots[Index].ItemID)) goto error;
	}

	Character->Data.CostumeWarehouseInfo.Info.MillagePoints -= TotalMillagePrice;
	Character->SyncMask.CostumeWarehouseInfo = true;
	Character->Data.AccountInfo.ForceGem -= TotalForceGemPrice;
	Character->SyncMask.AccountInfo = true;

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_COSTUME_WAREHOUSE_PURCHASE* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_PURCHASE);
		Response->Success = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_APPLY) {
	if (!Character) goto error;

	S2C_DATA_COSTUME_WAREHOUSE_APPLY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_APPLY);
	Response->Success = RTCharacterSetCostumePreset(Runtime, Character, Packet->PresetIndex, Packet->ItemIDs);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(COSTUME_WAREHOUSE_NAME_SET) {
	if (!Character) goto error;

	S2C_DATA_COSTUME_WAREHOUSE_NAME_SET* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, COSTUME_WAREHOUSE_NAME_SET);

	Response->Result = 1;

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
