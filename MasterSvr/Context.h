#pragma once

#include "Base.h"
#include "Config.h"
#include "IPCProtocol.h"

EXTERN_C_BEGIN

struct _AuthSocketClientContext {
    SocketConnectionRef Connection;
};

struct _WorldSocketClientContext {
    SocketConnectionRef Connection;
    Bool IsWorldInitialized;
    UInt8 WorldID;
    Char WorldHost[65];
    UInt16 WorldPort;
    UInt32 WorldType;
    UInt16 PlayerCount;
    UInt16 MaxPlayerCount;
};

union _ClientContext {
    struct _AuthSocketClientContext Auth;
    struct _WorldSocketClientContext World;
};
typedef union _ClientContext* ClientContextRef;

EXTERN_C_END