#pragma once

#include "Base.h"
#include "Context.h"

EXTERN_C_BEGIN

struct _ServerContext {
    ServerConfig Config;
    SocketRef AuthSocket;
    SocketRef WorldSocket;
    DatabaseRef Database;
    Timestamp WorldListBroadcastTimestamp;
    Timestamp WorldListUpdateTimestamp;
};
typedef struct _ServerContext* ServerContextRef;

ClientContextRef ServerGetWorldClient(
    ServerRef Server,
    ServerContextRef Context,
    Int32 WorldID
);

Void ServerBroadcastWorldList(
    ServerRef Server,
    ServerContextRef Context
);

EXTERN_C_END
