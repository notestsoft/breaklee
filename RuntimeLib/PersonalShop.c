#include "Character.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "PersonalShop.h"
#include "Runtime.h"
#include "WorldManager.h"

RTPersonalShopSlotRef RTCharacterPersonalShopGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt8 SlotIndex
) {
	if (!Character->Data.PersonalShopInfo.IsActive) return NULL;

	for (Int Index = 0; Index < Character->Data.PersonalShopInfo.Info.SlotCount; Index += 1) {
		RTPersonalShopSlotRef ShopSlot = &Character->Data.PersonalShopInfo.Slots[Index];
		if (ShopSlot->ShopSlotIndex != SlotIndex) continue;
		
		return ShopSlot;
	}

	return NULL;
}

Bool RTCharacterPersonalShopOpen(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTPersonalShopInfoRef ShopInfo,
	RTPersonalShopSlotRef ShopSlots,
	Char* ShopName
) {
	assert(ShopInfo && ShopSlots && ShopName);

	if (Character->Data.PersonalShopInfo.IsActive) return false;
	if (ShopInfo->SlotCount > RUNTIME_MAX_PERSONAL_SHOP_SLOT_COUNT) return false;
	if (ShopInfo->NameLength < 1) return false;
	if (ShopInfo->NameLength > RUNTIME_MAX_PERSONAL_SHOP_NAME_LENGTH) return false;

	if (ShopInfo->PremiumInventorySlotIndex) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, ShopInfo->PremiumInventorySlotIndex);
		if (!ItemSlot) return false;

		RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
		if (!ItemData) return false;
		if (ItemData->ItemType != RUNTIME_ITEM_TYPE_DECORATE_SHOP) return false;

		Character->Data.PersonalShopInfo.PremiumShopItemID = ItemSlot->Item.ID;
	}
	
	Character->Data.PersonalShopInfo.IsActive = true;
	Character->Data.PersonalShopInfo.Info = *ShopInfo;
	// TODO: Verify shop slots!!!
	memcpy(Character->Data.PersonalShopInfo.Slots, ShopSlots, sizeof(struct _RTPersonalShopSlot) * ShopInfo->SlotCount);
	memcpy(Character->Data.PersonalShopInfo.Name, ShopName, sizeof(Char) * ShopInfo->NameLength);
	Character->Data.PersonalShopInfo.Name[ShopInfo->NameLength] = '\0';

	NOTIFICATION_DATA_PERSONAL_SHOP_OPEN* Notification = RTNotificationInit(PERSONAL_SHOP_OPEN);
	Notification->CharacterIndex = Character->CharacterIndex;
	Notification->ShopMode = NOTIFICATION_PERSONAL_SHOP_OPEN_MODE_SELL;
	Notification->PremiumShopItemID = Character->Data.PersonalShopInfo.PremiumShopItemID;
	Notification->ShopNameLength = Character->Data.PersonalShopInfo.Info.NameLength;
	RTNotificationAppendCopy(Notification, Character->Data.PersonalShopInfo.Name, Character->Data.PersonalShopInfo.Info.NameLength);
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

	return true;
}

Bool RTCharacterPersonalShopClose(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (!Character->Data.PersonalShopInfo.IsActive) return false;

	Character->Data.PersonalShopInfo.IsActive = false;

	NOTIFICATION_DATA_PERSONAL_SHOP_CLOSE* Notification = RTNotificationInit(PERSONAL_SHOP_CLOSE);
	Notification->CharacterIndex = Character->CharacterIndex;
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

	return true;
}

Bool RTCharacterPersonalShopBuy(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTEntityID ShopCharacterID,
	UInt64 ItemID,
	UInt16 InventorySlotIndex,
	UInt8 ShopSlotIndex,
	UInt64 ItemPrice
) {
	if (Character->Data.Info.Alz < ItemPrice) return false;
	if (!RTInventoryIsSlotEmpty(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex)) return false;

	RTCharacterRef ShopCharacter = RTWorldManagerGetCharacter(Runtime->WorldManager, ShopCharacterID);
	if (!ShopCharacter) return false;
	if (ShopCharacter->CharacterIndex == Character->CharacterIndex) return false;
	if (!ShopCharacter->Data.PersonalShopInfo.IsActive) return false;

	for (Int Index = 0; Index < ShopCharacter->Data.PersonalShopInfo.Info.SlotCount; Index += 1) {
		RTPersonalShopSlotRef ShopSlot = &ShopCharacter->Data.PersonalShopInfo.Slots[Index];
		if (ShopSlot->ShopSlotIndex != ShopSlotIndex) continue;
		if (ShopSlot->ItemID != ItemID) return false;
		if (ShopSlot->ItemPrice != ItemPrice) return false;

		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &ShopCharacter->Data.InventoryInfo, ShopSlot->InventorySlotIndex);
		if (!ItemSlot) return false;
		if (ItemSlot->Item.Serial != ItemID) return false;
		if (ItemSlot->ItemOptions != ShopSlot->ItemOptions) return false;

		if (ShopCharacter->Data.Info.Alz + ItemPrice > Runtime->Config.MaxInventoryCurrency) {
			NOTIFICATION_DATA_PERSONAL_SHOP_BUY* Notification = RTNotificationInit(PERSONAL_SHOP_BUY);
			Notification->CharacterIndex = ShopCharacter->CharacterIndex;
			Notification->ShopSlotIndex = ShopSlotIndex;
			Notification->Result = 0;
			RTNotificationDispatchToNearby(Notification, ShopCharacter->Movement.WorldChunk);
			return false;
		}

		struct _RTItemSlot TempItemSlot = { 0 };
		if (!RTInventoryRemoveSlot(Runtime, &ShopCharacter->Data.InventoryInfo, ShopSlot->InventorySlotIndex, &TempItemSlot)) return false;
		ShopCharacter->SyncMask.InventoryInfo = true;
		ShopCharacter->Data.Info.Alz += ItemPrice;
		ShopCharacter->SyncMask.Info = true;

		TempItemSlot.SlotIndex = InventorySlotIndex;
		RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &TempItemSlot);
		Character->SyncMask.InventoryInfo = true;
		Character->Data.Info.Alz -= ItemPrice;
		Character->SyncMask.Info = true;

		Int TailLength = ShopCharacter->Data.PersonalShopInfo.Info.SlotCount - Index - 1;
		if (TailLength > 0) {
			memmove(
				&ShopCharacter->Data.PersonalShopInfo.Slots[Index],
				&ShopCharacter->Data.PersonalShopInfo.Slots[Index + 1],
				sizeof(struct _RTPersonalShopSlot) * TailLength
			);
		}
		ShopCharacter->Data.PersonalShopInfo.Info.SlotCount -= 1;

		NOTIFICATION_DATA_PERSONAL_SHOP_BUY* Notification = RTNotificationInit(PERSONAL_SHOP_BUY);
		Notification->CharacterIndex = ShopCharacter->CharacterIndex;
		Notification->ShopSlotIndex = ShopSlotIndex;
		Notification->Result = 1;
		RTNotificationDispatchToNearby(Notification, ShopCharacter->Movement.WorldChunk);

		return true;
	}

	return false;
}

Bool RTCharacterPersonalShopRegisterItem(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt16 InventorySlotIndex,
	UInt64 ItemID,
	UInt64 ItemOptions,
	UInt8 ShopSlotIndex,
	UInt64 ItemPrice
) {
	if (!Character->Data.PersonalShopInfo.IsActive) return false;
	if (Character->Data.PersonalShopInfo.Info.SlotCount >= RUNTIME_MAX_PERSONAL_SHOP_SLOT_COUNT) return false;
	if (ShopSlotIndex >= RUNTIME_MAX_PERSONAL_SHOP_SLOT_COUNT) return false;

	RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
	if (!ItemSlot) return false;
	if (ItemSlot->Item.IsCharacterBinding || ItemSlot->Item.IsAccountBinding || ItemSlot->Item.IsProtected) return false;
	if (ItemSlot->Item.Serial != ItemID || ItemSlot->ItemOptions != ItemOptions) return false;

	RTPersonalShopSlotRef ShopSlot = RTCharacterPersonalShopGetSlot(Runtime, Character, ShopSlotIndex);
	if (ShopSlot) return false;

	ShopSlot = &Character->Data.PersonalShopInfo.Slots[Character->Data.PersonalShopInfo.Info.SlotCount];
	ShopSlot->ShopSlotIndex = ShopSlotIndex;
	ShopSlot->InventorySlotIndex = InventorySlotIndex;
	ShopSlot->ItemPrice = ItemPrice;
	ShopSlot->ItemID = ItemID;
	ShopSlot->ItemOptions = ItemOptions;
	Character->Data.PersonalShopInfo.Info.SlotCount += 1;

	NOTIFICATION_DATA_PERSONAL_SHOP_REGISTER_ITEM* Notification = RTNotificationInit(PERSONAL_SHOP_REGISTER_ITEM);
	Notification->CharacterIndex = Character->CharacterIndex;
	Notification->ShopSlotIndex = ShopSlotIndex;
	Notification->ItemPrice = ItemPrice;
	Notification->ItemID = ItemID;
	Notification->ItemOptions = ItemOptions;
	Notification->ItemDuration = ItemSlot->ItemDuration.Serial;
	Notification->ItemSerial = ItemSlot->ItemSerial;
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

	return true;
}