#pragma once

#include "Base.h"
#include "Server.h"

EXTERN_C_BEGIN

Void BroadcastToParty(
    ServerContextRef Context,
    RTEntityID PartyID,
    Void *Notification
);

EXTERN_C_END
