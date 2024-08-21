#pragma once

#include "Base.h"
#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTDailyQuestSlot {
    UInt16 QuestIndex;
};

struct _RTDailyQuestInfo {
    UInt32 SlotCount;
};

struct _RTCharacterDailyQuestInfo {
    struct _RTDailyQuestInfo Info;
    struct _RTDailyQuestSlot Slots[RUNTIME_CHARACTER_MAX_DAILY_QUEST_SLOT_COUNT];
};

#pragma pack(pop)

EXTERN_C_END