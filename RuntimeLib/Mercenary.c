#include "Character.h"
#include "Inventory.h"
#include "Mercenary.h"
#include "Runtime.h"

Bool RTCharacterRegisterMercenaryCard(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt16 InventorySlotIndex
) {
    RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
    if (!ItemSlot) return false;

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
    if (!ItemData) return false;
    if (ItemData->ItemType != RUNTIME_ITEM_TYPE_MERCENARY_CARD) return false;

    return false;
}