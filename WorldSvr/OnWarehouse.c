#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_WAREHOUSE) {
	if (!Character) goto error;

	S2C_DATA_GET_WAREHOUSE* Response = PacketInit(S2C_DATA_GET_WAREHOUSE);
	Response->Command = S2C_GET_WAREHOUSE;
	Response->Count = Character->WarehouseInfo.Count;
	Response->Currency = Character->WarehouseInfo.Alz;

	for (int i = 0; i < Character->WarehouseInfo.Count; i++) {
		S2C_DATA_GET_WAREHOUSE_SLOT_INDEX* Slots = PacketAppendStruct(S2C_DATA_GET_WAREHOUSE_SLOT_INDEX);

		Slots->Item.IdLow = Character->WarehouseInfo.Slots[i].Item.IdLow;
		Slots->Item.BindToAccount = Character->WarehouseInfo.Slots[i].Item.BindToAccount;
		Slots->Item.UpgradeLevel = Character->WarehouseInfo.Slots[i].Item.UpgradeLevel;
		Slots->Item.PremiumPcOnly = Character->WarehouseInfo.Slots[i].Item.PremiumPcOnly;
		Slots->Item.BindToCharacter = Character->WarehouseInfo.Slots[i].Item.BindToCharacter;
		Slots->Item.BindOnEquip = Character->WarehouseInfo.Slots[i].Item.BindOnEquip;
		Slots->Item.Reserved0 = Character->WarehouseInfo.Slots[i].Item.Reserved0;
		Slots->Item.SpecialInventoryItem = Character->WarehouseInfo.Slots[i].Item.SpecialInventoryItem;
		Slots->Item.IdHigh = Character->WarehouseInfo.Slots[i].Item.IdHigh;
		Slots->Item.SpecialBit = Character->WarehouseInfo.Slots[i].Item.SpecialBit;
		Slots->Item.IsBroken = Character->WarehouseInfo.Slots[i].Item.IsBroken;
		Slots->Item.u0 = Character->WarehouseInfo.Slots[i].Item.u0;
		Slots->Item.ExtremeUpgrade = Character->WarehouseInfo.Slots[i].Item.ExtremeUpgrade;
		Slots->Item.DivineUpgrade = Character->WarehouseInfo.Slots[i].Item.DivineUpgrade;
		
		for (int i = 0; i < 3; i++) {
			Slots->Item.Reserved1[i] = Character->WarehouseInfo.Slots[i].Item.Reserved1[i];
		}

		Slots->Serial = Character->WarehouseInfo.Slots[i].Serial;
		Slots->ItemOptions = Character->WarehouseInfo.Slots[i].ItemOptions;
		Slots->SlotIndex = Character->WarehouseInfo.Slots[i].SlotIndex;
		Slots->Period.Year = Character->WarehouseInfo.Slots[i].Period.Year;
		Slots->Period.Month = Character->WarehouseInfo.Slots[i].Period.Month;
		Slots->Period.Day= Character->WarehouseInfo.Slots[i].Period.Day;
		Slots->Period.Hour= Character->WarehouseInfo.Slots[i].Period.Hour;
		Slots->Period.Minute= Character->WarehouseInfo.Slots[i].Period.Minute;
		Slots->Period.Id = Character->WarehouseInfo.Slots[i].Period.Id;
	}

	return SocketSend(Socket, Connection, Response);
error:
	return SocketDisconnect(Socket, Connection);
}
