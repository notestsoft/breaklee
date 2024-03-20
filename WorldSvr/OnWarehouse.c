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
	Response->Currency = Character->WarehouseInfo.Currency;

	for (Int32 Index = 0; Index < Character->WarehouseInfo.Count; Index++) {
		S2C_DATA_GET_WAREHOUSE_SLOT_INDEX* Slots = PacketAppendStruct(S2C_DATA_GET_WAREHOUSE_SLOT_INDEX);

		Slots->Item.Serial = Character->WarehouseInfo.Slots[Index].Item.Serial;
	}

	return SocketSend(Socket, Connection, Response);
error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(WAREHOUSE_CURRENCY_DEPOSIT) {
	memcpy(&kInventoryInfoBackup, &Character->InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&kWarehouseInfoBackup, &Character->WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));

	S2C_DATA_WAREHOUSE_CURRENCY_DEPOSIT* Response = PacketInit(S2C_DATA_WAREHOUSE_CURRENCY_DEPOSIT);

	Response->Command = S2C_WAREHOUSE_CURRENCY_DEPOSIT;
	Response->Result = 1;

	//TODO: Calculate the tax for the deposit

	if (Packet->Amount > 0) {
		if (Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < Packet->Amount) {
			Response->Result = 0;
			return SocketSend(Socket, Connection, Response);
		}

		Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Packet->Amount;
		Character->WarehouseInfo.Currency += Packet->Amount;
	}
	else {
		Int64 AbsAmount = ABS(Packet->Amount);

		if (Character->WarehouseInfo.Currency < AbsAmount) goto error;

		Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] += AbsAmount;
		Character->WarehouseInfo.Currency -= AbsAmount;
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
