#include "Timer.h"

Void RTTimerInitialize(
    RTTimerRef Timer,
    Timestamp UpdateInterval
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    Timer->LastUpdateTimestamp = CurrentTimestamp;
    Timer->NextUpdateTimestamp = CurrentTimestamp;
    Timer->NextUpdateTimestamp = (UpdateInterval > 0) ? CurrentTimestamp + UpdateInterval : UINT64_MAX;
    Timer->UpdateInterval = UpdateInterval;
    Timer->ElapsedInterval = 0;
}

Bool RTTimerUpdate(
    RTTimerRef Timer,
    Bool ForceUpdate
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    if (!ForceUpdate && Timer->NextUpdateTimestamp > CurrentTimestamp) return false;

    Timer->ElapsedInterval = CurrentTimestamp - Timer->LastUpdateTimestamp;
    Timer->LastUpdateTimestamp = CurrentTimestamp;
    Timer->NextUpdateTimestamp = (Timer->UpdateInterval > 0) ? CurrentTimestamp + Timer->UpdateInterval : UINT64_MAX;

    return true;
}

Void RTTimerAddUpdateTimestamp(
    RTTimerRef Timer,
    Timestamp UpdateTimestamp
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    Timer->NextUpdateTimestamp = MIN(Timer->NextUpdateTimestamp, UpdateTimestamp);
}