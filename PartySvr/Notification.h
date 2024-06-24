#pragma once

#include "Base.h"
#include "Server.h"

EXTERN_C_BEGIN

Void BroadcastToParty(
    ServerContextRef Context,
    RTEntityID PartyID,
    Void *Notification
);

Void BroadcastPartyInfo(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    RTPartyRef Party
);

Void BroadcastCreateParty(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    RTPartyRef Party
);

Void BroadcastDestroyParty(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    RTPartyRef Party
);

Void BroadcastPartyData(
    ServerRef Server,
    ServerContextRef Context,
    IPCSocketRef Socket,
    RTPartyRef Party
);

EXTERN_C_END
