#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTAchievementSlot {
    UInt16 AchievementIndex;
    UInt32 Unknown1;
    UInt16 Unknown2;
};

struct _RTAchievementRewardSlot {
    UInt16 TitleIndex;
    UInt32 Unknown1;
};

struct _RTAchievementExtendedRewardSlot {
    UInt16 TitleIndex;
    UInt32 Unknown1;
    UInt8 IsRewardTaken;
};

struct _RTCharacterAchievementInfo {
    Int32 AchievementCount;
    struct _RTAchievementSlot AchievementSlots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT];
    Int32 AchievementRewardCount;
    struct _RTAchievementRewardSlot AchievementRewardSlots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT];
    Int32 AchievementExtendedRewardCount;
    struct _RTAchievementExtendedRewardSlot AchievementExtendedRewardSlots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END
