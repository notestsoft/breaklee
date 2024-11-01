#include "AstralWeapon.h"
#include "Character.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "Runtime.h"

Bool RTCharacterIsAstralWeaponActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	return Character->Data.StyleInfo.ExtendedStyle.IsAstralWeaponActive;
}

Void RTCharacterToggleAstralWeapon(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Bool IsActivation,
	Bool IsNotificationEnabled
) {
	Character->Data.StyleInfo.ExtendedStyle.IsAstralWeaponActive = IsActivation;
	Character->SyncMask.StyleInfo = true;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySlotIndex(Runtime, Character, RUNTIME_SPECIAL_SKILL_SLOT_ASTRAL_SKILL);
	RTCharacterSkillDataRef SkillData = (SkillSlot) ? RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID) : NULL;
	if (!SkillData || SkillData->SkillGroup != RUNTIME_SKILL_GROUP_ASTRAL || SkillData->Intensity != RUNTIME_SKILL_INTENSITY_ASTRAL_WEAPON) {
		return;
	}

	RTItemSlotRef ItemSlot = RTEquipmentGetSlot(Runtime, &Character->Data.EquipmentInfo, RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT);
	RTItemDataRef ItemData = (ItemSlot) ? RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID) : NULL;
	Int32 SkillLevel = (ItemData) ? ItemData->ItemGrade : 0;

	if (IsNotificationEnabled) {
		NOTIFICATION_DATA_SKILL_TO_CHARACTER* Notification = RTNotificationInit(SKILL_TO_CHARACTER);
		Notification->SkillIndex = SkillData->SkillID;

		NOTIFICATION_DATA_SKILL_TO_CHARACTER_ASTRAL_MODE* NotificationData = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_SKILL_TO_CHARACTER_ASTRAL_MODE);
		NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
		NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
		NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
		NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
		NotificationData->IsActivation = Character->Data.StyleInfo.ExtendedStyle.IsAstralWeaponActive;
		RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
	}
}
