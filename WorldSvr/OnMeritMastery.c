#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(MERIT_MEDAL_EVALUATION) {
	if (!Character) goto error;
	if (Character->Data.StyleInfo.Nation < 1) goto error;

	// TODO: Consume WEXP

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	RTItemDataRef ItemData = (ItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID) : NULL;
	if (!ItemData || ItemData->ItemType != RUNTIME_ITEM_TYPE_MERITORIOUS_MEDAL) goto error;
	
	RTItemOptions ItemOptions = { .Serial = ItemSlot->ItemOptions };
	if (ItemData->MeritMedal.Nation != Character->Data.StyleInfo.Nation) goto error;
	if (ItemData->MeritMedal.MaxEvaluationCount <= ItemOptions.MeritMedal.EvaluationCount) goto error;

	RTDataMeritItemPoolRef ItemPool = RTRuntimeDataMeritItemPoolGet(Runtime->Context, ItemData->MeritMedal.Type);
	RTDataMeritItemPoolGroupRef ItemPoolGroup = (ItemPool) ? RTRuntimeDataMeritItemPoolGroupGet(ItemPool, ItemOptions.MeritMedal.EvaluationCount) : NULL;
	if (!ItemPoolGroup) goto error;

	Int32 Seed = PlatformGetTickCount();
	Int32 RandomRate = RandomRange(&Seed, 0, 1000000);
	Int32 RandomRateOffset = 0;

	for (Int32 Index = 0; Index < ItemPoolGroup->MeritItemPoolGroupItemCount; Index += 1) {
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

	S2C_DATA_MERIT_MEDAL_EVALUATION* Response = PacketBufferInit(Connection->PacketBuffer, S2C, MERIT_MEDAL_EVALUATION);
	Response->Success = 1;
	Response->ItemID = ItemSlot->Item.Serial;
	Response->ItemOptions = ItemSlot->ItemOptions;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}