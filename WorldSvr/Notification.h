#pragma once

#include "Base.h"
#include "Server.h"

EXTERN_C_BEGIN

Void BroadcastMessage(
    ServerContextRef Context,
    CString Message
);

Void BroadcastUserList(
    ServerContextRef Context
);

Void BroadcastToParty(
    ServerContextRef Context,
    RTEntityID PartyID,
    Void *Notification
);

Void BroadcastToWorld(
    ServerContextRef Context,
    RTWorldContextRef World,
    RTEntityID Entity,
    Int32 X,
    Int32 Y,
    Void *Packet
);

EXTERN_C_END
