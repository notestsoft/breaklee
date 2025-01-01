#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(MERIT_MEDAL_EVALUATION) {
	if (!Character) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	RTItemDataRef ItemData = (ItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID) : NULL;
	if (!ItemData || ItemData->ItemType != RUNTIME_ITEM_TYPE_MERIT_MEDAL) goto error;
	
	RTItemOptions ItemOptions = { .Serial = ItemSlot->ItemOptions };
	if (ItemData->MeritMedal.Nation != Character->Data.StyleInfo.Nation) goto error;
	if (ItemData->MeritMedal.MaxEvaluationCount <= ItemOptions.MeritMedal.EvaluationCount) goto error;

	RTDataMeritMedalEvaluationFeeRef MeritMedalEvaluationFee = RTRuntimeDataMeritMedalEvaluationFeeGet(
		Runtime->Context,
		ItemData->MeritMedal.Type,
		ItemOptions.MeritMedal.EvaluationCount + 1
	);
	if (!MeritMedalEvaluationFee) goto error;

	if (Character->Data.Info.Wexp < MeritMedalEvaluationFee->RequiredWexp) goto error;

	Character->Data.Info.Wexp -= MeritMedalEvaluationFee->RequiredWexp;
	Character->SyncMask.Info = true;

	RTDataMeritItemPoolRef ItemPool = RTRuntimeDataMeritItemPoolGet(Runtime->Context, ItemData->MeritMedal.Type);
	RTDataMeritItemPoolGroupRef ItemPoolGroup = (ItemPool) ? RTRuntimeDataMeritItemPoolGroupGet(ItemPool, ItemOptions.MeritMedal.EvaluationCount) : NULL;
	if (!ItemPoolGroup) goto error;

	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 RandomRate = RandomRange(&Seed, 0, 1000000);
	Int32 RandomRateOffset = 0;

	Int32 PoolIndex = 0;
	while (true) {
		RTDataMeritItemPoolGroupItemRef GroupItem = &ItemPoolGroup->MeritItemPoolGroupItemList[PoolIndex];
		if (RandomRate <= GroupItem->Rate * 1000 + RandomRateOffset) {
			UInt64 ItemID = GroupItem->ItemIDs[Character->Data.StyleInfo.Nation - 1] | (ItemSlot->Item.Serial & RUNTIME_ITEM_MASK_BINDING);
			ItemSlot->Item.Serial = ItemID;
			ItemOptions.MeritMedal.StackSizeOrGrade = ItemData->ItemGrade;
			ItemOptions.MeritMedal.EvaluationCount += 1;
			ItemSlot->ItemOptions = ItemOptions.Serial;
			Character->SyncMask.InventoryInfo = true;
			break;
		}

		RandomRateOffset += GroupItem->Rate * 1000;
		PoolIndex += 1;
		PoolIndex %= ItemPoolGroup->MeritItemPoolGroupItemCount;
	}

	S2C_DATA_MERIT_MEDAL_EVALUATION* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MEDAL_EVALUATION);
	Response->Success = 1;
	Response->ItemID = ItemSlot->Item.Serial;
	Response->ItemOptions = ItemSlot->ItemOptions;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MERIT_MEDAL_RESET) {
	if (!Character) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	RTItemDataRef ItemData = (ItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID) : NULL;
	if (!ItemData || ItemData->ItemType != RUNTIME_ITEM_TYPE_MERIT_MEDAL) goto error;
	if (ItemData->MeritMedal.Nation != Character->Data.StyleInfo.Nation) goto error;

	RTItemOptions ItemOptions = { .Serial = ItemSlot->ItemOptions };
	if (ItemOptions.MeritMedal.StackSizeOrGrade < 1) goto error;

	RTDataMeritItemResetPoolRef ItemPool = RTRuntimeDataMeritItemResetPoolGet(Runtime->Context, ItemOptions.MeritMedal.StackSizeOrGrade);
	if (!ItemPool) goto error;

	UInt64 ItemID = ItemPool->ItemIDs[Character->Data.StyleInfo.Nation - 1] | (ItemSlot->Item.Serial & RUNTIME_ITEM_MASK_BINDING);
	ItemSlot->Item.Serial = ItemID;
	ItemOptions.MeritMedal.StackSizeOrGrade = 0;
	ItemSlot->ItemOptions = ItemOptions.Serial;
	Character->SyncMask.InventoryInfo = true;

	S2C_DATA_MERIT_MEDAL_RESET* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MEDAL_RESET);
	Response->Success = 1;
	Response->ItemID = ItemSlot->Item.Serial;
	Response->ItemOptions = ItemSlot->ItemOptions;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(MERIT_MEDAL_REGISTRATION) {
	if (!Character) goto error;

	Int PayloadLength = sizeof(Int32) * Packet->InventorySlotCount;
	Int PacketLength = sizeof(C2S_DATA_MERIT_MEDAL_REGISTRATION) + PayloadLength;
	if (PacketLength != Packet->Length || PayloadLength != Packet->PayloadLength) goto error;

	S2C_DATA_MERIT_MEDAL_REGISTRATION* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MEDAL_REGISTRATION);
	Response->Success = RTCharacterRegisterMeritMedals(Runtime, Character, Packet->InventorySlotCount, Packet->InventorySlotIndex);
	Response->GoldMeritPoints = Character->Data.GoldMeritMasteryInfo.Info.Points;
	Response->PlatinumMeritPoints = Character->Data.PlatinumMeritMasteryInfo.Info.Points;
	Response->DiamondMeritPoints = Character->Data.DiamondMeritMasteryInfo.Info.Points;
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MERIT_MEDAL_REGISTRATION* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MEDAL_REGISTRATION);
		Response->Success = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(MERIT_MASTERY_TRAIN) {
	if (!Character) goto error;

	S2C_DATA_MERIT_MASTERY_TRAIN* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_TRAIN);
	Response->Success = RTCharacterMeritMasteryTrain(Runtime, Character, Packet->MasteryIndex, Packet->TargetLevel, &Response->MeritPoints);
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MERIT_MASTERY_TRAIN* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_TRAIN);
		Response->Success = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(MERIT_MASTERY_GRADE_UP) {
	if (!Character) goto error;

	S2C_DATA_MERIT_MASTERY_GRADE_UP* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_GRADE_UP);
	Response->Result = RTCharacterPlatinumMeritMasteryGradeUp(Runtime, Character, Packet->InventorySlotCount, Packet->InventorySlots);
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MERIT_MASTERY_GRADE_UP* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_GRADE_UP);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(MERIT_MASTERY_OPEN_SLOT) {
	if (!Character) goto error;

	Int32 PacketLength = sizeof(C2S_DATA_MERIT_MASTERY_OPEN_SLOT) + (Packet->MaterialSlotCount1 + Packet->MaterialSlotCount2) * sizeof(UInt16);
	if (Packet->Length != PacketLength) goto error;

	UInt16* MaterialSlotIndex1 = &Packet->MaterialSlotIndex[0];
	UInt16* MaterialSlotIndex2 = &Packet->MaterialSlotIndex[Packet->MaterialSlotCount1];

	S2C_DATA_MERIT_MASTERY_OPEN_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_OPEN_SLOT);
	Response->Result = RTCharacterPlatinumMeritMasteryOpenSlot(
		Runtime,
		Character,
		Packet->MasteryIndex,
		Packet->MaterialSlotCount1,
		MaterialSlotIndex1,
		Packet->MaterialSlotCount2,
		MaterialSlotIndex2
	);
	Response->MasteryIndex = Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex;
	Response->UnlockTime = Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotUnlockTime;
	Response->UnlockTime |= GetTimestamp() << 32;
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MERIT_MASTERY_OPEN_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_OPEN_SLOT);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(MERIT_MASTERY_OPEN_SLOT_CANCEL) {
	if (!Character) goto error;

	S2C_DATA_MERIT_MASTERY_OPEN_SLOT_CANCEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_OPEN_SLOT_CANCEL);
	Response->Result = 0;

	if (Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex == Packet->MasteryIndex) {
		Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex = -1;
		Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotUnlockTime = 0;
		Character->SyncMask.PlatinumMeritMasteryInfo = true;
		Response->Result = 1;
	}

	if (Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex == Packet->MasteryIndex) {
		Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex = -1;
		Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotUnlockTime = 0;
		Character->SyncMask.DiamondMeritMasteryInfo = true;
		Response->Result = 1;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MERIT_MASTERY_OPEN_SLOT_CANCEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_OPEN_SLOT_CANCEL);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(MERIT_MASTERY_SET_PAGE_INDEX) {
	if (!Character) goto error;
	
	S2C_DATA_MERIT_MASTERY_SET_PAGE_INDEX* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_SET_PAGE_INDEX);
	Response->Result = RTCharacterPlatinumMeritMasterySetActiveMemorizeIndex(Runtime, Character, Packet->MemorizeIndex);
	Response->MeritPoints = RTCharacterPlatinumMeritMasteryGetPoints(Runtime, Character);
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MERIT_MASTERY_SET_PAGE_INDEX* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_SET_PAGE_INDEX);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(MERIT_MASTERY_GRANT_SPECIAL_MASTERY) {
	if (!Character) goto error;

	Int32 PacketLength = sizeof(C2S_DATA_MERIT_MASTERY_GRANT_SPECIAL_MASTERY) + sizeof(UInt16) * Packet->InventorySlotCount;
	if (Packet->Length != PacketLength) goto error;

	S2C_DATA_MERIT_MASTERY_GRANT_SPECIAL_MASTERY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_GRANT_SPECIAL_MASTERY);
	Response->Result = RTCharacterPlatinumMeritMasteryGrantSpecialMastery(
		Runtime,
		Character,
		Packet->CategoryIndex,
		Packet->InventorySlotCount,
		Packet->InventorySlotIndex
	);

	RTPlatinumMeritSpecialMasterySlotRef SpecialMasterySlots[2] = { NULL, NULL };
	RTCharacterPlatinumMeritMasteryGetSpecialMasterySlots(
		Runtime,
		Character,
		Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex,
		Packet->CategoryIndex,
		&SpecialMasterySlots[0],
		&SpecialMasterySlots[1]
	);

	if (SpecialMasterySlots[0] && SpecialMasterySlots[1]) {
		memcpy(&Response->SpecialMasterySlots[0], SpecialMasterySlots[0], sizeof(struct _RTPlatinumMeritSpecialMasterySlot));
		memcpy(&Response->SpecialMasterySlots[1], SpecialMasterySlots[1], sizeof(struct _RTPlatinumMeritSpecialMasterySlot));
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MERIT_MASTERY_GRANT_SPECIAL_MASTERY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_GRANT_SPECIAL_MASTERY);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(MERIT_MASTERY_SEAL_SPECIAL_MASTERY) {
	if (!Character) goto error;
	if (!Character->Data.PlatinumMeritMasteryInfo.Info.IsEnabled) goto error;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	if (!ItemSlot) goto error;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) goto error;
	if (ItemData->ItemType != RUNTIME_ITEM_TYPE_SPECIAL_MASTERY_CHEST) goto error;

	RTPlatinumMeritSpecialMasterySlotRef SpecialMasterySlots[2] = { NULL, NULL };
	RTCharacterPlatinumMeritMasteryGetSpecialMasterySlots(
		Runtime,
		Character,
		Packet->MemorizeIndex,
		Packet->CategoryIndex,
		&SpecialMasterySlots[0],
		&SpecialMasterySlots[1]
	);
	if (!SpecialMasterySlots[0] || !SpecialMasterySlots[1]) goto error;

	S2C_DATA_MERIT_MASTERY_SEAL_SPECIAL_MASTERY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_SEAL_SPECIAL_MASTERY);
	Response->Result = C2S_DATA_MERIT_MASTERY_SEAL_SPECIAL_MASTERY_RESULT_FAIL;

	RTItemOptions ItemOptions = { .Serial = ItemSlot->ItemOptions };
	if (ItemOptions.Serial > 0) {
		if (SpecialMasterySlots[0]->Category != ItemOptions.MeritSeal.Category) goto error;
		if (SpecialMasterySlots[1]->Category != ItemOptions.MeritSeal.Category) goto error;

		SpecialMasterySlots[0]->Index = ItemOptions.MeritSeal.Index1;
		SpecialMasterySlots[0]->Grade = ItemOptions.MeritSeal.Grade1;
		SpecialMasterySlots[1]->Index = ItemOptions.MeritSeal.Index2;
		SpecialMasterySlots[1]->Grade = ItemOptions.MeritSeal.Grade2;
		Character->SyncMask.PlatinumMeritMasteryInfo = true;

		ItemOptions.Serial = 0;
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
		Character->SyncMask.InventoryInfo = true;

		Response->Result = C2S_DATA_MERIT_MASTERY_SEAL_SPECIAL_MASTERY_RESULT_SUCCESS_UNSEAL;
	}
	else {
		ItemOptions.MeritSeal.Category = SpecialMasterySlots[0]->Category;
		ItemOptions.MeritSeal.Index1 = SpecialMasterySlots[0]->Index;
		ItemOptions.MeritSeal.Grade1 = SpecialMasterySlots[0]->Grade;
		ItemOptions.MeritSeal.Index2 = SpecialMasterySlots[1]->Index;
		ItemOptions.MeritSeal.Grade2 = SpecialMasterySlots[1]->Grade;
		ItemSlot->ItemOptions = ItemOptions.Serial;
		Character->SyncMask.InventoryInfo = true;

		SpecialMasterySlots[0]->Index = 0;
		SpecialMasterySlots[0]->Grade = 0;
		SpecialMasterySlots[1]->Index = 0;
		SpecialMasterySlots[1]->Grade = 0;
		Character->SyncMask.PlatinumMeritMasteryInfo = true;
	
		Response->Result = C2S_DATA_MERIT_MASTERY_SEAL_SPECIAL_MASTERY_RESULT_SUCCESS_SEAL;
	}

	Response->ItemOptions = ItemOptions.Serial;
	memcpy(&Response->SpecialMasterySlots[0], SpecialMasterySlots[0], sizeof(struct _RTPlatinumMeritSpecialMasterySlot));
	memcpy(&Response->SpecialMasterySlots[1], SpecialMasterySlots[1], sizeof(struct _RTPlatinumMeritSpecialMasterySlot));
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_MERIT_MASTERY_SEAL_SPECIAL_MASTERY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, MERIT_MASTERY_SEAL_SPECIAL_MASTERY);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(DIAMOND_MERIT_MASTERY_GRADE_UP) {
	if (!Character) goto error;

	S2C_DATA_DIAMOND_MERIT_MASTERY_GRADE_UP* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIAMOND_MERIT_MASTERY_GRADE_UP);
	Response->Result = RTCharacterDiamondMeritMasteryGradeUp(Runtime, Character, Packet->InventorySlotCount, Packet->InventorySlots);
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_DIAMOND_MERIT_MASTERY_GRADE_UP* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIAMOND_MERIT_MASTERY_GRADE_UP);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(DIAMOND_MERIT_MASTERY_SET_PAGE_INDEX) {
	if (!Character) goto error;

	S2C_DATA_DIAMOND_MERIT_MASTERY_SET_PAGE_INDEX* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIAMOND_MERIT_MASTERY_SET_PAGE_INDEX);
	Response->Result = RTCharacterDiamondMeritMasterySetActiveMemorizeIndex(Runtime, Character, Packet->MemorizeIndex);
	Response->MeritPoints = RTCharacterDiamondMeritMasteryGetPoints(Runtime, Character);
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_DIAMOND_MERIT_MASTERY_SET_PAGE_INDEX* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIAMOND_MERIT_MASTERY_SET_PAGE_INDEX);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(DIAMOND_MERIT_MASTERY_OPEN_SLOT) {
	if (!Character) goto error;

	Int32 PacketLength = sizeof(C2S_DATA_MERIT_MASTERY_OPEN_SLOT) + (Packet->MaterialSlotCount1 + Packet->MaterialSlotCount2) * sizeof(UInt16);
	if (Packet->Length != PacketLength) goto error;

	UInt16* MaterialSlotIndex1 = &Packet->MaterialSlotIndex[0];
	UInt16* MaterialSlotIndex2 = &Packet->MaterialSlotIndex[Packet->MaterialSlotCount1];

	S2C_DATA_DIAMOND_MERIT_MASTERY_OPEN_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIAMOND_MERIT_MASTERY_OPEN_SLOT);
	Response->Result = RTCharacterDiamondMeritMasteryOpenSlot(
		Runtime,
		Character,
		Packet->MasteryIndex,
		Packet->MaterialSlotCount1,
		MaterialSlotIndex1,
		Packet->MaterialSlotCount2,
		MaterialSlotIndex2
	);
	Response->MasteryIndex = Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex;
	Response->UnlockTime = Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotUnlockTime;
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_DIAMOND_MERIT_MASTERY_OPEN_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIAMOND_MERIT_MASTERY_OPEN_SLOT);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(DIAMOND_MERIT_MASTERY_GRANT_SPECIAL_MASTERY) {
	if (!Character) goto error;

	Int32 PacketLength = sizeof(C2S_DATA_DIAMOND_MERIT_MASTERY_GRANT_SPECIAL_MASTERY) + sizeof(UInt16) * Packet->InventorySlotCount;
	if (Packet->Length != PacketLength) goto error;

	S2C_DATA_DIAMOND_MERIT_MASTERY_GRANT_SPECIAL_MASTERY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIAMOND_MERIT_MASTERY_GRANT_SPECIAL_MASTERY);
	Response->Result = RTCharacterDiamondMeritMasteryGrantSpecialMastery(
		Runtime,
		Character,
		Packet->CategoryIndex,
		Packet->InventorySlotCount,
		Packet->InventorySlotIndex
	);
	Response->Points = Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasteryExp;
	RTDiamondMeritSpecialMasterySlotRef SpecialMasterySlots[3] = { NULL, NULL, NULL };
	RTCharacterDiamondMeritMasteryGetSpecialMasterySlots(
		Runtime,
		Character,
		Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex,
		Packet->CategoryIndex,
		&SpecialMasterySlots[0],
		&SpecialMasterySlots[1],
		&SpecialMasterySlots[2]
	);

	if (SpecialMasterySlots[0] && SpecialMasterySlots[1] && SpecialMasterySlots[2]) {
		memcpy(&Response->SpecialMasterySlots[0], SpecialMasterySlots[0], sizeof(struct _RTDiamondMeritSpecialMasterySlot));
		memcpy(&Response->SpecialMasterySlots[1], SpecialMasterySlots[1], sizeof(struct _RTDiamondMeritSpecialMasterySlot));
		memcpy(&Response->SpecialMasterySlots[2], SpecialMasterySlots[2], sizeof(struct _RTDiamondMeritSpecialMasterySlot));
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_DIAMOND_MERIT_MASTERY_GRANT_SPECIAL_MASTERY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, DIAMOND_MERIT_MASTERY_GRANT_SPECIAL_MASTERY);
		Response->Result = 0;
		SocketSend(Socket, Connection, Response);
	}
}
