#include "Character.h"
#include "OverlordMastery.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationProtocolDefinition.h"

Void RTCharacterAddOverlordExp(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	UInt64 Exp
) {
	while (Exp > 0) {
		RTDataOverlordMasteryExpRef Mastery = RTRuntimeDataOverlordMasteryExpGet(Runtime->Context, Character->Data.OverlordMasteryInfo.Info.Level);
		assert(Mastery);

		RTDataOverlordMasteryExpRef NextMastery = RTRuntimeDataOverlordMasteryExpGet(Runtime->Context, Character->Data.OverlordMasteryInfo.Info.Level + 1);
		if (!NextMastery) break;

		UInt64 RequiredExp = NextMastery->AccumulatedExp - Character->Data.OverlordMasteryInfo.Info.Exp;
		Character->Data.OverlordMasteryInfo.Info.Exp += MIN(Exp, RequiredExp);
		Exp -= MIN(Exp, RequiredExp);

		if (Character->Data.OverlordMasteryInfo.Info.Exp >= Mastery->AccumulatedExp) {
			Character->Data.OverlordMasteryInfo.Info.Point += Mastery->MasteryPointCount;
			Character->Data.OverlordMasteryInfo.Info.Level = NextMastery->Level;
			Character->SyncMask.OverlordMasteryInfo = true;

			RTCharacterInitializeAttributes(Runtime, Character);
            RTRuntimeBroadcastCharacterData(
                Runtime,
                Character,
                NOTIFICATION_CHARACTER_DATA_TYPE_OVERLORD_LEVEL
            );
            
            {
                NOTIFICATION_DATA_CHARACTER_EVENT* Notification = RTNotificationInit(CHARACTER_EVENT);
                Notification->Type = NOTIFICATION_CHARACTER_EVENT_TYPE_OVERLORD_LEVEL_UP;
                Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
                RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
            }
		}
		else {
			break;
		}
	}
}

RTOverlordMasterySlotRef RTCharacterGetOverlordMasterySlot(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 MasteryIndex
) {
	for (Int Index = 0; Index < Character->Data.OverlordMasteryInfo.Info.SlotCount; Index += 1) {
		if (Character->Data.OverlordMasteryInfo.Slots[Index].MasteryIndex == MasteryIndex) {
			return &Character->Data.OverlordMasteryInfo.Slots[Index];
		}
	}

	return NULL;
}
