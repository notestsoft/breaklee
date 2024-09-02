#include "Character.h"
#include "Recovery.h"

Void RTCharacterAddRecoverySlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTItemSlotRef ItemSlot,
    UInt64 ItemPrice
) {
    Int32 RecoverySlotIndex = Character->Data.RecoveryInfo.Info.SlotCount % RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT;
    Character->Data.RecoveryInfo.Prices[RecoverySlotIndex] = ItemPrice;
    Character->Data.RecoveryInfo.Slots[RecoverySlotIndex] = *ItemSlot;
    Character->Data.RecoveryInfo.Info.SlotCount = MIN(Character->Data.RecoveryInfo.Info.SlotCount + 1, RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT);
    Character->SyncMask.RecoveryInfo = true;
}
