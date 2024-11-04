#include "Buff.h"
#include "Character.h"
#include "Force.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "Runtime.h"

Int32 RTCharacterGetBuffSlotIndexBuffType(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
) {
    UInt8* BuffSlotCounts[] = {
         &Character->Data.BuffInfo.Info.SkillBuffCount,
         &Character->Data.BuffInfo.Info.PotionBuffCount,
         &Character->Data.BuffInfo.Info.GmBuffCount,
         &Character->Data.BuffInfo.Info.UnknownBuffCount1,
         &Character->Data.BuffInfo.Info.UnknownBuffCount2,
         &Character->Data.BuffInfo.Info.ForceWingBuffCount,
         &Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };

    Int32 BuffSlotCount = 0;

    for (Int32 Index = 0; Index < 7; Index += 1) {
        BuffSlotCount += *BuffSlotCounts[Index];
        if (SlotIndex < BuffSlotCount) {
            return Index;
        }
    }

    return -1;
}

RTBuffSlotRef RTCharacterGetBuffSlotBySkillIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillIndex,
    Int32* ResultSlotIndex
) {
    for (Int32 Index = 0; Index < Character->Data.BuffInfo.Info.SkillBuffCount; Index += 1) {
        RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];
        if (BuffSlot->SkillIndex == SkillIndex) {
            *ResultSlotIndex = Index;
            return BuffSlot;
        }
    }

    return NULL;
}

Int32 RTCharacterGetGroupedBuffSlotIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
) {
    UInt8* BuffSlotCounts[] = {
        &Character->Data.BuffInfo.Info.SkillBuffCount,
        &Character->Data.BuffInfo.Info.PotionBuffCount,
        &Character->Data.BuffInfo.Info.GmBuffCount,
        &Character->Data.BuffInfo.Info.UnknownBuffCount1,
        &Character->Data.BuffInfo.Info.UnknownBuffCount2,
        &Character->Data.BuffInfo.Info.ForceWingBuffCount,
        &Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };

    Int32 BuffSlotCount = 0;
    Int32 BuffSlotOffset = 0;

    for (Int32 Index = 0; Index < 7; Index += 1) {
        BuffSlotCount += *BuffSlotCounts[Index];
        if (SlotIndex < BuffSlotCount) {
            return SlotIndex + *BuffSlotCounts[Index] - BuffSlotCount;
        }
    }

    return 0;
}

RTBuffSlotRef RTCharacterInsertBuffSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 BuffSlotType
) {
    UInt8* BuffSlotCounts[] = {
        &Character->Data.BuffInfo.Info.SkillBuffCount,
        &Character->Data.BuffInfo.Info.PotionBuffCount,
        &Character->Data.BuffInfo.Info.GmBuffCount,
        &Character->Data.BuffInfo.Info.UnknownBuffCount1,
        &Character->Data.BuffInfo.Info.UnknownBuffCount2,
        &Character->Data.BuffInfo.Info.ForceWingBuffCount,
        &Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };

    Int32 BuffSlotCount = 0;
    Int32 BuffSlotOffset = 0;

    for (Int32 Index = 0; Index < 7; Index += 1) {
        BuffSlotCount += *BuffSlotCounts[Index];
        BuffSlotOffset += (BuffSlotType >= Index) ? *BuffSlotCounts[Index] : 0;
    }

    assert(BuffSlotCount < RUNTIME_CHARACTER_MAX_BUFF_SLOT_COUNT);

    Int32 TailLength = BuffSlotCount - BuffSlotOffset;
    if (TailLength > 0) {
        memmove(
            &Character->Data.BuffInfo.Slots[BuffSlotOffset + 1],
            &Character->Data.BuffInfo.Slots[BuffSlotOffset],
            sizeof(struct _RTBuffSlot) * TailLength
        );
    }

    RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[BuffSlotOffset];
    memset(BuffSlot, 0, sizeof(struct _RTBuffSlot));
    *BuffSlotCounts[BuffSlotType] += 1;
    Character->SyncMask.BuffInfo = true;
    return BuffSlot;
}

Void RTCharacterRemoveBuffSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
) {
    UInt8* BuffSlotCounts[] = {
         &Character->Data.BuffInfo.Info.SkillBuffCount,
         &Character->Data.BuffInfo.Info.PotionBuffCount,
         &Character->Data.BuffInfo.Info.GmBuffCount,
         &Character->Data.BuffInfo.Info.UnknownBuffCount1,
         &Character->Data.BuffInfo.Info.UnknownBuffCount2,
         &Character->Data.BuffInfo.Info.ForceWingBuffCount,
         &Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };

    Int32 BuffSlotType = -1;
    Int32 BuffSlotCount = 0;

    for (Int32 Index = 0; Index < 7; Index += 1) {
        BuffSlotCount += *BuffSlotCounts[Index];
        if (BuffSlotType < 0 && SlotIndex < BuffSlotCount) {
            BuffSlotType = Index;
        }
    }

    Int32 TailLength = BuffSlotCount - SlotIndex - 1;
    if (TailLength > 0) {
        memmove(
            &Character->Data.BuffInfo.Slots[SlotIndex],
            &Character->Data.BuffInfo.Slots[SlotIndex + 1],
            sizeof(struct _RTBuffSlot) * TailLength
        );
    }

    *BuffSlotCounts[BuffSlotType] -= 1;
    Character->SyncMask.BuffInfo = true;
}

Void RTCharacterInitializeBuffs(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    Int32 BuffSlotCount = (
        Character->Data.BuffInfo.Info.SkillBuffCount +
        Character->Data.BuffInfo.Info.PotionBuffCount +
        Character->Data.BuffInfo.Info.GmBuffCount +
        Character->Data.BuffInfo.Info.UnknownBuffCount1 +
        Character->Data.BuffInfo.Info.UnknownBuffCount2 +
        Character->Data.BuffInfo.Info.ForceWingBuffCount +
        Character->Data.BuffInfo.Info.FirePlaceBuffCount
    );

    Character->LastBuffUpdateTimestamp = CurrentTimestamp;
    Character->BuffUpdateTimestamp = UINT64_MAX;
    for (Int32 Index = 0; Index < BuffSlotCount; Index += 1) {
        RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];
        if (BuffSlot->Duration) {
            Character->BuffUpdateTimestamp = MIN(Character->BuffUpdateTimestamp, CurrentTimestamp + BuffSlot->Duration);
        } 

        Int32 BuffType = RTCharacterGetBuffSlotIndexBuffType(Runtime, Character, Index);
        if (BuffType == RUNTIME_BUFF_SLOT_TYPE_SKILL) {
            RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, BuffSlot->SkillIndex);
            if (SkillData) {
                for (Int32 ValueIndex = 0; ValueIndex < SkillData->SkillValueCount; ValueIndex += 1) {
                    RTSkillValueDataRef SkillValue = &SkillData->SkillValues[ValueIndex];
                    Int64 ForceValue = ((Int64)SkillValue->ForceEffectValue[0] * BuffSlot->SkillLevel + SkillValue->ForceEffectValue[1] + SkillValue->ForceEffectValue[2]) / 10;

                    RTCharacterApplyForceEffect(
                        Runtime,
                        Character,
                        SkillValue->ForceEffectIndex,
                        ForceValue,
                        SkillValue->ValueType
                    );
                }
            }
        }
        else {
            assert(false && "Implementation missing!");
        }
    }
}

Void RTCharacterUpdateBuffs(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Bool ForceUpdate
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    if (ForceUpdate || Character->BuffUpdateTimestamp <= CurrentTimestamp) {
        Timestamp Interval = CurrentTimestamp - Character->LastBuffUpdateTimestamp;
        Character->BuffUpdateTimestamp = UINT64_MAX;
        Character->LastBuffUpdateTimestamp = CurrentTimestamp;

        Int32 BuffSlotCount = (
            Character->Data.BuffInfo.Info.SkillBuffCount +
            Character->Data.BuffInfo.Info.PotionBuffCount +
            Character->Data.BuffInfo.Info.GmBuffCount +
            Character->Data.BuffInfo.Info.UnknownBuffCount1 +
            Character->Data.BuffInfo.Info.UnknownBuffCount2 +
            Character->Data.BuffInfo.Info.ForceWingBuffCount +
            Character->Data.BuffInfo.Info.FirePlaceBuffCount
        );

        for (Int32 Index = 0; Index < BuffSlotCount; Index += 1) {
            RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];
            if (BuffSlot->Duration > Interval) {
                BuffSlot->Duration -= Interval;
                Character->BuffUpdateTimestamp = MIN(Character->BuffUpdateTimestamp, CurrentTimestamp + BuffSlot->Duration);
            }
            else {
                BuffSlot->Duration = 0;
            }

            Character->SyncMask.BuffInfo = true;
        }

        Bool UpdateAttributes = false;
        for (Int32 Index = BuffSlotCount - 1; Index >= 0; Index -= 1) {
            RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];
            if (BuffSlot->Duration > 0) continue;

            RTCharacterRemoveBuffSlot(Runtime, Character, Index);
            UpdateAttributes = true;
        }

        if (UpdateAttributes) {
            RTCharacterInitializeAttributes(Runtime, Character);
        }
    }
}

UInt32 RTCharacterApplyBuff(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTSkillSlotRef SkillSlot,
    RTCharacterSkillDataRef SkillData
) {
    Int32 BuffSlotIndex = 0;
    RTBuffSlotRef BuffSlot = RTCharacterGetBuffSlotBySkillIndex(Runtime, Character, SkillSlot->ID, &BuffSlotIndex);
    if (BuffSlot) {
        if (BuffSlot->SkillLevel > SkillSlot->Level) return 0;

        NOTIFICATION_DATA_REMOVE_BUFF* Notification = RTNotificationInit(REMOVE_BUFF);
        Notification->EntityType = RUNTIME_ENTITY_TYPE_CHARACTER;
        Notification->EntityID = Character->CharacterIndex;
        Notification->BuffType = RUNTIME_BUFF_TYPE_SKILL;
        Notification->BuffSlotIndex = 0;
        Notification->SkillIndex = BuffSlot->SkillIndex;
        RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

        RTCharacterRemoveBuffSlot(Runtime, Character, BuffSlotIndex);
    }
   
    BuffSlot = RTCharacterInsertBuffSlot(Runtime, Character, RUNTIME_BUFF_SLOT_TYPE_SKILL);
    BuffSlot->SkillIndex = SkillSlot->ID;
    BuffSlot->SkillLevel = SkillSlot->Level;
    BuffSlot->Duration = RTCalculateSkillDuration(SkillData, SkillSlot->Level, Character->Data.StyleInfo.Style.BattleRank);

    Character->SyncMask.BuffInfo = true;
    RTCharacterInitializeAttributes(Runtime, Character);

    Timestamp ExpirationTimestamp = GetTimestampMs() + BuffSlot->Duration;
    Character->BuffUpdateTimestamp = MIN(Character->BuffUpdateTimestamp, ExpirationTimestamp);

    return 1;
}

Bool RTCharacterRemoveBuff(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SkillIndex
) {
    Int32 ResultSlotIndex = 0;
    RTBuffSlotRef BuffSlot = RTCharacterGetBuffSlotBySkillIndex(Runtime, Character, SkillIndex, &ResultSlotIndex);
    if (BuffSlot) {
        NOTIFICATION_DATA_REMOVE_BUFF* Notification = RTNotificationInit(REMOVE_BUFF);
        Notification->EntityType = RUNTIME_ENTITY_TYPE_CHARACTER;
        Notification->EntityID = Character->CharacterIndex;
        Notification->BuffType = RUNTIME_BUFF_TYPE_SKILL;
        Notification->BuffSlotIndex = 0;
        Notification->SkillIndex = SkillIndex;
        RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

        RTCharacterRemoveBuffSlot(Runtime, Character, ResultSlotIndex);
        RTCharacterInitializeAttributes(Runtime, Character);
        return true;
    }

    return false;
}

UInt32 RTMobApplyBuff(
    RTRuntimeRef Runtime,
    RTMobRef Mob,
    RTSkillSlotRef SkillSlot,
    RTCharacterSkillDataRef SkillData
) {
    return 0;
}
