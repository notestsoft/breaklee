#include "Character.h"
#include "Drop.h"
#include "Mob.h"
#include "World.h"
#include "Runtime.h"

RTDropItemRef kDropItemList[RUNTIME_MEMORY_MAX_WORLD_DROP_ITEM_COUNT + RUNTIME_MEMORY_MAX_MOB_DROP_ITEM_COUNT + RUNTIME_MEMORY_MAX_QUEST_DROP_ITEM_COUNT];

// TODO: This has to be based on the actual terrain ...
Bool RTCalculateDrop(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTCharacterRef Character,
    RTDropResultRef Result
) {
    /*
    Int32 Seed = (Int32)PlatformGetTickCount();
    RTItemDataRef Item = &Runtime->ItemData[Random(&Seed) % Runtime->ItemDataCount];
    while (true) {
        Result->ItemID.ID = Random(&Seed);
        if (RTRuntimeGetItemDataByIndex(Runtime, Result->ItemID.ID)) {
            break;
        }
    }

    Result->ItemID.Extension = Random(&Seed);
    Result->ItemOptions = (UInt64)Random(&Seed) << 32 | Random(&Seed);
    return true;
    */
    Int32 DropItemCount = 0;
    Int32 DropRateLimit = 0;

    for (Int32 Index = 0; Index < World->WorldData->DropTable.MobItemCount; Index += 1) {
        RTMobDropItemRef DropItem = &World->WorldData->DropTable.MobItems[Index];
        if (DropItem->MobSpeciesIndex != Mob->Spawn.MobSpeciesIndex) continue;

        if (DropItem->Item.PerfectDrop) {
            Result->ItemID = DropItem->Item.ItemID;
            Result->ItemOptions = DropItem->Item.ItemOptions;
            Result->ItemDuration.DurationIndex = DropItem->Item.DurationIndex;
            // TODO: Calculate OptionPoolData
            return true;
        }

        kDropItemList[DropItemCount] = &DropItem->Item;
        DropItemCount += 1;
        DropRateLimit += DropItem->Item.DropRate;
    }

    for (Int32 Index = 0; Index < World->WorldData->DropTable.WorldItemCount; Index++) {
        RTWorldDropItemRef DropItem = &World->WorldData->DropTable.WorldItems[Index];

        if (DropItem->MinMobLevel <= Mob->SpeciesData->Level && Mob->SpeciesData->Level <= DropItem->MaxMobLevel) {
            kDropItemList[DropItemCount] = &DropItem->Item;
            DropItemCount += 1;
            DropRateLimit += DropItem->Item.DropRate;
        }
    }

    Int32 DropRate = RandomRange(&World->Seed, 0, DropRateLimit);
    Int32 DropRateOffset = 0;

    for (Int32 Index = 0; Index < DropItemCount; Index++) {
        RTDropItemRef DropItem = kDropItemList[Index];

        if (DropRate <= DropItem->DropRate + DropRateOffset) {
            Result->ItemID = DropItem->ItemID;
            Result->ItemOptions = DropItem->ItemOptions;
            Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
            // TODO: Calculate OptionPoolData
            return true;
        }

        DropRateOffset += DropItem->DropRate;
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
    Int32 DropItemCount = 0;
    Int32 DropRateLimit = 0;

    for (Int32 Index = 0; Index < World->WorldData->DropTable.QuestItemCount; Index += 1) {
        RTQuestDropItemRef DropItem = &World->WorldData->DropTable.QuestItems[Index];
        if (DropItem->MobSpeciesIndex != Mob->Spawn.MobSpeciesIndex) continue;

        if (RTCharacterHasQuestItemCounter(Runtime, Character, DropItem->Item.ItemID, DropItem->Item.ItemOptions)) {
            kDropItemList[DropItemCount] = &DropItem->Item;
            DropItemCount += 1;
            DropRateLimit += DropItem->Item.DropRate;
        }
    }

    Int32 DropRate = RandomRange(&World->Seed, 0, DropRateLimit);
    Int32 DropRateOffset = 0;

    for (Int32 Index = 0; Index < DropItemCount; Index += 1) {
        RTDropItemRef DropItem = kDropItemList[Index];

        if (DropRate <= DropItem->DropRate + DropRateOffset) {
            Result->ItemID = DropItem->ItemID;
            Result->ItemOptions = RTQuestItemOptions(DropItem->ItemOptions, 1);
            Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
            Result->ItemProperty.IsQuestItem = true;
            // TODO: Calculate OptionPoolData
            return true;
        }

        DropRateOffset += DropItem->DropRate;
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
    Int32 DropItemCount = 0;
    Int32 DropRateLimit = 0;

    for (Int32 Index = 0; Index < World->WorldData->DropTable.QuestItemCount; Index += 1) {
        RTQuestDropItemRef DropItem = &World->WorldData->DropTable.QuestItems[Index];
        if (DropItem->MobSpeciesIndex != Mob->Spawn.MobSpeciesIndex) continue;

        if (RTPartyHasQuestItemCounter(Runtime, Party, DropItem->Item.ItemID, DropItem->Item.ItemOptions)) {
            kDropItemList[DropItemCount] = &DropItem->Item;
            DropItemCount += 1;
            DropRateLimit += DropItem->Item.DropRate;
        }
    }

    Int32 DropRate = RandomRange(&World->Seed, 0, DropRateLimit);
    Int32 DropRateOffset = 0;

    for (Int32 Index = 0; Index < DropItemCount; Index += 1) {
        RTDropItemRef DropItem = kDropItemList[Index];

        if (DropRate <= DropItem->DropRate + DropRateOffset) {
            Result->ItemID = DropItem->ItemID;
            Result->ItemOptions = RTQuestItemOptions(DropItem->ItemOptions, 1);
            Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
            Result->ItemProperty.IsQuestItem = true;

            // TODO: Calculate OptionPoolData
            return true;
        }

        DropRateOffset += DropItem->DropRate;
    }

    return false;
}
