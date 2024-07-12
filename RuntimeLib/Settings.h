#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterSettingsInfo {
    UInt32 HotKeysDataLength;
    UInt32 OptionsDataLength;
    UInt32 MacrosDataLength;
    UInt8 HotKeysData[RUNTIME_MAX_SETTINGS_DATA_LENGTH];
    UInt8 OptionsData[RUNTIME_MAX_SETTINGS_DATA_LENGTH];
    UInt8 MacrosData[RUNTIME_MAX_SETTINGS_DATA_LENGTH];
};

#pragma pack(pop)

EXTERN_C_END