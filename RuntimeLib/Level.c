#include "Level.h"
#include "Runtime.h"

UInt8 RTRuntimeGetLevelByExp(
    RTRuntimeRef Runtime,
    UInt8 Level,
    UInt64* Exp
) {
    RTDataLevelRef LevelData = RTRuntimeDataLevelGet(Runtime->Context, Level);
    assert(LevelData);

    while (*Exp >= LevelData->AccumulatedExp) {
        Level += 1;
        RTDataLevelRef NextLevelData = RTRuntimeDataLevelGet(Runtime->Context, Level);
        if (!NextLevelData) {
            *Exp = MIN(*Exp, LevelData->AccumulatedExp);
            break;
        }

        LevelData = NextLevelData;
    }

    return LevelData->Level;
}

UInt64 RTRuntimeGetExpByLevel(
    RTRuntimeRef Runtime,
    UInt8 Level
) {
    RTDataLevelRef LevelData = RTRuntimeDataLevelGet(Runtime->Context, Level);
    assert(LevelData);
    return LevelData->AccumulatedExp;
}