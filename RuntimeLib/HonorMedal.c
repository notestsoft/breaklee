#include "Character.h"
#include "HonorMedal.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Int32 RTRuntimeGetHonorMedalMaxScore(
    RTRuntimeRef Runtime,
    Int32 CategoryIndex
) {
    RTDataHonorMedalScoreCategoryRef Category = RTRuntimeDataHonorMedalScoreCategoryGet(Runtime->Context, CategoryIndex);
    if (!Category) return 0;

    Int32 Score = 0;
    for (Index Index = 0; Index < Category->HonorMedalScoreMedalCount; Index += 1) {
        Score = MAX(Score, Category->HonorMedalScoreMedalList[Index].AccumulatedRequiredScore);
    }

    return Score;
}

Bool RTCharacterIsHonorMedalUnlocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex
) {
    RTDataHonorMedalMainRef HonorMedalMain = RTRuntimeDataHonorMedalMainGet(Runtime->Context, CategoryIndex);
    if (!HonorMedalMain) return false;
    
    return Character->Data.Info.HonorPoint >= HonorMedalMain->RequiredHonorPoint;
}

Bool RTCharacterAddHonorMedalScore(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 Score
) {
    assert(CategoryIndex == 0);
    
    if (!RTCharacterIsHonorMedalUnlocked(Runtime, Character, CategoryIndex)) return false;

    Int32 MaxScore = RTRuntimeGetHonorMedalMaxScore(Runtime, CategoryIndex);

    Character->Data.HonorMedalInfo.Info.Score = MIN(MaxScore, Character->Data.HonorMedalInfo.Info.Score + Score);
    Character->SyncMask.HonorMedalInfo = true;

    RTRuntimeBroadcastCharacterData(
        Runtime,
        Character,
        NOTIFICATION_CHARACTER_DATA_TYPE_HONOR_MEDAL
    );

    return true;
}

Int32 RTCharacterGetHonorMedalGrade(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex
) {
    if (!RTCharacterIsHonorMedalUnlocked(Runtime, Character, CategoryIndex)) return 0;

    RTDataHonorMedalScoreCategoryRef Category = RTRuntimeDataHonorMedalScoreCategoryGet(Runtime->Context, CategoryIndex);
    if (!Category) return 0;

    Int32 Grade = 0;
    for (Index Index = 0; Index < Category->HonorMedalScoreMedalCount; Index += 1) {
        Grade = MAX(Grade, Category->HonorMedalScoreMedalList[Index].Grade);

        if (Category->HonorMedalScoreMedalList[Index].AccumulatedRequiredScore > Character->Data.HonorMedalInfo.Info.Score) break;
    }

    return Grade;
}

Int32 RTCharacterGetHonorMedalSlotCount(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 GroupIndex
) {
    RTDataHonorMedalBasicCategoryRef Category = RTRuntimeDataHonorMedalBasicCategoryGet(Runtime->Context, CategoryIndex);
    if (!Category) return 0;

    RTDataHonorMedalBasicMedalRef Medal = RTRuntimeDataHonorMedalBasicMedalGet(Category, GroupIndex);
    if (!Medal) return 0;

    Int32 Grade = RTCharacterGetHonorMedalGrade(Runtime, Character, CategoryIndex);
    if (Grade < Medal->MinGrade) return 0;

    RTDataHonorMedalSlotCountCategoryRef SlotCategory = RTRuntimeDataHonorMedalSlotCountCategoryGet(Runtime->Context, CategoryIndex);
    if (!SlotCategory) return 0;

    RTDataHonorMedalSlotCountMedalRef SlotMedal = RTRuntimeDataHonorMedalSlotCountMedalGet(SlotCategory, GroupIndex);
    if (!SlotMedal) return 0;

    return SlotMedal->SlotCount;
}

Bool RTCharacterCanAddHonorMedalSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 GroupIndex,
    Int32 SlotIndex
) {
    if (Character->Data.HonorMedalInfo.Info.SlotCount >= RUNTIME_CHARACTER_MAX_HONOR_MEDAL_SLOT_COUNT) return false;

    Int32 MaxSlotCount = RTCharacterGetHonorMedalSlotCount(Runtime, Character, CategoryIndex, GroupIndex);
    if (SlotIndex < 0 || SlotIndex >= MaxSlotCount) return false;

    RTHonorMedalSlotRef Slot = RTCharacterGetHonorMedalSlot(Runtime, Character, CategoryIndex, GroupIndex, SlotIndex);
    if (Slot) return false;

    return true;
}

Void RTCharacterAddHonorMedalSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 GroupIndex,
    Int32 SlotIndex
) {
    assert(RTCharacterCanAddHonorMedalSlot(Runtime, Character, CategoryIndex, GroupIndex, SlotIndex));

    RTHonorMedalSlotRef Slot = &Character->Data.HonorMedalInfo.Slots[Character->Data.HonorMedalInfo.Info.SlotCount];
    Slot->CategoryIndex = CategoryIndex;
    Slot->GroupIndex = GroupIndex;
    Slot->SlotIndex = SlotIndex;
    Slot->ForceEffectIndex = 0;
    Slot->IsUnlocked = true;
    Character->Data.HonorMedalInfo.Info.SlotCount += 1;
    Character->SyncMask.HonorMedalInfo = true;
}

RTHonorMedalSlotRef RTCharacterGetHonorMedalSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 CategoryIndex,
    Int32 GroupIndex,
    Int32 SlotIndex
) {
    for (Index Index = 0; Index < Character->Data.HonorMedalInfo.Info.SlotCount; Index += 1) {
        RTHonorMedalSlotRef Slot = &Character->Data.HonorMedalInfo.Slots[Index];
        if (Slot->CategoryIndex != CategoryIndex) continue;
        if (Slot->GroupIndex != GroupIndex) continue;
        if (Slot->SlotIndex != SlotIndex) continue;

        return Slot;
    }

    return NULL;
}
