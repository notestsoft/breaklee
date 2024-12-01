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
    Int8* BuffSlotCounts[] = {
         &Character->Data.BuffInfo.Info.SkillBuffCount,
         &Character->Data.BuffInfo.Info.PotionBuffCount,
         &Character->Data.BuffInfo.Info.GmBuffCount,
         &Character->Data.BuffInfo.Info.ForceCaliburBuffCount,
         &Character->Data.BuffInfo.Info.UnknownBuffCount2,
         &Character->Data.BuffInfo.Info.ForceWingBuffCount,
         &Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };

    Int32 BuffSlotCount = 0;

    for (Int Index = 0; Index < 7; Index += 1) {
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
    for (Int Index = 0; Index < Character->Data.BuffInfo.Info.SkillBuffCount; Index += 1) {
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
    Int8* BuffSlotCounts[] = {
        &Character->Data.BuffInfo.Info.SkillBuffCount,
        &Character->Data.BuffInfo.Info.PotionBuffCount,
        &Character->Data.BuffInfo.Info.GmBuffCount,
        &Character->Data.BuffInfo.Info.ForceCaliburBuffCount,
        &Character->Data.BuffInfo.Info.UnknownBuffCount2,
        &Character->Data.BuffInfo.Info.ForceWingBuffCount,
        &Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };

    Int32 BuffSlotCount = 0;
    Int32 BuffSlotOffset = 0;

    for (Int Index = 0; Index < 7; Index += 1) {
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
    Int8* BuffSlotCounts[] = {
        &Character->Data.BuffInfo.Info.SkillBuffCount,
        &Character->Data.BuffInfo.Info.PotionBuffCount,
        &Character->Data.BuffInfo.Info.GmBuffCount,
        &Character->Data.BuffInfo.Info.ForceCaliburBuffCount,
        &Character->Data.BuffInfo.Info.UnknownBuffCount2,
        &Character->Data.BuffInfo.Info.ForceWingBuffCount,
        &Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };

    Int32 BuffSlotCount = 0;
    Int32 BuffSlotOffset = 0;

    for (Int Index = 0; Index < 7; Index += 1) {
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
    Int8* BuffSlotCounts[] = {
         &Character->Data.BuffInfo.Info.SkillBuffCount,
         &Character->Data.BuffInfo.Info.PotionBuffCount,
         &Character->Data.BuffInfo.Info.GmBuffCount,
         &Character->Data.BuffInfo.Info.ForceCaliburBuffCount,
         &Character->Data.BuffInfo.Info.UnknownBuffCount2,
         &Character->Data.BuffInfo.Info.ForceWingBuffCount,
         &Character->Data.BuffInfo.Info.FirePlaceBuffCount
    };

    Int32 BuffSlotType = -1;
    Int32 BuffSlotCount = 0;

    for (Int Index = 0; Index < 7; Index += 1) {
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
        Character->Data.BuffInfo.Info.ForceCaliburBuffCount +
        Character->Data.BuffInfo.Info.UnknownBuffCount2 +
        Character->Data.BuffInfo.Info.ForceWingBuffCount +
        Character->Data.BuffInfo.Info.FirePlaceBuffCount
    );

    Character->LastBuffUpdateTimestamp = CurrentTimestamp;
    Character->BuffUpdateTimestamp = UINT64_MAX;
    for (Int Index = 0; Index < BuffSlotCount; Index += 1) {
        RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];
        if (BuffSlot->Duration > 0) {
            Character->BuffUpdateTimestamp = MIN(Character->BuffUpdateTimestamp, CurrentTimestamp + BuffSlot->Duration);
        } 

        Int32 BuffType = RTCharacterGetBuffSlotIndexBuffType(Runtime, Character, Index);
        if (BuffType == RUNTIME_BUFF_SLOT_TYPE_SKILL || BuffType == RUNTIME_BUFF_SLOT_TYPE_FORCE_WING) {
            RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, BuffSlot->SkillIndex);
            if (SkillData) {
                for (Int ValueIndex = 0; ValueIndex < SkillData->SkillValueCount; ValueIndex += 1) {
                    RTSkillValueDataRef SkillValue = &SkillData->SkillValues[ValueIndex];
                    Int64 ForceValue = RTCalculateSkillValue(SkillValue, BuffSlot->SkillLevel, (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT]);

                    RTCharacterApplyForceEffect(
                        Runtime,
                        Character,
                        kEntityIDNull,
                        SkillValue->ForceEffectIndex,
                        ForceValue,
                        SkillValue->ValueType
                    );
                }
            }
        }
        else {
            //assert(false && "Implementation missing!");
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
            Character->Data.BuffInfo.Info.ForceCaliburBuffCount +
            Character->Data.BuffInfo.Info.UnknownBuffCount2 +
            Character->Data.BuffInfo.Info.ForceWingBuffCount +
            Character->Data.BuffInfo.Info.FirePlaceBuffCount
        );

        for (Int Index = 0; Index < BuffSlotCount; Index += 1) {
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
        for (Int Index = BuffSlotCount - 1; Index >= 0; Index -= 1) {
            RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];
            if (BuffSlot->Duration > 0) continue;

            NOTIFICATION_DATA_REMOVE_BUFF* Notification = RTNotificationInit(REMOVE_BUFF);
            Notification->EntityType = RUNTIME_ENTITY_TYPE_CHARACTER;
            Notification->EntityID = (UInt32)Character->CharacterIndex;
            Notification->BuffType = RUNTIME_BUFF_TYPE_BUFF;
            Notification->BuffSlotIndex = 0;
            Notification->SkillIndex = BuffSlot->SkillIndex;
            RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

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
    RTCharacterSkillDataRef SkillData,
    Int32 BuffType
) {
    Int32 BuffSlotIndex = 0;
    RTBuffSlotRef BuffSlot = RTCharacterGetBuffSlotBySkillIndex(Runtime, Character, SkillSlot->ID, &BuffSlotIndex);
    if (BuffSlot) {
        if (BuffSlot->SkillLevel > SkillSlot->Level) return 0;

        NOTIFICATION_DATA_REMOVE_BUFF* Notification = RTNotificationInit(REMOVE_BUFF);
        Notification->EntityType = RUNTIME_ENTITY_TYPE_CHARACTER;
        Notification->EntityID = (UInt32)Character->CharacterIndex;
        Notification->BuffType = BuffType;
        Notification->BuffSlotIndex = 0;
        Notification->SkillIndex = BuffSlot->SkillIndex;
        RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

        RTCharacterRemoveBuffSlot(Runtime, Character, BuffSlotIndex);
    }
    
    BuffSlot = RTCharacterInsertBuffSlot(Runtime, Character, BuffType);
    BuffSlot->SkillIndex = SkillSlot->ID;
    BuffSlot->SkillLevel = SkillSlot->Level;
    BuffSlot->Duration = RTCalculateSkillDuration(SkillData, SkillSlot->Level, Character->Data.StyleInfo.Style.BattleRank);

    Character->SyncMask.BuffInfo = true;
    RTCharacterInitializeAttributes(Runtime, Character);

    Timestamp ExpirationTimestamp = GetTimestampMs() + BuffSlot->Duration;
    Character->BuffUpdateTimestamp = MIN(Character->BuffUpdateTimestamp, ExpirationTimestamp);

    return 1;
}

Void RTCharacterRemoveAllBuffs(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    Int32 BuffSlotCount = (
        Character->Data.BuffInfo.Info.SkillBuffCount +
        Character->Data.BuffInfo.Info.PotionBuffCount +
        Character->Data.BuffInfo.Info.GmBuffCount +
        Character->Data.BuffInfo.Info.ForceCaliburBuffCount +
        Character->Data.BuffInfo.Info.UnknownBuffCount2 +
        Character->Data.BuffInfo.Info.ForceWingBuffCount +
        Character->Data.BuffInfo.Info.FirePlaceBuffCount
    );

    for (Int Index = 0; Index < BuffSlotCount; Index += 1) {
        RTBuffSlotRef BuffSlot = &Character->Data.BuffInfo.Slots[Index];

        NOTIFICATION_DATA_REMOVE_BUFF* Notification = RTNotificationInit(REMOVE_BUFF);
        Notification->EntityType = RUNTIME_ENTITY_TYPE_CHARACTER;
        Notification->EntityID = (UInt32)Character->CharacterIndex;
        Notification->BuffType = RUNTIME_BUFF_TYPE_BUFF;
        Notification->BuffSlotIndex = 0;
        Notification->SkillIndex = BuffSlot->SkillIndex;
        RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
    }

    Character->Data.BuffInfo.Info.SkillBuffCount = 0;
    Character->Data.BuffInfo.Info.PotionBuffCount = 0;
    Character->Data.BuffInfo.Info.GmBuffCount = 0;
    Character->Data.BuffInfo.Info.ForceCaliburBuffCount = 0;
    Character->Data.BuffInfo.Info.UnknownBuffCount2 = 0;
    Character->Data.BuffInfo.Info.ForceWingBuffCount = 0;
    Character->Data.BuffInfo.Info.FirePlaceBuffCount = 0;
    Character->SyncMask.BuffInfo = true;
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
        Notification->EntityID = (UInt32)Character->CharacterIndex;
        Notification->BuffType = RUNTIME_BUFF_TYPE_BUFF;
        Notification->BuffSlotIndex = 0;
        Notification->SkillIndex = SkillIndex;
        RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

        Int64 CurrentShield = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];

        RTCharacterRemoveBuffSlot(Runtime, Character, ResultSlotIndex);
        RTCharacterInitializeAttributes(Runtime, Character);

        if (CurrentShield != Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB]) {
            // TODO: Send notification to cancel shield
        }

        return true;
    }

    return false;
}

RTMobBuffSlotRef RTMobGetBuffSlotBySkillIndex(
    RTRuntimeRef Runtime,
    RTMobRef Mob,
    Int32 SkillIndex,
    Int32* ResultSlotIndex
) {
    for (Int Index = 0; Index < Mob->Buffs.SlotCount; Index += 1) {
        RTMobBuffSlotRef BuffSlot = &Mob->Buffs.Slots[Index];
        if (BuffSlot->SkillIndex == SkillIndex) {
            *ResultSlotIndex = Index;
            return BuffSlot;
        }
    }

    return NULL;
}

RTMobBuffSlotRef RTMobInsertBuffSlot(
    RTRuntimeRef Runtime,
    RTMobRef Mob
) {
    assert(Mob->Buffs.SlotCount < RUNTIME_MOB_MAX_BUFF_SLOT_COUNT);

    RTMobBuffSlotRef BuffSlot = &Mob->Buffs.Slots[Mob->Buffs.SlotCount];
    memset(BuffSlot, 0, sizeof(struct _RTMobBuffSlot));
    Mob->Buffs.SlotCount += 1;
    return BuffSlot;
}

Void RTMobRemoveBuffSlot(
    RTRuntimeRef Runtime,
    RTMobRef Mob,
    Int32 SlotIndex
) {
    RTMobBuffSlotRef BuffSlot = &Mob->Buffs.Slots[SlotIndex];

    RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, BuffSlot->SkillIndex);
    for (Int ValueIndex = 0; ValueIndex < SkillData->SkillValueCount; ValueIndex += 1) {
        RTSkillValueDataRef SkillValue = &SkillData->SkillValues[ValueIndex];
        Int64 ForceValue = RTCalculateSkillValue(SkillValue, BuffSlot->SkillLevel, BuffSlot->Rage);

        RTMobCancelForceEffect(
            Runtime,
            Mob,
            kEntityIDNull,
            SkillValue->ForceEffectIndex,
            ForceValue,
            SkillValue->ValueType
        );
    }

    NOTIFICATION_DATA_REMOVE_BUFF* Notification = RTNotificationInit(REMOVE_BUFF);
    Notification->EntityType = RUNTIME_ENTITY_TYPE_MOB;
    Notification->EntityID = Mob->ID.Serial;
    Notification->BuffType = RUNTIME_BUFF_TYPE_BUFF;
    Notification->BuffSlotIndex = 0;
    Notification->SkillIndex = BuffSlot->SkillIndex;
    RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);

    Int32 TailLength = Mob->Buffs.SlotCount - SlotIndex - 1;
    if (TailLength > 0) {
        memmove(
            &Mob->Buffs.Slots[SlotIndex],
            &Mob->Buffs.Slots[SlotIndex + 1],
            sizeof(struct _RTMobBuffSlot) * TailLength
        );
    }

    Mob->Buffs.SlotCount -= 1;
}

Void RTMobUpdateBuffs(
    RTRuntimeRef Runtime,
    RTMobRef Mob
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    if (Mob->BuffUpdateTimestamp <= CurrentTimestamp) {
        Timestamp Interval = CurrentTimestamp - Mob->LastBuffUpdateTimestamp;
        Mob->BuffUpdateTimestamp = UINT64_MAX;
        Mob->LastBuffUpdateTimestamp = CurrentTimestamp;

        for (Int Index = 0; Index < Mob->Buffs.SlotCount; Index += 1) {
            RTMobBuffSlotRef BuffSlot = &Mob->Buffs.Slots[Index];
            if (BuffSlot->Duration > Interval) {
                BuffSlot->Duration -= Interval;
                Mob->BuffUpdateTimestamp = MIN(Mob->BuffUpdateTimestamp, CurrentTimestamp + BuffSlot->Duration);
            }
            else {
                BuffSlot->Duration = 0;
            }
        }

        for (Int Index = Mob->Buffs.SlotCount - 1; Index >= 0; Index -= 1) {
            RTMobBuffSlotRef BuffSlot = &Mob->Buffs.Slots[Index];
            if (BuffSlot->Duration > 0) continue;

            RTMobRemoveBuffSlot(Runtime, Mob, Index);
        }
    }
}

UInt32 RTMobApplyBuff(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTMobRef Mob,
    RTSkillSlotRef SkillSlot,
    RTCharacterSkillDataRef SkillData
) {
    Int32 BuffSlotIndex = 0;
    RTMobBuffSlotRef BuffSlot = RTMobGetBuffSlotBySkillIndex(Runtime, Mob, SkillSlot->ID, &BuffSlotIndex);
    if (BuffSlot) {
        if (BuffSlot->SkillLevel > SkillSlot->Level) return 0;

        RTMobRemoveBuffSlot(Runtime, Mob, BuffSlotIndex);
    }

    BuffSlot = RTMobInsertBuffSlot(Runtime, Mob);
    BuffSlot->SkillIndex = SkillSlot->ID;
    BuffSlot->SkillLevel = SkillSlot->Level;
    BuffSlot->BattleRank = Character->Data.StyleInfo.Style.BattleRank;
    BuffSlot->Rage = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
    BuffSlot->Duration = RTCalculateSkillDuration(SkillData, SkillSlot->Level, BuffSlot->BattleRank);

    for (Int ValueIndex = 0; ValueIndex < SkillData->SkillValueCount; ValueIndex += 1) {
        RTSkillValueDataRef SkillValue = &SkillData->SkillValues[ValueIndex];
        Int64 ForceValue = RTCalculateSkillValue(SkillValue, BuffSlot->SkillLevel, BuffSlot->Rage);

        RTMobApplyForceEffect(
            Runtime,
            Mob,
            Character->ID,
            SkillValue->ForceEffectIndex,
            ForceValue,
            SkillValue->ValueType
        );
    }

    Timestamp ExpirationTimestamp = GetTimestampMs() + BuffSlot->Duration;
    Mob->BuffUpdateTimestamp = MIN(Mob->BuffUpdateTimestamp, ExpirationTimestamp);

    if (Mob->Pattern) {
        assert(Mob->Movement.WorldChunk->WorldContext);
        RTMobPatternSkillReceived(Runtime, Mob->Movement.WorldChunk->WorldContext, Mob, Mob->Pattern, SkillData->SkillID);
    }

    return 1;
}
