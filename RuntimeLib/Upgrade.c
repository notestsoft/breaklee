#include "Runtime.h"
#include "Upgrade.h"

Int32 RTCharacterGetUpgradePoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    // TODO: Add a reset timer for resetting instead
    if (Character->Data.UpgradeInfo.UpgradePointTimestamp <= CurrentTimestamp &&
        Character->Data.UpgradeInfo.UpgradePoints > 0) {
        Character->Data.UpgradeInfo.UpgradePoints = 0;
        Character->Data.UpgradeInfo.UpgradePointTimestamp = 0;
        Character->SyncMask.UpgradeInfo = true;
        return 0;
    }

    return Character->Data.UpgradeInfo.UpgradePoints;
}

Void RTCharacterSetUpgradePoints(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 UpgradePoints
) {
    if (UpgradePoints == Character->Data.UpgradeInfo.UpgradePoints) return;

    Character->Data.UpgradeInfo.UpgradePoints = UpgradePoints;
    Character->Data.UpgradeInfo.UpgradePointTimestamp = GetTimestampMs();
    if (UpgradePoints > 0) Character->Data.UpgradeInfo.UpgradePointTimestamp += Runtime->Config.UpgradePointDuration;
    Character->SyncMask.UpgradeInfo = true;
}

Int32 RTItemUpgradeNormal(
    RTRuntimeRef Runtime,
    RTItemSlotRef Item,
    Int32 UpgradeType,
    Int32* UpgradePoint,
    Int32* Seed
) {
    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Item->Item.ID);
    if (!ItemData) return RUNTIME_UPGRADE_RESULT_ERROR;
    
    RTDataUpgradeRateRef UpgradeRate = RTRuntimeDataUpgradeRateGet(Runtime->Context, UpgradeType);
    if (!UpgradeRate) return RUNTIME_UPGRADE_RESULT_ERROR;

    RTDataUpgradeRateValueRef UpgradeRateValue = RTRuntimeDataUpgradeRateValueGet(UpgradeRate, Item->Item.UpgradeLevel);
    if (!UpgradeRateValue) return RUNTIME_UPGRADE_RESULT_ERROR;

    Int32 TotalRate = 0;
    for (Int Index = 0; Index < RUNTIME_DATA_UPGRADE_RATE_VALUE_COUNT; Index += 1) {
        TotalRate += UpgradeRateValue->Rates[Index];
    }

    /*
    Int32 UpgradePointRate = 3000 * (*UpgradePoint) / (3000 + (*UpgradePoint));
    TotalRate += UpgradePointRate;
    */

    Int32 Value = RandomRange(Seed, 0, MAX(0, TotalRate - 1));
    Int32 CurrentRate = 0;

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_UPGRADE_1];
    if (Value < CurrentRate) {
        *UpgradePoint = 0;
        Item->Item.UpgradeLevel += 1;
        return RUNTIME_UPGRADE_RESULT_UPGRADE_1;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_UPGRADE_2];
    if (Value < CurrentRate) {
        *UpgradePoint = 0;
        Item->Item.UpgradeLevel += 2;
        return RUNTIME_UPGRADE_RESULT_UPGRADE_2;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_0];
    if (Value < CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_0;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_1];
    if (Value < CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;
        Item->Item.UpgradeLevel -= 1;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_1;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_2];
    if (Value < CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;
        Item->Item.UpgradeLevel -= 2;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_2;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_RESET];
    if (Value <= CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;
        Item->Item.UpgradeLevel -= 3;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_3;
    }

    Warn("Random range calculation should always result in perfect ranged matches!");
    return RUNTIME_UPGRADE_RESULT_ERROR;
}

Int32 RTItemUpgradeChaos(
    RTRuntimeRef Runtime,
    RTItemSlotRef Item,
    Int32* UpgradePoint,
    Int32* Seed,
    Bool* DestroyItem
) {
    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Item->Item.ID);
    if (!ItemData) return RUNTIME_CHAOS_UPGRADE_RESULT_ERROR;

    RTDataChaosUpgradeItemListRef UpgradeMain = RTRuntimeDataChaosUpgradeItemListGet(Runtime->Context, Item->Item.ID & RUNTIME_ITEM_MASK_INDEX);
    if (!UpgradeMain) return RUNTIME_CHAOS_UPGRADE_RESULT_ERROR;
    if (Item->Item.UpgradeLevel < UpgradeMain->CheckLevel) return RUNTIME_CHAOS_UPGRADE_RESULT_ERROR;
    if (Item->Item.UpgradeLevel >= UpgradeMain->ItemGrade) return RUNTIME_CHAOS_UPGRADE_RESULT_ERROR;

    RTDataChaosUpgradeGroupRateRef UpgradeGroupRate = RTRuntimeDataChaosUpgradeGroupRateGet(Runtime->Context, UpgradeMain->Group);
    if (!UpgradeGroupRate) return RUNTIME_CHAOS_UPGRADE_RESULT_ERROR;

    RTDataChaosUpgradeGroupRateLevelRef UpgradeRateValue = RTRuntimeDataChaosUpgradeGroupRateLevelGet(UpgradeGroupRate, Item->Item.UpgradeLevel);
    if (!UpgradeRateValue) return RUNTIME_CHAOS_UPGRADE_RESULT_ERROR;

    Int32 TotalRate = 0;
    for (Int Index = 0; Index < RUNTIME_DATA_UPGRADE_RATE_VALUE_COUNT; Index += 1) {
        TotalRate += UpgradeRateValue->Rates[Index];
    }
    
    /*
    Int32 UpgradePointRate = 3000 * (*UpgradePoint) / (3000 + (*UpgradePoint));
    TotalRate += UpgradePointRate;
    */

    Int32 Value = RandomRange(Seed, 0, MAX(0, TotalRate - 1));
    Int32 CurrentRate = 0;

    *DestroyItem = false;

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_CHAOS_UPGRADE_RATE_TYPE_UPGRADE_1];
    if (Value < CurrentRate) {
        *UpgradePoint = 0;
        Item->Item.UpgradeLevel += 1;
        return RUNTIME_CHAOS_UPGRADE_RESULT_UPGRADE;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_CHAOS_UPGRADE_RATE_TYPE_DOWNGRADE_RESET];
    if (Value < CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;

        if (UpgradeMain->CheckLevel > 0) {
            Item->Item.UpgradeLevel = 0;
            return RUNTIME_CHAOS_UPGRADE_RESULT_DESTROYED_SEAL;
        } else {
            *DestroyItem = true;
            Item->Item.UpgradeLevel = 0;
            return RUNTIME_CHAOS_UPGRADE_RESULT_DESTROYED;
        }
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_CHAOS_UPGRADE_RATE_TYPE_DOWNGRADE_0];
    if (Value < CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;
        return RUNTIME_CHAOS_UPGRADE_RESULT_DOWNGRADE_0;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_CHAOS_UPGRADE_RATE_TYPE_DOWNGRADE_1];
    if (Value < CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;
        Item->Item.UpgradeLevel -= 1;
        return RUNTIME_CHAOS_UPGRADE_RESULT_DOWNGRADE_1;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_CHAOS_UPGRADE_RATE_TYPE_DOWNGRADE_2];
    if (Value < CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;
        Item->Item.UpgradeLevel -= 2;
        return RUNTIME_CHAOS_UPGRADE_RESULT_DOWNGRADE_2;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_CHAOS_UPGRADE_RATE_TYPE_DOWNGRADE_3];
    if (Value <= CurrentRate) {
        *UpgradePoint += UpgradeRateValue->UpgradePoints;
        Item->Item.UpgradeLevel -= 3;
        return RUNTIME_CHAOS_UPGRADE_RESULT_DOWNGRADE_3;
    }

    Warn("Random range calculation should always result in perfect ranged matches!");
    return RUNTIME_CHAOS_UPGRADE_RESULT_ERROR;
}

Int32 RTItemUpgradeDivine(
    RTRuntimeRef Runtime,
    RTItemSlotRef Item,
    Int32* Seed,
    Int32* ResultLevel
) {
    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Item->Item.ID);
    if (!ItemData) {
        return RUNTIME_DIVINE_UPGRADE_RESULT_ERROR;
    }

    // convert other HELMED2 to HELMED
    Int32 ItemTypeOverride = ItemData->ItemType;
    if (ItemTypeOverride == RUNTIME_ITEM_TYPE_HELMED2) {
        ItemTypeOverride = RUNTIME_ITEM_TYPE_HELMED1;
    }

    RTDataDivineUpgradeMainRef UpgradeMain = RTRuntimeDataDivineUpgradeMainGet(Runtime->Context, ItemData->ItemGrade, ItemTypeOverride);
    if (!UpgradeMain)
        return RUNTIME_DIVINE_UPGRADE_RESULT_ERROR;

    RTDataDivineUpgradeGroupRateRef UpgradeGroupRate = RTRuntimeDataDivineUpgradeGroupRateGet(Runtime->Context, UpgradeMain->Group);
    if (!UpgradeGroupRate) return RUNTIME_DIVINE_UPGRADE_RESULT_ERROR;

    RTDataDivineUpgradeGroupRateLevelRef UpgradeRateValue = RTRuntimeDataDivineUpgradeGroupRateLevelGet(UpgradeGroupRate, Item->Item.DivineLevel);
    if (!UpgradeRateValue) return RUNTIME_DIVINE_UPGRADE_RESULT_ERROR;

    Int32 TotalRate = 0;
    for (Int Index = 0; Index < RUNTIME_DATA_UPGRADE_RATE_VALUE_COUNT; Index += 1) {
        TotalRate += UpgradeRateValue->Rates[Index];
    }

    Int32 Value = RandomRange(Seed, 0, MAX(0, TotalRate - 1));
    Int32 CurrentRate = 0;

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_DIVINE_UPGRADE_RATE_TYPE_UPGRADE_1];
    if (Value < CurrentRate) {
        Item->Item.DivineLevel += 1;
        *ResultLevel = 1;
        return RUNTIME_DIVINE_UPGRADE_RESULT_UPGRADE;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_DIVINE_UPGRADE_RATE_TYPE_DOWNGRADE_0];
    if (Value < CurrentRate) {
        return RUNTIME_DIVINE_UPGRADE_RESULT_DESTROYED;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_DIVINE_UPGRADE_RATE_TYPE_DOWNGRADE_RESET];
    if (Value < CurrentRate) {
        if (Item->Item.DivineLevel < 6) {
            *ResultLevel = 0;
            Item->Item.DivineLevel = 0;
            return RUNTIME_DIVINE_UPGRADE_RESULT_RESET;
        }

        *ResultLevel = 0;
        return RUNTIME_DIVINE_UPGRADE_RESULT_DOWNGRADE;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_DIVINE_UPGRADE_RATE_TYPE_DOWNGRADE_1];
    if (Value < CurrentRate) {
        Item->Item.DivineLevel -= 1;
        *ResultLevel = 1;
        return RUNTIME_DIVINE_UPGRADE_RESULT_DOWNGRADE;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_DIVINE_UPGRADE_RATE_TYPE_DOWNGRADE_2];
    if (Value < CurrentRate) {
        Item->Item.DivineLevel -= 2;
        *ResultLevel = 2;
        return RUNTIME_DIVINE_UPGRADE_RESULT_DOWNGRADE;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_DIVINE_UPGRADE_RATE_TYPE_DOWNGRADE_3];
    if (Value <= CurrentRate) {
        Item->Item.DivineLevel -= 3;
        *ResultLevel = 3;
        return RUNTIME_DIVINE_UPGRADE_RESULT_DOWNGRADE;
    }

    Warn("Random range calculation should always result in perfect ranged matches!");
    *ResultLevel = 0;
    return RUNTIME_DIVINE_UPGRADE_RESULT_ERROR;
}

Int32 RTItemUpgradeGet2HForceEffectIndex(
    Int32 InForceIndex
) {
    switch (InForceIndex) {
    case 3:
        return 23;
        break;
    case 4:
        return 24;
        break;
    case 6:
        return 26;
        break;
    case 8:
        return 28;
        break;
    case 9:
        return 29;
        break;
    case 51:
        return 52;
        break;
    case 41:
        return 42;
        break;
    case 30:
        return 31;
        break;
    case 32:
        return 33;
        break;
    case 113:
        return 115;
        break;
    case 10:
        return 11;
        break;
    case 39:
        return 40;
        break;
    case 43:
        return 44;
        break;
    case 49:
        return 50;
        break;
    case 114:
        return 116;
        break;
    }

    return 0;
}