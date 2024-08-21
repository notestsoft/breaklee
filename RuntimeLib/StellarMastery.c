#include "StellarMastery.h"
#include "Character.h"
#include "Constants.h"
#include "Runtime.h"
#include "Force.h"


/*
Rolls single option from the pool.

params:
- Pool - pool to roll from.
- Count - number of options in the pool.
- GetChance - (*PoolItemType) -> Int32 - callback to get the chance of the option.
- ResultRef - pointer to the result.
*/
#define ROLL_POOL(__POOL__, __COUNT__, __GET_CHANCE__, __RESULT_REF__)    \
    {                                                                     \
        Int32 MaxRandomValue = 0;                                         \
        for (Int32 Index = 0; Index < __COUNT__; Index += 1) {            \
            MaxRandomValue += __GET_CHANCE__(&__POOL__[Index]);           \
        }                                                                 \
                                                                          \
        Int32 Seed = (Int32)PlatformGetTickCount();                       \
        Int32 RandomValue = RandomRange(&Seed, 0, MaxRandomValue);        \
        Int32 CumulativeChance = 0;                                       \
                                                                          \
        for (Int32 Index = 0; Index < __COUNT__; Index += 1) {            \
            CumulativeChance += __GET_CHANCE__(&__POOL__[Index]);         \
            if (RandomValue < CumulativeChance) {                         \
                __RESULT_REF__ = &__POOL__[Index];                        \
                break;                                                    \
            }                                                             \
        }                                                                 \
    }



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

RTStellarMasterySlotRef RTCharacterStellarMasteryAssertSlot(
	RTCharacterRef Character,
	UInt8 GroupID,
	UInt8 SlotLine,
	UInt8 SlotIndex
) {
	RTStellarMasterySlotRef OutSlot = RTCharacterStellarMasteryGetSlot(Character, GroupID, SlotLine, SlotIndex);
	if (OutSlot == NULL) {
		if (Character->Data.StellarMasteryInfo.Info.SlotCount < RUNTIME_CHARACTER_MAX_STELLAR_SLOT_COUNT) {
			OutSlot = &Character->Data.StellarMasteryInfo.Slots[Character->Data.StellarMasteryInfo.Info.SlotCount];
			
			OutSlot->GroupID = GroupID;
			OutSlot->SlotLine = SlotLine;
			OutSlot->SlotIndex = SlotIndex;

			OutSlot->StellarLinkGrade = 0;
			OutSlot->StellarForceEffect = 0;
			OutSlot->StellarForceValue = 0;

			Character->Data.StellarMasteryInfo.Info.SlotCount++;
		}
	}
	return OutSlot;
}


static inline Int32 GetStellarForceEffectChance(const RTDataStellarForceEffectRef StellarForceEffect) {
	return StellarForceEffect->Chance;
}

static inline Int32 GetStellarLinkChance(const RTDataStellarLinkRef StellarLink) {
	return StellarLink->Chance;
}


Void RTStellarMasteryRollForce(
	RTRuntimeRef Runtime,
	RTDataStellarLineGradeRef StellarLineGrade,
	RTStellarMasterySlotRef MasterySlot
) {
	RTDataStellarForcePoolRef StellarForcePool = RTRuntimeDataStellarForcePoolGet(Runtime->Context, StellarLineGrade->ForceCodePer);
	
	RTDataStellarForceEffectRef StellarForceEffect = NULL;

	ROLL_POOL(
		StellarForcePool->StellarForceEffectList,
		StellarForcePool->StellarForceEffectCount,
		GetStellarForceEffectChance,
		StellarForceEffect
	);
	
	MasterySlot->StellarForceEffect = StellarForceEffect->ForceEffectID;
	MasterySlot->StellarForceValue = StellarForceEffect->Value;
	MasterySlot->StellarForceValueType = StellarForceEffect->ValueType;
}

Void RTStellarMasteryRollLink(
	RTRuntimeRef Runtime,
	RTDataStellarLineGradeRef StellarLineGrade,
	RTStellarMasterySlotRef MasterySlot
) {
	RTDataStellarLinkPoolRef StellarLinkPool = RTRuntimeDataStellarLinkPoolGet(Runtime->Context, StellarLineGrade->Grade);

	RTDataStellarLinkRef StellarLink = NULL;

	ROLL_POOL(
		StellarLinkPool->StellarLinkList,
		StellarLinkPool->StellarLinkCount,
		GetStellarLinkChance,
		StellarLink
	);

	MasterySlot->StellarLinkGrade = StellarLink->Grade;
}