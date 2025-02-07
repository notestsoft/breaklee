#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(MYTH_ROLL_SLOT) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level < 1) goto error;

	// copy existing slot into temporary slot
	if (RTCharacterMythMasteryGetSlotOccupied(Runtime, Character, Packet->MasteryIndex, Packet->SlotIndex)) {
		RTMythMasterySlotRef OldSlot = RTCharacterMythMasteryGetSlot(Runtime, Character, Packet->MasteryIndex, Packet->SlotIndex);

		if (!OldSlot) {
			Error("No old slot but slot occupied");
			goto error;
		}
		memcpy(&Character->Data.MythMasteryInfo.TemporarySlot, OldSlot, sizeof(struct _RTMythMasterySlot));
	}

	RTMythMasterySlotRef MasterySlot = RTCharacterMythMasteryGetOrCreateSlot(Runtime, Character, Packet->MasteryIndex, Packet->SlotIndex);
	if (!MasterySlot) {
		Error("Tried to get/create slot and failed!!!");
		goto error;
	}

	Bool Success = RTCharacterMythMasteryRollSlot(Runtime, Character, MasterySlot);

	S2C_DATA_MYTH_ROLL_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_ROLL_SLOT);
	Response->MythPoints = Character->Data.MythMasteryInfo.Info.Points;
	Response->HolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
	if (MasterySlot) memcpy(&Response->MasterySlot, MasterySlot, sizeof(struct _RTMythMasterySlot));
	Response->StigmaGrade = Character->Data.MythMasteryInfo.Info.StigmaGrade;
	Response->StigmaXP = Character->Data.MythMasteryInfo.Info.StigmaExp;
	Response->ErrorCode = Success ? 0 : 1;
	RTCharacterInitializeAttributes(Runtime, Character);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MYTH_FINISH_ROLL_SLOT) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level < 1) goto error;

	Int PacketLength = sizeof(C2S_DATA_MYTH_FINISH_ROLL_SLOT) + Packet->InventorySlotCount * sizeof(UInt16);
	if (Packet->Length < PacketLength) goto error;

	S2C_DATA_MYTH_FINISH_ROLL_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_FINISH_ROLL_SLOT);

	RTMythMasterySlotRef MasterySlot = NULL;
	if (Packet->IsRollback) {
		RTDataMythMasterySlotGroupRef SlotPage = RTRuntimeDataMythMasterySlotGroupGet(
			Runtime->Context,
			Character->Data.MythMasteryInfo.TemporarySlot.MasteryIndex,
			Character->Data.MythMasteryInfo.TemporarySlot.SlotIndex
		);
		if (!SlotPage) goto error;

		UInt64 MythRestoreItemID = RTCharacterMythMasteryGetRestoreItemID(Runtime);
		
		Int64 TotalConsumableItemCount = 0;
		for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
			TotalConsumableItemCount += RTInventoryGetConsumableItemCount(
				Runtime,
				&Character->Data.InventoryInfo,
				MythRestoreItemID,
				0,
				Packet->InventorySlotIndex[Index]
			);
		}
		if (TotalConsumableItemCount < SlotPage->RestoreCost) goto error;

		Int64 RemainingItemCount = TotalConsumableItemCount;
		for (Int Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
			RemainingItemCount -= RTInventoryConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				MythRestoreItemID,
				0,
				RemainingItemCount,
				Packet->InventorySlotIndex[Index]
			);
		}
		assert(RemainingItemCount <= 0);

		Character->SyncMask.InventoryInfo = true;

		MasterySlot = RTCharacterMythMasterySetSlot(
			Runtime, 
			Character, 
			Character->Data.MythMasteryInfo.TemporarySlot.MasteryIndex,
			Character->Data.MythMasteryInfo.TemporarySlot.SlotIndex, 
			Character->Data.MythMasteryInfo.TemporarySlot.Tier, 
			Character->Data.MythMasteryInfo.TemporarySlot.Grade, 
			Character->Data.MythMasteryInfo.TemporarySlot.ForceEffectIndex,
			Character->Data.MythMasteryInfo.TemporarySlot.ForceValue, 
			Character->Data.MythMasteryInfo.TemporarySlot.ForceValueType
		);
		memset(&Character->Data.MythMasteryInfo.TemporarySlot, 0, sizeof(struct _RTMythMasterySlot));
	}

	if (MasterySlot) memcpy(&Response->MasterySlot, MasterySlot, sizeof(struct _RTMythMasterySlot));
	Response->HolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
	Response->ErrorCode = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MYTH_FINISH_ROLL_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_FINISH_ROLL_SLOT);
		Response->ErrorCode = 1;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(MYTH_OPEN_LOCK) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level < 1) goto error;

	RTDataMythLockPageRef MythLockPageRef = RTRuntimeDataMythLockPageGet(Runtime->Context, Packet->MasteryIndex);
	RTDataMythLockInfoRef MythLockInfoRef = RTRuntimeDataMythLockInfoGet(MythLockPageRef, Packet->LockGroupIndex);

	S2C_DATA_MYTH_OPEN_LOCK* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_OPEN_LOCK);

	if (RTCharacterMythMasteryCanOpenLockGroup(Runtime, Character, Packet->MasteryIndex, Packet->LockGroupIndex)) {
		Response->MasteryIndex = Packet->MasteryIndex;
		Response->LockGroupIndex = MythLockInfoRef->LockGroup;
		Response->ErrorCode = 0;

		Character->Data.MythMasteryInfo.Info.UnlockedPageCount = MythLockInfoRef->LockGroup;
		RTCharacterMythMasteryAssertHolyPoints(Runtime, Character);
		//assert sets sync mask
	}
	else {
		Response->MasteryIndex = 0;
		Response->LockGroupIndex = 0;
		Response->ErrorCode = 1; //gives random error on client, but doesnt matter since normal players can't even get here
	}
	
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}