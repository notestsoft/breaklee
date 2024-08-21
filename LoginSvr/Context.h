#pragma once

#include "Base.h"
#include "Config.h"
#include "Constants.h"

EXTERN_C_BEGIN

enum {
    CLIENT_FLAGS_CONNECTED              = 1 << 0,
    CLIENT_FLAGS_VERSION_CHECKED        = 1 << 1,
    CLIENT_FLAGS_USERNAME_CHECKED       = 1 << 2,
    CLIENT_FLAGS_CAPTCHA_VERIFIED       = 1 << 3,
    CLIENT_FLAGS_PUBLICKEY_INITIALIZED  = 1 << 4,
    CLIENT_FLAGS_AUTHORIZED             = 1 << 5,
    CLIENT_FLAGS_AUTHENTICATED          = 1 << 6,
    CLIENT_FLAGS_VERIFIED               = 1 << 7,
    CLIENT_FLAGS_CHECK_DISCONNECT_TIMER = 1 << 8,
};

struct _CaptchaInfo {
    Char Name[MAX_PATH];
    Int32 DataLength;
    Void* Data;
};
typedef struct _CaptchaInfo* CaptchaInfoRef;

struct _WorldServerInfo {
    IPCNodeID NodeID;
    UInt16 PlayerCount;
    UInt16 MaxPlayerCount;
    Char WorldHost[65];
    UInt16 WorldPort;
    UInt32 WorldType;
};
typedef struct _WorldServerInfo* WorldServerInfoRef;

struct _ServerContext {
    AllocatorRef Allocator;
    ServerConfig Config;
    SocketRef ClientSocket;
    DatabaseRef Database;
    Timestamp WorldListBroadcastTimestamp;
    DictionaryRef WorldServerTable;
    ArrayRef CaptchaInfoList;
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    SocketConnectionRef Connection;
    UInt32 ClientVersion;
    UInt32 Flags;
    UInt32 AuthKey;
    Timestamp DisconnectTimestamp;
    RSA* RSA;
    UInt8 RSAPayloadBuffer[CLIENT_RSA_PAYLOAD_LENGTH];
    Int32 AccountID;
    Int32 LoginStatus;
    Int32 AccountStatus; 
    Char SessionKey[MAX_SESSIONKEY_LENGTH];
    Char Username[MAX_USERNAME_LENGTH];
    CaptchaInfoRef Captcha;
};
typedef struct _ClientContext* ClientContextRef;

EXTERN_C_END
