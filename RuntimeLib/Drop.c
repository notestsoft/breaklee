#include "Character.h"
#include "Drop.h"
#include "Mob.h"
#include "World.h"
#include "Runtime.h"

RTDropItemRef kDropItemList[RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT + RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT + RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT];

Void RTCalculateDropOptions(
    RTDropItemRef DropItem,
    RTDropResultRef DropResult
) {

}

Bool RTCalculateDrop(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
) {
    Int32 Seed = (Int32)PlatformGetTickCount();
    Int32 DropRateValue = RandomRange(&World->Seed, 0, INT32_MAX);
    Int32 DropRateOffset = 0;
    
    if (World->DungeonIndex) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
        
        for (Int32 Index = 0; Index < DungeonData->DropTable.MobItemCount; Index += 1) {
            RTMobDropItemRef DropItem = &DungeonData->DropTable.MobItems[Index];
            if (DropItem->MobSpeciesIndex != Mob->SpeciesData->MobSpeciesIndex) continue;

            if (DropRateValue <= DropRateOffset + DropItem->Item.DropRate) {
                Result->ItemID = DropItem->Item.ItemID;
                Result->ItemOptions = DropItem->Item.ItemOptions;
                Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
                RTCalculateDropOptions(&DropItem->Item, Result);
                return true;
            }

            DropRateOffset += DropItem->Item.DropRate;
        }

        for (Int32 Index = 0; Index < DungeonData->DropTable.WorldItemCount; Index += 1) {
            RTWorldDropItemRef DropItem = &DungeonData->DropTable.WorldItems[Index];
            if (DropItem->MinMobLevel > Mob->Spawn.Level) continue;
            if (DropItem->MaxMobLevel < Mob->Spawn.Level) continue;

            if (DropRateValue <= DropRateOffset + DropItem->Item.DropRate) {
                Result->ItemID = DropItem->Item.ItemID;
                Result->ItemOptions = DropItem->Item.ItemOptions;
                Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
                RTCalculateDropOptions(&DropItem->Item, Result);
                return true;
            }

            DropRateOffset += DropItem->Item.DropRate;
        }
    }

    for (Int32 Index = 0; Index < World->WorldData->DropTable.MobItemCount; Index += 1) {
        RTMobDropItemRef DropItem = &World->WorldData->DropTable.MobItems[Index];
        if (DropItem->MobSpeciesIndex != Mob->SpeciesData->MobSpeciesIndex) continue;

        if (DropRateValue <= DropRateOffset + DropItem->Item.DropRate) {
            Result->ItemID = DropItem->Item.ItemID;
            Result->ItemOptions = DropItem->Item.ItemOptions;
            Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
            RTCalculateDropOptions(&DropItem->Item, Result);
            return true;
        }

        DropRateOffset += DropItem->Item.DropRate;
    }

    for (Int32 Index = 0; Index < World->WorldData->DropTable.WorldItemCount; Index += 1) {
        RTWorldDropItemRef DropItem = &World->WorldData->DropTable.WorldItems[Index];
        if (DropItem->MinMobLevel > Mob->Spawn.Level) continue;
        if (DropItem->MaxMobLevel < Mob->Spawn.Level) continue;

        if (DropRateValue <= DropRateOffset + DropItem->Item.DropRate) {
            Result->ItemID = DropItem->Item.ItemID;
            Result->ItemOptions = DropItem->Item.ItemOptions;
            Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
            RTCalculateDropOptions(&DropItem->Item, Result);
            return true;
        }

        DropRateOffset += DropItem->Item.DropRate;
    }

    return false;
}

Bool RTDropCurrency(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTCharacterRef Character
) {
    Int32 CurrencyDropRate = RUNTIME_DROP_BASE_CURRENCY_RATE + Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALZ_DROP_RATE];
    Int32 CurrencyBombDropRate = RUNTIME_DROP_BASE_CURRENCY_BOMB_RATE + Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALZ_BOMB_RATE];

    Int32 MaxCurrencyDropRate = 10000;
    MaxCurrencyDropRate += Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALZ_DROP_RATE];
    MaxCurrencyDropRate += Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALZ_BOMB_RATE];
    Int32 FinalCurrencyDropRate = INT16_MAX / MaxCurrencyDropRate * CurrencyDropRate;
    Int32 FinalCurrencyBombDropRate = INT16_MAX / MaxCurrencyDropRate * CurrencyBombDropRate;

    Int32 DropRateValue = RandomRange(&World->Seed, 0, INT16_MAX);
    if (DropRateValue < FinalCurrencyDropRate) {
        Int32 BaseDropAmount = MAX(1, Mob->Spawn.Level) * RUNTIME_DROP_BASE_CURRENCY_MULTIPLIER;

        RTDropResult Drop = { 0 };
        Drop.ItemID.ID = RUNTIME_ITEM_ID_CURRENCY;
        Drop.ItemOptions = BaseDropAmount + RandomRange(&World->Seed, 1, BaseDropAmount);
        RTWorldSpawnItem(
            Runtime,
            World,
            Mob->ID,
            Character->Movement.PositionCurrent.X,
            Character->Movement.PositionCurrent.Y,
            Drop
        );

        return true;
    }
    else if (DropRateValue < FinalCurrencyDropRate + FinalCurrencyBombDropRate) {
        for (Int32 Index = 0; Index < RUNTIME_DROP_BASE_CURRENCY_BOMB_STACK_SIZE; Index += 1) {
            Int32 BaseDropAmount = MAX(1, Mob->Spawn.Level) * RUNTIME_DROP_BASE_CURRENCY_BOMB_MULTIPLIER;

            RTDropResult Drop = { 0 };
            Drop.ItemID.ID = RUNTIME_ITEM_ID_CURRENCY;
            Drop.ItemOptions = BaseDropAmount + RandomRange(&World->Seed, 1, BaseDropAmount);
            RTWorldSpawnItem(
                Runtime,
                World,
                Mob->ID,
                Character->Movement.PositionCurrent.X,
                Character->Movement.PositionCurrent.Y,
                Drop
            );
        }

        return true;
    }

    return false;
}

Bool RTCalculateQuestDrop(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
) {
    Int32 Seed = (Int32)PlatformGetTickCount();
    Int32 DropRateValue = RandomRange(&World->Seed, 0, INT32_MAX);
    Int32 DropRateOffset = 0;

    if (World->DungeonIndex) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);

        for (Int32 Index = 0; Index < DungeonData->DropTable.QuestItemCount; Index += 1) {
            RTQuestDropItemRef DropItem = &DungeonData->DropTable.QuestItems[Index];
            if (DropItem->MobSpeciesIndex != Mob->Spawn.MobSpeciesIndex) continue;
            if (!RTCharacterHasQuestItemCounter(Runtime, Character, DropItem->Item.ItemID, DropItem->Item.ItemOptions)) continue;

            if (DropRateValue <= DropRateOffset + DropItem->Item.DropRate) {
                Result->ItemID = DropItem->Item.ItemID;
                Result->ItemOptions = RTQuestItemOptions(DropItem->Item.ItemOptions, 1);
                Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
                Result->ItemProperty.IsQuestItem = true;
                return true;
            }

            DropRateOffset += DropItem->Item.DropRate;
        }
    }

    for (Int32 Index = 0; Index < World->WorldData->DropTable.QuestItemCount; Index += 1) {
        RTQuestDropItemRef DropItem = &World->WorldData->DropTable.QuestItems[Index];
        if (DropItem->MobSpeciesIndex != Mob->Spawn.MobSpeciesIndex) continue;
        if (!RTCharacterHasQuestItemCounter(Runtime, Character, DropItem->Item.ItemID, DropItem->Item.ItemOptions)) continue;

        if (DropRateValue <= DropRateOffset + DropItem->Item.DropRate) {
            Result->ItemID = DropItem->Item.ItemID;
            Result->ItemOptions = RTQuestItemOptions(DropItem->Item.ItemOptions, 1);
            Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
            Result->ItemProperty.IsQuestItem = true;
            return true;
        }

        DropRateOffset += DropItem->Item.DropRate;
    }

    return false;
}

Bool RTCalculatePartyQuestDrop(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTPartyRef Party,
    RTDropResultRef Result
) {
    Int32 Seed = (Int32)PlatformGetTickCount();
    Int32 DropRateValue = RandomRange(&World->Seed, 0, INT32_MAX);
    Int32 DropRateOffset = 0;

    if (World->DungeonIndex) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);

        for (Int32 Index = 0; Index < DungeonData->DropTable.QuestItemCount; Index += 1) {
            RTQuestDropItemRef DropItem = &DungeonData->DropTable.QuestItems[Index];
            if (DropItem->MobSpeciesIndex != Mob->Spawn.MobSpeciesIndex) continue;
            if (!RTPartyHasQuestItemCounter(Runtime, Party, DropItem->Item.ItemID, DropItem->Item.ItemOptions)) continue;

            if (DropRateValue <= DropRateOffset + DropItem->Item.DropRate) {
                Result->ItemID = DropItem->Item.ItemID;
                Result->ItemOptions = RTQuestItemOptions(DropItem->Item.ItemOptions, 1);
                Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
                Result->ItemProperty.IsQuestItem = true;
                return true;
            }

            DropRateOffset += DropItem->Item.DropRate;
        }
    }

    for (Int32 Index = 0; Index < World->WorldData->DropTable.QuestItemCount; Index += 1) {
        RTQuestDropItemRef DropItem = &World->WorldData->DropTable.QuestItems[Index];
        if (DropItem->MobSpeciesIndex != Mob->Spawn.MobSpeciesIndex) continue;
        if (!RTPartyHasQuestItemCounter(Runtime, Party, DropItem->Item.ItemID, DropItem->Item.ItemOptions)) continue;

        if (DropRateValue <= DropRateOffset + DropItem->Item.DropRate) {
            Result->ItemID = DropItem->Item.ItemID;
            Result->ItemOptions = RTQuestItemOptions(DropItem->Item.ItemOptions, 1);
            Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
            Result->ItemProperty.IsQuestItem = true;
            return true;
        }

        DropRateOffset += DropItem->Item.DropRate;
    }

    return false;
}
