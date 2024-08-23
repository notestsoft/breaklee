#include "StellarMastery.h"
#include "Character.h"
#include "Constants.h"
#include "Runtime.h"
#include "Force.h"


/*
Rolls single option from the pool.

params:
- __POOL__ - pool to roll from.
- __COUNT__ - number of options in the pool.
- __GET_CHANCE__ - (*PoolItemType) -> Int32 - callback to get the chance of the option.
- __RESULT_REF__ - pointer to the result.
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
	if (OutSlot != NULL) {
		return OutSlot;
	}

	assert(Character->Data.StellarMasteryInfo.Info.SlotCount <= RUNTIME_CHARACTER_MAX_STELLAR_SLOT_COUNT);

	OutSlot = &Character->Data.StellarMasteryInfo.Slots[Character->Data.StellarMasteryInfo.Info.SlotCount];
			
	OutSlot->GroupID = GroupID;
	OutSlot->SlotLine = SlotLine;
	OutSlot->SlotIndex = SlotIndex;

	OutSlot->LinkGrade = 0;
	OutSlot->ForceEffect = 0;
	OutSlot->ForceValue = 0;

	Character->Data.StellarMasteryInfo.Info.SlotCount++;

	return OutSlot;
}

Void RTStellarMasterySetLink(
	UInt8 LinkGrade,
	RTStellarMasterySlotRef MasterySlot
) {
	MasterySlot->LinkGrade = LinkGrade;
}


static inline Int32 GetStellarForceValueChance(const RTDataStellarForceValueRef StellarForceValue) {
	return StellarForceValue->Chance;
}

static inline Int32 GetStellarForceEffectChance(const RTDataStellarForceEffectRef StellarForceEffect) {
	return StellarForceEffect->Chance;
}

static inline Int32 GetStellarLinkChance(const RTDataStellarLinkRef StellarLink) {
	return StellarLink->Chance;
}

Void RTStellarMasteryRollForceEffect(
	RTRuntimeRef Runtime,
	RTDataStellarLineGradeRef StellarLineGrade,
	RTStellarMasterySlotRef MasterySlot
) {
	RTDataStellarForceValuePoolRef StellarForceValuePool = RTRuntimeDataStellarForceValuePoolGet(Runtime->Context, StellarLineGrade->Grade);
	RTDataStellarForceValueRef StellarForceValue = NULL;
	ROLL_POOL(
		StellarForceValuePool->StellarForceValueList,
		StellarForceValuePool->StellarForceValueCount,
		GetStellarForceValueChance,
		StellarForceValue
	);

	RTDataStellarForcePoolRef StellarForcePool = RTRuntimeDataStellarForcePoolGet(Runtime->Context, StellarForceValue->StellarForcePoolID);

	RTDataStellarForceEffectRef StellarForceEffect = NULL;
	ROLL_POOL(
		StellarForcePool->StellarForceEffectList,
		StellarForcePool->StellarForceEffectCount,
		GetStellarForceEffectChance,
		StellarForceEffect
	);
	
	MasterySlot->ForceEffect = StellarForceEffect->ForceEffectID;
	MasterySlot->ForceValue = StellarForceEffect->Value;
	MasterySlot->ForceValueType = StellarForceEffect->ValueType;
}

Void RTStellarMasteryRollLinkGrade(
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

	MasterySlot->LinkGrade = StellarLink->Grade;
}