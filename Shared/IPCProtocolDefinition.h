#ifndef IPC_PROTOCOL_ENUM
#define IPC_PROTOCOL_ENUM(...)
#endif

#ifndef IPC_PROTOCOL_STRUCT
#define IPC_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef IPC_PROTOCOL
#define IPC_PROTOCOL(__NAMESPACE__, __NAME__, __BODY__)
#endif

IPC_PROTOCOL(L2W, VERIFY_LINKS,
    Int32 AccountID;
	UInt32 AuthKey;
	UInt16 EntityID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	Char SessionIP[MAX_ADDRESSIP_LENGTH];
)

IPC_PROTOCOL(W2L, VERIFY_LINKS,
    Int32 AccountID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	UInt8 Status;
)

IPC_PROTOCOL(L2W, VERIFY_PASSWORD,
    Bool Success;
)

IPC_PROTOCOL(W2L, WORLD_VERIFY_LINKS,
    Int32 AccountID;
	UInt32 AuthKey;
	UInt16 EntityID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	UInt32 ClientMagicKey;
	Char SessionIP[MAX_ADDRESSIP_LENGTH];
)

IPC_PROTOCOL(W2W, REQUEST_VERIFY_LINKS,
	Int32 AccountID;
	UInt32 AuthKey;
	UInt16 EntityID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	Char SessionIP[MAX_ADDRESSIP_LENGTH];
	GAME_DATA_ACCOUNT Account;
	GAME_DATA_CHARACTER_INDEX Characters[MAX_CHARACTER_COUNT];
)

IPC_PROTOCOL(W2W, RESPONSE_VERIFY_LINKS,
	Int32 AccountID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	UInt8 Status;
)

IPC_PROTOCOL(L2W, WORLD_VERIFY_LINKS,
	Bool Success;
)

IPC_PROTOCOL(L2M, GET_WORLD_LIST,
)

IPC_PROTOCOL_STRUCT(IPC_M2L_DATA_SERVER_GROUP_NODE,
    UInt8 NodeIndex;
    UInt16 PlayerCount;
    UInt16 MaxPlayerCount;
    Char Host[64];
    UInt16 Port;
    UInt32 Type;
)

IPC_PROTOCOL_STRUCT(IPC_M2L_DATA_SERVER_GROUP,
    UInt8 GroupIndex;
    UInt8 NodeCount;
	// IPC_M2L_DATA_SERVER_GROUP_NODE Nodes[0];
)

IPC_PROTOCOL(M2L, GET_WORLD_LIST,
    UInt8 GroupCount;
    // IPC_M2L_DATA_SERVER_GROUP Groups[0];
)

IPC_PROTOCOL(W2M, GET_WORLD_LIST,
)

IPC_PROTOCOL_STRUCT(IPC_M2W_DATA_SERVER_GROUP_NODE,
	UInt8 NodeIndex;
	UInt16 PlayerCount;
	UInt16 MaxPlayerCount;
	Char Host[64];
	UInt16 Port;
	UInt32 Type;
)

IPC_PROTOCOL(M2W, GET_WORLD_LIST,
    UInt8 NodeCount;
    // IPC_M2L_DATA_SERVER_GROUP_NODE Nodes[0];
)

IPC_PROTOCOL(W2M, NFY_WORLD_INFO,
	UInt16 PlayerCount;
	UInt16 MaxPlayerCount;
	Char Host[64];
	UInt16 Port;
	UInt32 Type;
)

IPC_PROTOCOL(W2L, VERIFY_PASSWORD,
    Int64 AccountID;
	Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
)

IPC_PROTOCOL(W2M, GET_CHARACTER_LIST,
    Int64 AccountID;
)

IPC_PROTOCOL(M2W, GET_CHARACTER_LIST,
	GAME_DATA_CHARACTER_INDEX Characters[MAX_CHARACTER_COUNT];
)

IPC_PROTOCOL(W2M, GET_PREMIUM_SERVICE,
    Int64 AccountID;
)

IPC_PROTOCOL(M2W, GET_PREMIUM_SERVICE,
    Int64 AccountID;
    Bool HasService;
	UInt32 ServiceType;
	Timestamp StartedAt;
	Timestamp ExpiredAt;
)

IPC_PROTOCOL(W2M, CREATE_CHARACTER,
    Int64 AccountID;
    UInt8 SlotIndex;
	UInt8 NameLength;
	Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
	struct _RTCharacterInfo CharacterData;
    struct _RTCharacterEquipmentInfo CharacterEquipment;
	struct _RTCharacterInventoryInfo CharacterInventory;
	struct _RTCharacterSkillSlotInfo CharacterSkillSlots;
	struct _RTCharacterQuickSlotInfo CharacterQuickSlots;
)

IPC_PROTOCOL(M2W, CREATE_CHARACTER,
    UInt8 Status;
	UInt8 SlotIndex;
	GAME_DATA_CHARACTER_INDEX Character;
)

IPC_PROTOCOL(W2M, UPDATE_ACCOUNT_SESSION,
	Int64 AccountID;
	Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
	Timestamp SessionTimeout;
)

IPC_PROTOCOL(W2M, UPDATE_CHARACTER_SLOT,
	Int32 AccountID;
	Int32 CharacterSlotID;
	UInt64 CharacterSlotOrder;
	UInt32 CharacterSlotFlags;
)

IPC_PROTOCOL(W2M, UPDATE_SUBPASSWORD,
	Int64 AccountID;
	Char CharacterPassword[MAX_SUBPASSWORD_LENGTH + 1];
	UInt32 CharacterQuestion;
	Char CharacterAnswer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
)

IPC_PROTOCOL(W2M, GET_ACCOUNT,
	Int64 AccountID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	Index LinkConnectionID;
)

IPC_PROTOCOL(M2W, GET_ACCOUNT,
	Int64 AccountID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	Index LinkConnectionID;
	Bool Success;
    GAME_DATA_ACCOUNT Account;
)

IPC_PROTOCOL(W2M, GET_CHARACTER,
	Int64 AccountID;
	Int32 CharacterID;
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(M2W, GET_CHARACTER,
	Int64 AccountID;
	Int32 CharacterID;
	UInt32 CharacterIndex;
    Bool Success;
    
    struct {
        Int32 ID;
        Int32 Index;
        UInt64 CreationDate;
        Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
        struct _RTCharacterInfo CharacterData;
        struct _RTCharacterEquipmentInfo EquipmentData;
        struct _RTCharacterInventoryInfo InventoryData;
        struct _RTCharacterSkillSlotInfo SkillSlotData;
        struct _RTCharacterQuickSlotInfo QuickSlotData;
        struct _RTCharacterQuestSlotInfo QuestSlotData;
        struct _RTCharacterQuestFlagInfo QuestFlagData;
        struct _RTCharacterDungeonQuestFlagInfo DungeonQuestFlagData;
        GAME_DATA_CHARACTER_ACHIEVEMENT AchievementData;
        struct _RTCharacterEssenceAbilityInfo EssenceAbilityData;
        GAME_DATA_CHARACTER_BLENDEDABILITY BlendedAbilityData;
        GAME_DATA_CHARACTER_HONORMEDAL HonorMedalData;
        struct _RTCharacterOverlordMasteryInfo OverlordData;
        struct _RTCharacterForceWingInfo ForceWingData;
        GAME_DATA_CHARACTER_TRANSFORM TransformData;
        GAME_DATA_CHARACTER_TRANSCENDENCE TranscendenceData;
        GAME_DATA_CHARACTER_MERCENARY MercenaryData;
        GAME_DATA_CHARACTER_CRAFT CraftData;
        struct _RTCharacterWarehouseInfo WarehouseData;
        struct _RTCharacterCollectionInfo CollectionData;
        struct _RTCharacterNewbieSupportInfo NewbieSupportData;
    } Character;
)

IPC_PROTOCOL(W2M, DELETE_CHARACTER,
	Int64 AccountID;
	Int32 CharacterID;
)

IPC_PROTOCOL(M2W, DELETE_CHARACTER,
	Int32 CharacterID;
	Bool Success;
)

IPC_PROTOCOL(W2M, DBSYNC,
	Int64 AccountID;
	Int32 CharacterID;
	union _RTCharacterSyncMask SyncMask;
	union _RTCharacterSyncPriority SyncPriority;
	UInt8 Data[0];
)

IPC_PROTOCOL(M2W, DBSYNC,
	Int64 AccountID;
	Int32 CharacterID;
	union _RTCharacterSyncMask SyncMaskFailed;
	union _RTCharacterSyncPriority SyncPriority;
)

IPC_PROTOCOL(W2W, REQUEST_SERVER_STATUS,
)

IPC_PROTOCOL(W2W, RESPONSE_SERVER_STATUS,
	Int32 Status;
)

IPC_PROTOCOL_STRUCT(IPC_DATA_PARTY_INVITE_MEMBER,
	Index CharacterIndex;
	RTEntityID CharacterID;
	UInt8 NodeIndex;
	UInt8 CharacterType;
	Int32 Level;
	UInt16 OverlordLevel;
	Int32 MythRebirth;
	Int32 MythHolyPower;
	Int32 MythLevel;
	UInt8 ForceWingGrade;
	UInt8 ForceWingLevel;
	Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1];
)

IPC_PROTOCOL(W2P, PARTY_INVITE,
	UInt8 Result;
	struct _RTPartySlot Source;
	struct _RTPartySlot Target;
)

IPC_PROTOCOL(P2W, PARTY_INVITE,
	struct _RTPartySlot Source;
	struct _RTPartySlot Target;
)

IPC_PROTOCOL(W2P, PARTY_INVITE_ACK,
	Bool Success;
	struct _RTPartySlot Source;
	struct _RTPartySlot Target;
)

IPC_PROTOCOL(P2W, PARTY_INVITE_ACK,
	Bool Success;
	struct _RTPartySlot Source;
	struct _RTPartySlot Target;
)

IPC_PROTOCOL(W2P, PARTY_INVITE_CONFIRM,
	Bool IsAccept;
	Index SourceCharacterIndex;
	UInt8 SourceNodeIndex;
	Index TargetCharacterIndex;
	UInt8 TargetNodeIndex;
)

IPC_PROTOCOL(P2W, PARTY_INVITE_CONFIRM,
	Bool Success;
	Index SourceCharacterIndex;
	UInt8 SourceNodeIndex;
	Index TargetCharacterIndex;
	UInt8 TargetNodeIndex;
)

IPC_PROTOCOL(W2P, PARTY_INVITE_CANCEL,
	Index SourceCharacterIndex;
	UInt8 SourceNodeIndex;
	Index TargetCharacterIndex;
	UInt8 TargetNodeIndex;
)

IPC_PROTOCOL(P2W, PARTY_INVITE_CANCEL,
)

IPC_PROTOCOL(P2W, PARTY_INVITE_TIMEOUT,
	Index CharacterIndex;
	Int32 IsAccept;
	Bool IsCancel;
)

IPC_PROTOCOL(P2W, PARTY_INFO,
	struct _RTParty Party;
)

IPC_PROTOCOL(W2P, PARTY_INIT,
	Index CharacterIndex;
)

IPC_PROTOCOL(P2W, PARTY_INIT,
	Int32 Result;
	UInt32 DungeonIndex;
	RTEntityID PartyID;
	Index PartyLeaderIndex;
	UInt8 WorldServerIndex;
	Int32 MemberCount;
	struct _RTPartyMemberInfo Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
	UInt32 SoloDungeonIndex;
	Timestamp SoloDungeonTimeout;
)

IPC_PROTOCOL(W2P, PARTY_DATA,
	struct _RTPartyMemberInfo MemberInfo;
	struct _RTPartyMemberData MemberData;
)

IPC_PROTOCOL(P2W, PARTY_DATA, 
	UInt32 MemberCount;
	struct _RTPartyMemberInfo MemberInfo[RUNTIME_PARTY_MAX_MEMBER_COUNT];
	struct _RTPartyMemberData MemberData[RUNTIME_PARTY_MAX_MEMBER_COUNT];
)

#undef IPC_PROTOCOL_ENUM
#undef IPC_PROTOCOL_STRUCT
#undef IPC_PROTOCOL
