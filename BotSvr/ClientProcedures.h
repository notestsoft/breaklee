#pragma once

#include "Base.h"
#include "Context.h"
#include "ClientProtocol.h"

EXTERN_C_BEGIN

#ifndef SERVER_PROCEDURE_BINDING
#define SERVER_PROCEDURE_BINDING(__NAME__)  \
Void PROC_ ## __NAME__(                     \
    SocketRef Socket,                       \
    ServerContextRef Context,               \
    SocketConnectionRef Connection,         \
    S2C_DATA_ ## __NAME__* Packet           \
)
#endif

#define S2C_COMMAND(__NAME__, __COMMAND__) \
SERVER_PROCEDURE_BINDING(__NAME__ ## _LOGIN);
#include "LoginCommands.h"

#define S2C_COMMAND(__NAME__, __COMMAND__) \
SERVER_PROCEDURE_BINDING(__NAME__ ## _WORLD);
#include "WorldCommands.h"

EXTERN_C_END
