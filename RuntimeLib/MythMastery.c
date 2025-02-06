#include "Character.h"
#include "MythMastery.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

enum {
	RUNTIME_MYTH_ENABLE_QUEST_IDX = 3663,
};

Void RTCharacterMythMasteryFinishQuest(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 QuestIndex
) {
	if (QuestIndex != RUNTIME_MYTH_ENABLE_QUEST_IDX || Character->Data.MythMasteryInfo.Info.Level > 0) return;

	RTCharacterMythMasteryEnable(Runtime, Character);
}

Void RTCharacterMythMasteryEnable(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (Character->Data.MythMasteryInfo.Info.Level > 0) return;

	RTDataMythStartRef MythStartRef = RTRuntimeDataMythStartGet(Runtime->Context);
	assert(MythStartRef);

	// init to level 1
	Character->Data.MythMasteryInfo.Info.Level = 1;
	Character->SyncMask.MythMasteryInfo = true;

	// notify level up (actual MLV number change on client)
	NOTIFICATION_DATA_CHARACTER_DATA* LevelUpNotification = RTNotificationInit(CHARACTER_DATA);
	LevelUpNotification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_MYTH_LEVEL;
	LevelUpNotification->Level = 1;
	RTNotificationDispatchToCharacter(LevelUpNotification, Character);
	 
	// add initial myth points
	RTCharacterMythMasteryAddMythPoints(Runtime, Character, MythStartRef->InitialPoints);
}

Void RTCharacterMythMasteryStigmaEnable(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (Character->Data.MythMasteryInfo.Info.StigmaGrade > 0) return;

	RTDataMythStigmaOpenRef MythStigmaOpenRef = RTRuntimeDataMythStigmaOpenGet(Runtime->Context);
	assert(MythStigmaOpenRef);

	// add backwards check just in case
	if (MythStigmaOpenRef->RequiredSlot < Character->Data.MythMasteryInfo.Info.MasterySlotCount) {
		return;
	}

	// init to level 1
	// this should only be called when rolling NEW slot
	// the packet notifiying of stigma must be done there
	Character->Data.MythMasteryInfo.Info.StigmaExp = 1;
	Character->Data.MythMasteryInfo.Info.StigmaGrade = 1;
	Character->SyncMask.MythMasteryInfo = true;
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

	// notify XP gained
	// FIXME TODO: Does not appear in game log: Gained MXP (bottom right message log)
	NOTIFICATION_DATA_MYTH_GAIN_XP* Notification = RTNotificationInit(MYTH_GAIN_XP);
	Notification->MythXP = Character->Data.MythMasteryInfo.Info.Exp;
	RTNotificationDispatchToCharacter(Notification, Character);

	Character->SyncMask.MythMasteryInfo = true;
}

Void RTCharacterMythMasteryAddExpPercent(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 ExpPercent
) {
	if (Character->Data.MythMasteryInfo.Info.Level < 1) return;

	// get current level info
	RTDataMythLevelRef CurrentMythLevel = RTRuntimeDataMythLevelGet(Runtime->Context, Character->Data.MythMasteryInfo.Info.Level);
	assert(CurrentMythLevel);

	UInt64 GivenEXP = (CurrentMythLevel->RequiredExp * (UInt64)ExpPercent) / 100;

	// Add myth XP penalty mod
	GivenEXP *= RTCharacterMythMasteryGetExpPenaltyMultiplier(Runtime, Character->Data.MythMasteryInfo.Info.Rebirth);

	RTCharacterMythMasteryAddExp(Runtime, Character, GivenEXP);
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
		RTNotificationDispatchToCharacter(LevelUpNotification, Character);
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
	// sets resurrection count
	NOTIFICATION_DATA_CHARACTER_DATA* LevelUpNotification = RTNotificationInit(CHARACTER_DATA);
	LevelUpNotification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_MYTH_RESURRECT;
	LevelUpNotification->Level = Character->Data.MythMasteryInfo.Info.Rebirth;
	RTNotificationDispatchToCharacter(LevelUpNotification, Character);

	//TODO: Send this packet and add item to server side inventory
	// this adds item to inventory
	/*
	NOTIFICATION_DATA_MYTH_RESURRECT_REWARD* ResNotification = RTNotificationInit(MYTH_RESURRECT_REWARD);
	ResNotification->Result = 1;
	ResNotification->ItemId = 33562089;
	ResNotification->ItemOptions = 35;
	ResNotification->InventorySlotIndex = 41;
	ResNotification->Unknown2 = 1;
	RTNotificationDispatchToCharacter(ResNotification, Character);
	*/

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
		if (MasterySlot->MasteryIndex == MasteryIndex && MasterySlot->SlotIndex == SlotIndex) {
			return MasterySlot;
		}
	}

	return NULL;
}

RTMythMasterySlotRef RTCharacterMythMasteryGetOrCreateSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex
) {
	if (Character->Data.MythMasteryInfo.Info.Level < 1) return NULL;

	// get slot ends here
	RTMythMasterySlotRef MasterySlot = RTCharacterMythMasteryGetSlot(Runtime, Character, MasteryIndex, SlotIndex);
	if (MasterySlot) {
		Error("Get/Create failed on MasterySlot");
		return MasterySlot;
	}

	// here and below we are opening slot for first time
	RTDataMythSlotPageRef MasterySlotPage = RTRuntimeDataMythSlotPageGet(Runtime->Context, MasteryIndex);
	if (!MasterySlotPage) {
		Error("No myth mastery slot page!");
		return NULL;
	}

	RTDataMythSlotInfoRef MasterySlotInfo = RTRuntimeDataMythSlotInfoGet(MasterySlotPage, SlotIndex);
	if (!MasterySlotInfo) {
		Error("No myth mastery slot info!");
		return NULL;
	}

	if (Character->Data.MythMasteryInfo.Info.Points < MasterySlotInfo->UnlockCost || !RTCharacterMythMasteryGetPrerequisiteMetForSlot(Runtime, Character, MasteryIndex, MasterySlotInfo->PrerequisiteSlot, MasterySlotInfo->PrerequisiteSlot2)) {
		Error("Points: %d Required Points: %d", Character->Data.MythMasteryInfo.Info.Points, MasterySlotInfo->UnlockCost);
		Error("Failed prereq while creating slot!");
		return NULL;
	}

	assert(Character->Data.MythMasteryInfo.Info.MasterySlotCount <= RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT);

	MasterySlot = &Character->Data.MythMasteryInfo.Slots[Character->Data.MythMasteryInfo.Info.MasterySlotCount];
	memset(MasterySlot, 0, sizeof(struct _RTMythMasterySlot));
	MasterySlot->MasteryIndex = MasteryIndex;
	MasterySlot->SlotIndex = SlotIndex;
	Character->Data.MythMasteryInfo.Info.MasterySlotCount += 1;

	// hook here since MasterySlotCount is only incremented in this function
	// if you unlock slots else where, please call this!!!
	RTCharacterMythMasteryStigmaEnable(Runtime, Character);
	RTCharacterMythMasteryAssertHolyPoints(Runtime, Character);

	Character->SyncMask.MythMasteryInfo = true;

	return MasterySlot;
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
	RTCharacterMythMasteryAssertHolyPoints(Runtime, Character);
	Character->SyncMask.MythMasteryInfo = true;
}

Bool RTCharacterMythMasteryRollSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTMythMasterySlotRef MasterySlot
) {
	if (Character->Data.MythMasteryInfo.Info.Level < 1) return false;

	RTDataMythMasterySlotGroupRef MasterySlotGroup = RTRuntimeDataMythMasterySlotGroupGet(
		Runtime->Context,
		MasterySlot->MasteryIndex,
		MasterySlot->SlotIndex
	);
	if (!MasterySlotGroup) {
		Error("No myth mastery slot group!");
		return false;
	}

	RTDataMythMasterySlotValuePoolRef MasterySlotValuePool = RTRuntimeDataMythMasterySlotValuePoolGet(Runtime->Context, MasterySlotGroup->PoolID);
	if (!MasterySlotValuePool) {
		Error("No myth mastery slot value pool !");
		return false;
	}

	Int32 SlotRollPrice = MasterySlotGroup->UnlockCost;

	if (Character->Data.MythMasteryInfo.Info.Points < SlotRollPrice) {
		return false;
	}

	Int32 Seed = (Int32)PlatformGetTickCount();
	Int32 RateValue = RandomRange(&Seed, 0, INT32_MAX);
	Int32 RateOffset = 0;

	for (Int Index = 0; Index < MasterySlotValuePool->MythMasterySlotValueCount; Index += 1) {
		RTDataMythMasterySlotValueRef MasterySlotValue = &MasterySlotValuePool->MythMasterySlotValueList[Index];
		Int32 Rate = MasterySlotValue->Rate * INT32_MAX / 100;
		if (RateValue <= Rate + RateOffset) {
			MasterySlot->Tier = MasterySlotValue->Tier;
			MasterySlot->Grade = MasterySlotValue->Grade;
			MasterySlot->ForceEffectIndex = MasterySlotValue->ForceEffectIndex;
			MasterySlot->ForceValue = MasterySlotValue->ForceValue;
			MasterySlot->ForceValueType = MasterySlotValue->ForceValueType;
			Character->SyncMask.MythMasteryInfo = true;
			Character->Data.MythMasteryInfo.Info.Points -= SlotRollPrice;
			RTCharacterMythMasteryAddStigmaExp(Runtime, Character, SlotRollPrice);
			RTCharacterMythMasteryAssertHolyPoints(Runtime, Character);
			return true;
		}

		RateOffset += Rate;
	}

	return false;
}

Bool RTCharacterMythMasteryCanOpenLockGroup(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 LockGroup
) {
	RTDataMythLockPageRef LockPage = RTRuntimeDataMythLockPageGet(Runtime->Context, MasteryIndex);
	RTDataMythLockInfoRef LockInfo = RTRuntimeDataMythLockInfoGet(LockPage, LockGroup);

	if (LockInfo->OpenScore > RTCharacterMythMasteryGetRealHolyPoints(Runtime, Character)) return false;

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

// used for serverside only
// for calculating myth grade (bonus like Seraphim, Metatron etc)
Int32 RTCharacterMythMasteryGetRealHolyPoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	return Character->Data.MythMasteryInfo.Info.HolyPower + Character->Data.MythMasteryInfo.BonusHolyPower;
}

Void RTCharacterMythMasteryAssertHolyPoints(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Int32 PointsFromSlots = 0;
	Int32 PointsFromMythGrade = 0;

	// stigma -- COUNTS AS BONUS, DO NOT SEND TO CLIENT. IT CALCULATES CLIENT SIDE
	for (Int32 Index = 0; Index < Character->Data.MythMasteryInfo.Info.StigmaGrade; Index++) {
		RTDataStigmaInfoRef MythStigmaInfo = RTRuntimeDataStigmaInfoGet(Runtime->Context, Index);
		assert(MythStigmaInfo);
		// stop at our level. we must exceed the grade to collect it's points
		if (MythStigmaInfo->Grade >= Character->Data.MythMasteryInfo.Info.StigmaGrade) break;

		PointsFromMythGrade += MythStigmaInfo->AwardedHolyPoint;
	}

	RTDataMythLockPageRef MythLockPageInfo = RTRuntimeDataMythLockPageGet(Runtime->Context, 0);

	// lock group
	for (Int32 Index = 0; Index < MythLockPageInfo->MythLockInfoCount; Index++) {
		// stop at above our level. we must match or be greater than page count 1st
		if (Character->Data.MythMasteryInfo.Info.UnlockedPageCount < MythLockPageInfo->MythLockInfoList[Index].LockGroup) break;

		PointsFromSlots += MythLockPageInfo->MythLockInfoList[Index].BonusScore;
	}

	// slots
	// TODO make flexible with mastery index
	for (Int32 Index = 0; Index < RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT; Index++) {
		RTMythMasterySlotRef CurrentSlot = RTCharacterMythMasteryGetSlot(Runtime, Character, 0, Index);

		// not sure if this will exit too early with break, but wont hurt to use continue only
		if (!CurrentSlot) continue;

		PointsFromSlots += RTCharacterMythMasteryGetSlotHolyValue(Runtime, Character, CurrentSlot);
	}

	Character->Data.MythMasteryInfo.Info.HolyPower = PointsFromSlots;
	Character->Data.MythMasteryInfo.BonusHolyPower = PointsFromMythGrade;
	RTCharacterInitializeAttributes(Runtime, Character);
	Character->SyncMask.MythMasteryInfo = true;
}

Int32 RTCharacterMythMasteryGetSlotHolyValue(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTMythMasterySlotRef Slot
) {
	RTDataMythSlotTierRewardRef MythSlotTierReward = RTRuntimeDataMythSlotTierRewardGet(Runtime->Context, Slot->Tier);
	if (!MythSlotTierReward) return false;

	RTDataMythTierLevelRef MythTierLevel = RTRuntimeDataMythTierLevelGet(MythSlotTierReward, Slot->Grade);

	return MythTierLevel->Points;
}

Void RTCharacterMythMasteryAddStigmaExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 ExpAmount
) {
	if (Character->Data.MythMasteryInfo.Info.StigmaGrade < 1) return;

	Int32 CurrentExp = Character->Data.MythMasteryInfo.Info.StigmaExp + ExpAmount;

	// get current level
	RTDataStigmaInfoRef MythStigmaInfo = RTRuntimeDataStigmaInfoGet(Runtime->Context, Character->Data.MythMasteryInfo.Info.StigmaGrade);
	assert(MythStigmaInfo);

	Int32 ExpForLevelup = MythStigmaInfo->RequiredExp;

	while (CurrentExp >= ExpForLevelup) {
		CurrentExp -= ExpForLevelup;

		// this function only calls when rolling slot
		// holy power and stigma XP will be updated on the slot return
		Character->Data.MythMasteryInfo.Info.StigmaGrade++;
		Character->Data.MythMasteryInfo.Info.HolyPower += MythStigmaInfo->AwardedHolyPoint;

		// prepare next loop
		MythStigmaInfo = RTRuntimeDataStigmaInfoGet(Runtime->Context, Character->Data.MythMasteryInfo.Info.StigmaGrade);
		ExpForLevelup = MythStigmaInfo->RequiredExp;
		RTCharacterMythMasteryAssertHolyPoints(Runtime, Character);
	}

	Character->Data.MythMasteryInfo.Info.StigmaExp = CurrentExp;
	Character->SyncMask.MythMasteryInfo = true;
}

Bool RTCharacterMythMasteryGetPrerequisiteMetForSlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 SlotIndex1,
	Int32 SlotIndex2
) {
	// if 0 then prereq doesn't exist and is an initial open slot
	// check index first, faster
	Bool Slot1Full = SlotIndex1 == 0 || RTCharacterMythMasteryGetSlotOccupied(Runtime, Character, MasteryIndex, SlotIndex1);
	Bool Slot2Full = SlotIndex2 == 0 || RTCharacterMythMasteryGetSlotOccupied(Runtime, Character, MasteryIndex, SlotIndex2);

	return Slot1Full && Slot2Full;
}

Void RTCharacterMythDebugResetSlots(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (Character->Data.MythMasteryInfo.Info.MasterySlotCount == 0) return;

	RTMythMasterySlotRef FirstSlot = &Character->Data.MythMasteryInfo.Slots[0];
	memset(FirstSlot, 0, sizeof(struct _RTMythMasterySlot) * RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT);

	Character->Data.MythMasteryInfo.Info.MasterySlotCount = 0;
	Character->SyncMask.MythMasteryInfo = true;
	RTCharacterInitializeAttributes(Runtime, Character);
}

Bool RTCharacterMythMasteryRollback(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	RTMythMasterySlotRef Slot
) {
	return true;
	//TODO: Check item is in inventory and consume
	/*
	RTDataMythMasterySlotGroupRef SlotPage = RTRuntimeDataMythMasterySlotGroupGet(Runtime->Context, Slot->MasteryIndex, Slot->SlotIndex);

	if (!SlotPage) {
		return false;
	}

	UInt64 RestoreItemID = RTCharacterMythMasteryGetRestoreItemID(Runtime);
	Int32 RestoreItemAmount = SlotPage->RestoreCost;
	Int64 FoundItemAmount = 0;
	Int64 ItemCounts[] = { 0 };

	for (Int32 Index = 0; Index < InventoryIndexCount; Index++) {
		Int64 ItemCount = RTInventoryGetConsumableItemCount(Runtime, &Character->Data.InventoryInfo, RestoreItemID, 0, InventoryIndexs[Index]);
		FoundItemAmount += ItemCount;
		ItemCounts[Index] = ItemCount;
	}

	if (FoundItemAmount < SlotPage->RestoreCost) return false;

	for (Int32 Index = 0; Index < InventoryIndexCount; Index++) {
		RTInventoryConsumeStackableItems(Runtime, &Character->Data.InventoryInfo, RestoreItemID, ItemCounts[Index], InventoryIndexCount, InventoryIndexs[Index]);
	}

	return true;
	*/
}

UInt32 RTCharacterMythMasteryGetRestoreItemID(
	RTRuntimeRef Runtime
) {
	RTDataMythRestoreItemRef MythRestoreItemRef = RTRuntimeDataMythRestoreItemGet(Runtime->Context);
	if (!MythRestoreItemRef) {
		return 0;
	}

	return MythRestoreItemRef->ItemID;
}
