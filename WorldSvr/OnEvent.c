#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_EVENT_LIST) {
	if (!Character) goto error;

	S2C_DATA_GET_EVENT_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_EVENT_LIST);
	Response->EventCount = Context->Runtime->Context->EventCount;

	for (Int32 Index = 0; Index < Context->Runtime->Context->EventCount; Index += 1) {
		RTDataEventRef EventData = &Context->Runtime->Context->EventList[Index];

		S2C_DATA_EVENT* Event = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT);
		Event->EventIndex = EventData->ID;

		S2C_DATA_EVENT_LABEL_HEADER* LabelHeader = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_LABEL_HEADER);
		LabelHeader->Count = EventData->EventDescriptionCount;

		for (Int32 DescriptionIndex = 0; DescriptionIndex < EventData->EventDescriptionCount; DescriptionIndex += 1) {
            CString Description = EventData->EventDescriptionList[DescriptionIndex].Description;
			PacketBufferAppendCString(Connection->PacketBuffer, Description);
		}

		S2C_DATA_EVENT_UNKNOWN1_HEADER* Unknown1Header = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_UNKNOWN1_HEADER);
		Unknown1Header->Count = 0;

		S2C_DATA_EVENT_UNKNOWN2_HEADER* Unknown2Header = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_UNKNOWN2_HEADER);
		Unknown2Header->Count = 0;

		S2C_DATA_EVENT_SHOP_HEADER* ShopHeader = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_SHOP_HEADER);
		if (EventData->EventShopCount > 0) {
			RTDataEventShopRef EventShopData = &EventData->EventShopList[0];

			ShopHeader->ItemCount = EventShopData->EventShopItemCount;
			for (Int32 ItemIndex = 0; ItemIndex < ShopHeader->ItemCount; ItemIndex += 1) {
				RTDataEventShopItemRef ItemData = &EventShopData->EventShopItemList[ItemIndex];

				S2C_DATA_EVENT_SHOP_ITEM* ShopItem = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_SHOP_ITEM);
				ShopItem->ShopSlotIndex = ItemData->SlotIndex;
				ShopItem->ItemID.Serial = ItemData->ItemID;
				ShopItem->ItemOptions = ItemData->ItemOptions;
				ShopItem->CurrencyPrice = ItemData->ItemPrice;

				if (ItemData->ItemOptionForceCount > 0) {
					RTDataItemOptionForceRef ForceData = &ItemData->ItemOptionForceList[0];
					RTItemOptions Options = { 0 };
					Options.Equipment.SlotCount = ForceData->SlotCount;

					for (Int32 SlotIndex = 0; SlotIndex < ForceData->ItemOptionForceSlotCount; SlotIndex += 1) {
						RTDataItemOptionForceSlotRef SlotData = &ForceData->ItemOptionForceSlotList[SlotIndex];

						if (SlotData->Type == RUNTIME_ITEM_OPTION_TYPE_SLOT ||
							SlotData->Type == RUNTIME_ITEM_OPTION_TYPE_EPIC) {
							Options.Equipment.Slots[SlotData->Slot].ForceLevel = SlotData->Level;
							Options.Equipment.Slots[SlotData->Slot].ForceIndex = SlotData->Option;
						}
						else if (SlotData->Type == RUNTIME_ITEM_OPTION_TYPE_MASTER) {
							Options.Equipment.Slots[SlotData->Slot].MasterIndex = SlotData->Option;
						}

						Options.Equipment.Slots[SlotData->Slot].IsEpic = (
							SlotData->Type == RUNTIME_ITEM_OPTION_TYPE_EPIC ||
							SlotData->Type == RUNTIME_ITEM_OPTION_TYPE_MASTER
						);
					}

					ShopItem->ItemOptions = Options.Serial;
				}

				/*
				ShopItem->ItemPriceCount = 10;
				for (Int32 Index = 0; Index < ShopItem->ItemPriceCount; Index += 1) {
					S2C_DATA_EVENT_SHOP_ITEM_PRICE* ShopItemPrice = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_SHOP_ITEM_PRICE);
					ShopItemPrice->ItemID.ID = 1;
					ShopItemPrice->ItemCount = 1;
				}
				*/
			}
		}

		S2C_DATA_EVENT_UNKNOWN3_HEADER* Unknown3Header = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_UNKNOWN3_HEADER);
		Unknown3Header->Count = 0;

		S2C_DATA_EVENT_BINGO_HEADER* BingoHeader = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_BINGO_HEADER);
		BingoHeader->Count = 0;

		S2C_DATA_EVENT_BINGO_INITIALIZE_HEADER* BingoInitHeader = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_BINGO_INITIALIZE_HEADER);
		BingoInitHeader->Count = 0;

		S2C_DATA_EVENT_PERIOD_HEADER* PeriodHeader = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_PERIOD_HEADER);
		PeriodHeader->Count = 0;

		S2C_DATA_EVENT_DUNGEON_REWARD_HEADER* DungeonRewardHeader = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_DUNGEON_REWARD_HEADER);
		DungeonRewardHeader->Count = 0;
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(EVENT_ACTION) {
	if (!Character) goto error;

	Index PacketLength = sizeof(C2S_DATA_EVENT_ACTION) + sizeof(UInt16) * Packet->InventorySlotCount;
	if (Packet->Length != PacketLength) goto error;

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
	
	S2C_DATA_EVENT_ACTION* Response = PacketBufferInit(Connection->PacketBuffer, S2C, EVENT_ACTION);
	Response->EventIndex = Packet->EventIndex;
	Response->Unknown1 = Packet->Unknown1;
	Response->ItemCount = Packet->InventorySlotCount;

	struct _RTItemSlot ItemSlot = { 0 };
	for (Index Index = 0; Index < Packet->InventorySlotCount; Index += 1) {
		ItemSlot.SlotIndex = Packet->InventorySlotIndex[Index];
		ItemSlot.Item.Serial = Item->ItemID;
		ItemSlot.ItemOptions = Item->ItemOptions;
		// ItemSlot.ItemDuration = Item->ItemDurationID;

		if (!RTInventorySetSlot(Runtime, &Character->InventoryInfo, &ItemSlot)) goto error;

		Character->SyncMask.InventoryInfo = true;
		Character->SyncPriority.Low = true;

		S2C_DATA_EVENT_ACTION_SHOP_ITEM* ResponseItem = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_EVENT_ACTION_SHOP_ITEM);
		ResponseItem->Item = ItemSlot.Item;
		ResponseItem->ItemOptions = ItemSlot.ItemOptions;
		ResponseItem->ItemDuration = ItemSlot.ItemDuration;
		ResponseItem->SlotIndex = ItemSlot.SlotIndex;
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
