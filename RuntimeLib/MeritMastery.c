#include "Character.h"
#include "MeritMastery.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Int32 RTMeritMasteryGetCategoryIndex(
	RTRuntimeRef Runtime,
	Int32 MasteryIndex
) {
	for (Int CategoryIndex = 0; CategoryIndex < Runtime->Context->MeritMasteryCategoryCount; CategoryIndex += 1) {
		RTDataMeritMasteryCategoryRef Category = &Runtime->Context->MeritMasteryCategoryList[CategoryIndex];
		for (Int SlotIndex = 0; SlotIndex < Category->MeritMasteryBaseCount; SlotIndex += 1) {
			RTDataMeritMasteryBaseRef MasteryBase = &Category->MeritMasteryBaseList[SlotIndex];
			if (MasteryBase->MasteryIndex != MasteryIndex) continue;

			return Category->Index;
		}
	}

	return 0;
}

Bool RTRuntimeDataGetMeritMasteryData(
	RTRuntimeRef Runtime,
	Int32 MasteryIndex,
	RTDataMeritMasteryCategoryRef* ResultCategory,
	RTDataMeritMasteryBaseRef* ResultMasteryBase
) {
	for (Int CategoryIndex = 0; CategoryIndex < Runtime->Context->MeritMasteryCategoryCount; CategoryIndex += 1) {
		RTDataMeritMasteryCategoryRef Category = &Runtime->Context->MeritMasteryCategoryList[CategoryIndex];
		for (Int SlotIndex = 0; SlotIndex < Category->MeritMasteryBaseCount; SlotIndex += 1) {
			RTDataMeritMasteryBaseRef MasteryBase = &Category->MeritMasteryBaseList[SlotIndex];
			if (MasteryBase->MasteryIndex != MasteryIndex) continue;

			*ResultCategory = Category;
			*ResultMasteryBase = MasteryBase;
			return true;
		}
	}

	return false;
}

Void RTCharacterInitializeMeritMastery(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	RTCharacterUpdateMeritMastery(Runtime, Character, true);

	// TODO: Calculate character attributes!
}

Void RTCharacterUpdateMeritMastery(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Bool ForceUpdate
) {
	Timestamp CurrentTimestamp = GetTimestamp();

	if (Character->Data.PlatinumMeritMasteryInfo.Info.IsEnabled &&
		Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex > 0 &&
		Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotUnlockTime <= CurrentTimestamp) {
		assert(Character->Data.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount < RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT);
		
		RTPlatinumMeritUnlockedSlotRef UnlockedSlot = &Character->Data.PlatinumMeritMasteryInfo.UnlockedSlots[Character->Data.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount];
		UnlockedSlot->MasteryIndex = Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex;
		Character->Data.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount += 1;

		NOTIFICATION_DATA_MERIT_MASTERY_OPEN_SLOT* Notification = RTNotificationInit(MERIT_MASTERY_OPEN_SLOT);
		Notification->MasteryIndex = Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex;
		RTNotificationDispatchToCharacter(Notification, Character);

		Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex = -1;
		Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotUnlockTime = 0;
		Character->SyncMask.PlatinumMeritMasteryInfo = true;
	}

	if (Character->Data.DiamondMeritMasteryInfo.Info.IsEnabled &&
		Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex > 0 &&
		Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotUnlockTime <= CurrentTimestamp) {

		Int32 CategoryIndex = RTMeritMasteryGetCategoryIndex(
			Runtime, 
			Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex
		);
		RTDataMeritMasteryGroupRef MasteryGroup = RTRuntimeDataMeritMasteryGroupGet(
			Runtime->Context,
			RUNTIME_MERIT_MASTERY_GRADE_DIAMOND,
			CategoryIndex
		);
		Bool IsMasteryGroup = false;
		if (MasteryGroup) {
			for (Int32 Index = 0; Index < MasteryGroup->MasteryIndexCount; Index += 1) {
				if (MasteryGroup->MasteryIndex[Index] != Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex) continue;

				IsMasteryGroup = true;
				break;
			}

		}

		if (MasteryGroup && IsMasteryGroup) {
			for (Int32 Index = 0; Index < MasteryGroup->MasteryIndexCount; Index += 1) {
				assert(Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount < RUNTIME_CHARACTER_MAX_DIAMOND_MERIT_MASTERY_SLOT_COUNT);
				RTDiamondMeritUnlockedSlotRef UnlockedSlot = &Character->Data.DiamondMeritMasteryInfo.UnlockedSlots[Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount];
				UnlockedSlot->MasteryIndex = MasteryGroup->MasteryIndex[Index];
				Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount += 1;

				NOTIFICATION_DATA_DIAMOND_MERIT_MASTERY_OPEN_SLOT* Notification = RTNotificationInit(DIAMOND_MERIT_MASTERY_OPEN_SLOT);
				assert(MasteryGroup->MasteryIndexCount <= 3);
				Notification->MasteryIndex[Index] = MasteryGroup->MasteryIndex[Index];
				RTNotificationDispatchToCharacter(Notification, Character);
			}
		}
		else {
			assert(Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount < RUNTIME_CHARACTER_MAX_DIAMOND_MERIT_MASTERY_SLOT_COUNT);
			RTDiamondMeritUnlockedSlotRef UnlockedSlot = &Character->Data.DiamondMeritMasteryInfo.UnlockedSlots[Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount];
			UnlockedSlot->MasteryIndex = Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex;
			Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount += 1;

			NOTIFICATION_DATA_DIAMOND_MERIT_MASTERY_OPEN_SLOT* Notification = RTNotificationInit(DIAMOND_MERIT_MASTERY_OPEN_SLOT);
			Notification->MasteryIndex[0] = Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex;
			RTNotificationDispatchToCharacter(Notification, Character);
		}

		Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex = -1;
		Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotUnlockTime = 0;
		Character->SyncMask.DiamondMeritMasteryInfo = true;
	}
}

RTGoldMeritMasterySlotRef RTCharacterGoldMeritAddMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 MasteryLevel
) {
	if (Character->Data.GoldMeritMasteryInfo.Info.SlotCount >= RUNTIME_CHARACTER_MAX_GOLD_MERIT_MASTERY_SLOT_COUNT) return NULL;

	RTGoldMeritMasterySlotRef MasterySlot = &Character->Data.GoldMeritMasteryInfo.Slots[Character->Data.GoldMeritMasteryInfo.Info.SlotCount];
	MasterySlot->Index = MasteryIndex;
	MasterySlot->Level = MasteryLevel;
	Character->Data.GoldMeritMasteryInfo.Info.SlotCount += 1;
	Character->SyncMask.GoldMeritMasteryInfo = true;
	return MasterySlot;
}

RTGoldMeritMasterySlotRef RTCharacterGoldMeritGetMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex
) {
	for (Int Index = 0; Index < Character->Data.GoldMeritMasteryInfo.Info.SlotCount; Index += 1) {
		RTGoldMeritMasterySlotRef MasterySlot = &Character->Data.GoldMeritMasteryInfo.Slots[Index];
		if (MasterySlot->Index != MasteryIndex) continue;

		return MasterySlot;
	}

	return NULL;
}

Void RTCharacterGoldMeritMasteryAddExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Exp
) {
	Int32 CurrentScore = Character->Data.GoldMeritMasteryInfo.Info.Exp;
	Int32 NewScore = Character->Data.GoldMeritMasteryInfo.Info.Exp + Exp;
	Int32 TotalRewardPoints = 0;

	for (Int32 StartIndex = 0; StartIndex < Runtime->Context->GoldMeritScoreFormulaCount; StartIndex += 1) {
		RTDataGoldMeritScoreFormulaRef Formula = &Runtime->Context->GoldMeritScoreFormulaList[StartIndex];
		if (Formula->MinScore > CurrentScore) continue;
		if (Formula->MaxScore <= CurrentScore) continue;

		for (Int32 Index = StartIndex; Index < Runtime->Context->GoldMeritScoreFormulaCount; Index += 1) {
			RTDataGoldMeritScoreFormulaRef NextFormula = &Runtime->Context->GoldMeritScoreFormulaList[Index];
			if (NextFormula->MaxScore > NewScore) break;

			TotalRewardPoints += NextFormula->RewardPoints;
		}
	}

	if (Runtime->Context->GoldMeritScoreFormulaCount > 0) {
		// NOTE: This assumes the last one is the biggest value and the list is sorted correctly!
		RTDataGoldMeritScoreFormulaRef Formula = &Runtime->Context->GoldMeritScoreFormulaList[Runtime->Context->GoldMeritScoreFormulaCount - 1];
		NewScore = MIN(NewScore, Formula->MaxScore);
	}

	Character->Data.GoldMeritMasteryInfo.Info.Exp = NewScore;
	Character->Data.GoldMeritMasteryInfo.Info.Points += TotalRewardPoints;
	Character->SyncMask.GoldMeritMasteryInfo = true;
}

Void RTPlatinumMeritMasteryRollSpecialMastery(
	RTDataMeritMasterySpecialMasteryPoolCategoryRef Category,
	Int32* Seed,
	RTPlatinumMeritSpecialMasterySlotRef Result
) {
	assert(Result->Category == Category->Index);

	Result->Index = 0;
	Result->Grade = 0;

	Int32 RandomRate = RandomRange(Seed, 0, 100000);
	Int32 RandomRateOffset = 0;
	for (Int MasteryIndex = 0; MasteryIndex < Category->MeritMasterySpecialMasteryPoolMasteryCount; MasteryIndex += 1) {
		RTDataMeritMasterySpecialMasteryPoolMasteryRef Mastery = &Category->MeritMasterySpecialMasteryPoolMasteryList[MasteryIndex];
		for (Int OptionIndex = 0; OptionIndex < Mastery->MeritMasterySpecialMasteryPoolOptionCount; OptionIndex += 1) {
			RTDataMeritMasterySpecialMasteryPoolOptionRef Option = &Mastery->MeritMasterySpecialMasteryPoolOptionList[OptionIndex];
			RandomRateOffset += Option->Rate;
			if (RandomRate <= RandomRateOffset) {
				Result->Index = Mastery->Index;
				Result->Grade = Option->Grade;
				return;
			}
		}
	}
}

Bool RTCharacterPlatinumMeritMasteryGradeUp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
) {
	if (Character->Data.PlatinumMeritMasteryInfo.Info.IsEnabled) return false;

	// NOTE: This assumes the last one is the biggest value and the list is sorted correctly!
	RTDataGoldMeritScoreFormulaRef Formula = &Runtime->Context->GoldMeritScoreFormulaList[Runtime->Context->GoldMeritScoreFormulaCount - 1];
	if (Character->Data.GoldMeritMasteryInfo.Info.Exp < Formula->MaxScore) return false;
	if (Character->Data.GoldMeritMasteryInfo.Info.Points > 0) return false;

	RTDataMeritMasteryGradeRef MasteryGrade = RTRuntimeDataMeritMasteryGradeGet(Runtime->Context, RUNTIME_MERIT_MASTERY_GRADE_PLATINUM);
	if (!MasteryGrade) return false;
	if (Character->Data.Info.Wexp < MasteryGrade->RequiredWexp) return false;

	if (MasteryGrade->RequiredItemID > 0) {
		Bool CanConsumeItems = RTInventoryCanConsumeStackableItems(
			Runtime,
			&Character->Data.InventoryInfo,
			MasteryGrade->RequiredItemID,
			MasteryGrade->RequiredItemCount,
			InventorySlotCount,
			InventorySlotIndex
		);
		if (!CanConsumeItems) return false;

		RTInventoryConsumeStackableItems(
			Runtime,
			&Character->Data.InventoryInfo,
			MasteryGrade->RequiredItemID,
			MasteryGrade->RequiredItemCount,
			InventorySlotCount,
			InventorySlotIndex
		);

		Character->SyncMask.InventoryInfo = true;
	}

	Character->Data.Info.Wexp -= MasteryGrade->RequiredWexp;
	Character->SyncMask.Info = true;

	Character->Data.PlatinumMeritMasteryInfo.Info.IsEnabled = true;
	Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex = 1;
	Character->Data.PlatinumMeritMasteryInfo.Info.TotalMemorizeCount = RUNTIME_CHARACTER_DEFAULT_PLATINUM_MERIT_MEMORIZE_COUNT;
	Character->Data.PlatinumMeritMasteryInfo.Info.ExtendedMemorizeCount = 0;

	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots[Index].MemorizeIndex = Index + 1;
		Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots[Index].Points = 0;
	}

	Character->SyncMask.PlatinumMeritMasteryInfo = true;
	return true;
}

Bool RTCharacterPlatinumMeritIsSlotOpen(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex
) {
	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount; Index += 1) {
		RTPlatinumMeritUnlockedSlotRef UnlockedSlot = &Character->Data.PlatinumMeritMasteryInfo.UnlockedSlots[Index];
		if (UnlockedSlot->MasteryIndex == MasteryIndex) return true;
	}

	return false;
}

RTPlatinumMeritMasterySlotRef RTCharacterPlatinumMeritAddMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MemorizeIndex,
	Int32 MasteryIndex,
	Int32 MasteryLevel
) {
	if (Character->Data.PlatinumMeritMasteryInfo.Info.MasterySlotCount >= RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT) return NULL;

	RTPlatinumMeritMasterySlotRef MasterySlot = &Character->Data.PlatinumMeritMasteryInfo.MasterySlots[Character->Data.PlatinumMeritMasteryInfo.Info.MasterySlotCount];
	MasterySlot->MemorizeIndex = MemorizeIndex;
	MasterySlot->Index = MasteryIndex;
	MasterySlot->Level = MasteryLevel;
	Character->Data.PlatinumMeritMasteryInfo.Info.MasterySlotCount += 1;
	Character->SyncMask.PlatinumMeritMasteryInfo = true;
	return MasterySlot;
}

RTPlatinumMeritMasterySlotRef RTCharacterPlatinumMeritGetMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MemorizeIndex,
	Int32 MasteryIndex
) {
	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.MasterySlotCount; Index += 1) {
		RTPlatinumMeritMasterySlotRef MasterySlot = &Character->Data.PlatinumMeritMasteryInfo.MasterySlots[Index];
		if (MasterySlot->MemorizeIndex != MemorizeIndex) continue;
		if (MasterySlot->Index != MasteryIndex) continue;

		return MasterySlot;
	}

	return NULL;
}

Void RTCharacterPlatinumMeritMasteryAddExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Exp
) {
	Int32 CurrentScore = Character->Data.PlatinumMeritMasteryInfo.Info.Exp;
	Int32 NewScore = Character->Data.PlatinumMeritMasteryInfo.Info.Exp + Exp;
	Int32 TotalRewardPoints = 0;

	for (Int32 StartIndex = 0; StartIndex < Runtime->Context->PlatinumMeritScoreFormulaCount; StartIndex += 1) {
		RTDataPlatinumMeritScoreFormulaRef Formula = &Runtime->Context->PlatinumMeritScoreFormulaList[StartIndex];
		if (Formula->MinScore > CurrentScore) continue;
		if (Formula->MaxScore <= CurrentScore) continue;

		for (Int32 Index = StartIndex; Index < Runtime->Context->PlatinumMeritScoreFormulaCount; Index += 1) {
			RTDataPlatinumMeritScoreFormulaRef NextFormula = &Runtime->Context->PlatinumMeritScoreFormulaList[Index];
			if (NextFormula->MaxScore > NewScore) break;

			TotalRewardPoints += NextFormula->RewardPoints;
		}
	}

	if (Runtime->Context->PlatinumMeritScoreFormulaCount > 0) {
		// NOTE: This assumes the last one is the biggest value and the list is sorted correctly!
		RTDataPlatinumMeritScoreFormulaRef Formula = &Runtime->Context->PlatinumMeritScoreFormulaList[Runtime->Context->PlatinumMeritScoreFormulaCount - 1];
		NewScore = MIN(NewScore, Formula->MaxScore);
	}

	Character->Data.PlatinumMeritMasteryInfo.Info.Exp = NewScore;
	Character->Data.PlatinumMeritMasteryInfo.Info.Points += TotalRewardPoints;

	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots[Index].Points += TotalRewardPoints;
	}

	Character->SyncMask.PlatinumMeritMasteryInfo = true;
}

Int32 RTCharacterPlatinumMeritMasteryGetSlotCount(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTDataMeritMasteryCategoryRef Category,
	Int8 MemorizeIndex
) {
	Int32 SlotCount = 0;

	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.MasterySlotCount; Index += 1) {
		if (Character->Data.PlatinumMeritMasteryInfo.MasterySlots[Index].MemorizeIndex != MemorizeIndex) continue;

		for (Int SlotIndex = 0; SlotIndex < Category->MeritMasteryBaseCount; SlotIndex += 1) {
			RTDataMeritMasteryBaseRef MasteryBase = &Category->MeritMasteryBaseList[SlotIndex];
			if (MasteryBase->MasteryType > 1) continue;
			if (MasteryBase->MasteryIndex == Character->Data.PlatinumMeritMasteryInfo.MasterySlots[Index].Index) {
				SlotCount += 1;
			}
		}
	}

	return SlotCount;
}

Int32 RTCharacterPlatinumMeritMasteryGetOpenedSlotCount(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTDataMeritMasteryCategoryRef Category
) {
	Int32 SlotCount = 0;

	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount; Index += 1) {
		for (Int SlotIndex = 0; SlotIndex < Category->MeritMasteryBaseCount; SlotIndex += 1) {
			RTDataMeritMasteryBaseRef MasteryBase = &Category->MeritMasteryBaseList[SlotIndex];
			if (MasteryBase->MasteryIndex == Character->Data.PlatinumMeritMasteryInfo.UnlockedSlots[Index].MasteryIndex) {
				SlotCount += 1;
			}
		}
	}

	return SlotCount;
}

Bool RTCharacterPlatinumMeritMasteryOpenSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	UInt16 InventorySlotCount1,
	UInt16* InventorySlotIndices1,
	UInt16 InventorySlotCount2,
	UInt16* InventorySlotIndices2
) {
	if (!Character->Data.PlatinumMeritMasteryInfo.Info.IsEnabled) return false;
	
	RTCharacterUpdateMeritMastery(Runtime, Character, true);

	if (Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex > 0) return false;

	RTDataMeritMasteryCategoryRef Category = NULL;
	RTDataMeritMasteryBaseRef MasteryBase = NULL;
	if (!RTRuntimeDataGetMeritMasteryData(
		Runtime, 
		MasteryIndex, 
		&Category, 
		&MasteryBase
	)) return false;

	Int32 TrainedSlotCount = RTCharacterPlatinumMeritMasteryGetSlotCount(Runtime, Character, Category, Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex);
	Int32 OpenedSlotCount = RTCharacterPlatinumMeritMasteryGetOpenedSlotCount(Runtime, Character, Category);

	RTDataMeritMasterySlotOpenCategoryRef SlotOpenCategory = RTRuntimeDataMeritMasterySlotOpenCategoryGet(Runtime->Context, Category->Index);
	if (!SlotOpenCategory) return false;
	RTDataMeritMasterySlotOpenMaterialRef SlotOpenMaterial1 = RTRuntimeDataMeritMasterySlotOpenMaterialGet(SlotOpenCategory, TrainedSlotCount, 0);
	if (!SlotOpenMaterial1) return false;
	RTDataMeritMasterySlotOpenMaterialRef SlotOpenMaterial2 = RTRuntimeDataMeritMasterySlotOpenMaterialGet(SlotOpenCategory, TrainedSlotCount, 1);
	if (!SlotOpenMaterial2) return false;
	RTDataMeritMasterySlotOpenTimeRef SlotOpenTime = RTRuntimeDataMeritMasterySlotOpenTimeGet(Runtime->Context, Category->Index, OpenedSlotCount);
	if (!SlotOpenTime) return false;

	if (SlotOpenMaterial1->RequiredItemID > 0 && SlotOpenMaterial1->RequiredItemCount > 0) {
		Int64 TotalConsumableItemCount = 0;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount1; SlotIndex += 1) {
			Int64 ConsumableItemCount = RTInventoryGetConsumableItemCount(
				Runtime,
				&Character->Data.InventoryInfo,
				SlotOpenMaterial1->RequiredItemID,
				0,
				InventorySlotIndices1[SlotIndex]
			);
			if (ConsumableItemCount < 1) return false;

			TotalConsumableItemCount += ConsumableItemCount;
		}

		if (TotalConsumableItemCount < SlotOpenMaterial1->RequiredItemCount) return false;
	}

	if (SlotOpenMaterial2->RequiredItemID > 0 && SlotOpenMaterial2->RequiredItemCount > 0) {
		Int64 TotalConsumableItemCount = 0;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount2; SlotIndex += 1) {
			Int64 ConsumableItemCount = RTInventoryGetConsumableItemCount(
				Runtime,
				&Character->Data.InventoryInfo,
				SlotOpenMaterial2->RequiredItemID,
				0,
				InventorySlotIndices2[SlotIndex]
			);
			if (ConsumableItemCount < 1) return false;

			TotalConsumableItemCount += ConsumableItemCount;
		}

		if (TotalConsumableItemCount < SlotOpenMaterial2->RequiredItemCount) return false;
	}

	if (SlotOpenMaterial1->RequiredItemID > 0 && SlotOpenMaterial1->RequiredItemCount > 0) {
		Int64 RemainingItemCount = SlotOpenMaterial1->RequiredItemCount;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount1; SlotIndex += 1) {
			Int64 ConsumedItemCount = RTInventoryConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				SlotOpenMaterial1->RequiredItemID,
				0,
				RemainingItemCount,
				InventorySlotIndices1[SlotIndex]
			);

			RemainingItemCount -= ConsumedItemCount;
		}

		Character->SyncMask.InventoryInfo = true;
	}

	if (SlotOpenMaterial2->RequiredItemID > 0 && SlotOpenMaterial2->RequiredItemCount > 0) {
		Int64 RemainingItemCount = SlotOpenMaterial2->RequiredItemCount;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount2; SlotIndex += 1) {
			Int64 ConsumedItemCount = RTInventoryConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				SlotOpenMaterial2->RequiredItemID,
				0,
				RemainingItemCount,
				InventorySlotIndices2[SlotIndex]
			);

			RemainingItemCount -= ConsumedItemCount;
		}

		Character->SyncMask.InventoryInfo = true;
	}

	Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotMasteryIndex = MasteryIndex;
	Character->Data.PlatinumMeritMasteryInfo.Info.OpenSlotUnlockTime = GetTimestamp() + SlotOpenTime->OpenTime;
	Character->SyncMask.PlatinumMeritMasteryInfo = true;
	return true;
}

Bool RTCharacterPlatinumMeritMasterySetActiveMemorizeIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MemorizeIndex
) {
	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		RTPlatinumMeritExtendedMemorizeSlotRef MemorizeSlot = &Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots[Index];
		if (MemorizeSlot->MemorizeIndex == MemorizeIndex) {
			Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex = MemorizeIndex;
			Character->SyncMask.PlatinumMeritMasteryInfo = true;
			return true;
		}
	}

	return false;
}

Int32 RTCharacterPlatinumMeritMasteryGetPoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		RTPlatinumMeritExtendedMemorizeSlotRef MemorizeSlot = &Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots[Index];
		if (MemorizeSlot->MemorizeIndex != Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex) continue;

		return MemorizeSlot->Points;
	}

	return 0;
}

Void RTCharacterPlatinumMeritSetPoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Points
) {
	for (Int Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		RTPlatinumMeritExtendedMemorizeSlotRef MemorizeSlot = &Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots[Index];
		if (MemorizeSlot->MemorizeIndex != Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex) continue;

		MemorizeSlot->Points = Points;
		break;
	}
}

Void RTCharacterPlatinumMeritMasteryGetSpecialMasterySlots(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int8 MemorizeIndex,
	Int32 CategoryIndex,
	RTPlatinumMeritSpecialMasterySlotRef* Result1,
	RTPlatinumMeritSpecialMasterySlotRef* Result2
) {
	assert(Result1 && Result2);

	*Result1 = NULL;
	*Result2 = NULL;

	for (Int32 Index = 0; Index < Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount; Index += 1) {
		RTPlatinumMeritSpecialMasterySlotRef Slot = &Character->Data.PlatinumMeritMasteryInfo.SpecialMasterySlots[Index];
		if (Slot->MemorizeIndex != MemorizeIndex) continue;
		if (Slot->Category != CategoryIndex) continue;

		if (!*Result1) *Result1 = Slot;
		else if (!*Result2) *Result2 = Slot;
		else break;
	}
}

Bool RTCharacterPlatinumMeritMasteryGrantSpecialMastery(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 CategoryIndex,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
) {
	if (!Character->Data.PlatinumMeritMasteryInfo.Info.IsEnabled) return false;

	RTDataMeritMasterySpecialMasteryMaterialRef MasteryMaterial = RTRuntimeDataMeritMasterySpecialMasteryMaterialGet(
		Runtime->Context,
		RUNTIME_MERIT_MASTERY_GRADE_PLATINUM
	);
	if (!MasteryMaterial) return false;
	if (Character->Data.Info.Wexp < MasteryMaterial->RequiredWexp) return false;

	RTDataMeritMasterySpecialMasteryPoolRef Pool = RTRuntimeDataMeritMasterySpecialMasteryPoolGet(
		Runtime->Context,
		RUNTIME_MERIT_MASTERY_GRADE_PLATINUM
	);
	if (!Pool) return false;

	RTDataMeritMasterySpecialMasteryPoolCategoryRef Category = RTRuntimeDataMeritMasterySpecialMasteryPoolCategoryGet(
		Pool,
		CategoryIndex
	);
	if (!Category) return false;

	if (MasteryMaterial->RequiredItemID > 0 && MasteryMaterial->RequiredItemCount > 0) {
		Int64 TotalConsumableItemCount = 0;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount; SlotIndex += 1) {
			Int64 ConsumableItemCount = RTInventoryGetConsumableItemCount(
				Runtime,
				&Character->Data.InventoryInfo,
				MasteryMaterial->RequiredItemID,
				0,
				InventorySlotIndex[SlotIndex]
			);
			if (ConsumableItemCount < 1) return false;

			TotalConsumableItemCount += ConsumableItemCount;
		}

		if (TotalConsumableItemCount < MasteryMaterial->RequiredItemCount) return false;

		Int64 RemainingItemCount = MasteryMaterial->RequiredItemCount;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount; SlotIndex += 1) {
			Int64 ConsumedItemCount = RTInventoryConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				MasteryMaterial->RequiredItemID,
				0,
				RemainingItemCount,
				InventorySlotIndex[SlotIndex]
			);

			RemainingItemCount -= ConsumedItemCount;
		}

		Character->SyncMask.InventoryInfo = true;
	}

	RTPlatinumMeritSpecialMasterySlotRef SpecialMasterySlots[2] = { NULL, NULL };
	RTCharacterPlatinumMeritMasteryGetSpecialMasterySlots(
		Runtime,
		Character,
		Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex,
		CategoryIndex,
		&SpecialMasterySlots[0],
		&SpecialMasterySlots[1]
	);

	// TODO: Apply sorted insert for clean data structure (by MemorizeIndex, Category)
	if (!SpecialMasterySlots[0]) {
		assert(Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount < RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_SPECIAL_MASTERY_SLOT_COUNT);
		SpecialMasterySlots[0] = &Character->Data.PlatinumMeritMasteryInfo.SpecialMasterySlots[Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount];
		SpecialMasterySlots[0]->MemorizeIndex = Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex;
		SpecialMasterySlots[0]->Category = CategoryIndex;
		SpecialMasterySlots[0]->Index = 0;
		SpecialMasterySlots[0]->Grade = 0;
		Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount += 1;
	}

	if (!SpecialMasterySlots[1]) {
		assert(Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount < RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_SPECIAL_MASTERY_SLOT_COUNT);
		SpecialMasterySlots[1] = &Character->Data.PlatinumMeritMasteryInfo.SpecialMasterySlots[Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount];
		SpecialMasterySlots[1]->MemorizeIndex = Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex;
		SpecialMasterySlots[1]->Category = CategoryIndex;
		SpecialMasterySlots[1]->Index = 0;
		SpecialMasterySlots[1]->Grade = 0;
		Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount += 1;
	}

	Int32 Seed = (Int32)PlatformGetTickCount();
	RTPlatinumMeritMasteryRollSpecialMastery(Category, &Seed, SpecialMasterySlots[0]);
	RTPlatinumMeritMasteryRollSpecialMastery(Category, &Seed, SpecialMasterySlots[1]);
	Character->SyncMask.PlatinumMeritMasteryInfo = true;

	Character->Data.Info.Wexp -= MasteryMaterial->RequiredWexp;
	Character->SyncMask.Info = true;

	return true;
}

Void RTDiamondMeritMasteryRollSpecialMastery(
	RTDataMeritMasterySpecialMasteryPoolCategoryRef Category,
	Int32* Seed,
	Int32* RewardExp,
	RTDiamondMeritSpecialMasterySlotRef Result
) {
	assert(Result->Category == Category->Index);

	Result->Index = 0;
	Result->Grade = 0;

	Int32 RandomRate = RandomRange(Seed, 0, 100000);
	Int32 RandomRateOffset = 0;
	for (Int MasteryIndex = 0; MasteryIndex < Category->MeritMasterySpecialMasteryPoolMasteryCount; MasteryIndex += 1) {
		RTDataMeritMasterySpecialMasteryPoolMasteryRef Mastery = &Category->MeritMasterySpecialMasteryPoolMasteryList[MasteryIndex];
		for (Int OptionIndex = 0; OptionIndex < Mastery->MeritMasterySpecialMasteryPoolOptionCount; OptionIndex += 1) {
			RTDataMeritMasterySpecialMasteryPoolOptionRef Option = &Mastery->MeritMasterySpecialMasteryPoolOptionList[OptionIndex];
			RandomRateOffset += Option->Rate;
			if (RandomRate <= RandomRateOffset) {
				Result->Index = Mastery->Index;
				Result->Grade = Option->Grade;
				*RewardExp += Option->RewardExp;
				return;
			}
		}
	}
}

Bool RTCharacterDiamondMeritMasteryGradeUp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
) {
	if (Character->Data.DiamondMeritMasteryInfo.Info.IsEnabled) return false;

	// NOTE: This assumes the last one is the biggest value and the list is sorted correctly!
	RTDataPlatinumMeritScoreFormulaRef Formula = &Runtime->Context->PlatinumMeritScoreFormulaList[Runtime->Context->PlatinumMeritScoreFormulaCount - 1];
	if (Character->Data.PlatinumMeritMasteryInfo.Info.Exp < Formula->MaxScore) return false;

	RTDataMeritMasteryGradeRef MasteryGrade = RTRuntimeDataMeritMasteryGradeGet(Runtime->Context, RUNTIME_MERIT_MASTERY_GRADE_DIAMOND);
	if (!MasteryGrade) return false;
	if (Character->Data.Info.Wexp < MasteryGrade->RequiredWexp) return false;

	if (MasteryGrade->RequiredItemID > 0) {
		Bool CanConsumeItems = RTInventoryCanConsumeStackableItems(
			Runtime,
			&Character->Data.InventoryInfo,
			MasteryGrade->RequiredItemID,
			MasteryGrade->RequiredItemCount,
			InventorySlotCount,
			InventorySlotIndex
		);
		if (!CanConsumeItems) return false;

		RTInventoryConsumeStackableItems(
			Runtime,
			&Character->Data.InventoryInfo,
			MasteryGrade->RequiredItemID,
			MasteryGrade->RequiredItemCount,
			InventorySlotCount,
			InventorySlotIndex
		);

		Character->SyncMask.InventoryInfo = true;
	}

	Character->Data.Info.Wexp -= MasteryGrade->RequiredWexp;
	Character->SyncMask.Info = true;

	Character->Data.DiamondMeritMasteryInfo.Info.IsEnabled = true;
	Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex = 1;
	Character->Data.DiamondMeritMasteryInfo.Info.TotalMemorizeCount = RUNTIME_CHARACTER_DEFAULT_DIAMOND_MERIT_MEMORIZE_COUNT;
	Character->Data.DiamondMeritMasteryInfo.Info.ExtendedMemorizeCount = 0;

	for (Int Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots[Index].MemorizeIndex = Index + 1;
		Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots[Index].Points = 0;
	}

	Character->SyncMask.DiamondMeritMasteryInfo = true;
	return true;
}

RTDiamondMeritMasterySlotRef RTCharacterDiamondMeritAddMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MemorizeIndex,
	Int32 MasteryIndex,
	Int32 MasteryLevel
) {
	if (Character->Data.DiamondMeritMasteryInfo.Info.MasterySlotCount >= RUNTIME_CHARACTER_MAX_DIAMOND_MERIT_MASTERY_SLOT_COUNT) return NULL;

	RTDiamondMeritMasterySlotRef MasterySlot = &Character->Data.DiamondMeritMasteryInfo.MasterySlots[Character->Data.DiamondMeritMasteryInfo.Info.MasterySlotCount];
	MasterySlot->MemorizeIndex = MemorizeIndex;
	MasterySlot->Index = MasteryIndex;
	MasterySlot->Level = MasteryLevel;
	Character->Data.DiamondMeritMasteryInfo.Info.MasterySlotCount += 1;
	Character->SyncMask.DiamondMeritMasteryInfo = true;
	return MasterySlot;
}

RTDiamondMeritMasterySlotRef RTCharacterDiamondMeritGetMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MemorizeIndex,
	Int32 MasteryIndex
) {
	for (Int Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.MasterySlotCount; Index += 1) {
		RTDiamondMeritMasterySlotRef MasterySlot = &Character->Data.DiamondMeritMasteryInfo.MasterySlots[Index];
		if (MasterySlot->MemorizeIndex != MemorizeIndex) continue;
		if (MasterySlot->Index != MasteryIndex) continue;

		return MasterySlot;
	}

	return NULL;
}

Void RTCharacterDiamondMeritMasteryAddExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Exp
) {
	Int32 CurrentScore = Character->Data.DiamondMeritMasteryInfo.Info.Exp;
	Int32 NewScore = Character->Data.DiamondMeritMasteryInfo.Info.Exp + Exp;
	Int32 TotalRewardPoints = 0;

	for (Int StartIndex = 0; StartIndex < Runtime->Context->DiamondMeritScoreFormulaCount; StartIndex += 1) {
		RTDataDiamondMeritScoreFormulaRef Formula = &Runtime->Context->DiamondMeritScoreFormulaList[StartIndex];
		if (Formula->MinScore > CurrentScore) continue;
		if (Formula->MaxScore <= CurrentScore) continue;

		for (Int Index = StartIndex; Index < Runtime->Context->DiamondMeritScoreFormulaCount; Index += 1) {
			RTDataDiamondMeritScoreFormulaRef NextFormula = &Runtime->Context->DiamondMeritScoreFormulaList[Index];
			if (NextFormula->MaxScore > NewScore) break;

			TotalRewardPoints += NextFormula->RewardPoints;
		}
	}

	if (Runtime->Context->DiamondMeritScoreFormulaCount > 0) {
		// NOTE: This assumes the last one is the biggest value and the list is sorted correctly!
		RTDataDiamondMeritScoreFormulaRef Formula = &Runtime->Context->DiamondMeritScoreFormulaList[Runtime->Context->DiamondMeritScoreFormulaCount - 1];
		NewScore = MIN(NewScore, Formula->MaxScore);
	}

	Character->Data.DiamondMeritMasteryInfo.Info.Exp = NewScore;
	Character->Data.DiamondMeritMasteryInfo.Info.Points += TotalRewardPoints;

	for (Int Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots[Index].Points += TotalRewardPoints;
	}

	Character->SyncMask.DiamondMeritMasteryInfo = true;
}

Int32 RTCharacterDiamondMeritMasteryGetSlotCount(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTDataMeritMasteryCategoryRef Category,
	Int8 MemorizeIndex
) {
	Int32 SlotCount = 0;

	for (Int Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.MasterySlotCount; Index += 1) {
		if (Character->Data.DiamondMeritMasteryInfo.MasterySlots[Index].MemorizeIndex != MemorizeIndex) continue;

		for (Int SlotIndex = 0; SlotIndex < Category->MeritMasteryBaseCount; SlotIndex += 1) {
			RTDataMeritMasteryBaseRef MasteryBase = &Category->MeritMasteryBaseList[SlotIndex];
			if (MasteryBase->MasteryType > 1) continue;
			if (MasteryBase->MasteryIndex == Character->Data.DiamondMeritMasteryInfo.MasterySlots[Index].Index) {
				SlotCount += 1;
			}
		}
	}

	return SlotCount;
}

Int32 RTCharacterDiamondMeritMasteryGetOpenedSlotCount(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTDataMeritMasteryCategoryRef Category
) {
	Int32 SlotCount = 0;

	for (Int Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount; Index += 1) {
		for (Int SlotIndex = 0; SlotIndex < Category->MeritMasteryBaseCount; SlotIndex += 1) {
			RTDataMeritMasteryBaseRef MasteryBase = &Category->MeritMasteryBaseList[SlotIndex];
			if (MasteryBase->MasteryIndex == Character->Data.DiamondMeritMasteryInfo.UnlockedSlots[Index].MasteryIndex) {
				SlotCount += 1;
			}
		}
	}

	return SlotCount;
}

Bool RTCharacterDiamondMeritMasteryOpenSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	UInt16 InventorySlotCount1,
	UInt16* InventorySlotIndices1,
	UInt16 InventorySlotCount2,
	UInt16* InventorySlotIndices2
) {
	if (!Character->Data.DiamondMeritMasteryInfo.Info.IsEnabled) return false;

	RTCharacterUpdateMeritMastery(Runtime, Character, true);

	if (Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex > 0) return false;

	RTDataMeritMasteryCategoryRef Category = NULL;
	RTDataMeritMasteryBaseRef MasteryBase = NULL;
	if (!RTRuntimeDataGetMeritMasteryData(
		Runtime,
		MasteryIndex,
		&Category,
		&MasteryBase
	)) return false;

	Int32 TrainedSlotCount = RTCharacterDiamondMeritMasteryGetSlotCount(Runtime, Character, Category, Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex);
	Int32 OpenedSlotCount = RTCharacterDiamondMeritMasteryGetOpenedSlotCount(Runtime, Character, Category);

	RTDataMeritMasterySlotOpenCategoryRef SlotOpenCategory = RTRuntimeDataMeritMasterySlotOpenCategoryGet(Runtime->Context, Category->Index);
	if (!SlotOpenCategory) return false;
	RTDataMeritMasterySlotOpenMaterialRef SlotOpenMaterial1 = RTRuntimeDataMeritMasterySlotOpenMaterialGet(SlotOpenCategory, TrainedSlotCount, 0);
	if (!SlotOpenMaterial1) return false;
	RTDataMeritMasterySlotOpenMaterialRef SlotOpenMaterial2 = RTRuntimeDataMeritMasterySlotOpenMaterialGet(SlotOpenCategory, TrainedSlotCount, 1);
	if (!SlotOpenMaterial2) return false;
	RTDataMeritMasterySlotOpenTimeRef SlotOpenTime = RTRuntimeDataMeritMasterySlotOpenTimeGet(Runtime->Context, Category->Index, OpenedSlotCount);
	if (!SlotOpenTime) return false;

	if (SlotOpenMaterial1->RequiredItemID > 0 && SlotOpenMaterial1->RequiredItemCount > 0) {
		Int64 TotalConsumableItemCount = 0;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount1; SlotIndex += 1) {
			Int64 ConsumableItemCount = RTInventoryGetConsumableItemCount(
				Runtime,
				&Character->Data.InventoryInfo,
				SlotOpenMaterial1->RequiredItemID,
				0,
				InventorySlotIndices1[SlotIndex]
			);
			if (ConsumableItemCount < 1) return false;

			TotalConsumableItemCount += ConsumableItemCount;
		}

		if (TotalConsumableItemCount < SlotOpenMaterial1->RequiredItemCount) return false;
	}

	if (SlotOpenMaterial2->RequiredItemID > 0 && SlotOpenMaterial2->RequiredItemCount > 0) {
		Int64 TotalConsumableItemCount = 0;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount2; SlotIndex += 1) {
			Int64 ConsumableItemCount = RTInventoryGetConsumableItemCount(
				Runtime,
				&Character->Data.InventoryInfo,
				SlotOpenMaterial2->RequiredItemID,
				0,
				InventorySlotIndices2[SlotIndex]
			);
			if (ConsumableItemCount < 1) return false;

			TotalConsumableItemCount += ConsumableItemCount;
		}

		if (TotalConsumableItemCount < SlotOpenMaterial2->RequiredItemCount) return false;
	}

	if (SlotOpenMaterial1->RequiredItemID > 0 && SlotOpenMaterial1->RequiredItemCount > 0) {
		Int64 RemainingItemCount = SlotOpenMaterial1->RequiredItemCount;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount1; SlotIndex += 1) {
			Int64 ConsumedItemCount = RTInventoryConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				SlotOpenMaterial1->RequiredItemID,
				0,
				RemainingItemCount,
				InventorySlotIndices1[SlotIndex]
			);

			RemainingItemCount -= ConsumedItemCount;
		}

		Character->SyncMask.InventoryInfo = true;
	}

	if (SlotOpenMaterial2->RequiredItemID > 0 && SlotOpenMaterial2->RequiredItemCount > 0) {
		Int64 RemainingItemCount = SlotOpenMaterial2->RequiredItemCount;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount2; SlotIndex += 1) {
			Int64 ConsumedItemCount = RTInventoryConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				SlotOpenMaterial2->RequiredItemID,
				0,
				RemainingItemCount,
				InventorySlotIndices2[SlotIndex]
			);

			RemainingItemCount -= ConsumedItemCount;
		}

		Character->SyncMask.InventoryInfo = true;
	}

	Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotMasteryIndex = MasteryIndex;
	Character->Data.DiamondMeritMasteryInfo.Info.OpenSlotUnlockTime = GetTimestamp() + SlotOpenTime->OpenTime;
	Character->SyncMask.DiamondMeritMasteryInfo = true;
	return true;
}

Bool RTCharacterDiamondMeritMasterySetActiveMemorizeIndex(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MemorizeIndex
) {
	for (Int Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		RTDiamondMeritExtendedMemorizeSlotRef MemorizeSlot = &Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots[Index];
		if (MemorizeSlot->MemorizeIndex == MemorizeIndex) {
			Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex = MemorizeIndex;
			Character->SyncMask.DiamondMeritMasteryInfo = true;
			return true;
		}
	}

	return false;
}

Int32 RTCharacterDiamondMeritMasteryGetPoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	for (Int Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		RTDiamondMeritExtendedMemorizeSlotRef MemorizeSlot = &Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots[Index];
		if (MemorizeSlot->MemorizeIndex != Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex) continue;

		return MemorizeSlot->Points;
	}

	return 0;
}

Void RTCharacterDiamondMeritSetPoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Points
) {
	for (Int Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.TotalMemorizeCount; Index += 1) {
		RTDiamondMeritExtendedMemorizeSlotRef MemorizeSlot = &Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots[Index];
		if (MemorizeSlot->MemorizeIndex != Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex) continue;

		MemorizeSlot->Points = Points;
		break;
	}
}

Void RTCharacterDiamondMeritMasteryGetSpecialMasterySlots(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int8 MemorizeIndex,
	Int32 CategoryIndex,
	RTDiamondMeritSpecialMasterySlotRef* Result1,
	RTDiamondMeritSpecialMasterySlotRef* Result2,
	RTDiamondMeritSpecialMasterySlotRef* Result3
) {
	assert(Result1 && Result2 && Result3);

	*Result1 = NULL;
	*Result2 = NULL;
	*Result3 = NULL;

	for (Int32 Index = 0; Index < Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount; Index += 1) {
		RTDiamondMeritSpecialMasterySlotRef Slot = &Character->Data.DiamondMeritMasteryInfo.SpecialMasterySlots[Index];
		if (Slot->MemorizeIndex != MemorizeIndex) continue;
		if (Slot->Category != CategoryIndex) continue;

		if (!*Result1) *Result1 = Slot;
		else if (!*Result2) *Result2 = Slot;
		else if (!*Result3) *Result3 = Slot;
		else break;
	}
}

Bool RTCharacterDiamondMeritMasteryGrantSpecialMastery(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 CategoryIndex,
	UInt16 InventorySlotCount,
	UInt16* InventorySlotIndex
) {
	if (!Character->Data.DiamondMeritMasteryInfo.Info.IsEnabled) return false;

	RTDataMeritMasterySpecialMasteryMaterialRef MasteryMaterial = RTRuntimeDataMeritMasterySpecialMasteryMaterialGet(
		Runtime->Context,
		RUNTIME_MERIT_MASTERY_GRADE_DIAMOND
	);
	if (!MasteryMaterial) return false;
	if (Character->Data.Info.Wexp < MasteryMaterial->RequiredWexp) return false;

	RTDataMeritMasterySpecialMasteryPoolRef Pool = RTRuntimeDataMeritMasterySpecialMasteryPoolGet(
		Runtime->Context,
		RUNTIME_MERIT_MASTERY_GRADE_DIAMOND
	);
	if (!Pool) return false;

	RTDataMeritMasterySpecialMasteryPoolCategoryRef Category = RTRuntimeDataMeritMasterySpecialMasteryPoolCategoryGet(
		Pool,
		CategoryIndex
	);
	if (!Category) return false;

	if (MasteryMaterial->RequiredItemID > 0 && MasteryMaterial->RequiredItemCount > 0) {
		Int64 TotalConsumableItemCount = 0;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount; SlotIndex += 1) {
			Int64 ConsumableItemCount = RTInventoryGetConsumableItemCount(
				Runtime,
				&Character->Data.InventoryInfo,
				MasteryMaterial->RequiredItemID,
				0,
				InventorySlotIndex[SlotIndex]
			);
			if (ConsumableItemCount < 1) return false;

			TotalConsumableItemCount += ConsumableItemCount;
		}

		if (TotalConsumableItemCount < MasteryMaterial->RequiredItemCount) return false;

		Int64 RemainingItemCount = MasteryMaterial->RequiredItemCount;
		for (Int SlotIndex = 0; SlotIndex < InventorySlotCount; SlotIndex += 1) {
			Int64 ConsumedItemCount = RTInventoryConsumeItem(
				Runtime,
				&Character->Data.InventoryInfo,
				MasteryMaterial->RequiredItemID,
				0,
				RemainingItemCount,
				InventorySlotIndex[SlotIndex]
			);

			RemainingItemCount -= ConsumedItemCount;
		}

		Character->SyncMask.InventoryInfo = true;
	}

	RTDiamondMeritSpecialMasterySlotRef SpecialMasterySlots[3] = { NULL, NULL, NULL };
	RTCharacterDiamondMeritMasteryGetSpecialMasterySlots(
		Runtime,
		Character,
		Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex,
		CategoryIndex,
		&SpecialMasterySlots[0],
		&SpecialMasterySlots[1],
		&SpecialMasterySlots[2]
	);

	// TODO: Apply sorted insert for clean data structure (by MemorizeIndex, Category)
	if (!SpecialMasterySlots[0]) {
		assert(Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount < RUNTIME_CHARACTER_MAX_DIAMOND_MERIT_SPECIAL_MASTERY_SLOT_COUNT);
		SpecialMasterySlots[0] = &Character->Data.DiamondMeritMasteryInfo.SpecialMasterySlots[Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount];
		SpecialMasterySlots[0]->MemorizeIndex = Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex;
		SpecialMasterySlots[0]->Category = CategoryIndex;
		SpecialMasterySlots[0]->Index = 0;
		SpecialMasterySlots[0]->Grade = 0;
		Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount += 1;
	}

	if (!SpecialMasterySlots[1]) {
		assert(Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount < RUNTIME_CHARACTER_MAX_DIAMOND_MERIT_SPECIAL_MASTERY_SLOT_COUNT);
		SpecialMasterySlots[1] = &Character->Data.DiamondMeritMasteryInfo.SpecialMasterySlots[Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount];
		SpecialMasterySlots[1]->MemorizeIndex = Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex;
		SpecialMasterySlots[1]->Category = CategoryIndex;
		SpecialMasterySlots[1]->Index = 0;
		SpecialMasterySlots[1]->Grade = 0;
		Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount += 1;
	}

	if (!SpecialMasterySlots[2]) {
		assert(Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount < RUNTIME_CHARACTER_MAX_DIAMOND_MERIT_SPECIAL_MASTERY_SLOT_COUNT);
		SpecialMasterySlots[2] = &Character->Data.DiamondMeritMasteryInfo.SpecialMasterySlots[Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount];
		SpecialMasterySlots[2]->MemorizeIndex = Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex;
		SpecialMasterySlots[2]->Category = CategoryIndex;
		SpecialMasterySlots[2]->Index = 0;
		SpecialMasterySlots[2]->Grade = 0;
		Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount += 1;
	}

	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 RewardExp = 0;
	RTDiamondMeritMasteryRollSpecialMastery(Category, &Seed, &RewardExp, SpecialMasterySlots[0]);
	RTDiamondMeritMasteryRollSpecialMastery(Category, &Seed, &RewardExp, SpecialMasterySlots[1]);
	RTDiamondMeritMasteryRollSpecialMastery(Category, &Seed, &RewardExp, SpecialMasterySlots[2]);
	Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasteryExp += RewardExp;
	Character->SyncMask.DiamondMeritMasteryInfo = true;

	Character->Data.Info.Wexp -= MasteryMaterial->RequiredWexp;
	Character->SyncMask.Info = true;

	return true;
}

Bool RTCharacterRegisterMeritMedals(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
	Int32 InventorySlotCount,
    UInt32* InventorySlotIndices
) {
	Int32 TotalRewardMeritScore = 0;
	Int64 TotalRewardHonorPoint = 0;

	for (Int Index = 0; Index < InventorySlotCount; Index += 1) {
		RTItemSlotRef ItemSlot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndices[Index]);
		RTItemDataRef ItemData = (ItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID) : NULL;
		if (!ItemData || ItemData->ItemType != RUNTIME_ITEM_TYPE_MERIT_MEDAL) return false;

		RTItemOptions ItemOptions = { .Serial = ItemSlot->ItemOptions };
		if (ItemData->MeritMedal.Nation != Character->Data.StyleInfo.Nation) return false;
		if (ItemOptions.MeritMedal.EvaluationCount < 1) return false;

		RTDataMeritMedalRewardScoreRef MeritMedalRewardScore = RTRuntimeDataMeritMedalRewardScoreGet(
			Runtime->Context,
			ItemData->ItemGrade
		);
		if (MeritMedalRewardScore) {
			TotalRewardMeritScore += MeritMedalRewardScore->RewardScore;
		}

		RTDataMeritMedalRewardHonorRef MeritMedalRewardHonor = RTRuntimeDataMeritMedalRewardHonorGet(
			Runtime->Context,
			ItemData->MeritMedal.Type, // TODO: Verify if this is the grade type!
			ItemData->ItemGrade
		);
		if (MeritMedalRewardHonor) {
			Int32 HonorRankRewards[] = {
				MeritMedalRewardHonor->HonorRank10,
				MeritMedalRewardHonor->HonorRank11,
				MeritMedalRewardHonor->HonorRank12,
				MeritMedalRewardHonor->HonorRank13,
				MeritMedalRewardHonor->HonorRank14,
				MeritMedalRewardHonor->HonorRank15,
				MeritMedalRewardHonor->HonorRank16,
				MeritMedalRewardHonor->HonorRank17,
				MeritMedalRewardHonor->HonorRank18,
				MeritMedalRewardHonor->HonorRank19,
				MeritMedalRewardHonor->HonorRank20,
				MeritMedalRewardHonor->HonorRank21,
				MeritMedalRewardHonor->HonorRank22,
				MeritMedalRewardHonor->HonorRank23,
				MeritMedalRewardHonor->HonorRank24,
				MeritMedalRewardHonor->HonorRank25,
				MeritMedalRewardHonor->HonorRank26,
				MeritMedalRewardHonor->HonorRank27,
				MeritMedalRewardHonor->HonorRank28,
				MeritMedalRewardHonor->HonorRank29,
				MeritMedalRewardHonor->HonorRank30,
			};

			Int32 RewardIndex = RTCharacterGetHonorRank(Runtime, Character) - 10;
			if (0 <= RewardIndex && RewardIndex < sizeof(HonorRankRewards) / sizeof(HonorRankRewards[0])) {
				TotalRewardHonorPoint += HonorRankRewards[RewardIndex];
			}
		}
	}

	for (Int Index = 0; Index < InventorySlotCount; Index += 1) {
		RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, InventorySlotIndices[Index]);
	}

	if (Character->Data.DiamondMeritMasteryInfo.Info.IsEnabled) {
		RTCharacterDiamondMeritMasteryAddExp(Runtime, Character, TotalRewardMeritScore);
	}
	else if (Character->Data.PlatinumMeritMasteryInfo.Info.IsEnabled) {
		RTCharacterPlatinumMeritMasteryAddExp(Runtime, Character, TotalRewardMeritScore);
	}
	else {
		RTCharacterGoldMeritMasteryAddExp(Runtime, Character, TotalRewardMeritScore);
	}

	RTCharacterAddHonorPoint(
		Runtime,
		Character,
		NOTIFICATION_HONOR_POINT_CHANGE_TYPE_REPORT_MEDAL,
		TotalRewardHonorPoint
	);

	return true;
}

Bool RTCharacterHasMeritMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 MasteryLevel
) {
	for (Int CategoryIndex = 0; CategoryIndex < Runtime->Context->MeritMasteryCategoryCount; CategoryIndex += 1) {
		RTDataMeritMasteryCategoryRef Category = &Runtime->Context->MeritMasteryCategoryList[CategoryIndex];
		for (Int Index = 0; Index < Category->MeritMasteryBaseCount; Index += 1) {
			RTDataMeritMasteryBaseRef MasteryBase = &Category->MeritMasteryBaseList[Index];
			if (MasteryBase->MasteryIndex != MasteryIndex) continue;

			if (Category->Grade == RUNTIME_MERIT_MASTERY_GRADE_GOLD) {
				RTGoldMeritMasterySlotRef MasterySlot = RTCharacterGoldMeritGetMasterySlot(Runtime, Character, MasteryIndex);
				return MasterySlot->Level >= MasteryLevel;
			}

			if (Category->Grade == RUNTIME_MERIT_MASTERY_GRADE_PLATINUM) {
				RTPlatinumMeritMasterySlotRef MasterySlot = RTCharacterPlatinumMeritGetMasterySlot(
					Runtime, 
					Character, 
					Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex, 
					MasteryIndex
				);
				return MasterySlot->Level >= MasteryLevel;
			}

			if (Category->Grade == RUNTIME_MERIT_MASTERY_GRADE_DIAMOND) {
				RTDiamondMeritMasterySlotRef MasterySlot = RTCharacterDiamondMeritGetMasterySlot(
					Runtime,
					Character,
					Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex,
					MasteryIndex
				);
				return MasterySlot->Level >= MasteryLevel;
			}

			return false;
		}
	}

	return false;
}

Int RTMeritMasteryGetRequiredMeritPoints(
	RTRuntimeRef Runtime,
	Int32 MasteryIndex,
	Int32 CurrentLevel,
	Int32 TargetLevel
) {
	RTDataMeritMasteryValueRef MasteryValue = RTRuntimeDataMeritMasteryValueGet(Runtime->Context, MasteryIndex);
	if (!MasteryValue) return INT32_MAX;

	Int RequiredMeritPoints = 0;
	for (Int Index = 0; Index < MasteryValue->MeritMasteryValueLevelCount; Index += 1) {
		RTDataMeritMasteryValueLevelRef MasteryValueLevel = &MasteryValue->MeritMasteryValueLevelList[Index];
		if (MasteryValueLevel->MasteryLevel <= CurrentLevel) continue;
		if (MasteryValueLevel->MasteryLevel > TargetLevel) continue;

		RequiredMeritPoints += MasteryValueLevel->RequiredMeritPoints;
	}
	
	return RequiredMeritPoints;
}

Bool RTCharacterMeritMasteryTrain(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 TargetLevel,
	Int32* RemainingMeritPoints
) {
	*RemainingMeritPoints = 0;

	RTDataMeritMasteryCategoryRef Category = NULL;
	RTDataMeritMasteryBaseRef MasteryBase = NULL;
	if (!RTRuntimeDataGetMeritMasteryData(
		Runtime,
		MasteryIndex,
		&Category,
		&MasteryBase
	)) return false;

	if (MasteryBase->MaxLevel < TargetLevel) return false;
	if (MasteryBase->PreMasteryIndex > 0 && MasteryBase->PreMasteryMinLevel > 0) {
		if (!RTCharacterHasMeritMasterySlot(
			Runtime,
			Character,
			MasteryBase->PreMasteryIndex,
			MasteryBase->PreMasteryMinLevel
		)) return false;
	}

	// TODO: Check if Platinum & Diamond slots are already unlocked!!!
	// TODO: Check extended memorize shits!!!

	Int CurrentLevel = 0;
	if (Category->Grade == RUNTIME_MERIT_MASTERY_GRADE_GOLD) {
		RTGoldMeritMasterySlotRef MasterySlot = RTCharacterGoldMeritGetMasterySlot(Runtime, Character, MasteryIndex);
		if (MasterySlot) CurrentLevel = MasterySlot->Level;

		Int RequiredMeritPoints = RTMeritMasteryGetRequiredMeritPoints(Runtime, MasteryIndex, CurrentLevel, TargetLevel);
		if (RequiredMeritPoints > Character->Data.GoldMeritMasteryInfo.Info.Points) return false;

		if (!MasterySlot) MasterySlot = RTCharacterGoldMeritAddMasterySlot(Runtime, Character, MasteryIndex, TargetLevel);
		if (!MasterySlot) return false;
		MasterySlot->Level = TargetLevel;

		Character->Data.GoldMeritMasteryInfo.Info.Points -= RequiredMeritPoints;
		Character->SyncMask.GoldMeritMasteryInfo = true;
		*RemainingMeritPoints = Character->Data.GoldMeritMasteryInfo.Info.Points;

		return true;
	}

	if (Category->Grade == RUNTIME_MERIT_MASTERY_GRADE_PLATINUM) {
		if (!RTCharacterPlatinumMeritIsSlotOpen(Runtime, Character, MasteryIndex)) return false;

		RTPlatinumMeritMasterySlotRef MasterySlot = RTCharacterPlatinumMeritGetMasterySlot(
			Runtime,
			Character,
			Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex,
			MasteryIndex
		);
		if (MasterySlot) CurrentLevel = MasterySlot->Level;

		Int32 CurrentMeritPoints = RTCharacterPlatinumMeritMasteryGetPoints(Runtime, Character);
		Int32 RequiredMeritPoints = RTMeritMasteryGetRequiredMeritPoints(Runtime, MasteryIndex, CurrentLevel, TargetLevel);
		if (RequiredMeritPoints > CurrentMeritPoints) return false;

		if (!MasterySlot) MasterySlot = RTCharacterPlatinumMeritAddMasterySlot(
			Runtime, 
			Character,
			Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex,
			MasteryIndex, 
			TargetLevel
		);
		if (!MasterySlot) return false;
		MasterySlot->Level = TargetLevel;

		CurrentMeritPoints -= RequiredMeritPoints;
		RTCharacterPlatinumMeritSetPoints(Runtime, Character, CurrentMeritPoints);

		Character->SyncMask.PlatinumMeritMasteryInfo = true;
		*RemainingMeritPoints = CurrentMeritPoints;

		return true;
	}

	if (Category->Grade == RUNTIME_MERIT_MASTERY_GRADE_DIAMOND) {
		RTDiamondMeritMasterySlotRef MasterySlot = RTCharacterDiamondMeritGetMasterySlot(
			Runtime,
			Character,
			Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex,
			MasteryIndex
		);
		if (MasterySlot) CurrentLevel = MasterySlot->Level;

		Int32 CurrentMeritPoints = RTCharacterDiamondMeritMasteryGetPoints(Runtime, Character);
		Int RequiredMeritPoints = RTMeritMasteryGetRequiredMeritPoints(Runtime, MasteryIndex, CurrentLevel, TargetLevel);
		if (RequiredMeritPoints > CurrentMeritPoints) return false;

		if (!MasterySlot) MasterySlot = RTCharacterDiamondMeritAddMasterySlot(
			Runtime,
			Character,
			Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex,
			MasteryIndex,
			TargetLevel
		);
		if (!MasterySlot) return false;
		MasterySlot->Level = TargetLevel;

		CurrentMeritPoints -= RequiredMeritPoints;
		RTCharacterPlatinumMeritSetPoints(Runtime, Character, CurrentMeritPoints);

		Character->SyncMask.DiamondMeritMasteryInfo = true;
		*RemainingMeritPoints = CurrentMeritPoints;

		return true;
	}

	return false;
}
