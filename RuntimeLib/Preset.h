#pragma once

#include "Base.h"
#include "Constants.h"

#define RUNTIME_CHARACTER_MAX_PRESET_COUNT  5
#define RUNTIME_CHARACTER_MAX_PRESET_NAME_LENGTH 30
#define RUNTIME_CHARACTER_MAX_PRESET_DATA_COUNT  10

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTPresetConfiguration {
    Int8 PresetIndex;
    Char PresetName[RUNTIME_CHARACTER_MAX_PRESET_NAME_LENGTH];
    Int8 PresetData[RUNTIME_CHARACTER_MAX_PRESET_DATA_COUNT];
};

struct _RTCharacterPresetInfo {
    struct _RTPresetConfiguration Configurations[RUNTIME_CHARACTER_MAX_PRESET_COUNT];
    Int32 ActiveEquipmentPresetIndex;
    Int32 ActiveAnimaMasteryPresetIndex;
};

#pragma pack(pop)

EXTERN_C_END