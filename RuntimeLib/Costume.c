#include "Character.h"
#include "Costume.h"

Bool RTCharacterIsCostumeUnlocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 CostumeItemID
) {
    for (Int Index = 0; Index < Character->Data.CostumeWarehouseInfo.Info.SlotCount; Index += 1) {
        RTAccountCostumeSlotRef CostumeSlot = &Character->Data.CostumeWarehouseInfo.Slots[Index];
        if (CostumeSlot->ItemID == CostumeItemID) return true;
    }

    return false;
}

Bool RTCharacterUnlockCostume(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 CostumeItemID
) {
    if (Character->Data.CostumeWarehouseInfo.Info.SlotCount >= RUNTIME_CHARACTER_MAX_ACCOUNT_COSTUME_SLOT_COUNT) return false;

    RTAccountCostumeSlotRef CostumeSlot = &Character->Data.CostumeWarehouseInfo.Slots[Character->Data.CostumeWarehouseInfo.Info.SlotCount];
    CostumeSlot->ItemID = CostumeItemID;
    CostumeSlot->Unknown1 = 0;
    Character->Data.CostumeWarehouseInfo.Info.SlotCount += 1;
    Character->SyncMask.CostumeWarehouseInfo = true;
    return true;
}