#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTSecretShopSlot {
    Int16 SlotIndex;
    Int32 ShopItemIndex;
};

struct _RTCharacterSecretShopData {
    UInt8 RefreshCost;
    Timestamp ResetTimestamp;
    UInt8 RefreshCount;
    struct _RTSecretShopSlot Slots[RUNTIME_CHARACTER_MAX_SECRET_SHOP_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END