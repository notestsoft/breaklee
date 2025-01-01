#include "Character.h"
#include "DailyReset.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "Runtime.h"

Void RTCharacterInitializeDailyReset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    Timestamp NextResetTimestamp = RTRuntimeGetNextDailyResetTimeMs(Runtime);
    if (Character->Data.DailyResetInfo.NextResetTimestamp > NextResetTimestamp) {
        Character->Data.DailyResetInfo.NextResetTimestamp = NextResetTimestamp;
        Character->SyncMask.DailyResetInfo = true;
    }

    RTCharacterUpdateDailyReset(Runtime, Character);
}

Void RTCharacterUpdateDailyReset(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    Timestamp CurrentTimestamp = GetTimestampMs();
    if (Character->Data.DailyResetInfo.NextResetTimestamp > CurrentTimestamp) return;

    Character->Data.DailyResetInfo.LastResetTimestamp = CurrentTimestamp;
    Character->Data.DailyResetInfo.NextResetTimestamp = RTRuntimeGetNextDailyResetTimeMs(Runtime);
    Character->SyncMask.DailyResetInfo = true;

    RTCharacterResetDailyQuests(Runtime, Character);
    RTCharacterGiftBoxDailyReset(Runtime, Character);

    // TODO: Notification is not working at runtime after login!
    NOTIFICATION_DATA_DAILY_QUEST_RESET* Notification = RTNotificationInit(DAILY_QUEST_RESET);
    RTNotificationDispatchToCharacter(Notification, Character);
}
