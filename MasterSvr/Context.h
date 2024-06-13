#pragma once

#include "Base.h"
#include "Config.h"
#include "MasterDBProtocol.h"

EXTERN_C_BEGIN

struct _WorldInfo {
    IPCNodeID NodeID;
    UInt16 PlayerCount;
    UInt16 MaxPlayerCount;
    Char Host[64];
    UInt16 Port;
    UInt32 Type;
};
typedef struct _WorldInfo* WorldInfoRef;

struct _ServerContext {
    ServerConfig Config;
    DictionaryRef WorldInfoTable;
};
typedef struct _ServerContext* ServerContextRef;

EXTERN_C_END