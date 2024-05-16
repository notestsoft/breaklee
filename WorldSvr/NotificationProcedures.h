#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

#ifndef NOTIFICATION_PROCEDURE_BINDING
#define NOTIFICATION_PROCEDURE_BINDING(__NAME__)  \
Void NOTIFICATION_PROC_ ## __NAME__(              \
    ServerRef Server,                             \
    ServerContextRef Context,                     \
    SocketRef Socket,                             \
    SocketConnectionRef Connection,               \
    ClientContextRef Client,                      \
    RTRuntimeRef Runtime,                         \
    RTCharacterRef Character,                     \
    NOTIFICATION_DATA_ ## __NAME__* Notification  \
)
#endif

#define NOTIFICATION_PROTOCOL(__NAME__, __COMMAND__, __BODY__) \
NOTIFICATION_PROCEDURE_BINDING(__NAME__);
#include "RuntimeLib/NotificationProtocolDefinition.h"

EXTERN_C_END
