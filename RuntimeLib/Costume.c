#include "Character.h"
#include "Costume.h"
#include "NotificationProtocol.h"
#include "Runtime.h"

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

RTCostumePageRef RTCharacterGetCostumePage(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetIndex
) {
    if (PresetIndex >= RUNTIME_CHARACTER_MAX_COSTUME_PAGE_COUNT) return NULL;

    for (Int Index = 0; Index < Character->Data.CostumeInfo.Info.PageCount; Index += 1) {
        RTCostumePageRef Page = &Character->Data.CostumeInfo.Pages[Index];
        if (Page->PageIndex == PresetIndex) return Page;
    }

    if (Character->Data.CostumeInfo.Info.PageCount >= RUNTIME_CHARACTER_MAX_COSTUME_PAGE_COUNT) return NULL;

    RTCostumePageRef Page = &Character->Data.CostumeInfo.Pages[Character->Data.CostumeInfo.Info.PageCount];
    memset(Page, 0, sizeof(struct _RTCostumePage));
    Page->PageIndex = PresetIndex;
    Character->Data.CostumeInfo.Info.PageCount += 1;
    Character->SyncMask.CostumeInfo = true;
    return Page;
}

Bool RTCharacterSetCostumePreset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetIndex,
    UInt32* CostumeSlots
) {
    RTCostumePageRef CostumePage = RTCharacterGetCostumePage(Runtime, Character, PresetIndex);
    if (!CostumePage) return false;

    for (Int Index = 0; Index < RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT; Index += 1) {
        if (CostumeSlots[Index] < 1) continue;
        if (!RTCharacterIsCostumeUnlocked(Runtime, Character, CostumeSlots[Index])) return false;
    }
    
    UInt16 EquipmentSlots[] = {
        RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY,
        RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_HELMET,
        RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_WEAPON_LEFT,
        RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_WEAPON_RIGHT,
        RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_FORCE_WING,
        RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_VEHICLE,
        RUNTIME_EQUIPMENT_SLOT_INDEX_PET_DISPLAY,
    };

    RTCostumePageRef CurrentCostumePage = RTCharacterGetCostumePage(Runtime, Character, Character->Data.CostumeInfo.Info.ActivePageIndex);
    if (CurrentCostumePage) {
        for (Int Index = 0; Index < RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT; Index += 1) {
            if (CurrentCostumePage && CurrentCostumePage->CostumeSlots[Index] > 0) {
                NOTIFICATION_DATA_CHARACTER_ITEM_UNEQUIP* Notification = RTNotificationInit(CHARACTER_ITEM_UNEQUIP);
                Notification->CharacterIndex = Character->CharacterIndex;
                Notification->EquipmentSlotIndex = EquipmentSlots[Index];
                RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
            }
        }
    }

    memcpy(CostumePage->CostumeSlots, CostumeSlots, sizeof(UInt32) * RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT);
    Character->SyncMask.CostumeInfo = true;
    RTCharacterSetActiveCostumePreset(Runtime, Character, PresetIndex);

    for (Int Index = 0; Index < RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT; Index += 1) {
        if (CostumePage->CostumeSlots[Index] > 0) {
            NOTIFICATION_DATA_CHARACTER_ITEM_EQUIP* Notification = RTNotificationInit(CHARACTER_ITEM_EQUIP);
            Notification->CharacterIndex = Character->CharacterIndex;
            Notification->Item.Serial = CostumePage->CostumeSlots[Index];
            Notification->EquipmentSlotIndex = EquipmentSlots[Index];
            RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
        }
    }

    return true;
}

Bool RTCharacterSetActiveCostumePreset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PresetIndex
) {
    RTCostumePageRef CostumePage = RTCharacterGetCostumePage(Runtime, Character, PresetIndex);
    if (!CostumePage) return false;

    Character->Data.CostumeInfo.Info.ActivePageIndex = PresetIndex;
    Character->Data.CostumeInfo.Info.AppliedSlotCount = RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT;
    memcpy(Character->Data.CostumeInfo.AppliedSlots, CostumePage->CostumeSlots, sizeof(struct _RTAppliedCostumeSlot) * RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT);
    Character->SyncMask.CostumeInfo = true;
    return true;
}
