#pragma once

#include "Base.h"
#include "Config.h"
#include "Constants.h"

EXTERN_C_BEGIN

enum {
    CLIENT_FLAGS_CONNECTED                      = 1 << 0,
    CLIENT_FLAGS_VERIFIED                       = 1 << 1,
    CLIENT_FLAGS_CHARACTER_INDEX_LOADED         = 1 << 2,
    CLIENT_FLAGS_VERIFIED_SUBPASSWORD_DELETION  = 1 << 5,
};

struct _ServerContext {
    SocketRef ClientSocket;
    SocketRef WorldSocket;
    ServerConfig Config;
    RTPartyManagerRef PartyManager;
    DictionaryRef CharacterToWorldServer;
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    SocketConnectionRef Connection;
    UInt32 Flags;
    UInt32 CharacterIndex;
    Char CharacterName[MAX_CHARACTER_NAME_LENGTH + 1];
};
typedef struct _ClientContext* ClientContextRef;

EXTERN_C_END
