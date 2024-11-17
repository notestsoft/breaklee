#include "AstralWeapon.h"
#include "Character.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "Runtime.h"

Bool RTCharacterIsAstralVehicleActive(
	RTRuntimeRef Runtime,
	RTCharacterRef Character
) {
	return Character->Data.StyleInfo.ExtendedStyle.IsVehicleActive;
}

Void RTCharacterToggleAstralVehicle(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Bool IsActivation,
	Int32 SkillIndex
) {
	RTItemSlotRef ItemSlot = RTEquipmentGetSlot(Runtime, &Character->Data.EquipmentInfo, RUNTIME_EQUIPMENT_SLOT_INDEX_VEHICLE);
	if (!ItemSlot) {
		IsActivation = false;
	}

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySkillIndex(Runtime, Character, SkillIndex);
	RTCharacterSkillDataRef SkillData = (SkillSlot) ? RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID) : NULL;
	if (!SkillData || SkillData->SkillGroup != RUNTIME_SKILL_GROUP_ASTRAL || SkillData->Intensity != RUNTIME_SKILL_INTENSITY_ASTRAL_VEHICLE) {
		IsActivation = false;
	}

	Character->Data.BattleModeInfo.VehicleState = (IsActivation) ? 1 : -1;
	Character->SyncMask.BattleModeInfo = true;

	Character->Data.StyleInfo.ExtendedStyle.IsVehicleActive = IsActivation;
	Character->SyncMask.StyleInfo = true;

	RTCharacterInitializeAttributes(Runtime, Character);

	NOTIFICATION_DATA_SKILL_TO_CHARACTER* Notification = RTNotificationInit(SKILL_TO_CHARACTER);
	Notification->SkillIndex = SkillIndex;

	NOTIFICATION_DATA_SKILL_TO_CHARACTER_ASTRAL_MODE* NotificationData = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_SKILL_TO_CHARACTER_ASTRAL_MODE);
	NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
	NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
	NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
	NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
	NotificationData->IsActivation = Character->Data.StyleInfo.ExtendedStyle.IsVehicleActive;
	RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
}
