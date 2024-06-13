#pragma once

#include "Base.h"
#include "Config.h"
#include "MasterDBProtocol.h"

EXTERN_C_BEGIN

struct _ServerContext {
    ServerConfig Config;
    DatabaseRef Database;
};
typedef struct _ServerContext* ServerContextRef;

EXTERN_C_END