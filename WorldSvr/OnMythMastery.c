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

		if (!OldSlot) goto error;
		memcpy(&Character->Data.MythMasteryInfo.TemporarySlot, OldSlot, sizeof(struct _RTMythMasterySlot));
	}

	RTMythMasterySlotRef MasterySlot = RTCharacterMythMasteryGetOrCreateSlot(Runtime, Character, Packet->MasteryIndex, Packet->SlotIndex);
	if (!MasterySlot) goto error;

	Bool Success = RTCharacterMythMasteryRollSlot(Runtime, Character, MasterySlot);

	S2C_DATA_MYTH_ROLL_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_ROLL_SLOT);
	Response->MythPoints = Character->Data.MythMasteryInfo.Info.Points;
	Response->HolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
	if (MasterySlot) memcpy(&Response->MasterySlot, MasterySlot, sizeof(struct _RTMythMasterySlot));
	Response->StigmaGrade = Character->Data.MythMasteryInfo.Info.StigmaGrade;
	Response->StigmaXP = Character->Data.MythMasteryInfo.Info.StigmaExp;
	Response->ErrorCode = Success ? 0 : 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MYTH_FINISH_ROLL_SLOT) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level < 1) goto error;

	S2C_DATA_MYTH_FINISH_ROLL_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MYTH_FINISH_ROLL_SLOT);

	// if 0, revert to temporary. 1 == return with holypower & errorcode

	// not sure but shouldnt allow client to determine array size. also shouldnt split currency that much realistically
	UInt16 InventoryIndexCount = MIN(Packet->InventorySlotCount, 5);
	UInt16 InventoryPositions[5] = {};

	//Packet->InventorySlotIndex;

	for (Int32 Index = 0; Index < InventoryIndexCount; Index++) {
		//memset(&InventoryPositions[Index], &Packet->InventorySlotIndex[Index], sizeof(UInt16));
		//Warn("positions %u", InventoryPositions[Index]);
		InventoryPositions[Index] = Packet->InventorySlotIndex[Index];
	}

	Bool RollbackSuccess = false;
	
	if (Packet->RollbackToTempSlot == 1) {
		RollbackSuccess = RTCharacterMythMasteryRollback(Runtime, Character, &Character->Data.MythMasteryInfo.TemporarySlot);
	}

	// rollback
	if (Packet->RollbackToTempSlot == 1 && RollbackSuccess) {
		assert(&Character->Data.MythMasteryInfo.TemporarySlot);

		struct _RTMythMasterySlot MasterySlot;
		memcpy(&MasterySlot, &Character->Data.MythMasteryInfo.TemporarySlot, sizeof(struct _RTMythMasterySlot));

		RTCharacterMythMasterySetSlot(Runtime, Character, MasterySlot.MasteryIndex, MasterySlot.SlotIndex, MasterySlot.Tier, MasterySlot.Grade, MasterySlot.ForceEffectIndex, MasterySlot.ForceValue, MasterySlot.ForceValueType);
		
		Response->MasteryIndex = Character->Data.MythMasteryInfo.TemporarySlot.MasteryIndex;
		Response->SlotIndex = Character->Data.MythMasteryInfo.TemporarySlot.SlotIndex;
		Response->TierIndex = Character->Data.MythMasteryInfo.TemporarySlot.Tier;
		Response->TierLevel = Character->Data.MythMasteryInfo.TemporarySlot.Grade;
		Response->StatOption = Character->Data.MythMasteryInfo.TemporarySlot.ForceEffectIndex;
		Response->StatValue = Character->Data.MythMasteryInfo.TemporarySlot.ForceValue;
		Response->ValueType = Character->Data.MythMasteryInfo.TemporarySlot.ForceValueType;
		Response->HolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
		Response->ErrorCode = 0;
		SocketSend(Socket, Connection, Response);
		return;
	}

	// accept slot
	Response->MasteryIndex = 0;
	Response->SlotIndex = 0;
	Response->TierIndex = 0;
	Response->TierLevel = 0;
	Response->StatOption = 0;
	Response->ValueType = 0;
	Response->HolyPower = Character->Data.MythMasteryInfo.Info.HolyPower;
	Response->ErrorCode = 0;

	if (Packet->RollbackToTempSlot == 1)
		Response->ErrorCode = RollbackSuccess ? 0 : 1;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
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
		Character->Data.MythMasteryInfo.Info.HolyPower += MythLockInfoRef->BonusScore;
		Character->SyncMask.MythMasteryInfo = true;
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