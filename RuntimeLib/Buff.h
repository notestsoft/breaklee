#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTBuffSlot {
    Int32 SkillIndex;
    Int32 SkillLevel;
    Int32 Duration;
    UInt8 Unknown1[10];
    Int32 SkillTranscendenceLevel;
    Int32 SkillTranscendenceIndex;
    UInt8 Unknown2[66];
};

struct _RTBuffInfo {
    Int8 SkillBuffCount;
    Int8 PotionBuffCount;
    Int8 GmBuffCount;
    Int8 UnknownBuffCount1;
    Int8 UnknownBuffCount2;
    Int8 ForceWingBuffCount;
    Int8 FirePlaceBuffCount;
};

struct _RTCharacterBuffInfo {
    struct _RTBuffInfo Info;
    struct _RTBuffSlot Slots[RUNTIME_CHARACTER_MAX_BUFF_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END