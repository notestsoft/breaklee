#include "Character.h"
#include "Runtime.h"
#include "SecretShop.h"

Void RTCharacterInitializeSecretShop(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    RTCharacterUpdateSecretShop(Runtime, Character, false);
}

Void RTCharacterUpdateSecretShop(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Bool ForceUpdate
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    if (Character->Data.SecretShopInfo.ResetTimestamp <= CurrentTimestamp) {
        Character->Data.SecretShopInfo.ResetTimestamp = CurrentTimestamp + Runtime->Config.SecretShopResetInterval;
        Character->Data.SecretShopInfo.RefreshCount = 0;
        Character->Data.SecretShopInfo.RefreshCost = 0;

        for (Int Index = 0; Index < RUNTIME_CHARACTER_MAX_SECRET_SHOP_SLOT_COUNT; Index += 1) {
            Character->Data.SecretShopInfo.Slots[Index].SlotIndex = Index;
            Character->Data.SecretShopInfo.Slots[Index].ShopItemIndex = 0;
        }
    }
}
