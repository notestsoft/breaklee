#include "Character.h"
#include "MythMastery.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "NotificationProtocolDefinition.h"


/*
Rolls single option from the pool.

params:
- __POOL__ - pool to roll from.
- __COUNT__ - number of options in the pool.
- __GET_CHANCE__ - (*PoolItemType) -> Float32 - callback to get the chance of the option.
- __RESULT_REF__ - pointer to the result.
*/
#define ROLL_POOL(__POOL__, __COUNT__, __GET_CHANCE__, __RESULT_REF__)    \
    {                                                                     \
        Float32 MaxRandomValue = 0;                                         \
        for (Int Index = 0; Index < __COUNT__; Index += 1) {            \
            MaxRandomValue += __GET_CHANCE__(&__POOL__[Index]);           \
        }                                                                 \
                                                                          \
        Float32 RandomValue = RandomFloat(0, MaxRandomValue);        \
        Float32 CumulativeChance = 0;                                       \
                                                                          \
        for (Int Index = 0; Index < __COUNT__; Index += 1) {            \
            CumulativeChance += __GET_CHANCE__(&__POOL__[Index]);         \
            if (RandomValue < CumulativeChance) {                         \
                __RESULT_REF__ = &__POOL__[Index];                        \
                break;                                                    \
            }                                                             \
        }                                                                 \
    }


Void RTCharacterMythMasteryEnable(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (Character->Data.MythMasteryInfo.Info.Level > 0) return;

	//Int32 InitialPoints = Int32 RTData

	// add points from passing current level
	Character->Data.MythMasteryInfo.Info.Level = 1;
	Character->Data.MythMasteryInfo.Info.Points = 3;
	Character->SyncMask.MythMasteryInfo = true;

	// notify level up (actual MLV number change on client)
	NOTIFICATION_DATA_CHARACTER_DATA* LevelUpNotification = RTNotificationInit(CHARACTER_DATA);
	LevelUpNotification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_MYTH_LEVEL;
	LevelUpNotification->Level = 1;
	RTNotificationDispatchToNearby(LevelUpNotification, Character->Movement.WorldChunk);
}

Void RTCharacterMythMasteryAddExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	if (Character->Data.MythMasteryInfo.Info.Level < 1) return;

	// get current level info
	RTDataMythLevelRef CurrentLevel = RTRuntimeDataMythLevelGet(Runtime->Context, Character->Data.MythMasteryInfo.Info.Level);
	assert(CurrentLevel);

	Float RebirthPenaltyExpMultiplier = RTCharacterMythMasteryGetExpPenaltyMultiplier(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);

	// check how much we need to pass current level
	UInt64 RequiredCumulativeExp = RTCharacterMythMasteryGetCumulativeLevelUpExp(Runtime, Character);

	// add to exp
	Character->Data.MythMasteryInfo.Info.Exp += Exp;

	UInt64 MaxXP = RTCharacterMythMasteryGetMaximumExp(Runtime, Character);

	// cap the exp to the max
	if (Character->Data.MythMasteryInfo.Info.Exp > MaxXP) {
		Character->Data.MythMasteryInfo.Info.Exp = MaxXP;
	}

	// level gain loop, enter here when we surpass required exp
	while (RequiredCumulativeExp <= Character->Data.MythMasteryInfo.Info.Exp) {
		// look up data for next level
		RTDataMythLevelRef NextLevel = RTRuntimeDataMythLevelGet(Runtime->Context, Character->Data.MythMasteryInfo.Info.Level + 1);

		// if next level doesnt exist, we are at max level, so exit the loop
		if (!NextLevel) break;

		RTCharacterMythMasteryAddMythLevel(Runtime, Character, 1);

		// add onto required exp since we just leveled up
		RequiredCumulativeExp += NextLevel->RequiredExp * RebirthPenaltyExpMultiplier;
	}

	//struct _RTMythMasterySlot TestSlot = { 0 };
	//TestSlot.MasteryIndex = 0;
	//estSlot.SlotIndex = 0;
	//TestSlot.TierIndex = 1;
	//TestSlot.TierLevel = 2;
	//TestSlot.StatOption = 1;
	//TestSlot.StatValue = 10;
	//estSlot.ValueType = 1;

	//Character->Data.MythMasteryInfo.Slots[0] = TestSlot;
	//Character->SyncMask.MythMasteryInfo = true;

	// notify XP gained
	// FIXME TODO: Does not appear in game log: Gained MXP (bottom right message log)
	NOTIFICATION_DATA_MYTH_GAIN_XP* Notification = RTNotificationInit(MYTH_GAIN_XP);
	Notification->MythXP = Character->Data.MythMasteryInfo.Info.Exp;
	RTNotificationDispatchToCharacter(Notification, Character);
}

Void RTCharacterMythMasteryAddMythLevel(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 LevelsGiven
) {
	RTDataMythLevelRef CurrentLevel = RTRuntimeDataMythLevelGet(Runtime->Context, Character->Data.MythMasteryInfo.Info.Level);
	assert(CurrentLevel);

	RTDataMythLevelRef NextLevel = RTRuntimeDataMythLevelGet(Runtime->Context, Character->Data.MythMasteryInfo.Info.Level + 1);
	assert(NextLevel);

	// process each level up individually
	for (Int Index = 0; Index < LevelsGiven; Index++) {
		// add points from passing current level
		Character->Data.MythMasteryInfo.Info.Level = NextLevel->MythLevel;
		// update level before calling this func
		// this func also calls SyncMask update
		RTCharacterMythMasteryAddMythPoints(Runtime, Character, CurrentLevel->MythPoints);

		// notify level up (actual MLV number change on client)
		NOTIFICATION_DATA_CHARACTER_DATA* LevelUpNotification = RTNotificationInit(CHARACTER_DATA);
		LevelUpNotification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_MYTH_LEVEL;
		LevelUpNotification->Level = NextLevel->MythLevel;
		RTNotificationDispatchToNearby(LevelUpNotification, Character->Movement.WorldChunk);
	}
}

Void RTCharacterMythMasteryAddMythPoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 PointsGiven
) {
	Character->Data.MythMasteryInfo.Info.Points += PointsGiven;
	Character->SyncMask.MythMasteryInfo = true;

	// notify myth points
	NOTIFICATION_DATA_MYTH_POINTS* NotificationPersonal = RTNotificationInit(MYTH_POINTS);
	NotificationPersonal->MythPoints = Character->Data.MythMasteryInfo.Info.Points;
	RTNotificationDispatchToCharacter(NotificationPersonal, Character);
}

Float RTCharacterMythMasteryGetExpPenaltyMultiplier(
	RTRuntimeRef Runtime,
	Int32 CharacterRebirthCount
) {
	RTDataMythRepeatPenaltyRef MythRepeatPenaltyRef = RTRuntimeDataMythRepeatPenaltyGet(Runtime->Context);
	Float PenaltyPercentPerRebirth = 0.f;

	if (!MythRepeatPenaltyRef || MythRepeatPenaltyRef->Limit < 0) {
		Error("RTCharacterMythMasteryGetExpPenaltyMultiplier function ref was null or illegal! Failing gracefully.");
		return 0;
	}

	// cap the rebirth count at the limit
	// 0 or negative to disable the limit
	if (MythRepeatPenaltyRef->Limit > 0) {
		CharacterRebirthCount = MIN(MythRepeatPenaltyRef->Limit, CharacterRebirthCount);
	}

	// 10 AddMxp = 1.0% increase
	// adjust for amount of rebirths
	PenaltyPercentPerRebirth = MythRepeatPenaltyRef->AddMxp * CharacterRebirthCount;
	PenaltyPercentPerRebirth = PenaltyPercentPerRebirth / 1000.f;

	// min of 1
	return MAX(1.f + PenaltyPercentPerRebirth, 1.f);
}

// assumes the characters rebirth info is already updated
Int32 RTCharacterMythMasteryGetRepeatBonus(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	RTDataMythRepeatRef MythRepeatBonusRef = RTRuntimeDataMythRepeatGet(Runtime->Context);
	Bool IsEligibleForBonus = RTCharacterMythMasteryCheckEligibleForRepeatBonus(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);

	if (!IsEligibleForBonus || !MythRepeatBonusRef) {
		return 0;
	}

	return MythRepeatBonusRef->BonusPoints;
}

Bool RTCharacterMythMasteryCheckEligibleForRepeatBonus(
	RTRuntimeRef Runtime,
	Int32 RebirthCount
) {
	RTDataMythRepeatRef MythRepeatBonusRef = RTRuntimeDataMythRepeatGet(Runtime->Context);
	Int32 Remainder = 0;

	if (!MythRepeatBonusRef || MythRepeatBonusRef->RepeatCondition <= 0) {
		Error("RTCharacterMythMasteryCheckEligibleForRepeatBonus function ref was null or illegal! Failing gracefully.");
		return false;
	}

	// Repeat condition could be something like 5, so give the bonus every 5 levels
	Remainder = RebirthCount % MythRepeatBonusRef->RepeatCondition;

	return Remainder == 0;
}

Int32 RTCharacterMythMasteryGetMaximumLevel(
	RTRuntimeRef Runtime
) {
	RTDataMythMaxRef MythMaxRef = RTRuntimeDataMythMaxGet(Runtime->Context);

	if (!MythMaxRef || MythMaxRef->MaxMythLevel <= 0) {
		Error("RTCharacterMythMasteryGetMaximumLevel function ref was null or illegal! Failing gracefully.");
		return 100;
	}

	return MythMaxRef->MaxMythLevel;
}

UInt64 RTCharacterMythMasteryGetMaximumExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	UInt64 ExpNeeded = 0;
	Int32 MaxLevel = RTCharacterMythMasteryGetMaximumLevel(Runtime);
	Float ExpPenaltyMultiplier = RTCharacterMythMasteryGetExpPenaltyMultiplier(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);

	// from 1 to max
	for (int Index = 1; Index < MaxLevel + 1; Index++) {
		RTDataMythLevelRef PrevLevelRef = RTRuntimeDataMythLevelGet(Runtime->Context, Index);

		// no more to go back
		if (!PrevLevelRef) break;

		ExpNeeded += PrevLevelRef->RequiredExp;
	}

	// add penalty from rebirth count
	ExpNeeded *= ExpPenaltyMultiplier;

	return ExpNeeded;
}

UInt64 RTCharacterMythMasteryGetCumulativeLevelUpExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	UInt64 ExpNeeded = 0;
	// 0% displayed XP = all previous levels added together
	// 100% displayed XP = prev XP + required XP of current

	// from 1 to current level
	for (int Index = 1; Index < Character->Data.MythMasteryInfo.Info.Level + 1; Index++) {
		RTDataMythLevelRef PrevMythLevelRef = RTRuntimeDataMythLevelGet(Runtime->Context, Index);

		// no more to go back
		if (!PrevMythLevelRef) break;

		ExpNeeded += PrevMythLevelRef->RequiredExp * RTCharacterMythMasteryGetExpPenaltyMultiplier(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);
	}

	return ExpNeeded;
}

// does NOT check force gem price!
Bool RTCharacterMythMasteryRebirth(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (!RTCharacterMythMasteryGetCanRebirth(Runtime, Character)) {
		return false;
	}

	Int32 RebirthReward = RTCharacterMythMasteryGetRepeatBonus(Runtime, Character);

	if (RebirthReward > 0) {
		RTCharacterMythMasteryAddMythPoints(Runtime, Character, RebirthReward);
	}

	// update level and sync
	Character->Data.MythMasteryInfo.Info.Rebirth += 1;
	Character->Data.MythMasteryInfo.Info.Exp = 0;
	Character->Data.MythMasteryInfo.Info.Level = 1;
	Character->SyncMask.MythMasteryInfo = true;

	// myth resurrect up
	// resets client XP to 0
	// resets client lvl to 1
	// increases resurrection points
	NOTIFICATION_DATA_CHARACTER_DATA* LevelUpNotification = RTNotificationInit(CHARACTER_DATA);
	LevelUpNotification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_MYTH_RESURRECT;
	LevelUpNotification->Level = 1;
	RTNotificationDispatchToNearby(LevelUpNotification, Character->Movement.WorldChunk);

	// this adds item to inventory
	NOTIFICATION_DATA_MYTH_RESURRECT_REWARD* ResNotification = RTNotificationInit(MYTH_RESURRECT_REWARD);
	ResNotification->Result = 1;
	ResNotification->ItemId = 33562089;
	ResNotification->ItemOptions = 35;
	ResNotification->InventorySlotIndex = 41;
	ResNotification->Unknown2 = 1;
	RTNotificationDispatchToCharacter(ResNotification, Character);

	return true;
}

// does NOT check force gem price!
Bool RTCharacterMythMasteryGetCanRebirth(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	RTDataMythResetRef MythResetRef = RTRuntimeDataMythResetGet(Runtime->Context);

	if (!MythResetRef) {
		Error("RTCharacterMythMasteryGetCanRebirth function ref was null or illegal! Failing gracefully.");
		return false;
	}

	// allow 0 or negative to disable the min
	if (MythResetRef->RequiredLevel <= 0) {
		return true;
	}

	return Character->Data.MythMasteryInfo.Info.Level >= MythResetRef->RequiredLevel;
}

UInt32 RTCharacterMythMasteryGetRebirthGemCost(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 MaxLevel = RTCharacterMythMasteryGetMaximumLevel(Runtime);
	if (Character->Data.MythMasteryInfo.Info.Level == MaxLevel) {
		return 0;
	}

	RTDataMythRebirthPenaltyRef MythRebirthPenaltyRef = RTRuntimeDataMythRebirthPenaltyGet(Runtime->Context);

	if (!MythRebirthPenaltyRef) {
		return 0;
	}

	Int32 MissingLevels = MaxLevel - Character->Data.MythMasteryInfo.Info.Level;
	Int32 GemCost = MythRebirthPenaltyRef->PenaltyPerMissingLevel * MissingLevels;

	return GemCost;
}

RTMythMasterySlotRef RTCharacterMythMasteryGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex
) {
	RTMythMasterySlotRef OutSlot = NULL;

	for (UInt32 i = 0; i < Character->Data.MythMasteryInfo.Info.PropertySlotCount; i++) {
		RTMythMasterySlotRef Slot = &Character->Data.MythMasteryInfo.Slots[i];
		if (Slot->MasteryIndex == MasteryIndex && Slot->SlotIndex == SlotIndex) {
			OutSlot = Slot;
			break;
		}
	}

	return OutSlot;
}

Void RTCharacterMythMasterySetSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex,
	RTDataMythMasterySlotRef NewSlotValues
) {
	if (!RTCharacterMythMasteryGetSlot(Runtime, Character, MasteryIndex, SlotIndex)) {
		assert(Character->Data.MythMasteryInfo.Info.PropertySlotCount <= RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT);
		RTMythMasterySlotRef NewSlot = &Character->Data.MythMasteryInfo.Slots[Character->Data.MythMasteryInfo.Info.PropertySlotCount];
		NewSlot->StatOption = NewSlotValues->ForceCode;
		NewSlot->StatValue = NewSlotValues->Value;
		NewSlot->ValueType = NewSlotValues->ValueType;
		NewSlot->Tier = NewSlotValues->Tier;
		NewSlot->Grade = NewSlotValues->Grade;
	}

	for (UInt32 i = 0; i < Character->Data.MythMasteryInfo.Info.PropertySlotCount; i++) {
		RTMythMasterySlotRef Slot = &Character->Data.MythMasteryInfo.Slots[i];
		if (Slot->MasteryIndex == MasteryIndex && Slot->SlotIndex == SlotIndex) {
			Character->Data.MythMasteryInfo.Slots[i].StatOption = NewSlotValues->ForceCode;
			Character->Data.MythMasteryInfo.Slots[i].StatValue = NewSlotValues->Value;
			Character->Data.MythMasteryInfo.Slots[i].ValueType = NewSlotValues->ValueType;
			Character->Data.MythMasteryInfo.Slots[i].Tier = NewSlotValues->Tier;
			Character->Data.MythMasteryInfo.Slots[i].Grade = NewSlotValues->Grade;
			break;
		}
	}

	Character->SyncMask.MythMasteryInfo = true;

	//return OutSlot;
}

Float32 GetMythSlotValueChance(RTDataMythMasterySlotRef MythSlot) {
	return MythSlot->Chance;
}

RTDataMythMasterySlotRef RTCharacterMythMasteryRollSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex
) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level == 0) {
		return NULL;
	}

	// all groups are currently in this pool, i don't know why
	RTDataMythSlotGroupPoolRef MythSlotGroupPoolRef = RTRuntimeDataMythSlotGroupPoolGet(Runtime->Context);
	RTDataMythSlotGroupRef MythSlotGroupRef = NULL;

	// loop all slot groups
	for (UInt32 SlotGroups = 0; SlotGroups < MythSlotGroupPoolRef->MythSlotGroupCount; SlotGroups++) {
		RTDataMythSlotGroupRef SlotGroup = &MythSlotGroupPoolRef->MythSlotGroupList[SlotGroups];

		// this group has our slot indexes
		if (SlotGroup->MasteryIndex == MasteryIndex && SlotGroup->SlotIndex == SlotIndex) {
			// pass along
			MythSlotGroupRef = SlotGroup;
			break;
		}
	}
	if (!MythSlotGroupRef) return NULL;

	// get the slot value group for our slot
	RTDataMythSlotValuePoolRef MythSlotValuePool = RTRuntimeDataMythSlotValuePoolGet(Runtime->Context);
	RTDataMythMasterySlotRef AvailableSlots[RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT];

	UInt32 SlotsIndex = 0;
	for (SlotsIndex = 0; SlotsIndex < Character->Data.MythMasteryInfo.Info.PropertySlotCount; SlotsIndex++) {
		RTDataMythMasterySlotRef SlotValue = &MythSlotValuePool->MythMasterySlotList[SlotsIndex];

		if (SlotValue->OptPoolID == MythSlotGroupRef->OptPoolID) {
			AvailableSlots[SlotsIndex] = SlotValue;
		}
	}

	// finally, roll the slot value from our proper slot group value pool
	RTDataMythMasterySlotRef MythSlotValue = NULL;
	ROLL_POOL(
		AvailableSlots,
		SlotsIndex + 1,
		GetMythSlotValueChance,
		MythSlotValue
	);
	if (!MythSlotValue) return NULL;

	//assert(Character->Data.MythMasteryInfo.Info.PropertySlotCount <= RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT);
	//RTMythMasterySlotRef ReturnSlot = &Character->Data.MythMasteryInfo.Slots[Character->Data.MythMasteryInfo.Info.PropertySlotCount];

	return MythSlotValue;
}

Bool RTCharacterMythMasteryGetCanOpenLockGroup(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 LockGroup
) {
	RTDataMythLockPageRef MythLockPageRef = RTRuntimeDataMythLockPageGet(Runtime->Context, MasteryIndex);
	RTDataMythLockInfoRef MythLockInfoRef = RTRuntimeDataMythLockInfoGet(MythLockPageRef, LockGroup);
	RTDataMythLockInfoRef PrevMythLockInfoRef = RTRuntimeDataMythLockInfoGet(MythLockPageRef, MAX(LockGroup - 1, 0));

	// sanity check on only 1 progression at a time, a bit ugly
	if (PrevMythLockInfoRef && PrevMythLockInfoRef != MythLockInfoRef && MythLockInfoRef->LockGroup - PrevMythLockInfoRef->LockGroup != 1) {
		return false;
	}

	if (MythLockInfoRef->OpenScore <= Character->Data.MythMasteryInfo.Info.HolyPower) {
		return true;
	}

	return false;
}

Bool RTCharacterMythMasteryGetSlotOccupied(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex
) {
	if (Character->Data.MythMasteryInfo.Info.PropertySlotCount == 0) {
		return false;
	}

	return RTCharacterMythMasteryGetSlot(Runtime, Character, MasteryIndex, SlotIndex) != NULL;
}

