#include "Runtime.h"
#include "Upgrade.h"

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
    for (Int32 Index = 0; Index < RUNTIME_DATA_UPGRADE_RATE_VALUE_COUNT; Index += 1) {
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
        // TODO: Calculate the item upgrade level based point addition
        *UpgradePoint += Item->Item.UpgradeLevel * 100;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_0;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_1];
    if (Value < CurrentRate) {
        // TODO: Calculate the item upgrade level based point addition
        *UpgradePoint += Item->Item.UpgradeLevel * 100;
        Item->Item.UpgradeLevel -= 1;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_1;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_2];
    if (Value < CurrentRate) {
        // TODO: Calculate the item upgrade level based point addition
        *UpgradePoint += Item->Item.UpgradeLevel * 100;
        Item->Item.UpgradeLevel -= 2;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_2;
    }

    CurrentRate += UpgradeRateValue->Rates[RUNTIME_UPGRADE_RATE_TYPE_DOWNGRADE_RESET];
    if (Value <= CurrentRate) {
        // TODO: Calculate the item upgrade level based point addition
        *UpgradePoint += Item->Item.UpgradeLevel * 100;
        Item->Item.UpgradeLevel -= 3;
        return RUNTIME_UPGRADE_RESULT_DOWNGRADE_3;
    }

    LogMessage(LOG_LEVEL_WARNING, "Random range calculation should always result in perfect ranged matches!");
    return RUNTIME_UPGRADE_RESULT_ERROR;
}
