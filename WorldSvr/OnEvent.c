#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_EVENT_LIST) {
	if (!Character) goto error;

	S2C_DATA_GET_EVENT_LIST* Response = PacketInit(S2C_DATA_GET_EVENT_LIST);
	Response->Command = S2C_GET_EVENT_LIST;
	Response->EventCount = Context->Runtime->Context->EventCount;

	for (Int32 Index = 0; Index < Context->Runtime->Context->EventCount; Index += 1) {
		RTDataEventRef EventData = &Context->Runtime->Context->EventList[Index];

		S2C_DATA_EVENT* Event = PacketAppendStruct(S2C_DATA_EVENT);
		Event->EventIndex = EventData->ID;

		S2C_DATA_EVENT_LABEL_HEADER* LabelHeader = PacketAppendStruct(S2C_DATA_EVENT_LABEL_HEADER);
		LabelHeader->Count = EventData->EventDescriptionCount;

		for (Int32 DescriptionIndex = 0; DescriptionIndex < EventData->EventDescriptionCount; DescriptionIndex += 1) {
			PacketAppendCString(EventData->EventDescriptionList[DescriptionIndex].Description);
		}

		S2C_DATA_EVENT_UNKNOWN1_HEADER* Unknown1Header = PacketAppendStruct(S2C_DATA_EVENT_UNKNOWN1_HEADER);
		Unknown1Header->Count = 0;

		S2C_DATA_EVENT_UNKNOWN2_HEADER* Unknown2Header = PacketAppendStruct(S2C_DATA_EVENT_UNKNOWN2_HEADER);
		Unknown2Header->Count = 0;

		S2C_DATA_EVENT_SHOP_HEADER* ShopHeader = PacketAppendStruct(S2C_DATA_EVENT_SHOP_HEADER);
		if (EventData->EventShopCount > 0) {
			RTDataEventShopRef EventShopData = &EventData->EventShopList[0];

			ShopHeader->ItemCount = EventShopData->EventShopItemCount;
			for (Int32 ItemIndex = 0; ItemIndex < ShopHeader->ItemCount; ItemIndex += 1) {
				RTDataEventShopItemRef ItemData = &EventShopData->EventShopItemList[ItemIndex];

				S2C_DATA_EVENT_SHOP_ITEM* ShopItem = PacketAppendStruct(S2C_DATA_EVENT_SHOP_ITEM);
				ShopItem->ShopSlotIndex = ItemData->SlotIndex;
				ShopItem->ItemID.Serial = ItemData->ItemID;
				ShopItem->ItemOptions = ItemData->ItemOptions;
				ShopItem->CurrencyPrice = ItemData->ItemPrice;

				/*
				ShopItem->ItemPriceCount = 10;
				for (Int32 Index = 0; Index < ShopItem->ItemPriceCount; Index += 1) {
					S2C_DATA_EVENT_SHOP_ITEM_PRICE* ShopItemPrice = PacketAppendStruct(S2C_DATA_EVENT_SHOP_ITEM_PRICE);
					ShopItemPrice->ItemID.ID = 1;
					ShopItemPrice->ItemCount = 1;
				}
				*/
			}
		}

		S2C_DATA_EVENT_UNKNOWN3_HEADER* Unknown3Header = PacketAppendStruct(S2C_DATA_EVENT_UNKNOWN3_HEADER);
		Unknown3Header->Count = 0;

		S2C_DATA_EVENT_BINGO_HEADER* BingoHeader = PacketAppendStruct(S2C_DATA_EVENT_BINGO_HEADER);
		BingoHeader->Count = 0;

		S2C_DATA_EVENT_BINGO_INITIALIZE_HEADER* BingoInitHeader = PacketAppendStruct(S2C_DATA_EVENT_BINGO_INITIALIZE_HEADER);
		BingoInitHeader->Count = 0;

		S2C_DATA_EVENT_PERIOD_HEADER* PeriodHeader = PacketAppendStruct(S2C_DATA_EVENT_PERIOD_HEADER);
		PeriodHeader->Count = 0;

		S2C_DATA_EVENT_DUNGEON_REWARD_HEADER* DungeonRewardHeader = PacketAppendStruct(S2C_DATA_EVENT_DUNGEON_REWARD_HEADER);
		DungeonRewardHeader->Count = 0;
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(EVENT_ACTION) {
	if (!Character) goto error;

	Index PacketLength = sizeof(C2S_DATA_EVENT_ACTION) + sizeof(UInt16) * Packet->InventorySlotCount;
	if (Packet->Signature.Length != PacketLength) goto error;

	RTDataEventRef Event = RTRuntimeDataEventGet(Runtime->Context, Packet->EventIndex);
	if (!Event) goto error;
	if (Event->NpcIndex != Packet->NpcIndex) goto error;

	// TODO: Add action types for other events...
	// TODO: Check NPC distance to character
	// TODO: Check and subtract item prices

	if (Event->EventShopCount < 1) goto error;

	RTDataEventShopRef EventShop = &Event->EventShopList[0];
	RTDataEventShopItemRef Item = RTRuntimeDataEventShopItemGet(EventShop, Packet->ShopSlotIndex);
	if (!Item) goto error;

	for (Index Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
		if (!RTInventoryIsSlotEmpty(Runtime, &Character->InventoryInfo, Packet->InventorySlotIndex[Index])) {
			goto error;
		}
	}
	
	S2C_DATA_EVENT_ACTION* Response = PacketInit(S2C_DATA_EVENT_ACTION);
	Response->Command = S2C_EVENT_ACTION;
	Response->EventIndex = Packet->EventIndex;
	Response->Unknown1 = Packet->Unknown1;
	Response->ItemCount = Packet->InventorySlotCount;

	struct _RTItemSlot ItemSlot = { 0 };
	for (Index Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
		ItemSlot.SlotIndex = Packet->InventorySlotIndex[Index];
		ItemSlot.Item.ID = Item->ItemID;
		ItemSlot.ItemOptions = Item->ItemOptions;
		// ItemSlot.ItemDuration = Item->ItemDurationID;

		if (!RTInventorySetSlot(Runtime, &Character->InventoryInfo, &ItemSlot)) goto error;

		Character->SyncMask.InventoryInfo = true;
		Character->SyncPriority.Low = true;

		S2C_DATA_EVENT_ACTION_SHOP_ITEM* ResponseItem = PacketAppendStruct(S2C_DATA_EVENT_ACTION_SHOP_ITEM);
		ResponseItem->Item = ItemSlot.Item;
		ResponseItem->ItemOptions = ItemSlot.ItemOptions;
		ResponseItem->ItemDuration = ItemSlot.ItemDuration;
		ResponseItem->SlotIndex = ItemSlot.SlotIndex;
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}