#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(EXTRACT_ITEM) {
	if (!Character) {
		SocketDisconnect(Socket, Connection);
		return;
	}

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_EXTRACT_ITEM* Response = PacketBufferInit(PacketBuffer, S2C, EXTRACT_ITEM);
	Response->ItemCount = 0;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
	if (!ItemSlot) goto error;

	RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
	if (!ItemData) goto error;

	Int32 Seed = (Int32)PlatformGetTickCount();

	for (Int DestroyItemIndex = 0; DestroyItemIndex < Runtime->Context->DestroyItemCount; DestroyItemIndex += 1) {
		RTDataDestroyItemRef ItemPattern = &Runtime->Context->DestroyItemList[DestroyItemIndex];
		if (ItemPattern->ItemIndex != (ItemSlot->Item.ID & RUNTIME_ITEM_MASK_INDEX)) continue;
		if (ItemPattern->MinEnchantLevel > ItemSlot->Item.UpgradeLevel) continue;
		if (ItemPattern->MaxEnchantLevel < ItemSlot->Item.UpgradeLevel) continue;

		RTItemOptions ItemOptions = { 0 };
		ItemOptions.Serial = ItemSlot->ItemOptions;

		if (ItemPattern->Slot == 0 && ItemOptions.Equipment.SlotCount > 0) continue;
		if (ItemPattern->Slot == 1 && ItemOptions.Equipment.SlotCount < 1) continue;
		if (ItemPattern->Epic != -1) {
			Int32 EpicSlotIndex = -1;
			for (Int SlotIndex = 0; SlotIndex < ItemOptions.Equipment.SlotCount; SlotIndex += 1) {
				if (!ItemOptions.Equipment.Slots[SlotIndex].IsEpic) continue;

				EpicSlotIndex = SlotIndex;
				break;
			}

			if (ItemPattern->Epic == 0 && EpicSlotIndex >= 0) continue;
			if (ItemPattern->Epic == 1) {
				if (EpicSlotIndex < 0) continue;
				
				Bool Found = false;
				for (Int EpicLevelIndex = 0; EpicLevelIndex < ItemPattern->EpicLevelCount; EpicLevelIndex += 1) {
					if (ItemPattern->EpicLevel[EpicLevelIndex] == -1 ||
						ItemPattern->EpicLevel[EpicLevelIndex] == ItemOptions.Equipment.Slots[EpicSlotIndex].ForceLevel) {
						Found = true;
						break;
					}
				}
				if (!Found) continue;

				Found = false;
				for (Int EpicOptionIndex = 0; EpicOptionIndex < ItemPattern->EpicOptionCount; EpicOptionIndex += 1) {
					if (ItemPattern->EpicOption[EpicOptionIndex] == -1 ||
						ItemPattern->EpicOption[EpicOptionIndex] == ItemOptions.Equipment.Slots[EpicSlotIndex].ForceIndex) {
						Found = true;
						break;
					}
				}
				if (!Found) continue;
			}
		}

		if (ItemPattern->AccountBind == 0 && ItemSlot->Item.IsAccountBinding) continue;
		if (ItemPattern->AccountBind > 0 && !ItemSlot->Item.IsAccountBinding) continue;
		if (ItemPattern->CharacterBind == 0 && ItemSlot->Item.IsCharacterBinding) continue;
		if (ItemPattern->CharacterBind > 0 && !ItemSlot->Item.IsCharacterBinding) continue;
		
		// TODO: Add support for alert

		RTDataDestroyItemPoolRef ItemPool = RTRuntimeDataDestroyItemPoolGet(Runtime->Context, ItemPattern->PoolID);
		if (!ItemPool) continue;

		for (Int ItemResultIndex = 0; ItemResultIndex < ItemPool->DestroyItemPoolResultCount; ItemResultIndex += 1) {
			RTDataDestroyItemPoolResultRef ItemResult = &ItemPool->DestroyItemPoolResultList[ItemResultIndex];
			Int32 ItemCount = RandomRange(&Seed, ItemResult->MinCount, ItemResult->MaxCount);
			if (ItemCount < 1) continue;

			RTItemDataRef ItemResultData = RTRuntimeGetItemDataByIndex(Runtime, ItemResult->ItemKind);
			if (!ItemResultData) continue;

			UInt64 ItemOptions = 0;
			RTDataDestroyItemOptionPoolRef ItemOptionPool = RTRuntimeDataDestroyItemOptionPoolGet(Runtime->Context, ItemResult->ItemKind);
			if (ItemOptionPool) {
				Int32 RateLimit = 1000000;
				Int32 Rate = RandomRange(&Seed, 0, RateLimit);
				Int32 RateOffset = 0;

				for (Int OptionIndex = 0; OptionIndex < ItemOptionPool->DestroyItemOptionPoolOptionCount; OptionIndex += 1) {
					if (Rate <= ItemOptionPool->DestroyItemOptionPoolOptionList[OptionIndex].Rate + RateOffset) {
						ItemOptions = ItemOptionPool->DestroyItemOptionPoolOptionList[OptionIndex].Option;
						break;
					}

					RateOffset += ItemOptionPool->DestroyItemOptionPoolOptionList[OptionIndex].Rate;
				}
			}

			if (ItemResultData->MaxStackSize > 0) {
				struct _RTItemSlot Slot = { 0 };
				Slot.Item.ID = ItemResult->ItemKind;
				Slot.ItemOptions = ItemOptions ? ItemOptions : ItemCount;
				Slot.SlotIndex = Character->Data.TemporaryInventoryInfo.Info.SlotCount;

				if (RTInventorySetSlot(Runtime, &Character->Data.TemporaryInventoryInfo, &Slot)) {
					S2C_EXTRACT_ITEM_SLOT_INDEX* ResponseItemSlot = PacketBufferAppendStruct(PacketBuffer, S2C_EXTRACT_ITEM_SLOT_INDEX);
					ResponseItemSlot->ItemID = Slot.Item.Serial;
					ResponseItemSlot->ItemOption = Slot.ItemOptions;
					ResponseItemSlot->InventorySlotIndex = RUNTIME_INVENTORY_TOTAL_SIZE + Slot.SlotIndex;
					Response->ItemCount += 1;
				}
			}
			else {
				for (Int Index = 0; Index < ItemCount; Index += 1) {
					struct _RTItemSlot Slot = { 0 };
					Slot.Item.ID = ItemResult->ItemKind;
					Slot.ItemOptions = ItemOptions;
					Slot.SlotIndex = Character->Data.TemporaryInventoryInfo.Info.SlotCount;

					if (RTInventorySetSlot(Runtime, &Character->Data.TemporaryInventoryInfo, &Slot)) {
						S2C_EXTRACT_ITEM_SLOT_INDEX* ResponseItemSlot = PacketBufferAppendStruct(PacketBuffer, S2C_EXTRACT_ITEM_SLOT_INDEX);
						ResponseItemSlot->ItemID = Slot.Item.Serial;
						ResponseItemSlot->ItemOption = Slot.ItemOptions;
						ResponseItemSlot->InventorySlotIndex = RUNTIME_INVENTORY_TOTAL_SIZE + Slot.SlotIndex;
						Response->ItemCount += 1;
					}
				}
			}
		}
	}

	if (Response->ItemCount > 0) {
		Response->Result = 1;
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, Packet->InventorySlotIndex);
		Character->SyncMask.InventoryInfo = true;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	Response->Result = 0;
	Response->ItemCount = 0;
	SocketSend(Socket, Connection, Response);
}
