#include "Cooldown.h"
#include "Character.h"
#include "Runtime.h"

Int RTCharacterGetCooldownSlotIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int CooldownIndex
) {
    for (Int SlotIndex = 0; SlotIndex < Character->Data.CooldownInfo.Info.SlotCount; SlotIndex += 1) {
        RTCooldownSlotRef CooldownSlot = &Character->Data.CooldownInfo.Slots[SlotIndex];
        if (CooldownSlot->CooldownIndex == CooldownIndex) return SlotIndex;
    }

    return -1;
}

RTCooldownSlotRef RTCharacterAddCooldownSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int CooldownIndex
) {
    RTCooldownSlotRef CooldownSlot = NULL;
    Int SlotIndex = RTCharacterGetCooldownSlotIndex(Runtime, Character, CooldownIndex);
    if (SlotIndex >= 0) {
        CooldownSlot = &Character->Data.CooldownInfo.Slots[SlotIndex];
    }
    else {
        assert(Character->Data.CooldownInfo.Info.SlotCount < RUNTIME_CHARACTER_MAX_COOLDOWN_SLOT_COUNT);
        CooldownSlot = &Character->Data.CooldownInfo.Slots[Character->Data.CooldownInfo.Info.SlotCount];
    }

    memset(CooldownSlot, 0, sizeof(struct _RTCooldownSlot));
    Character->Data.CooldownInfo.Info.SlotCount += 1;
    Character->SyncMask.CooldownInfo = true;
    return CooldownSlot;
}

Void RTCharacterRemoveCooldownSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int SlotIndex
) {
    Int32 TailLength = Character->Data.CooldownInfo.Info.SlotCount - SlotIndex - 1;
    if (TailLength > 0) {
        memmove(
            &Character->Data.CooldownInfo.Slots[SlotIndex],
            &Character->Data.CooldownInfo.Slots[SlotIndex + 1],
            sizeof(struct _RTCooldownSlot) * TailLength
        );
    }

    Character->Data.CooldownInfo.Info.SlotCount -= 1;
    Character->SyncMask.CooldownInfo = true;
}

Void RTCharacterInitializeCooldowns(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    RTTimerInitialize(&Character->CooldownTimer, 0);

    Timestamp CurrentTimestamp = GetTimestampMs();
    for (Int SlotIndex = 0; SlotIndex < Character->Data.CooldownInfo.Info.SlotCount; SlotIndex += 1) {
        RTCooldownSlotRef Cooldown = &Character->Data.CooldownInfo.Slots[SlotIndex];
        RTTimerAddUpdateTimestamp(&Character->CooldownTimer, CurrentTimestamp + Cooldown->Interval);
    }

    if (Character->Data.CooldownInfo.Info.SpiritRaiseCooldown > 0) {
        RTTimerAddUpdateTimestamp(&Character->CooldownTimer, CurrentTimestamp + Character->Data.CooldownInfo.Info.SpiritRaiseCooldown);
    }
}

Void RTCharacterUpdateCooldowns(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Bool ForceUpdate
) {
    if (!RTTimerUpdate(&Character->CooldownTimer, ForceUpdate)) return;

    Timestamp CurrentTimestamp = GetTimestampMs();
    Timestamp NextCooldownTimestamp = UINT64_MAX;

    for (Int SlotIndex = 0; SlotIndex < Character->Data.CooldownInfo.Info.SlotCount; SlotIndex += 1) {
        RTCooldownSlotRef CooldownSlot = &Character->Data.CooldownInfo.Slots[SlotIndex];
        if (CooldownSlot->Interval >= Character->CooldownTimer.ElapsedInterval) {
            CooldownSlot->Interval -= Character->CooldownTimer.ElapsedInterval;
            NextCooldownTimestamp = MIN(NextCooldownTimestamp, CurrentTimestamp + CooldownSlot->Interval);
        }
        else {
            CooldownSlot->Interval = 0;
        }
    }

    for (Int SlotIndex = Character->Data.CooldownInfo.Info.SlotCount - 1; SlotIndex >= 0; SlotIndex -= 1) {
        RTCooldownSlotRef CooldownSlot = &Character->Data.CooldownInfo.Slots[SlotIndex];
        if (CooldownSlot->Interval > 0) continue;
        
        RTCharacterRemoveCooldownSlot(Runtime, Character, SlotIndex);
    }

    if (Character->Data.CooldownInfo.Info.SpiritRaiseCooldown > 0) {
        Character->Data.CooldownInfo.Info.SpiritRaiseCooldown = MAX(
            0,
            Character->Data.CooldownInfo.Info.SpiritRaiseCooldown - Character->CooldownTimer.ElapsedInterval
        );

        NextCooldownTimestamp = MIN(
            NextCooldownTimestamp, 
            CurrentTimestamp + Character->Data.CooldownInfo.Info.SpiritRaiseCooldown
        );
    }

    RTTimerInitialize(&Character->CooldownTimer, 0);
    RTTimerAddUpdateTimestamp(&Character->CooldownTimer, NextCooldownTimestamp);
    Character->SyncMask.CooldownInfo = true;
}

Bool RTCharacterHasCooldown(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int CooldownIndex
) {
    return RTCharacterGetCooldownInterval(Runtime, Character, CooldownIndex) > 0;
}

Timestamp RTCharacterGetCooldownInterval(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int CooldownIndex
) {
    if (CooldownIndex < 1) return 0;

    RTCharacterUpdateCooldowns(Runtime, Character, true);

    Int SlotIndex = RTCharacterGetCooldownSlotIndex(Runtime, Character, CooldownIndex);
    if (SlotIndex >= 0) {
        RTCooldownSlotRef CooldownSlot = &Character->Data.CooldownInfo.Slots[SlotIndex];
        return CooldownSlot->Interval;
    }

    return 0;
}

Void RTCharacterSetCooldown(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int CooldownIndex,
    Timestamp CooldownInterval,
    Bool IsNationWar
) {
    if (CooldownIndex < 1) return;

    RTDataCooltimeRef CooldownData = RTRuntimeDataCooltimeGet(Runtime->Context, CooldownIndex);
    if (!CooldownData) {
        Warn("Cooldown not found for index: %d", (Int32)CooldownIndex);
        return;
    }

    if (CooldownData->CheckSave < 1) return;

    Timestamp Interval = (IsNationWar) ? CooldownData->WarInterval : CooldownData->Interval;
    if (CooldownInterval > 0) Interval = CooldownInterval;

    RTCooldownSlotRef CooldownSlot = RTCharacterAddCooldownSlot(Runtime, Character, CooldownIndex);
    CooldownSlot->CooldownIndex = CooldownData->Index;
    CooldownSlot->Interval = Interval;

    for (Int Index = 0; Index < CooldownData->LinkListCount; Index += 1) {
        if (CooldownData->LinkList[Index] < 1) continue;

        CooldownSlot = RTCharacterAddCooldownSlot(Runtime, Character, CooldownData->LinkList[Index]);
        CooldownSlot->CooldownIndex = CooldownData->LinkList[Index];
        CooldownSlot->Interval = Interval;
    }

    Timestamp CurrentTimestamp = GetTimestampMs();
    RTTimerAddUpdateTimestamp(&Character->CooldownTimer, CurrentTimestamp + Interval);
}

Bool RTCharacterHasSpiritRaiseCooldown(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    return Character->Data.CooldownInfo.Info.SpiritRaiseCooldown > 0;
}

Void RTCharacterSetSpiritRaiseCooldown(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CooldownInterval
) {
    Character->Data.CooldownInfo.Info.SpiritRaiseCooldown = CooldownInterval;
    Character->SyncMask.CooldownInfo = true;
}
