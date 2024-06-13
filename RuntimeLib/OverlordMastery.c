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
		RTDataOverlordMasteryExpRef Mastery = RTRuntimeDataOverlordMasteryExpGet(Runtime->Context, Character->Data.Info.Overlord.Level);
		assert(Mastery);

		RTDataOverlordMasteryExpRef NextMastery = RTRuntimeDataOverlordMasteryExpGet(Runtime->Context, Character->Data.Info.Overlord.Level + 1);
		if (!NextMastery) break;

		UInt64 RequiredExp = NextMastery->AccumulatedExp - Character->Data.Info.Overlord.Exp;
		Character->Data.Info.Overlord.Exp += MIN(Exp, RequiredExp);
		Exp -= MIN(Exp, RequiredExp);

		if (Character->Data.Info.Overlord.Exp >= Mastery->AccumulatedExp) {
			Character->Data.Info.Overlord.Point += Mastery->MasteryPointCount;
			Character->Data.Info.Overlord.Level = NextMastery->Level;
			Character->SyncMask.Info = true;

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
	for (Int32 Index = 0; Index < Character->Data.OverlordMasteryInfo.Count; Index += 1) {
		if (Character->Data.OverlordMasteryInfo.Slots[Index].MasteryIndex == MasteryIndex) {
			return &Character->Data.OverlordMasteryInfo.Slots[Index];
		}
	}

	return NULL;
}
