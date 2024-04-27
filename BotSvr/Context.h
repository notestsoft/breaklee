#pragma once

#include "Base.h"
#include "Constants.h"

#include <openssl/rsa.h>

EXTERN_C_BEGIN

struct _ServerContext {
    SocketRef LoginSocket;
    SocketRef WorldSocket;
    Bool IsLoggingIn;
    Char Username[MAX_PATH];
    Char Password[MAX_PATH];
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    RSA* RSA;
    UInt8 RSAPayloadBuffer[CLIENT_RSA_PAYLOAD_LENGTH];
};
typedef struct _ClientContext* ClientContextRef;

EXTERN_C_END
