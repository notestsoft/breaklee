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

struct _RuntimeDataCharacterTemplate {
    Int32 BattleStyleIndex;
    struct _RTCharacterInventoryInfo Inventory;
    struct _RTCharacterSkillSlotInfo SkillSlots;
    struct _RTCharacterQuickSlotInfo QuickSlots;
};

struct _RuntimeData {
    struct _RuntimeDataCharacterTemplate CharacterTemplate[RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX];
};
typedef struct _RuntimeData* RuntimeDataRef;

struct _ServerContext {
    ServerRef Server;
    SocketRef ClientSocket;
    IPCSocketRef IPCSocket;
    ServerConfig Config;
    RuntimeDataRef RuntimeData;
    RTRuntimeRef Runtime;
    Timestamp UserListBroadcastTimestamp;
};
typedef struct _ServerContext* ServerContextRef;

struct _ClientContext {
    SocketConnectionRef Connection;
    UInt32 Flags;
    Timestamp PasswordVerificationTimestamp;
    UInt32 AuthKey;
    UInt8 SubpasswordFailureCount;
    IPC_DATA_ACCOUNT Account;
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
    Char CharacterName[MAX_CHARACTER_NAME_LENGTH + 1];
};
typedef struct _ClientContext* ClientContextRef;

EXTERN_C_END
