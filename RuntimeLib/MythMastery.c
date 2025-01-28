#include "Character.h"
#include "MythMastery.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "NotificationProtocolDefinition.h"

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

		// add points from passing current level
		Character->Data.MythMasteryInfo.Info.Level = NextLevel->MythLevel;
		// update the level first before calling this function
		Character->Data.MythMasteryInfo.Info.Points += CurrentLevel->MythPoints + RTCharacterMythMasteryGetRepeatBonus(Runtime, Character);

		// add onto required exp since we just leveled up
		RequiredCumulativeExp += NextLevel->RequiredExp * RebirthPenaltyExpMultiplier;

		// notify level up (actual MLV number change on client)
		NOTIFICATION_DATA_CHARACTER_DATA* LevelUpNotification = RTNotificationInit(CHARACTER_DATA);
		LevelUpNotification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_MYTH_LEVEL;
		LevelUpNotification->Level = NextLevel->MythLevel;
		RTNotificationDispatchToNearby(LevelUpNotification, Character->Movement.WorldChunk);

		// notify myth points
		NOTIFICATION_DATA_MYTH_POINTS* NotificationPersonal = RTNotificationInit(MYTH_POINTS);
		NotificationPersonal->MythPoints = Character->Data.MythMasteryInfo.Info.Points;
		RTNotificationDispatchToCharacter(NotificationPersonal, Character);
	}

	// write to db
	Character->SyncMask.MythMasteryInfo = true;

	// notify XP gained
	// FIXME TODO: Does not appear in game log: Gained MXP (bottom right message log)
	NOTIFICATION_DATA_MYTH_GAIN_XP* Notification = RTNotificationInit(MYTH_GAIN_XP);
	Notification->MythXP = Character->Data.MythMasteryInfo.Info.Exp;
	RTNotificationDispatchToCharacter(Notification, Character);
}

Float RTCharacterMythMasteryGetExpPenaltyMultiplier(
	RTRuntimeRef Runtime,
	Int32 RebirthCount
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
		RebirthCount = MIN(MythRepeatPenaltyRef->Limit, RebirthCount);
	}

	// 10 AddMxp = 1.0% increase
	// adjust for amount of rebirths
	PenaltyPercentPerRebirth = MythRepeatPenaltyRef->AddMxp * RebirthCount;
	PenaltyPercentPerRebirth = PenaltyPercentPerRebirth / 1000.f;

	// min of 1
	return MAX(1.f + PenaltyPercentPerRebirth, 1.f);
}

// assumes the characters level info is already updated
Int32 RTCharacterMythMasteryGetRepeatBonus(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	RTDataMythRepeatRef MythRepeatBonusRef = RTRuntimeDataMythRepeatGet(Runtime->Context);
	Bool IsEligibleForBonus = RTCharacterMythMasteryCheckEligibleForRepeatBonus(Runtime, Character->Data.MythMasteryInfo.Info.Level);

	if (!IsEligibleForBonus || !MythRepeatBonusRef) {
		return 0;
	}

	return MythRepeatBonusRef->BonusPoints;
}

Bool RTCharacterMythMasteryCheckEligibleForRepeatBonus(
	RTRuntimeRef Runtime,
	Int32 MythLevel
) {
	RTDataMythRepeatRef MythRepeatBonusRef = RTRuntimeDataMythRepeatGet(Runtime->Context);
	Int32 Remainder = 0;

	if (!MythRepeatBonusRef || MythRepeatBonusRef->RepeatCondition <= 0) {
		Error("RTCharacterMythMasteryCheckEligibleForRepeatBonus function ref was null or illegal! Failing gracefully.");
		return false;
	}

	// Repeat condition could be something like 5, so give the bonus every 5 levels
	Remainder = MythLevel % MythRepeatBonusRef->RepeatCondition;

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

Bool RTCharacterMythMasteryGetCanOpenPage(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex,
	Int32 LockGroup
) {
	RTDataMythLockPageRef MythLockPageRef = RTRuntimeDataMythLockPageGet(Runtime->Context, MasteryIndex);
	RTDataMythLockInfoRef MythLockInfoRef = RTRuntimeDataMythLockInfoGet(MythLockPageRef, LockGroup);

	if (MythLockInfoRef->OpenScore <= Character->Data.MythMasteryInfo.Info.HolyPower) {
		Warn("true");
		return true;
	}

	Warn("false");
	return false;
}