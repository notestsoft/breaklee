#pragma once

#include <NetLib/NetLib.h>
#include <RuntimeLib/RuntimeLib.h>
#include "Constants.h"
#include "GameProtocol.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
	IPC_CONNECT = 1,
	IPC_ROUTE	= 2,

#define IPC_AUTH_COMMAND(__NAME__, __COMMAND__) \
	__NAME__ = __COMMAND__,

#define IPC_WORLD_COMMAND(__NAME__, __COMMAND__) \
	__NAME__ = __COMMAND__,
   
#include "IPCCommands.h"
};

enum {
	IPC_TYPE_NONE		= 0,
    IPC_TYPE_AUCTION    = 1,
    IPC_TYPE_AUTH       = 2,
    IPC_TYPE_CHAT       = 3,
    IPC_TYPE_MASTER     = 4,
    IPC_TYPE_PARTY      = 5,
    IPC_TYPE_WORLD      = 6,

    IPC_TYPE_COUNT,
};

struct _IPCRouteIndex {
	union {
		struct {
			UInt16 Index;
			UInt8 Group;
			UInt8 Type;
		};
		Index Serial;
	};
};
typedef struct _IPCRouteIndex IPCRouteIndex;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	IPCRouteIndex Source;
} IPC_DATA_CONNECT;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	IPCRouteIndex Source;
	IPCRouteIndex Target;
	Bool Broadcast;
	Index DataLength;
	UInt8 Data[0];
} IPC_DATA_ROUTE;

/* --- AUTH <> MASTER --- */

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 ServerID;
} IPC_DATA_AUTH_REQCONNECT;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Bool Success;
} IPC_DATA_AUTH_ACKCONNECT;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 WorldCount;
	UInt8 Data[0];
	/* IPC_DATA_AUTH_WORLD[WorldCount] */
} IPC_DATA_AUTH_NFYWORLDLIST;

typedef struct {
	UInt8 WorldID;
	UInt16 PlayerCount;
	UInt16 MaxPlayerCount;
	Char WorldHost[65];
	UInt16 WorldPort;
	UInt32 WorldType;
} IPC_DATA_AUTH_WORLD;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 ServerID;
	UInt8 WorldID;
	Int32 AccountID;
	UInt32 AuthKey;
	UInt16 EntityID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH];
} IPC_DATA_AUTH_VERIFYLINKS;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 ServerID;
	UInt8 WorldID;
	UInt8 Status;
} IPC_DATA_AUTH_VERIFYRESULT;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
} IPC_DATA_AUTH_REQVERIFYPASSWORD;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Bool Success;
} IPC_DATA_AUTH_ACKVERIFYPASSWORD;

/* --- WORLD <> MASTER --- */

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 WorldID;
	Char WorldHost[65];
	UInt16 WorldPort;
	UInt32 WorldType;
	UInt16 MaxPlayerCount;
} IPC_DATA_WORLD_REQCONNECT;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 ServerID;
	Bool Success;
} IPC_DATA_WORLD_ACKCONNECT;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt16 PlayerCount;
	UInt16 MaxPlayerCount;
} IPC_DATA_WORLD_NFYUSERLIST;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 ServerID;
	UInt8 WorldID;
	UInt32 AuthKey;
	UInt16 EntityID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
	GAME_DATA_ACCOUNT Account;
} IPC_DATA_WORLD_VERIFYLINKS;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 ServerID;
	UInt8 WorldID;
	Int64 AccountID;
	UInt8 Status;
} IPC_DATA_WORLD_VERIFYRESULT;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
} IPC_DATA_WORLD_REQGETCHARACTERS;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	GAME_DATA_CHARACTER_INDEX Characters[MAX_CHARACTER_COUNT];
} IPC_DATA_WORLD_ACKGETCHARACTERS;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
} IPC_DATA_WORLD_REQPREMIUMSERVICE;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Bool HasService;
	UInt32 ServiceType;
	Timestamp StartedAt;
	Timestamp ExpiredAt;
} IPC_DATA_WORLD_ACKPREMIUMSERVICE;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	UInt8 SlotIndex;
	UInt8 NameLength;
	Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
	struct _RTCharacterInfo CharacterData;
    struct _RTCharacterEquipmentInfo CharacterEquipment;
	struct _RTCharacterInventoryInfo CharacterInventory;
	struct _RTCharacterSkillSlotInfo CharacterSkillSlots;
	struct _RTCharacterQuickSlotInfo CharacterQuickSlots;
} IPC_DATA_WORLD_REQCREATECHARACTER;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt8 Status;
	UInt8 SlotIndex;
	GAME_DATA_CHARACTER_INDEX Character;
} IPC_DATA_WORLD_ACKCREATECHARACTER;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
	Timestamp SessionTimeout;
} IPC_DATA_WORLD_UPDATE_ACCOUNT_SESSION_DATA;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int32 AccountID;
	Int32 CharacterSlotID;
	UInt64 CharacterSlotOrder;
	UInt32 CharacterSlotFlags;
} IPC_DATA_WORLD_UPDATE_ACCOUNT_CHARACTER_DATA;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Char CharacterPassword[MAX_SUBPASSWORD_LENGTH + 1];
	UInt32 CharacterQuestion;
	Char CharacterAnswer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
} IPC_DATA_WORLD_UPDATE_ACCOUNT_SUBPASSWORD_DATA;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Int32 CharacterID;
	UInt32 CharacterIndex;
} IPC_DATA_WORLD_REQGETCHARACTER;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	UInt32 CharacterIndex;
	Bool Success;
	GAME_DATA_CHARACTER Character;
} IPC_DATA_WORLD_ACKGETCHARACTER;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Int32 CharacterID;
	union _RTCharacterSyncMask SyncMask;
	union _RTCharacterSyncPriority SyncPriority;
	UInt8 Data[0];
} IPC_DATA_WORLD_REQDBSYNC;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Int32 CharacterID;
	union _RTCharacterSyncMask SyncMaskFailed;
	union _RTCharacterSyncPriority SyncPriority;
} IPC_DATA_WORLD_ACKDBSYNC;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Int64 AccountID;
	Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
} IPC_DATA_WORLD_REQVERIFYPASSWORD;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Bool Success;
} IPC_DATA_WORLD_ACKVERIFYPASSWORD;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;

	struct {
		UInt32 CharacterIndex;
		UInt8 WorldServerID;
		UInt8 CharacterType;
		Int32 Level;
		UInt8 NameLength;
		Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
	} Source;

	struct {
		UInt32 CharacterIndex;
		UInt8 WorldServerID;
		UInt8 CharacterType;
		Int32 Level;
		UInt8 NameLength;
		Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
	} Target;

} IPC_DATA_WORLD_REQPARTYINVITE;

typedef struct {
	IPC_DATA_SIGNATURE_EXTENDED;
	Bool Success;

	struct {
		UInt32 CharacterIndex;
		UInt8 WorldServerID;
		UInt8 CharacterType;
		Int32 Level;
		UInt8 NameLength;
		Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
	} Source;

	struct {
		UInt32 CharacterIndex;
		UInt8 WorldServerID;
		UInt8 CharacterType;
		Int32 Level;
		UInt8 NameLength;
		Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
	} Target;

} IPC_DATA_WORLD_ACKPARTYINVITE;

#pragma pack(pop)

EXTERN_C_END
