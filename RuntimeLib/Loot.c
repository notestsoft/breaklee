#include "Loot.h"
#include "Character.h"
#include "Runtime.h"

RTLootResult RTCharacterLootItem(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTEntityID ItemEntityID,
    UInt16 UniqueKey,
    UInt16 InventorySlotIndex
) {
    RTLootResult Result = { 0 };
    Result.Result = 0;

    RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (!WorldContext) return Result;

    RTWorldItemRef Item = RTWorldGetItem(Runtime, WorldContext, ItemEntityID, UniqueKey);
    if (!Item) return Result;

    struct _RTItemSlot Slot = { 0 };
    Slot.Item = Item->Item;
    Slot.ItemOptions = Item->ItemOptions;
    Slot.SlotIndex = InventorySlotIndex;
    if (RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &Slot)) {
        RTItemSlotRef InventorySlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndex);
        assert(InventorySlot);
        Result.Result = 96; // TODO: Add message code tables
        Result.ItemID = InventorySlot->Item;
        Result.ItemOptions = InventorySlot->ItemOptions;
        Result.InventorySlotIndex = InventorySlotIndex;
        RTCharacterUpdateQuestItemCounter(Runtime, Character, Slot.Item, Slot.ItemOptions);
    }
    else {
        Result.Result = 99; // TODO: Add message code tables
    }

    Character->SyncMask.Info = true;
    Character->SyncMask.InventoryInfo = true;
    RTWorldDespawnItem(Runtime, WorldContext, Item);

    return Result;
}
