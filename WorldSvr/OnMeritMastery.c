#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(MERIT_MEDAL_EVALUATION) {
	if (!Character) goto error;
	if (Character->Data.StyleInfo.Nation < 1) goto error;

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

	for (Int Index = 0; Index < ItemPoolGroup->MeritItemPoolGroupItemCount; Index += 1) {
		RTDataMeritItemPoolGroupItemRef GroupItem = &ItemPoolGroup->MeritItemPoolGroupItemList[Index];
		if (RandomRate <= GroupItem->Rate * 1000 + RandomRateOffset) {
			ItemSlot->Item.Serial = GroupItem->ItemIDs[Character->Data.StyleInfo.Nation - 1];
			ItemOptions.MeritMedal.EvaluationCount += 1;
			ItemSlot->ItemOptions = ItemOptions.Serial;
			Character->SyncMask.InventoryInfo = true;
			break;
		}

		RandomRateOffset += GroupItem->Rate * 1000;
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

CLIENT_PROCEDURE_BINDING(MERIT_MEDAL_REGISTRATION) {
	if (!Character) goto error;
	if (Character->Data.StyleInfo.Nation < 1) goto error;

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
	if (Character->Data.StyleInfo.Nation < 1) goto error;

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