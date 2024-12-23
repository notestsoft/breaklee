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

Void ServerClearState(
    ServerContextRef Context
);

Void StartAuthTimer(
    ServerRef Server,
    ServerContextRef Context,
    SocketRef Socket,
    SocketConnectionRef Connection,
    ClientContextRef Client,
    UInt32 Timeout
);

Void SendLoginSuccess(
    ServerRef Server,
    ServerContextRef Context,
    SocketRef Socket,
    SocketConnectionRef Connection,
    ClientContextRef Client
);

EXTERN_C_END
