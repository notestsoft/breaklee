#pragma once

#include "Base.h"
#include "Constants.h"

struct _RTTimer {
    Timestamp LastUpdateTimestamp;
    Timestamp NextUpdateTimestamp;
    Timestamp UpdateInterval;
    Timestamp ElapsedInterval;
};
typedef struct _RTTimer RTTimer;

EXTERN_C_BEGIN

Void RTTimerInitialize(
    RTTimerRef Timer,
    Timestamp UpdateInterval
);

Bool RTTimerUpdate(
    RTTimerRef Timer,
    Bool ForceUpdate
);

Void RTTimerAddUpdateTimestamp(
    RTTimerRef Timer,
    Timestamp UpdateTimestamp
);

EXTERN_C_END