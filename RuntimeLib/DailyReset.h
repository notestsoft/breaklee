#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterDailyResetInfo {
    Timestamp LastResetTimestamp;
    Timestamp NextResetTimestamp;
};

#pragma pack(pop)

Void RTCharacterInitializeDailyReset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

Void RTCharacterUpdateDailyReset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

EXTERN_C_END