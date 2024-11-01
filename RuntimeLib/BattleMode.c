#include "BattleMode.h"
#include "Character.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "Runtime.h"
#include "Skill.h"

RTCharacterSkillDataRef RTCharacterGetBattleModeSkillData(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 BattleModeIndex
) {
	Int32 SkillIndex = RTCharacterGetBattleModeSkillIndex(Runtime, Character, BattleModeIndex);
	return RTRuntimeGetCharacterSkillDataByID(Runtime, SkillIndex);
}

RTCharacterSkillDataRef RTCharacterGetAuraModeSkillData(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 AuraModeIndex
) {
	Int32 SkillIndex = RTCharacterGetAuraModeSkillIndex(Runtime, Character, AuraModeIndex);
	return RTRuntimeGetCharacterSkillDataByID(Runtime, SkillIndex);
}

Bool RTCharacterIsBattleModeActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	return Character->Data.BattleModeInfo.Info.BattleModeIndex > 0;
}

Bool RTCharacterStartBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex
) {
	if (RTCharacterIsBattleModeActive(Runtime, Character)) return false;
	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] < RUNTIME_BATTLE_MODE_SP_CONSUMPTION) return false;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySkillIndex(Runtime, Character, SkillIndex);
	if (!SkillSlot) return false;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillIndex);
	if (!SkillData) return false;
	if (SkillData->SkillGroup != RUNTIME_SKILL_GROUP_BATTLE_MODE) return false;

	RTCharacterAddSP(Runtime, Character, -RUNTIME_BATTLE_MODE_SP_CONSUMPTION);

	Character->Data.BattleModeInfo.Info.BattleModeIndex = SkillData->Intensity;
	Character->Data.BattleModeInfo.Info.BattleModeStyleRank = Character->Data.StyleInfo.Style.BattleRank;
	Character->Data.BattleModeInfo.Info.BattleModeDuration += RUNTIME_BATTLE_MODE_SP_CONSUMPTION;
	Character->SyncMask.BattleModeInfo = true;

	Character->Data.StyleInfo.ExtendedStyle.BattleModeFlags |= (1 << (SkillData->Intensity - 1));
	Character->SyncMask.StyleInfo = true;

	NOTIFICATION_DATA_SKILL_TO_CHARACTER* Notification = RTNotificationInit(SKILL_TO_CHARACTER);
	Notification->SkillIndex = SkillIndex;

	NOTIFICATION_DATA_SKILL_TO_CHARACTER_BATTLE_MODE* NotificationData = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_SKILL_TO_CHARACTER_BATTLE_MODE);
	NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
	NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
	NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
	NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
	NotificationData->IsActivation = true;
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

	return true;
}

Void RTCharacterUpdateBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Timestamp CurrentTimestamp = GetTimestampMs();
	if (Character->RegenUpdateTimestamp <= CurrentTimestamp) {
		Character->RegenUpdateTimestamp = CurrentTimestamp + RUNTIME_REGENERATION_INTERVAL;

		// TODO: Calculate spirit point regeneration
		// TODO: Calculate hp, mp regeneration

		Int32 SpConsumptionRate = 0;

		Bool IsBattleModeActive = RTCharacterIsBattleModeActive(Runtime, Character);
		if (IsBattleModeActive) {
			RTCharacterSkillDataRef SkillData = RTCharacterGetBattleModeSkillData(
				Runtime,
				Character,
				Character->Data.BattleModeInfo.Info.BattleModeIndex
			);
			if (SkillData) {
				SpConsumptionRate += SkillData->Sp;
			}

		}

		Bool IsAuraModeActive = RTCharacterIsAuraModeActive(Runtime, Character);
		if (IsAuraModeActive) {
			RTCharacterSkillDataRef SkillData = RTCharacterGetAuraModeSkillData(
				Runtime,
				Character,
				Character->Data.BattleModeInfo.Info.AuraModeIndex
			);
			if (SkillData) {
				SpConsumptionRate += SkillData->Sp;
			}
		}

		if (SpConsumptionRate > 0) {
			Character->Data.BattleModeInfo.Info.BattleModeDuration = MAX(0,
				Character->Data.BattleModeInfo.Info.BattleModeDuration - SpConsumptionRate
			);
			
			if (IsAuraModeActive) {
				Character->Data.BattleModeInfo.Info.AuraModeDuration += SpConsumptionRate;
			}

			Character->SyncMask.BattleModeInfo = true;
		}

		Bool CancelBattleMode = (
			(IsBattleModeActive || IsAuraModeActive) &&
			Character->Data.BattleModeInfo.Info.BattleModeDuration <= 0
		);
		if (CancelBattleMode) {
			RTCharacterCancelBattleMode(Runtime, Character);
			RTCharacterCancelAuraMode(Runtime, Character);

			NOTIFICATION_DATA_CHARACTER_DATA* Notification = RTNotificationInit(CHARACTER_DATA);
			Notification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_SP_DECREASE_EX;
			Notification->SP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
			RTNotificationDispatchToCharacter(Notification, Character);
		}
	}
}

Bool RTCharacterCancelBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (!RTCharacterIsBattleModeActive(Runtime, Character)) return false;

	Int32 BattleModeIndex = Character->Data.BattleModeInfo.Info.BattleModeIndex;

	Character->Data.BattleModeInfo.Info.BattleModeIndex = 0;
	Character->Data.BattleModeInfo.Info.BattleModeStyleRank = 0;
	Character->Data.BattleModeInfo.Info.BattleModeDuration = 0;
	Character->SyncMask.BattleModeInfo = true;

	Character->Data.StyleInfo.ExtendedStyle.BattleModeFlags = 0;
	Character->SyncMask.StyleInfo = true;

	NOTIFICATION_DATA_SKILL_TO_CHARACTER* Notification = RTNotificationInit(SKILL_TO_CHARACTER);
	Notification->SkillIndex = RTCharacterGetBattleModeSkillIndex(Runtime, Character, BattleModeIndex);

	NOTIFICATION_DATA_SKILL_TO_CHARACTER_BATTLE_MODE* NotificationData = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_SKILL_TO_CHARACTER_BATTLE_MODE);
	NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
	NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
	NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
	NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
	NotificationData->IsActivation = false;
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

	return true;
}

Bool RTCharacterIsAuraModeActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	return Character->Data.BattleModeInfo.Info.AuraModeIndex > 0;
}

Bool RTCharacterStartAuraMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex
) {
	if (RTCharacterIsAuraModeActive(Runtime, Character)) return false;
	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] < RUNTIME_BATTLE_MODE_SP_CONSUMPTION) return false;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySkillIndex(Runtime, Character, SkillIndex);
	if (!SkillSlot) return false;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillIndex);
	if (!SkillData) return false;
	if (SkillData->SkillGroup != RUNTIME_SKILL_GROUP_ASTRAL) return false;
	if (SkillData->Intensity != RUNTIME_SKILL_INTENSITY_ASTRAL_AURA) return false;

	RTCharacterAddSP(Runtime, Character, -RUNTIME_BATTLE_MODE_SP_CONSUMPTION);

	Character->Data.BattleModeInfo.Info.BattleModeDuration += RUNTIME_BATTLE_MODE_SP_CONSUMPTION;
	Character->Data.BattleModeInfo.Info.AuraModeIndex = RTCharacterGetAuraModeIndexForSkillIndex(Runtime, Character, SkillIndex);
	Character->Data.BattleModeInfo.Info.AuraModeStyleRank = Character->Data.StyleInfo.Style.BattleRank;
	Character->SyncMask.BattleModeInfo = true;

	Character->Data.StyleInfo.ExtendedStyle.IsAuraActive = true;
	Character->SyncMask.StyleInfo = true;

	NOTIFICATION_DATA_SKILL_TO_CHARACTER* Notification = RTNotificationInit(SKILL_TO_CHARACTER);
	Notification->SkillIndex = SkillIndex;

	NOTIFICATION_DATA_SKILL_TO_CHARACTER_ASTRAL_MODE* NotificationData = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_SKILL_TO_CHARACTER_ASTRAL_MODE);
	NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
	NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
	NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
	NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
	NotificationData->IsActivation = true;
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

	return true;
}

Bool RTCharacterCancelAuraMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (!RTCharacterIsAuraModeActive(Runtime, Character)) return false;

	Int32 AuraModeIndex = Character->Data.BattleModeInfo.Info.AuraModeIndex;

	Character->Data.BattleModeInfo.Info.BattleModeDuration = 0;
	Character->Data.BattleModeInfo.Info.AuraModeIndex = 0;
	Character->Data.BattleModeInfo.Info.AuraModeStyleRank = 0;
	Character->Data.BattleModeInfo.Info.AuraModeDuration = 0;
	Character->SyncMask.BattleModeInfo = true;

	Character->Data.StyleInfo.ExtendedStyle.IsAuraActive = 0;
	Character->SyncMask.StyleInfo = true;
	
	NOTIFICATION_DATA_SKILL_TO_CHARACTER* Notification = RTNotificationInit(SKILL_TO_CHARACTER);
	Notification->SkillIndex = RTCharacterGetAuraModeSkillIndex(Runtime, Character, AuraModeIndex);

	NOTIFICATION_DATA_SKILL_TO_CHARACTER_ASTRAL_MODE* NotificationData = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_SKILL_TO_CHARACTER_ASTRAL_MODE);
	NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
	NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
	NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
	NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
	NotificationData->IsActivation = false;
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

	return true;
}