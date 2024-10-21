#include "BattleMode.h"
#include "Character.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "Runtime.h"
#include "Skill.h"

Bool RTCharacterIsBattleModeActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	return Character->Data.StyleInfo.ExtendedStyle.BattleModeFlags > 0;
}

Bool RTCharacterStartBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SkillIndex
) {
	if (RTCharacterIsBattleModeActive(Runtime, Character)) return false;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySkillIndex(Runtime, Character, SkillIndex);
	if (!SkillSlot) return false;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillIndex);
	if (!SkillData) return false;
	if (SkillData->SkillGroup != RUNTIME_SKILL_GROUP_BATTLE_MODE) return false;



	return true;
}

Void RTCharacterUpdateBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	Timestamp Timestamp = PlatformGetTickCount();
	if (Character->Data.StyleInfo.ExtendedStyle.BattleModeFlags && Character->BattleModeTimeout <= Timestamp) {
		RTCharacterCancelBattleMode(Runtime, Character);
	}
}

Bool RTCharacterCancelBattleMode(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	if (!RTCharacterIsBattleModeActive(Runtime, Character)) return false;

	Character->Data.StyleInfo.ExtendedStyle.BattleModeFlags = 0;
	Character->SyncMask.StyleInfo = true;

	NOTIFICATION_DATA_SKILL_TO_CHARACTER* Notification = RTNotificationInit(SKILL_TO_CHARACTER);
	Notification->SkillIndex = Character->BattleModeSkillIndex;

	NOTIFICATION_DATA_SKILL_TO_CHARACTER_BATTLE_MODE* NotificationData = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_SKILL_TO_CHARACTER_BATTLE_MODE);
	NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
	NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
	NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
	NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
	NotificationData->IsActivation = false;
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);

	Character->BattleModeSkillIndex = 0;
	Character->BattleModeTimeout = INT64_MAX;

	{
		NOTIFICATION_DATA_CHARACTER_DATA* Notification = RTNotificationInit(CHARACTER_DATA);
		Notification->Type = NOTIFICATION_CHARACTER_DATA_TYPE_SP_DECREASE_EX;
		Notification->SP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
		RTNotificationDispatchToCharacter(Notification, Character);
	}

	return true;
}
