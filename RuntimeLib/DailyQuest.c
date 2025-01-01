#include "DailyQuest.h"
#include "Quest.h"
#include "Runtime.h"

Void RTCharacterResetDailyQuests(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    for (Int Index = 0; Index < Character->Data.DailyQuestInfo.Info.SlotCount; Index += 1) {
        RTDailyQuestSlotRef DailyQuestSlot = &Character->Data.DailyQuestInfo.Slots[Index];
        RTCharacterQuestFlagClear(Character, DailyQuestSlot->QuestIndex);
    }

    Character->Data.DailyQuestInfo.Info.SlotCount = 0;
    Character->SyncMask.DailyQuestInfo = true;
}

Void RTCharacterDailyQuestClear(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt16 QuestIndex
) {
    RTQuestDataRef QuestData = RTRuntimeGetQuestByIndex(Runtime, QuestIndex);
    if (!QuestData) return;
    if (QuestData->DailyCount < 1) return;

    if (RTCharacterIsDailyQuestCleared(Runtime, Character, QuestIndex)) return;

    assert(Character->Data.DailyQuestInfo.Info.SlotCount < RUNTIME_CHARACTER_MAX_DAILY_QUEST_SLOT_COUNT);
    RTDailyQuestSlotRef DailyQuestSlot = &Character->Data.DailyQuestInfo.Slots[Character->Data.DailyQuestInfo.Info.SlotCount];
    DailyQuestSlot->QuestIndex = QuestIndex;
    Character->Data.DailyQuestInfo.Info.SlotCount += 1;
    Character->SyncMask.DailyQuestInfo = true;
}

Bool RTCharacterIsDailyQuestCleared(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt16 QuestIndex
) {
    for (Int Index = 0; Index < Character->Data.DailyQuestInfo.Info.SlotCount; Index += 1) {
        RTDailyQuestSlotRef DailyQuestSlot = &Character->Data.DailyQuestInfo.Slots[Index];
        if (DailyQuestSlot->QuestIndex == QuestIndex) return true;
    }

    return false;
}
