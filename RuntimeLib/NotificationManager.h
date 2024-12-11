#pragma once

#include "Base.h"

#define RUNTIME_MAX_NOTIFICATION_BUFFER_LENGTH 0x4000

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTNotification {
    UInt16 Magic;
    UInt16 Length;
    UInt16 Command;
};

#pragma pack(pop)

typedef Void (*RTNotificationCallback)(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTNotificationRef Notification,
    Void* UserData
);

RTNotificationManagerRef RTNotificationManagerCreate(
    RTRuntimeRef Runtime
);

Void RTNotificationManagerDestroy(
    RTNotificationManagerRef NotificationManager
);

Void RTNotificationManagerRegisterCallback(
    RTNotificationManagerRef NotificationManager,
    Int32 Command,
    RTNotificationCallback Callback,
    Void* UserData
);

Void RTNotificationManagerDispatchToCharacter(
    RTNotificationManagerRef NotificationManager,
    Void* Notification,
    RTCharacterRef Character
);

Void RTNotificationManagerDispatchToParty(
    RTNotificationManagerRef NotificationManager,
    Void* Notification,
    RTPartyRef Party
);

Void RTNotificationManagerDispatchToChunk(
    RTNotificationManagerRef NotificationManager,
    Void* Notification,
    RTWorldChunkRef WorldChunk
);

Void RTNotificationManagerDispatchToNearby(
    RTNotificationManagerRef NotificationManager,
    Void* Notification,
    RTWorldChunkRef WorldChunk
);

RTNotificationRef _RTNotificationInit(
    Int32 Length,
    Int32 Command
);

#define RTNotificationInit(__NAME__) \
((NOTIFICATION_DATA_ ## __NAME__*)_RTNotificationInit(sizeof(NOTIFICATION_DATA_ ## __NAME__), NOTIFICATION_ ## __NAME__))

#define RTNotificationDispatchToCharacter(Notification, Character) \
RTNotificationManagerDispatchToCharacter(Runtime->NotificationManager, Notification, Character)

#define RTNotificationDispatchToParty(Notification, Party) \
RTNotificationManagerDispatchToParty(Runtime->NotificationManager, Notification, Party)

#define RTNotificationDispatchToChunk(Notification, WorldChunk) \
RTNotificationManagerDispatchToChunk(Runtime->NotificationManager, Notification, WorldChunk)

#define RTNotificationDispatchToNearby(Notification, WorldChunk) \
RTNotificationManagerDispatchToNearby(Runtime->NotificationManager, Notification, WorldChunk)

Void* RTNotificationAppend(
    Void* Notification,
    Int Length
);

Void* RTNotificationAppendCopy(
    Void* Notification,
    Void* Source,
    Int Length
);

CString RTNotificationAppendCString(
    Void* Notification,
    CString Value
);

#define RTNotificationAppendValue(Notification, __TYPE__, __VALUE__) \
*((__TYPE__*)RTNotificationAppend(Notification, sizeof(__TYPE__))) = __VALUE__

#define RTNotificationAppendStruct(Notification, __TYPE__) \
(__TYPE__*)RTNotificationAppend(Notification, sizeof(__TYPE__))

EXTERN_C_END
