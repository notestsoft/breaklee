#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

Void StartDisconnectTimer(
    ServerRef Server,
    SocketRef Socket,
    ClientContextRef Client,
    SocketConnectionRef Connection,
    UInt32 Timeout,
    UInt32 SystemMessage
);

ClientContextRef ServerGetClientByAuthKey(
    ServerContextRef Context,
    UInt32 AuthKey,
    UInt16 EntityID
);

Void ServerLoadMigrationData(
    ServerConfig Config,
    ServerContextRef Context
);

EXTERN_C_END
