#include "Character.h"
#include "MeritMastery.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

RTGoldMeritMasterySlotRef RTCharacterGoldMeritAddMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int MasteryIndex,
	Int MasteryLevel
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
	Int MasteryIndex
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

RTPlatinumMeritMasterySlotRef RTCharacterPlatinumMeritAddMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int MemorizeIndex,
	Int MasteryIndex,
	Int MasteryLevel
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
	Int MemorizeIndex,
	Int MasteryIndex
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
	Character->SyncMask.PlatinumMeritMasteryInfo = true;
}

RTDiamondMeritMasterySlotRef RTCharacterDiamondMeritAddMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int MemorizeIndex,
	Int MasteryIndex,
	Int MasteryLevel
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
	Int MemorizeIndex,
	Int MasteryIndex
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

	for (Int32 StartIndex = 0; StartIndex < Runtime->Context->DiamondMeritScoreFormulaCount; StartIndex += 1) {
		RTDataDiamondMeritScoreFormulaRef Formula = &Runtime->Context->DiamondMeritScoreFormulaList[StartIndex];
		if (Formula->MinScore > CurrentScore) continue;
		if (Formula->MaxScore <= CurrentScore) continue;

		for (Int32 Index = StartIndex; Index < Runtime->Context->DiamondMeritScoreFormulaCount; Index += 1) {
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
	Character->SyncMask.DiamondMeritMasteryInfo = true;
}

Bool RTCharacterRegisterMeritMedals(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int InventorySlotCount,
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
	Int MasteryIndex,
	Int MasteryLevel
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
	Int MasteryIndex,
	Int CurrentLevel,
	Int TargetLevel
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
	Int MasteryIndex,
	Int TargetLevel,
	Int32* RemainingMeritPoints
) {
	*RemainingMeritPoints = 0;

	for (Int CategoryIndex = 0; CategoryIndex < Runtime->Context->MeritMasteryCategoryCount; CategoryIndex += 1) {
		RTDataMeritMasteryCategoryRef Category = &Runtime->Context->MeritMasteryCategoryList[CategoryIndex];
		for (Int Index = 0; Index < Category->MeritMasteryBaseCount; Index += 1) {
			RTDataMeritMasteryBaseRef MasteryBase = &Category->MeritMasteryBaseList[Index];
			if (MasteryBase->MasteryIndex != MasteryIndex) continue;

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
				RTPlatinumMeritMasterySlotRef MasterySlot = RTCharacterPlatinumMeritGetMasterySlot(
					Runtime,
					Character,
					Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex,
					MasteryIndex
				);
				if (MasterySlot) CurrentLevel = MasterySlot->Level;

				Int RequiredMeritPoints = RTMeritMasteryGetRequiredMeritPoints(Runtime, MasteryIndex, CurrentLevel, TargetLevel);
				if (RequiredMeritPoints > Character->Data.PlatinumMeritMasteryInfo.Info.Points) return false;

				if (!MasterySlot) MasterySlot = RTCharacterPlatinumMeritAddMasterySlot(
					Runtime, 
					Character,
					Character->Data.PlatinumMeritMasteryInfo.Info.ActiveMemorizeIndex,
					MasteryIndex, 
					TargetLevel
				);
				if (!MasterySlot) return false;
				MasterySlot->Level = TargetLevel;

				Character->Data.PlatinumMeritMasteryInfo.Info.Points -= RequiredMeritPoints;
				Character->SyncMask.PlatinumMeritMasteryInfo = true;
				*RemainingMeritPoints = Character->Data.PlatinumMeritMasteryInfo.Info.Points;

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

				Int RequiredMeritPoints = RTMeritMasteryGetRequiredMeritPoints(Runtime, MasteryIndex, CurrentLevel, TargetLevel);
				if (RequiredMeritPoints > Character->Data.DiamondMeritMasteryInfo.Info.Points) return false;

				if (!MasterySlot) MasterySlot = RTCharacterDiamondMeritAddMasterySlot(
					Runtime,
					Character,
					Character->Data.DiamondMeritMasteryInfo.Info.ActiveMemorizeIndex,
					MasteryIndex,
					TargetLevel
				);
				if (!MasterySlot) return false;
				MasterySlot->Level = TargetLevel;

				Character->Data.DiamondMeritMasteryInfo.Info.Points -= RequiredMeritPoints;
				Character->SyncMask.DiamondMeritMasteryInfo = true;
				*RemainingMeritPoints = Character->Data.DiamondMeritMasteryInfo.Info.Points;

				return true;
			}

			return false;
		}
	}

	return false;
}
