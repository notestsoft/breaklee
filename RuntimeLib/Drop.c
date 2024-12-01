#include "Character.h"
#include "Drop.h"
#include "Mob.h"
#include "OptionPool.h"
#include "Runtime.h"
#include "World.h"

Bool RTDropPoolRollItem( 
    RTRuntimeRef Runtime,
    Int32 DropRateValue,
    Int32* DropRateOffset,
    ArrayRef DropPool,
    Int32 MobLevel,
    RTDropResultRef Result
) {
    for (Int DropIndex = 0; DropIndex < ArrayGetElementCount(DropPool); DropIndex += 1) {
        RTDropItemRef DropItem = (RTDropItemRef)ArrayGetElementAtIndex(DropPool, DropIndex);
        if (MobLevel > 0 && DropItem->MinMobLevel > MobLevel) continue;
        if (MobLevel > 0 && DropItem->MaxMobLevel < MobLevel) continue;
        if (DropRateValue <= *DropRateOffset + DropItem->DropRate) {
            Result->ItemID = DropItem->ItemID;
            Result->ItemOptions = DropItem->ItemOptions;
            Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
            RTOptionPoolManagerCalculateOptions(Runtime, Runtime->OptionPoolManager, DropItem->OptionPoolIndex, Result);
            return true;
        }

        *DropRateOffset += DropItem->DropRate;
    }

    return false;
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
    Int32 Level = (Int32)Mob->Attributes.Values[RUNTIME_ATTRIBUTE_LEVEL];

    if (World->DungeonIndex > 0) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
        assert(DungeonData);

        Int DropPoolIndex = Mob->SpeciesData->MobSpeciesIndex;
        ArrayRef DropPool = (ArrayRef)DictionaryLookup(DungeonData->DropTable.MobDropPool, &DropPoolIndex);
        if (DropPool) {
            if (RTDropPoolRollItem(Runtime, DropRateValue, &DropRateOffset, DropPool, 0, Result)) return true;
        }

        DropPool = DungeonData->DropTable.WorldDropPool;
        if (RTDropPoolRollItem(Runtime, DropRateValue, &DropRateOffset, DropPool, Level, Result)) return true;
    }

    Int DropPoolIndex = Mob->SpeciesData->MobSpeciesIndex;
    ArrayRef DropPool = (ArrayRef)DictionaryLookup(World->WorldData->DropTable.MobDropPool, &DropPoolIndex);
    if (DropPool) {
        if (RTDropPoolRollItem(Runtime, DropRateValue, &DropRateOffset, DropPool, 0, Result)) return true;
    }

    DropPool = World->WorldData->DropTable.WorldDropPool;
    if (RTDropPoolRollItem(Runtime, DropRateValue, &DropRateOffset, DropPool, Level, Result)) return true;
    
    DropPoolIndex = Mob->SpeciesData->MobSpeciesIndex;
    DropPool = (ArrayRef)DictionaryLookup(Runtime->DropTable.MobDropPool, &DropPoolIndex);
    if (DropPool) {
        if (RTDropPoolRollItem(Runtime, DropRateValue, &DropRateOffset, DropPool, 0, Result)) return true;
    }

    DropPool = Runtime->DropTable.WorldDropPool;
    if (RTDropPoolRollItem(Runtime, DropRateValue, &DropRateOffset, DropPool, Level, Result)) return true;

    return false;
}

Bool RTDropCurrency(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMobRef Mob,
    RTCharacterRef Character
) {
    Int64 CurrencyDropRate = RUNTIME_DROP_BASE_CURRENCY_RATE + Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALZ_DROP_RATE];
    Int64 CurrencyBombDropRate = RUNTIME_DROP_BASE_CURRENCY_BOMB_RATE + Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALZ_BOMB_RATE];

    Int64 MaxCurrencyDropRate = 10000;
    MaxCurrencyDropRate += Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALZ_DROP_RATE];
    MaxCurrencyDropRate += Character->Attributes.Values[RUNTIME_ATTRIBUTE_ALZ_BOMB_RATE];
    Int64 FinalCurrencyDropRate = INT16_MAX / MaxCurrencyDropRate * CurrencyDropRate;
    Int64 FinalCurrencyBombDropRate = INT16_MAX / MaxCurrencyDropRate * CurrencyBombDropRate;
    Int32 Level = (Int32)Mob->Attributes.Values[RUNTIME_ATTRIBUTE_LEVEL];

    Int64 DropRateValue = RandomRange(&World->Seed, 0, INT16_MAX);
    if (DropRateValue < FinalCurrencyDropRate) {
        Int32 BaseDropAmount = MAX(1, Level) * RUNTIME_DROP_BASE_CURRENCY_MULTIPLIER;

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
        for (Int Index = 0; Index < RUNTIME_DROP_BASE_CURRENCY_BOMB_STACK_SIZE; Index += 1) {
            Int32 BaseDropAmount = MAX(1, Level) * RUNTIME_DROP_BASE_CURRENCY_BOMB_MULTIPLIER;

            RTDropResult Drop = { 0 };
            Drop.ItemID.ID = RUNTIME_ITEM_ID_CURRENCY;
            Drop.ItemOptions = BaseDropAmount + (Int64)RandomRange(&World->Seed, 1, BaseDropAmount);
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
    Int DropPoolIndex = Mob->SpeciesData->MobSpeciesIndex;

    if (World->DungeonIndex > 0) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);

        ArrayRef DropPool = (ArrayRef)DictionaryLookup(DungeonData->DropTable.QuestDropPool, &DropPoolIndex);
        if (DropPool) {
            for (Int DropIndex = 0; DropIndex < ArrayGetElementCount(DropPool); DropIndex += 1) {
                RTDropItemRef DropItem = (RTDropItemRef)ArrayGetElementAtIndex(DropPool, DropIndex);
                if (!RTCharacterHasQuestItemCounter(Runtime, Character, DropItem->ItemID, DropItem->ItemOptions)) continue;
                
                if (DropRateValue <= DropRateOffset + DropItem->DropRate) {
                    Result->ItemID = DropItem->ItemID;
                    Result->ItemOptions = DropItem->ItemOptions;
                    Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
                    Result->ItemProperty.IsQuestItem = true;
                    return true;
                }

                DropRateOffset += DropItem->DropRate;
            }
        }
    }

    ArrayRef DropPool = (ArrayRef)DictionaryLookup(World->WorldData->DropTable.QuestDropPool, &DropPoolIndex);
    if (DropPool) {
        for (Int DropIndex = 0; DropIndex < ArrayGetElementCount(DropPool); DropIndex += 1) {
            RTDropItemRef DropItem = (RTDropItemRef)ArrayGetElementAtIndex(DropPool, DropIndex);
            if (!RTCharacterHasQuestItemCounter(Runtime, Character, DropItem->ItemID, DropItem->ItemOptions)) continue;

            if (DropRateValue <= DropRateOffset + DropItem->DropRate) {
                Result->ItemID = DropItem->ItemID;
                Result->ItemOptions = DropItem->ItemOptions;
                Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
                Result->ItemProperty.IsQuestItem = true;
                return true;
            }

            DropRateOffset += DropItem->DropRate;
        }
    }

    DropPool = (ArrayRef)DictionaryLookup(Runtime->DropTable.QuestDropPool, &DropPoolIndex);
    if (DropPool) {
        for (Int DropIndex = 0; DropIndex < ArrayGetElementCount(DropPool); DropIndex += 1) {
            RTDropItemRef DropItem = (RTDropItemRef)ArrayGetElementAtIndex(DropPool, DropIndex);
            if (!RTCharacterHasQuestItemCounter(Runtime, Character, DropItem->ItemID, DropItem->ItemOptions)) continue;

            if (DropRateValue <= DropRateOffset + DropItem->DropRate) {
                Result->ItemID = DropItem->ItemID;
                Result->ItemOptions = DropItem->ItemOptions;
                Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
                Result->ItemProperty.IsQuestItem = true;
                return true;
            }

            DropRateOffset += DropItem->DropRate;
        }
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
    Int DropPoolIndex = Mob->SpeciesData->MobSpeciesIndex;

    if (World->DungeonIndex > 0) {
        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);

        ArrayRef DropPool = (ArrayRef)DictionaryLookup(DungeonData->DropTable.QuestDropPool, &DropPoolIndex);
        if (DropPool) {
            for (Int DropIndex = 0; DropIndex < ArrayGetElementCount(DropPool); DropIndex += 1) {
                RTDropItemRef DropItem = (RTDropItemRef)ArrayGetElementAtIndex(DropPool, DropIndex);
                if (!RTPartyHasQuestItemCounter(Runtime, Party, DropItem->ItemID, DropItem->ItemOptions)) continue;

                if (DropRateValue <= DropRateOffset + DropItem->DropRate) {
                    Result->ItemID = DropItem->ItemID;
                    Result->ItemOptions = DropItem->ItemOptions;
                    Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
                    Result->ItemProperty.IsQuestItem = true;
                    return true;
                }

                DropRateOffset += DropItem->DropRate;
            }
        }
    }

    ArrayRef DropPool = (ArrayRef)DictionaryLookup(World->WorldData->DropTable.QuestDropPool, &DropPoolIndex);
    if (DropPool) {
        for (Int DropIndex = 0; DropIndex < ArrayGetElementCount(DropPool); DropIndex += 1) {
            RTDropItemRef DropItem = (RTDropItemRef)ArrayGetElementAtIndex(DropPool, DropIndex);
            if (!RTPartyHasQuestItemCounter(Runtime, Party, DropItem->ItemID, DropItem->ItemOptions)) continue;

            if (DropRateValue <= DropRateOffset + DropItem->DropRate) {
                Result->ItemID = DropItem->ItemID;
                Result->ItemOptions = DropItem->ItemOptions;
                Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
                Result->ItemProperty.IsQuestItem = true;
                return true;
            }

            DropRateOffset += DropItem->DropRate;
        }
    }

    DropPool = (ArrayRef)DictionaryLookup(Runtime->DropTable.QuestDropPool, &DropPoolIndex);
    if (DropPool) {
        for (Int DropIndex = 0; DropIndex < ArrayGetElementCount(DropPool); DropIndex += 1) {
            RTDropItemRef DropItem = (RTDropItemRef)ArrayGetElementAtIndex(DropPool, DropIndex);
            if (!RTPartyHasQuestItemCounter(Runtime, Party, DropItem->ItemID, DropItem->ItemOptions)) continue;

            if (DropRateValue <= DropRateOffset + DropItem->DropRate) {
                Result->ItemID = DropItem->ItemID;
                Result->ItemOptions = DropItem->ItemOptions;
                Result->ItemDuration.DurationIndex = DropItem->DurationIndex;
                Result->ItemProperty.IsQuestItem = true;
                return true;
            }

            DropRateOffset += DropItem->DropRate;
        }
    }

    return false;
}
