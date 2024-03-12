#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

static struct _RTCharacterInventoryInfo kInventoryInfoBackup;
static struct _RTCharacterWarehouseInfo kWarehouseInfoBackup;

CLIENT_PROCEDURE_BINDING(GET_WAREHOUSE) {
	if (!Character) goto error;

	S2C_DATA_GET_WAREHOUSE* Response = PacketInit(S2C_DATA_GET_WAREHOUSE);
	Response->Command = S2C_GET_WAREHOUSE;
	Response->Count = Character->WarehouseInfo.Count;
	Response->Currency = Character->WarehouseInfo.Alz;

	for (int i = 0; i < Character->WarehouseInfo.Count; i++) {
		S2C_DATA_GET_WAREHOUSE_SLOT_INDEX* Slots = PacketAppendStruct(S2C_DATA_GET_WAREHOUSE_SLOT_INDEX);

		Slots->Item.DivineLevel = Character->WarehouseInfo.Slots[i].Item.DivineLevel;
		Slots->Item.Extension = Character->WarehouseInfo.Slots[i].Item.Extension;
		Slots->Item.ID = Character->WarehouseInfo.Slots[i].Item.ID;
		Slots->Item.ExtremeLevel = Character->WarehouseInfo.Slots[i].Item.ExtremeLevel;
		Slots->Item.IsAccountBinding = Character->WarehouseInfo.Slots[i].Item.IsAccountBinding;
		Slots->Item.IsCharacterBinding = Character->WarehouseInfo.Slots[i].Item.IsCharacterBinding;
		Slots->Item.IsTradableBinding = Character->WarehouseInfo.Slots[i].Item.IsTradableBinding;
		Slots->Item.PartyQuestItemID = Character->WarehouseInfo.Slots[i].Item.PartyQuestItemID;
		Slots->Item.PartyQuestItemStackable = Character->WarehouseInfo.Slots[i].Item.PartyQuestItemStackable;
		Slots->Item.Serial = Character->WarehouseInfo.Slots[i].Item.Serial;
		Slots->Item.UpgradeLevel = Character->WarehouseInfo.Slots[i].Item.UpgradeLevel;
		Slots->Item._1 = Character->WarehouseInfo.Slots[i].Item._1;
		Slots->Item._2 = Character->WarehouseInfo.Slots[i].Item._2;
		Slots->Item._3 = Character->WarehouseInfo.Slots[i].Item._3;
		Slots->Item._4 = Character->WarehouseInfo.Slots[i].Item._4;
		Slots->ItemDuration = Character->WarehouseInfo.Slots[i].ItemDuration;
		Slots->ItemOptions = Character->WarehouseInfo.Slots[i].ItemOptions;
		Slots->SlotIndex = Character->WarehouseInfo.Slots[i].SlotIndex;
		Slots->Unknown1 = Character->WarehouseInfo.Slots[i].Unknown1;
	}

	return SocketSend(Socket, Connection, Response);
error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ALZ_DEPOSIT_WITHDRAW) {
	memcpy(&kInventoryInfoBackup, &Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&kWarehouseInfoBackup, &Character->WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));

	S2C_DATA_ALZ_DEPOSIT_WITHDRAW* Response = PacketInit(S2C_DATA_ALZ_DEPOSIT_WITHDRAW);

	Response->Command = S2C_ALZ_DEPOSIT_WITHDRAW;
	Response->Result = 1;

	//TODO: Calculate the tax for the deposit

	if (Packet->Amount > 0) {
		if (Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < Packet->Amount) {
			Response->Result = 0;
			return SocketSend(Socket, Connection, Response);
		}

		Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Packet->Amount;
		Character->WarehouseInfo.Alz += Packet->Amount;
	}
	else {
		Int64 absAmount = abs(Packet->Amount);

		if (Character->WarehouseInfo.Alz < absAmount) {
			Response->Result = 0;
			return SocketSend(Socket, Connection, Response);
		}

		Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] += absAmount;
		Character->WarehouseInfo.Alz -= absAmount;
	}

	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INVENTORY;
	Character->SyncMask |= RUNTIME_CHARACTER_SYNC_WAREHOUSE;
	Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;


	return SocketSend(Socket, Connection, Response);
error:
	memcpy(&Character->InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Character->WarehouseInfo, &kWarehouseInfoBackup, sizeof(struct _RTCharacterWarehouseInfo));
	return SocketDisconnect(Socket, Connection);
}