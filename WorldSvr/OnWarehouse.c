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
	Response->Count = Character->Data.WarehouseInfo.Info.SlotCount;
	Response->Currency = Character->Data.WarehouseInfo.Info.Currency;

	for (Int32 Index = 0; Index < Character->Data.WarehouseInfo.Info.SlotCount; Index += 1) {
		RTItemSlotRef ItemSlot = &Character->Data.WarehouseInfo.Slots[Index];
		
		S2C_DATA_GET_WAREHOUSE_SLOT_INDEX* ResponseSlot = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_GET_WAREHOUSE_SLOT_INDEX);
		ResponseSlot->Item.Serial = ItemSlot->Item.Serial;
		ResponseSlot->Unknown1 = ItemSlot->ItemSerial;
		ResponseSlot->ItemOptions = ItemSlot->ItemOptions;
		ResponseSlot->SlotIndex = ItemSlot->SlotIndex;
		ResponseSlot->ItemDuration = ItemSlot->ItemDuration;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(WAREHOUSE_CURRENCY_DEPOSIT) {
	S2C_DATA_WAREHOUSE_CURRENCY_DEPOSIT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, WAREHOUSE_CURRENCY_DEPOSIT);

	Response->Result = 1;

	// Deposit
	if (Packet->Amount > 0) {
		// TODO: Move tax to config?
		Int64 MinimumTax = 1000;
		Int64 MaximumTax = 10000;
		Float64 TaxPercent = 0.4;

		Int64 TaxAmount = ((TaxPercent / 100) * Packet->Amount);

		TaxAmount = MAX(MinimumTax, TaxAmount);
		TaxAmount = MIN(MaximumTax, TaxAmount);

		Int64 RealDepositAmount = Packet->Amount;

		// Error: Deposit more than available or smaller/equal than minimum amount
		if (Character->Data.Info.Alz < Packet->Amount || MinimumTax >= Packet->Amount) {
			goto error;
		}

		// Subtract from inventory first, to match client behaviour
		if (Character->Data.Info.Alz > (Packet->Amount + TaxAmount)) {
			Character->Data.Info.Alz -= TaxAmount;
		}
		// Subtract from deposit
		else {
			RealDepositAmount -= TaxAmount;
		}

		Character->Data.Info.Alz -= Packet->Amount;
		Character->Data.WarehouseInfo.Info.Currency += RealDepositAmount;
	}
	// Withdraw
	else {
		Int64 AbsAmount = ABS(Packet->Amount);

		// Error: Withdraw more than available
		if (Character->Data.WarehouseInfo.Info.Currency < AbsAmount) {
			goto error;
		}

		Character->Data.Info.Alz += AbsAmount;
		Character->Data.WarehouseInfo.Info.Currency -= AbsAmount;
	}

	Character->SyncMask.Info = true;
	Character->SyncMask.WarehouseInfo = true;

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
		return;
	}
}

CLIENT_PROCEDURE_BINDING(SORT_WAREHOUSE) {
	if (!Character) {
		SocketDisconnect(Socket, Connection);
		return;
	}

	struct _RTCharacterWarehouseInfo TempWarehouseMemory = { 0 };
	RTCharacterWarehouseInfoRef TempWarehouse = &TempWarehouseMemory;
	memcpy(TempWarehouse, &Character->Data.WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));

	Bool WarehouseOccupancyMask[RUNTIME_WAREHOUSE_TOTAL_SIZE] = { 0 };
	memset(WarehouseOccupancyMask, 0, sizeof(Bool) * RUNTIME_WAREHOUSE_TOTAL_SIZE);
	for (Int32 Index = 0; Index < Character->Data.WarehouseInfo.Info.SlotCount; Index += 1) {
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
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_SORT_WAREHOUSE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SORT_WAREHOUSE);
		Response->Success = 0;
		SocketSend(Socket, Connection, Response);
	}
}
