#pragma once

#include "Base.h"
#include "Config.h"
#include "Constants.h"
#include "IPCProtocol.h"
#include "RuntimeProtocol.h"
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
    RTRuntimeRef Runtime;
    Timestamp UserListBroadcastTimestamp;
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    SocketConnectionRef Connection;
    UInt32 Flags;
    Timestamp PasswordVerificationTimestamp;
    UInt32 AuthKey;
    Bool IsSubpasswordSet;
    Int32 AccountID;
    struct _RTCharacterAccountInfo AccountInfo;
    IPC_DATA_CHARACTER_INFO Characters[MAX_CHARACTER_COUNT];
    // TODO: Store upgrade point & expiration time in temp memory
    Int32 UpgradePoint;
    Int32 DiceSeed;

    // @Next
    // TODO: Add a cleanup flag and cleanup the character from world when the connection closes 
    //       verify that it is not called multiple times together with the DEINITIALIZE packet!

    /* Runtime Data */
    Int32 CharacterDatabaseID;
    Index CharacterIndex;
};
typedef struct _ClientContext* ClientContextRef;

EXTERN_C_END
