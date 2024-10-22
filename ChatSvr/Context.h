#pragma once

#include "Base.h"
#include "Config.h"
#include "Constants.h"
#include "IPCProtocol.h"
#include "MasterDBProtocol.h"

EXTERN_C_BEGIN

enum {
    CLIENT_FLAGS_CONNECTED                      = 1 << 0,
    CLIENT_FLAGS_VERIFIED                       = 1 << 1,
    CLIENT_FLAGS_CHARACTER_INDEX_LOADED         = 1 << 2,
    CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION  = 1 << 5,
};

struct _ServerContext {
    ServerRef Server;
    SocketRef ClientSocket;
    IPCSocketRef IPCSocket;
    ServerConfig Config;
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    SocketConnectionRef Connection;
    UInt32 Flags;
    UInt32 AuthKey;	
    UInt32 CharacterIndex;
    Int32 WorldServerIndex;
};
typedef struct _ClientContext* ClientContextRef;

EXTERN_C_END
