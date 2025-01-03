#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTPersonalShopSlot {
	UInt8 ShopSlotIndex;
	UInt16 InventorySlotIndex;
	UInt64 ItemPrice;
	UInt64 ItemID;
	UInt64 ItemOptions;
};

struct _RTPersonalShopInfo {
	UInt16 PremiumInventorySlotIndex;
	UInt8 SlotCount;
	UInt8 NameLength;
};

struct _RTCharacterPersonalShopInfo {
	Bool IsActive;
	UInt64 PremiumShopItemID;
	struct _RTPersonalShopInfo Info;
	struct _RTPersonalShopSlot Slots[RUNTIME_MAX_PERSONAL_SHOP_SLOT_COUNT];
	Char Name[RUNTIME_MAX_PERSONAL_SHOP_NAME_LENGTH + 1];
};

#pragma pack(pop)

Bool RTCharacterPersonalShopOpen(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTPersonalShopInfoRef ShopInfo,
	RTPersonalShopSlotRef ShopSlots,
	Char* ShopName
);

Bool RTCharacterPersonalShopClose(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
);

EXTERN_C_END