#include "Character.h"
#include "GiftBox.h"
#include "Runtime.h"

Bool RTGiftBoxIsActive(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int Index
) {
    RTDataGiftBoxConditionRef Condition = RTRuntimeDataGiftBoxConditionGet(Runtime->Context, Index);
    if (!Condition) return false;
    if (!Condition->IsEnabled) return false;

    if (Condition->IsPremiumService > 0) {
        if (Character->Data.PremiumServiceInfo.Info.SlotCount < 1) return false;
    }

    if (Condition->IsPremiumPC > 0) {
        return false;
    }

    if (Condition->BlessingBeadIndex > 0) {
        for (Int SlotIndex = 0; SlotIndex < Character->Data.BlessingBeadInfo.Info.SlotCount; SlotIndex += 1) {
            if (Character->Data.BlessingBeadInfo.Slots[SlotIndex].Index != Condition->BlessingBeadIndex) continue;

            return true;
        }

        return false;
    }

    return true;
}

RTGiftBoxSlotRef RTCharacterGetGiftBox(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int Index
) {
    for (Int SlotIndex = 0; SlotIndex < Character->Data.GiftboxInfo.Info.SlotCount; SlotIndex += 1) {
        RTGiftBoxSlotRef GiftBoxSlot = &Character->Data.GiftboxInfo.Slots[SlotIndex];
        if (GiftBoxSlot->Index == Index) return GiftBoxSlot;
    }

    return NULL;
}

Void RTCharacterUpdateGiftBox(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    Int32 Seed = (Int32)PlatformGetTickCount();
    Timestamp CurrentTimestamp = GetTimestamp();

    for (Int Index = 0; Index < RUNTIME_CHARACTER_MAX_GIFT_BOX_SLOT_COUNT; Index += 1) {
        RTGiftBoxSlotRef GiftBoxSlot = RTCharacterGetGiftBox(Runtime, Character, Index);
        if (!GiftBoxSlot) {
            GiftBoxSlot = &Character->Data.GiftboxInfo.Slots[Character->Data.GiftboxInfo.Info.SlotCount];
            memset(GiftBoxSlot, 0, sizeof(struct _RTGiftBoxSlot));
            GiftBoxSlot->Index = Index;
            Character->Data.GiftboxInfo.Info.SlotCount += 1;
            Character->SyncMask.GiftboxInfo = true;
        }

        Bool IsActive = RTGiftBoxIsActive(Runtime, Character, Index);
        Timestamp CooldownTime = GiftBoxSlot->CooldownTime;
        if (CooldownTime < 1) {
            RTDataGiftBoxTimePoolRef TimePool = RTRuntimeDataGiftBoxTimePoolGet(Runtime->Context, GiftBoxSlot->Index);
            if (TimePool) {
                Int32 RandomRate = RandomRange(&Seed, 0, 10000);
                Int32 RandomRateOffset = 0;

                for (Int TimeIndex = 0; TimeIndex < TimePool->GiftBoxTimePoolValueCount; TimeIndex += 1) {
                    RTDataGiftBoxTimePoolValueRef Value = &TimePool->GiftBoxTimePoolValueList[TimeIndex];
                    if (RandomRate <= Value->Rate + RandomRateOffset) {
                        CooldownTime = Value->RequiredTime;
                        break;
                    }

                    RandomRateOffset += Value->Rate;
                }
            }
        }

        IsActive = IsActive && CooldownTime > 0;
        if (Character->GiftBoxUpdateTimestamps[Index] < 1) {
            Character->GiftBoxUpdateTimestamps[Index] = CurrentTimestamp;
        }

        GiftBoxSlot->CooldownTime = CooldownTime;
        GiftBoxSlot->Active = IsActive;
        
        if (GiftBoxSlot->Active) {
            Timestamp ElapsedTime = CurrentTimestamp - Character->GiftBoxUpdateTimestamps[Index];
            GiftBoxSlot->ElapsedTime = MIN(GiftBoxSlot->CooldownTime, GiftBoxSlot->ElapsedTime + ElapsedTime);
            Character->GiftBoxUpdateTimestamps[Index] = CurrentTimestamp;
        }

        Character->SyncMask.GiftboxInfo = true;
    }
}

Bool RTCharacterRollGiftBox(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int Index
) {
    if (Index < 0 || Index >= Character->Data.GiftboxInfo.Info.SlotCount) return false;

    RTGiftBoxRewardSlotRef RewardSlot = &Character->Data.GiftboxInfo.RewardSlots[Index];
    if (RewardSlot->IsRolled) return true;

    RTCharacterUpdateGiftBox(Runtime, Character);

    RTGiftBoxSlotRef GiftBoxSlot = RTCharacterGetGiftBox(Runtime, Character, Index);
    if (!GiftBoxSlot) return false;

    Int32 Seed = (Int32)PlatformGetTickCount();
    if (GiftBoxSlot->ElapsedTime >= GiftBoxSlot->CooldownTime && GiftBoxSlot->CooldownTime > 0) {
        RTDataGiftBoxRewardPoolRef RewardPool = RTRuntimeDataGiftBoxRewardPoolGet(Runtime->Context, Index);
        if (!RewardPool) return false;

        Int32 RandomRate = RandomRange(&Seed, 0, 1000000);
        Int32 RandomRateOffset = 0;

        for (Int RewardIndex = 0; RewardIndex < RewardPool->GiftBoxRewardPoolItemCount; RewardIndex += 1) {
            RTDataGiftBoxRewardPoolItemRef RewardItem = &RewardPool->GiftBoxRewardPoolItemList[RewardIndex];
            if (RandomRate <= RewardItem->Rate + RandomRateOffset) {
                RewardSlot->IsRolled = true;
                RewardSlot->ItemID.Serial = RewardItem->ItemID;
                RewardSlot->ItemOptions.Serial = RewardItem->ItemOption;
                Character->SyncMask.GiftboxInfo = true;
                break;
            }

            RandomRateOffset += RewardItem->Rate;
        }
    }

    return RewardSlot->IsRolled;
}

// TODO: Check for stack size
Bool RTCharacterReceiveGiftBox(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int Index,
    Int32 InventorySlotIndex,
    Int32 StackSize
) {
    if (Index < 0 || Index >= Character->Data.GiftboxInfo.Info.SlotCount) return false;

    RTGiftBoxRewardSlotRef RewardSlot = &Character->Data.GiftboxInfo.RewardSlots[Index];
    if (!RewardSlot->IsRolled) return false;

    struct _RTItemSlot ItemSlot = { 0 };
    ItemSlot.Item = RewardSlot->ItemID;
    ItemSlot.ItemOptions = RewardSlot->ItemOptions.Serial;
    ItemSlot.SlotIndex = InventorySlotIndex;
    if (!RTInventorySetSlot(Runtime, &Character->Data.InventoryInfo, &ItemSlot)) return false;

    memset(RewardSlot, 0, sizeof(struct _RTGiftBoxRewardSlot));

    RTGiftBoxSlotRef GiftBoxSlot = RTCharacterGetGiftBox(Runtime, Character, Index);
    if (GiftBoxSlot) {
        GiftBoxSlot->ElapsedTime = 0;
        GiftBoxSlot->CooldownTime = 0;
        GiftBoxSlot->ReceivedCount += 1;
        RTCharacterUpdateGiftBox(Runtime, Character);
    }

    // TODO: Add Special Giftbox Point Reward

    Character->SyncMask.GiftboxInfo = true;
    return true;
}
