#pragma once

#include "Base.h"
#include "Server.h"

EXTERN_C_BEGIN

Void BroadcastWorldList(
    ServerRef Server,
    ServerContextRef Context
);

Void BroadcastWorldListToConnection(
    ServerRef Server,
    ServerContextRef Context,
    SocketConnectionRef Connection,
    ClientContextRef Client
);

EXTERN_C_END
