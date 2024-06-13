#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

static struct _RTCharacterInventoryInfo kInventoryInfoBackup;
static struct _RTCharacterWarehouseInfo kWarehouseInfoBackup;

CLIENT_PROCEDURE_BINDING(GET_WAREHOUSE) {
	if (!Character) goto error;

	S2C_DATA_GET_WAREHOUSE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_WAREHOUSE);
	Response->Count = Character->Data.WarehouseInfo.Count;
	Response->Currency = Character->Data.WarehouseInfo.Currency;

	for (Int32 Index = 0; Index < Character->Data.WarehouseInfo.Count; Index += 1) {
		RTItemSlotRef ItemSlot = &Character->Data.WarehouseInfo.Slots[Index];
		
		S2C_DATA_GET_WAREHOUSE_SLOT_INDEX* ResponseSlot = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_WAREHOUSE_SLOT_INDEX);
		ResponseSlot->Item.Serial = ItemSlot->Item.Serial;
		ResponseSlot->Unknown1 = ItemSlot->ItemSerial;
		ResponseSlot->ItemOptions = ItemSlot->ItemOptions;
		ResponseSlot->SlotIndex = ItemSlot->SlotIndex;
		ResponseSlot->ItemDuration = ItemSlot->ItemDuration;
	}

	return SocketSend(Socket, Connection, Response);
error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(WAREHOUSE_CURRENCY_DEPOSIT) {
	memcpy(&kInventoryInfoBackup, &Character->Data.InventoryInfo, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&kWarehouseInfoBackup, &Character->Data.WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));

	S2C_DATA_WAREHOUSE_CURRENCY_DEPOSIT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, WAREHOUSE_CURRENCY_DEPOSIT);

	Response->Result = 1;

	// TODO: Calculate the tax for the deposit

	if (Packet->Amount > 0) {
		if (Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < Packet->Amount) {
			Response->Result = 0;
			return SocketSend(Socket, Connection, Response);
		}

		Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Packet->Amount;
		Character->Data.WarehouseInfo.Currency += Packet->Amount;
	}
	else {
		Int64 AbsAmount = ABS(Packet->Amount);

		if (Character->Data.WarehouseInfo.Currency < AbsAmount) goto error;

		Character->Data.Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] += AbsAmount;
		Character->Data.WarehouseInfo.Currency -= AbsAmount;
	}

	Character->SyncMask.InventoryInfo = true;
	Character->SyncMask.WarehouseInfo = true;

	return SocketSend(Socket, Connection, Response);

error:
	memcpy(&Character->Data.InventoryInfo, &kInventoryInfoBackup, sizeof(struct _RTCharacterInventoryInfo));
	memcpy(&Character->Data.WarehouseInfo, &kWarehouseInfoBackup, sizeof(struct _RTCharacterWarehouseInfo));
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SORT_WAREHOUSE) {
	if (!Character) {
		return SocketDisconnect(Socket, Connection);
	}

	struct _RTCharacterWarehouseInfo TempWarehouseMemory = { 0 };
	RTCharacterWarehouseInfoRef TempWarehouse = &TempWarehouseMemory;
	memcpy(TempWarehouse, &Character->Data.WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));

	Bool WarehouseOccupancyMask[RUNTIME_WAREHOUSE_TOTAL_SIZE] = { 0 };
	memset(WarehouseOccupancyMask, 0, sizeof(Bool) * RUNTIME_WAREHOUSE_TOTAL_SIZE);
	for (Int32 Index = 0; Index < Character->Data.WarehouseInfo.Count; Index += 1) {
		WarehouseOccupancyMask[Character->Data.WarehouseInfo.Slots[Index].SlotIndex] = true;
	}

	for (Int32 Index = 0; Index < Packet->Count; Index += 1) {
		WarehouseOccupancyMask[Packet->WarehouseSlots[Index]] = false;
	}

	for (Int32 Index = 0; Index < Packet->Count; Index += 1) {
		Int32 SlotIndex = RTWarehouseGetSlotIndex(Runtime, &Character->Data.WarehouseInfo, Packet->WarehouseSlots[Index]);
		if (SlotIndex < 0) goto error;

		RTItemSlotRef Slot = &TempWarehouse->Slots[SlotIndex];

		Bool Found = false;
		for (Int32 SlotIndex = 0; SlotIndex < RUNTIME_WAREHOUSE_TOTAL_SIZE; SlotIndex += 1) {
			if (!WarehouseOccupancyMask[SlotIndex]) {
				WarehouseOccupancyMask[SlotIndex] = true;
				Slot->SlotIndex = SlotIndex;
				Found = true;
				break;
			}
		}
		assert(Found);
	}

	memcpy(&Character->Data.WarehouseInfo, TempWarehouse, sizeof(struct _RTCharacterWarehouseInfo));
	Character->SyncMask.WarehouseInfo = true;

	S2C_DATA_SORT_WAREHOUSE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SORT_WAREHOUSE);
	Response->Success = 1;
	return SocketSend(Socket, Connection, Response);

error:
	{
		S2C_DATA_SORT_WAREHOUSE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SORT_WAREHOUSE);
		Response->Success = 0;
		return SocketSend(Socket, Connection, Response);
	}
}
