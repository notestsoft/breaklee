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

struct _RTAchievementInfo {
    Int32 AllAchievementScore;
    Int32 NormalAchievementScore;
    Int32 QuestAchievementScore;
    Int32 DungeonAchievementScore;
    Int32 ItemsAchievementScore;
    Int32 PvpAchievementScore;
    Int32 WarAchievementScore;
    Int32 HuntingAchievementScore;
    Int32 CraftAchievementScore;
    Int32 CommunityAchievementScore;
    Int32 SharedAchievementScore;
    Int32 SpecialAchievementScore;
    Int32 GeneralMemoirAchievementScore;
    Int32 SharedMemoirAchievementScore;
    UInt16 DisplayTitle;
    UInt16 EventTitle;
    UInt16 GuildTitle;
    UInt16 WarTitle;
    Int32 SlotCount;
    Int32 RewardSlotCount;
};

struct _RTCharacterAchievementInfo {
    struct _RTAchievementInfo Info;
    struct _RTAchievementSlot Slots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT];
    struct _RTAchievementRewardSlot RewardSlots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT];
};

struct _RTAchievementExtendedRewardSlot {
    UInt16 TitleIndex;
    UInt32 Unknown1;
    UInt8 IsRewardTaken;
};

struct _RTAchievementExtendedInfo {
    Int32 SlotCount;
};

struct _RTCharacterAchievementExtendedInfo {
    struct _RTAchievementExtendedInfo Info;
    struct _RTAchievementExtendedRewardSlot Slots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END
