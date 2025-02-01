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
	RTDataMythLevelRef CurrentMythLevel = RTRuntimeDataMythLevelGet(Runtime->Context, Character->Data.MythMasteryInfo.Info.Level);
	assert(CurrentMythLevel);

	Float RebirthPenaltyExpMultiplier = RTCharacterMythMasteryGetExpPenaltyMultiplier(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);

	// check how much we need to pass current level
	UInt64 RequiredCumulativeExp = RTCharacterMythMasteryGetCumulativeLevelUpExp(Runtime, Character);
	UInt64 MaxCumulativeExp = RTCharacterMythMasteryGetMaximumExp(Runtime, Character);

	// add to exp
	Character->Data.MythMasteryInfo.Info.Exp = MIN(
		Character->Data.MythMasteryInfo.Info.Exp + Exp,
		MaxCumulativeExp
	);

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
	Int32 Levels
) {
	Int32 TotalPointReward = 0;
	Int32 TargetLevel = Character->Data.MythMasteryInfo.Info.Level;

	for (Int Index = 0; Index < Levels; Index += 1) {
		RTDataMythLevelRef CurrentLevel = RTRuntimeDataMythLevelGet(Runtime->Context, TargetLevel);
		assert(CurrentLevel);

		RTDataMythLevelRef NextLevel = RTRuntimeDataMythLevelGet(Runtime->Context, TargetLevel + 1);
		if (!NextLevel) break;

		TotalPointReward += CurrentLevel->MythPoints;
		TargetLevel = NextLevel->MythLevel;
	}

	// add points from passing current level
	RTCharacterMythMasteryAddMythPoints(Runtime, Character, TotalPointReward);

	// notify level up (actual MLV number change on client)
	if (Character->Data.MythMasteryInfo.Info.Level != TargetLevel) {
		Character->Data.MythMasteryInfo.Info.Level = TargetLevel;
		Character->SyncMask.MythMasteryInfo = true;

		NOTIFICATION_DATA_CHARACTER_DATA* LevelUpNotification = RTNotificationInit(CHARACTER_DATA);
		LevelUpNotification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_MYTH_LEVEL;
		LevelUpNotification->Level = Character->Data.MythMasteryInfo.Info.Level;
		RTNotificationDispatchToNearby(LevelUpNotification, Character->Movement.WorldChunk);
	}
}

Void RTCharacterMythMasteryAddMythPoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 Points
) {
	if (Points < 1) return;

	Character->Data.MythMasteryInfo.Info.Points += Points;
	Character->SyncMask.MythMasteryInfo = true;

	// notify myth points
	NOTIFICATION_DATA_MYTH_POINTS* NotificationPersonal = RTNotificationInit(MYTH_POINTS);
	NotificationPersonal->MythPoints = Character->Data.MythMasteryInfo.Info.Points;
	RTNotificationDispatchToCharacter(NotificationPersonal, Character);
}

Float RTCharacterMythMasteryGetExpPenaltyMultiplier(
	RTRuntimeRef Runtime,
	Int32 RebirthCount
) {
	RTDataMythRepeatPenaltyRef MythRepeatPenalty = RTRuntimeDataMythRepeatPenaltyGet(Runtime->Context);
	if (!MythRepeatPenalty || MythRepeatPenalty->Limit < 0) {
		Error("RTCharacterMythMasteryGetExpPenaltyMultiplier function ref was null or illegal! Failing gracefully.");
		return 0;
	}

	// cap the rebirth count at the limit
	// 0 or negative to disable the limit
	if (MythRepeatPenalty->Limit > 0) {
		RebirthCount = MIN(MythRepeatPenalty->Limit, RebirthCount);
	}

	// 10 AddMxp = 1.0% increase
	// adjust for amount of rebirths
	Float PenaltyPercentPerRebirth = MythRepeatPenalty->AddMxp * RebirthCount;
	PenaltyPercentPerRebirth = PenaltyPercentPerRebirth / 1000.f;

	// min of 1
	return MAX(1.f + PenaltyPercentPerRebirth, 1.f);
}

// assumes the characters rebirth info is already updated
Int32 RTCharacterMythMasteryGetRepeatBonus(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	RTDataMythRepeatRef MythRepeatBonus = RTRuntimeDataMythRepeatGet(Runtime->Context);
	Bool IsEligibleForBonus = RTCharacterMythMasteryCheckEligibleForRepeatBonus(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);
	if (!IsEligibleForBonus || !MythRepeatBonus) return 0;

	return MythRepeatBonus->BonusPoints;
}

Bool RTCharacterMythMasteryCheckEligibleForRepeatBonus(
	RTRuntimeRef Runtime,
	Int32 RebirthCount
) {
	RTDataMythRepeatRef MythRepeatBonus = RTRuntimeDataMythRepeatGet(Runtime->Context);
	if (!MythRepeatBonus || MythRepeatBonus->RepeatCondition <= 0) {
		Error("RTCharacterMythMasteryCheckEligibleForRepeatBonus function ref was null or illegal! Failing gracefully.");
		return false;
	}

	// Repeat condition could be something like 5, so give the bonus every 5 levels
	Int32 Remainder = RebirthCount % MythRepeatBonus->RepeatCondition;
	return Remainder == 0;
}

Int32 RTCharacterMythMasteryGetMaximumLevel(
	RTRuntimeRef Runtime
) {
	RTDataMythMaxRef MythMax = RTRuntimeDataMythMaxGet(Runtime->Context);
	if (!MythMax || MythMax->MaxMythLevel <= 0) {
		Error("RTCharacterMythMasteryGetMaximumLevel function ref was null or illegal! Failing gracefully.");
		return 100;
	}

	return MythMax->MaxMythLevel;
}

UInt64 RTCharacterMythMasteryGetMaximumExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	UInt64 ExpNeeded = 0;
	Int32 MaxLevel = RTCharacterMythMasteryGetMaximumLevel(Runtime);
	Float ExpPenaltyMultiplier = RTCharacterMythMasteryGetExpPenaltyMultiplier(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);

	// from 1 to max
	for (Int Index = 1; Index < MaxLevel + 1; Index += 1) {
		RTDataMythLevelRef PrevLevel = RTRuntimeDataMythLevelGet(Runtime->Context, Index);

		// no more to go back
		if (!PrevLevel) break;

		ExpNeeded += PrevLevel->RequiredExp;
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
	for (Int Index = 1; Index < Character->Data.MythMasteryInfo.Info.Level + 1; Index += 1) {
		RTDataMythLevelRef PrevMythLevel = RTRuntimeDataMythLevelGet(Runtime->Context, Index);

		// no more to go back
		if (!PrevMythLevel) break;

		ExpNeeded += PrevMythLevel->RequiredExp * RTCharacterMythMasteryGetExpPenaltyMultiplier(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);
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
	RTDataMythResetRef MythReset = RTRuntimeDataMythResetGet(Runtime->Context);
	if (!MythReset) {
		Error("RTCharacterMythMasteryGetCanRebirth function ref was null or illegal! Failing gracefully.");
		return false;
	}

	// allow 0 or negative to disable the min
	if (MythReset->RequiredLevel <= 0) {
		return true;
	}

	return Character->Data.MythMasteryInfo.Info.Level >= MythReset->RequiredLevel;
}

UInt32 RTCharacterMythMasteryGetRebirthGemCost(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 MaxLevel = RTCharacterMythMasteryGetMaximumLevel(Runtime);
	if (Character->Data.MythMasteryInfo.Info.Level == MaxLevel) {
		return 0;
	}

	RTDataMythRebirthPenaltyRef MythRebirthPenalty = RTRuntimeDataMythRebirthPenaltyGet(Runtime->Context);

	if (!MythRebirthPenalty) {
		return 0;
	}

	Int32 MissingLevels = MaxLevel - Character->Data.MythMasteryInfo.Info.Level;
	Int32 GemCost = MythRebirthPenalty->PenaltyPerMissingLevel * MissingLevels;

	return GemCost;
}

RTMythMasterySlotRef RTCharacterMythMasteryGetSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex
) {
	for (Int Index = 0; Index < Character->Data.MythMasteryInfo.Info.MasterySlotCount; Index += 1) {
		RTMythMasterySlotRef MasterySlot = &Character->Data.MythMasteryInfo.Slots[Index];
		if (MasterySlot->MasteryIndex == MasteryIndex && MasterySlot->SlotIndex == SlotIndex) return MasterySlot;
	}

	return NULL;
}

Void RTCharacterMythMasterySetSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex,
	Int32 Tier,
	Int32 Grade,
	Int32 ForceEffectIndex,
	Int32 ForceValue,
	Int32 ForceValueType
) {
	RTMythMasterySlotRef MasterySlot = RTCharacterMythMasteryGetSlot(Runtime, Character, MasteryIndex, SlotIndex);
	if (!MasterySlot) {
		assert(Character->Data.MythMasteryInfo.Info.MasterySlotCount <= RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT);

		MasterySlot = &Character->Data.MythMasteryInfo.Slots[Character->Data.MythMasteryInfo.Info.MasterySlotCount];
		memset(MasterySlot, 0, sizeof(struct _RTMythMasterySlot));
		MasterySlot->MasteryIndex = MasteryIndex;
		MasterySlot->SlotIndex = SlotIndex;
	}

	assert(MasterySlot);
	MasterySlot->Tier = Tier;
	MasterySlot->Grade = Grade;
	MasterySlot->ForceEffectIndex = ForceEffectIndex;
	MasterySlot->ForceValue = ForceValue;
	MasterySlot->ForceValueType = ForceValueType;
	Character->SyncMask.MythMasteryInfo = true;
}

RTMythMasterySlotRef RTCharacterMythMasteryRollSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex
) {
	if (!Character || Character->Data.MythMasteryInfo.Info.Level == 0) {
		return NULL;
	}

	// all groups are currently in this pool, i don't know why
	RTDataMythSlotGroupPoolRef MythSlotGroupPool = RTRuntimeDataMythSlotGroupPoolGet(Runtime->Context);
	RTDataMythSlotGroupRef MythSlotGroup = NULL;

	// loop all slot groups
	for (Int SlotGroupIndex = 0; SlotGroupIndex < MythSlotGroupPool->MythSlotGroupCount; SlotGroupIndex++) {
		RTDataMythSlotGroupRef SlotGroup = &MythSlotGroupPool->MythSlotGroupList[SlotGroupIndex];

		// this group has our slot indexes
		if (SlotGroup->MasteryIndex == MasteryIndex && SlotGroup->SlotIndex == SlotIndex) {
			// pass along
			MythSlotGroup = SlotGroup;
			break;
		}
	}
	if (!MythSlotGroup) return NULL;

	// get the slot value group for our slot
	RTDataMythSlotValuePoolRef MythSlotValuePool = RTRuntimeDataMythSlotValuePoolGet(Runtime->Context);
	RTDataMythMasterySlotRef AvailableSlots[RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT] = { 0 };

	Int SlotsIndex = 0;
	for (SlotsIndex = 0; SlotsIndex < Character->Data.MythMasteryInfo.Info.MasterySlotCount; SlotsIndex += 1) {
		RTDataMythMasterySlotRef SlotValue = &MythSlotValuePool->MythMasterySlotList[SlotsIndex];
		if (SlotValue->OptPoolID == MythSlotGroup->OptPoolID) {
			AvailableSlots[SlotsIndex] = SlotValue;
		}
	}
	// finally, roll the slot value from our proper slot group value pool
	RTDataMythMasterySlotRef MythSlotValue = NULL;
/*
	ROLL_POOL(
		AvailableSlots,
		SlotsIndex + 1,
		GetMythSlotValueChance,
		MythSlotValue
	);
	if (!MythSlotValue) return NULL;
	*/
	//assert(Character->Data.MythMasteryInfo.Info.PropertySlotCount <= RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT);
	//RTMythMasterySlotRef ReturnSlot = &Character->Data.MythMasteryInfo.Slots[Character->Data.MythMasteryInfo.Info.PropertySlotCount];

	return MythSlotValue;
}

Bool RTCharacterMythMasteryCanOpenLockGroup(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 LockGroup
) {
	RTDataMythLockPageRef LockPage = RTRuntimeDataMythLockPageGet(Runtime->Context, MasteryIndex);
	RTDataMythLockInfoRef LockInfo = RTRuntimeDataMythLockInfoGet(LockPage, LockGroup);
	if (LockInfo->OpenScore > Character->Data.MythMasteryInfo.Info.HolyPower) return false;

	// sanity check on only 1 progression at a time, a bit ugly
	RTDataMythLockInfoRef PreviousLockInfo = RTRuntimeDataMythLockInfoGet(LockPage, MAX(0, LockGroup - 1));
	if (PreviousLockInfo && PreviousLockInfo != LockInfo && LockInfo->LockGroup - PreviousLockInfo->LockGroup != 1) {
		return false;
	}

	return true;
}

Bool RTCharacterMythMasteryGetSlotOccupied(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex
) {
	return RTCharacterMythMasteryGetSlot(Runtime, Character, MasteryIndex, SlotIndex) != NULL;
}

