#include "Character.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "PersonalShop.h"
#include "Runtime.h"

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
