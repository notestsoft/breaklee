#include "StellarMastery.h"
#include "Character.h"
#include "Constants.h"
#include "Runtime.h"
#include "Force.h"


RTDataStellarLineGradeRef RTDataStellarMasteryGetLineGrade(
	RTRuntimeRef Runtime,
	RTDataStellarLineRef StellarLine,
	UInt32 MaterialCount
) {
	RTDataStellarLineGradeRef OutGrade = NULL;
	
	for (UInt32 i = 0; i < StellarLine->StellarLineGradeCount; i++) {
		RTDataStellarLineGradeRef Grade = &StellarLine->StellarLineGradeList[i];
		if (Grade->ItemCount == MaterialCount) {
			OutGrade = Grade;
			break;
		}
	}
	return OutGrade;
}


RTStellarMasterySlotRef RTCharacterStellarMasteryGetSlot(
	RTCharacterRef Character,
	UInt8 GroupID,
	UInt8 SlotLine,
	UInt8 SlotIndex
) {
	RTStellarMasterySlotRef OutSlot = NULL;

	for (UInt32 i = 0; i < Character->Data.StellarMasteryInfo.Info.SlotCount; i++) {
		RTStellarMasterySlotRef Slot = &Character->Data.StellarMasteryInfo.Slots[i];
		if (Slot->GroupID == GroupID && Slot->SlotLine == SlotLine && Slot->SlotIndex == SlotIndex) {
			OutSlot = Slot;
			break;
		}
	}
	return OutSlot;
}

Bool RTCharacterStellarMasterySetSlot(
	RTCharacterRef Character,
	RTStellarMasterySlotRef Slot
) {
	Bool OutResult = false;

	RTStellarMasterySlotRef ExistingSlot = RTCharacterStellarMasteryGetSlot(
		Character,
		Slot->GroupID,
		Slot->SlotLine,
		Slot->SlotIndex
	);

	if (ExistingSlot != NULL) {
		ExistingSlot = Slot;
		OutResult = true;
	}
	else {
		if (Character->Data.StellarMasteryInfo.Info.SlotCount < RUNTIME_CHARACTER_MAX_STELLAR_SLOT_COUNT) {
			Character->Data.StellarMasteryInfo.Slots[Character->Data.StellarMasteryInfo.Info.SlotCount] = *Slot;
			Character->Data.StellarMasteryInfo.Info.SlotCount++;
			OutResult = true;
		}
	}
	
	return OutResult;
}