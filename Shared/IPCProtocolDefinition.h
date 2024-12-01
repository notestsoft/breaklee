#ifndef IPC_PROTOCOL_ENUM
#define IPC_PROTOCOL_ENUM(...)
#endif

#ifndef IPC_PROTOCOL_STRUCT
#define IPC_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef IPC_PROTOCOL
#define IPC_PROTOCOL(__NAMESPACE__, __NAME__, __BODY__)
#endif

IPC_PROTOCOL_STRUCT(IPC_DATA_CHARACTER_INFO,
	Int32 CharacterIndex;
	UInt64 CreationDate;
	UInt32 Style;
	Int32 Level;
	UInt16 OverlordLevel;
	Int32 MythRebirth;
	Int32 MythHolyPower;
	Int32 MythLevel;
	Int32 SkillRank;
	UInt8 NationMask;
	Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
	UInt64 HonorPoint;
	UInt32 CostumeActivePageIndex;
	UInt32 CostumeAppliedSlots[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT];
	UInt64 Currency;
	UInt8 WorldIndex;
	UInt16 PositionX;
	UInt16 PositionY;
	UInt16 EquipmentCount;
	struct _RTItemSlot Equipment[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT];
	struct _RTItemSlotAppearance EquipmentAppearance[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT];
)

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
	Bool IsSubpasswordSet;
	struct _RTCharacterAccountInfo AccountInfo;
	IPC_DATA_CHARACTER_INFO Characters[MAX_CHARACTER_COUNT];
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
    Int32 AccountID;
	Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
)

IPC_PROTOCOL(W2D, GET_CHARACTER_LIST,
    Int32 AccountID;
)

IPC_PROTOCOL(D2W, GET_CHARACTER_LIST,
	IPC_DATA_CHARACTER_INFO Characters[MAX_CHARACTER_COUNT];
)

IPC_PROTOCOL(W2D, GET_PREMIUM_SERVICE,
    Int32 AccountID;
)

IPC_PROTOCOL(D2W, GET_PREMIUM_SERVICE,
    Int32 AccountID;
    Bool HasService;
	UInt32 ServiceType;
	Timestamp StartedAt;
	Timestamp ExpiredAt;
)

IPC_PROTOCOL(W2D, CREATE_CHARACTER,
    Int32 AccountID;
    UInt8 CharacterSlotIndex;
	UInt8 CharacterNameLength;
	Char CharacterName[MAX_CHARACTER_NAME_LENGTH + 1];
	union _RTCharacterStyle CharacterStyle;
	UInt16 StatSTR;
	UInt16 StatDEX;
	UInt16 StatINT;
	UInt8 WorldIndex;
	UInt16 PositionX;
	UInt16 PositionY;
	UInt8 Data[0];
)

IPC_PROTOCOL(D2W, CREATE_CHARACTER,
    UInt8 Status;
	UInt8 CharacterSlotIndex;
	IPC_DATA_CHARACTER_INFO Character;
)

IPC_PROTOCOL(W2D, AUTHENTICATE,
	Int32 AccountID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	Int LinkConnectionID;
)

IPC_PROTOCOL(D2W, AUTHENTICATE,
	Int32 AccountID;
	UInt8 NodeIndex;
	UInt8 GroupIndex;
	Int LinkConnectionID;
	Bool Success;
	Bool IsSubpasswordSet;
	struct _RTCharacterAccountInfo AccountInfo;
)

IPC_PROTOCOL(W2D, GET_CHARACTER,
	Int32 AccountID;
	Int32 CharacterIndex;
)

IPC_PROTOCOL_STRUCT(IPC_D2W_DATA_INITIALIZE_WAR_TIMER,
	UInt32 TimeRemainingTimer;
	UInt32 TimeAttackTimer;
	UInt8 IsTimeAttackEnabled;
)

IPC_PROTOCOL_STRUCT(IPC_D2W_DATA_INITIALIZE_WAR,
	Int32 LobbyEntryOrder;
	IPC_D2W_DATA_INITIALIZE_WAR_TIMER LobbyTimer;
	IPC_D2W_DATA_INITIALIZE_WAR_TIMER BattleFieldTimer;
	Int32 CapellaScore;
	Int32 ProcyonScore;
	Int32 CapellaPoints;
	Int32 ProcyonPoints;
	Int32 EntryLimitPerNation;
	Int32 CapellaBattleFieldTicketCount;
	Int32 ProcyonBattleFieldTicketCount;
)

IPC_PROTOCOL_STRUCT(IPC_D2W_DATA_INITIALIZE_SERVER_ADDRESS,
	Char Host[MAX_HOSTNAME_LENGTH + 1];
	UInt16 Port;
)

IPC_PROTOCOL_STRUCT(IPC_D2W_DATA_INITIALIZE_SERVER,
	UInt8 ServerID;
	UInt8 WorldServerID;
	UInt16 PlayerCount;
	UInt8 Unknown1[18];
	UInt8 MaxPlayerLevel;
	UInt8 MinPlayerLevel;
	UInt8 MaxRank;
	UInt8 MinRank;
	UInt16 MaxPlayerCount;
	IPC_D2W_DATA_INITIALIZE_SERVER_ADDRESS Address;
	UInt32 WorldType;
	UInt32 Unknown2;
)

IPC_PROTOCOL_STRUCT(IPC_D2W_DATA_INITIALIZE_CHARACTER,
	UInt8 IsWarehousePasswordSet;
	UInt8 IsInventoryPasswordSet;
	UInt8 IsWarehouseLocked;
	UInt8 IsInventoryLocked;
	struct _RTCharacterInfo CharacterInfo;
	struct _RTCharacterStyleInfo CharacterStyleInfo;
	struct _RTCharacterBattleModeInfo BattleModeInfo;
	struct _RTBuffInfo BuffInfo;
	struct _RTEquipmentInfo EquipmentInfo;
	struct _RTInventoryInfo InventoryInfo;
	struct _RTVehicleInventoryInfo VehicleInventoryInfo;
	struct _RTSkillSlotInfo SkillSlotInfo;
	struct _RTQuickSlotInfo QuickSlotInfo;
	struct _RTMercenaryInfo MercenaryInfo;
	UInt16 ItemPeriodCount;
	struct _RTAbilityInfo AbilityInfo;
	struct _RTBlessingBeadInfo BlessingBeadInfo;
	struct _RTPremiumServiceInfo PremiumServiceInfo;
	struct _RTQuestInfo QuestInfo;
	struct _RTDailyQuestInfo DailyQuestInfo;
	UInt32 HelpWindow;
	struct _RTAppearanceInfo AppearanceInfo;
	struct _RTAchievementInfo AchievementInfo;
	struct _RTCraftInfo CraftInfo;
	struct _RTRequestCraftInfo RequestCraftInfo;
	struct _RTCooldownInfo CooldownInfo;
	struct _RTUpgradeInfo UpgradeInfo;
	struct _RTGoldMeritMasteryInfo GoldMeritMasteryInfo;
	struct _RTPlatinumMeritMasteryInfo PlatinumMeritMasteryInfo;
	struct _RTDiamondMeritMasteryInfo DiamondMeritMasteryInfo;
	struct _RTAchievementExtendedInfo AchievementExtendedInfo;
	UInt32 ForceGem;
	struct _RTWarpServiceInfo WarpServiceInfo;
	struct _RTOverlordMasteryInfo OverlordMasteryInfo;
	struct _RTHonorMedalInfo HonorMedalInfo;
	struct _RTForceWingInfo ForceWingInfo;
	struct _RTGiftBoxInfo GiftBoxInfo;
	struct _RTCollectionInfo CollectionInfo;
	struct _RTTransformInfo TransformInfo;
	struct _RTCharacterSecretShopData SecretShopData;
	struct _RTAuraMasteryInfo AuraMasteryInfo;
	struct _RTTranscendenceInfo TranscendenceInfo;
	struct _RTDamageBoosterInfo DamageBoosterInfo;
	struct _RTCharacterResearchSupportInfo ResearchSupportInfo;
	struct _RTStellarMasteryInfo StellarMasteryInfo;
	struct _RTMythMasteryInfo MythMasteryInfo;
	struct _RTNewbieSupportInfo NewbieSupportInfo;
	struct _RTEventPassInfo EventPassInfo;
	struct _RTCostumeWarehouseInfo CostumeWarehouseInfo;
	struct _RTCostumeInfo CostumeInfo;
	struct _RTExplorationInfo ExplorationInfo;
	struct _RTAnimaMasteryInfo AnimaMasteryInfo;
	struct _RTCharacterPresetInfo PresetInfo;
	UInt8 NameLength;
	Char Name[MAX_CHARACTER_NAME_LENGTH];
)

IPC_PROTOCOL(D2W, GET_CHARACTER,
	Bool Success;
	Int32 AccountID;
	Int32 CharacterIndex;
	IPC_D2W_DATA_INITIALIZE_CHARACTER Character;
	struct _RTCharacterSettingsInfo SettingsInfo;
	struct _RTWarehouseInfo WarehouseInfo;
	UInt8 Data[0];
)

IPC_PROTOCOL(W2D, DELETE_CHARACTER,
	Int32 AccountID;
	Int32 CharacterIndex;
)

IPC_PROTOCOL(D2W, DELETE_CHARACTER,
	Bool Success;
	Int32 CharacterIndex;
)

IPC_PROTOCOL(W2D, VERIFY_SUBPASSWORD,
	Int32 AccountID;
	Char Password[MAX_SUBPASSWORD_LENGTH + 1];
	Int32 Type;
	Int32 ExpirationInHours;
	Int32 IsLocked;
)

IPC_PROTOCOL(D2W, VERIFY_SUBPASSWORD,
	Int32 Success;
	Int8 FailureCount;
	Int32 IsLocked;
	Int32 Type;
)

IPC_PROTOCOL(W2D, CREATE_SUBPASSWORD,
	Int32 AccountID;
	Char Password[MAX_SUBPASSWORD_LENGTH + 1];
	Int32 Type;
	Int32 Question;
	Char Answer[MAX_SUBPASSWORD_ANSWER_LENGTH];
	Int32 IsChange;
)

IPC_PROTOCOL(D2W, CREATE_SUBPASSWORD,
	Int32 Success;
	Int32 IsChange;
	Int32 Type;
	Int32 IsLocked;
)

IPC_PROTOCOL(W2D, DELETE_SUBPASSWORD,
	Int32 AccountID;
	Int32 Type;
)

IPC_PROTOCOL(D2W, DELETE_SUBPASSWORD,
	Int32 Success;
	Int32 Type;
)

IPC_PROTOCOL(W2D, VERIFY_DELETE_SUBPASSWORD,
	Int32 AccountID;
	Int32 Type;
	Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

IPC_PROTOCOL(D2W, VERIFY_DELETE_SUBPASSWORD,
	Int32 Success;
	Int8 FailureCount;
	Int32 Type;
)

IPC_PROTOCOL(W2D, VERIFY_CREDENTIALS_SUBPASSWORD,
	Int32 AccountID;
	Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

IPC_PROTOCOL(D2W, VERIFY_CREDENTIALS_SUBPASSWORD,
	Int8 Success;
)

IPC_PROTOCOL(W2D, CHECK_SUBPASSWORD,
	Int32 AccountID;
)

IPC_PROTOCOL(D2W, CHECK_SUBPASSWORD,
	Int32 IsVerificationRequired;
)

IPC_PROTOCOL(W2D, DBSYNC,
	Int32 AccountID;
	Int32 CharacterIndex;
	union _RTCharacterSyncMask SyncMask;
	Bool IsTransaction;
	UInt8 Data[0];
)

IPC_PROTOCOL(D2W, DBSYNC,
	Int32 AccountID;
	Int32 CharacterIndex;
	union _RTCharacterSyncMask SyncMaskFailed;
)

IPC_PROTOCOL(W2D, SET_CHARACTER_SLOT_ORDER,
	Int32 AccountID;
	UInt64 CharacterSlotOrder;
)

IPC_PROTOCOL(D2W, SET_CHARACTER_SLOT_ORDER,
	UInt8 Result;
)

IPC_PROTOCOL(W2W, REQUEST_SERVER_STATUS,
)

IPC_PROTOCOL(W2W, RESPONSE_SERVER_STATUS,
	Int32 Status;
)

IPC_PROTOCOL_STRUCT(IPC_DATA_PARTY_INVITE_MEMBER,
	UInt32 CharacterIndex;
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

IPC_PROTOCOL(W2P, CLIENT_CONNECT,
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(P2W, CLIENT_CONNECT,
	Int32 Result;
	UInt32 DungeonIndex;
	RTEntityID PartyID;
	Int PartyLeaderIndex;
	UInt8 WorldServerIndex;
	Int32 MemberCount;
	struct _RTPartyMemberInfo Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
	UInt32 SoloDungeonIndex;
	Timestamp SoloDungeonTimeout;
)

IPC_PROTOCOL(W2P, CLIENT_DISCONNECT,
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(W2P, PARTY_INVITE,
	UInt8 Result;
	struct _RTPartyMemberInfo Source;
	struct _RTPartyMemberInfo Target;
)

IPC_PROTOCOL(P2W, PARTY_INVITE,
	struct _RTPartyMemberInfo Source;
	struct _RTPartyMemberInfo Target;
)

IPC_PROTOCOL(W2P, PARTY_INVITE_ACK,
	Bool Success;
	struct _RTPartyMemberInfo Source;
	struct _RTPartyMemberInfo Target;
)

IPC_PROTOCOL(P2W, PARTY_INVITE_ACK,
	Bool Success;
	struct _RTPartyMemberInfo Source;
	struct _RTPartyMemberInfo Target;
)

IPC_PROTOCOL(W2P, PARTY_INVITE_CONFIRM,
	Bool IsAccept;
	Int SourceCharacterIndex;
	UInt8 SourceNodeIndex;
	Int TargetCharacterIndex;
	UInt8 TargetNodeIndex;
)

IPC_PROTOCOL(P2W, PARTY_INVITE_CONFIRM,
	Bool Success;
	RTEntityID PartyID;
	Int SourceCharacterIndex;
	UInt8 SourceNodeIndex;
	Int TargetCharacterIndex;
	UInt8 TargetNodeIndex;
)

IPC_PROTOCOL(W2P, PARTY_INVITE_CANCEL,
	Int SourceCharacterIndex;
	UInt8 SourceNodeIndex;
	Int TargetCharacterIndex;
	UInt8 TargetNodeIndex;
)

IPC_PROTOCOL(P2W, PARTY_INVITE_CANCEL,
)

IPC_PROTOCOL(P2W, PARTY_INVITE_TIMEOUT,
	UInt32 CharacterIndex;
	Int32 IsAccept;
	Bool IsCancel;
)

IPC_PROTOCOL(W2P, PARTY_LEAVE,
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(P2W, PARTY_LEAVE,
	RTEntityID PartyID;
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(P2W, PARTY_LEAVE_ACK,
	Int32 Result;
)

IPC_PROTOCOL(W2P, PARTY_CHANGE_LEADER,
	RTEntityID PartyID;
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(P2W, PARTY_CHANGE_LEADER,
	RTEntityID PartyID;
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(P2W, PARTY_CHANGE_LEADER_ACK,
	Int32 Result;
)

IPC_PROTOCOL(P2W, PARTY_INFO,
	struct _RTParty Party;
)

IPC_PROTOCOL(W2P, PARTY_DATA,
	struct _RTPartyMemberInfo MemberInfo;
)

IPC_PROTOCOL(P2W, PARTY_DATA, 
	UInt32 MemberCount;
	struct _RTPartyMemberInfo MemberInfo[RUNTIME_PARTY_MAX_MEMBER_COUNT];
)

IPC_PROTOCOL(W2P, BROADCAST_TO_PARTY,
	RTEntityID PartyID;
	Int32 Length;
	UInt8 Data[0];
)

IPC_PROTOCOL(P2W, BROADCAST_TO_CHARACTER,
	UInt32 CharacterIndex;
	Int32 Length;
	UInt8 Data[0];
)

IPC_PROTOCOL(P2W, CREATE_PARTY,
	struct _RTParty Party;
)

IPC_PROTOCOL(P2W, DESTROY_PARTY,
	struct _RTParty Party;
)

IPC_PROTOCOL(N2M, CLIENT_CONNECT,
	Int32 AccountID;
)

IPC_PROTOCOL(M2N, CLIENT_CONNECT,
	Int32 AccountID;
	Bool IsOnline;
)

IPC_PROTOCOL(N2M, CLIENT_DISCONNECT,
	Int32 AccountID;
)

IPC_PROTOCOL(M2N, CLIENT_DISCONNECT,
	Int32 AccountID;
	Bool IsOnline;
)

IPC_PROTOCOL(W2A, VERIFY_LINKS,
	Int32 AccountID;
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(A2D, GET_BOOKMARK,
	Int32 AccountID;
)

IPC_PROTOCOL_STRUCT(IPC_DATA_GET_BOOKMARK_SLOT,
	UInt8 SlotIndex;
	UInt16 CategoryIndex[5];
	UInt8 SubCategoryIndex[2];
	Char Description[MAX_AUCTION_BOOKMARK_DESCRIPTION_LENGTH];
)

IPC_PROTOCOL(D2A, GET_BOOKMARK,
	UInt8 Result;
	UInt8 Count;
	IPC_DATA_GET_BOOKMARK_SLOT Slots[0];
)

IPC_PROTOCOL(A2D, SET_BOOKMARK,
	Int32 AccountID;
	IPC_DATA_GET_BOOKMARK_SLOT Bookmark;
)

IPC_PROTOCOL(D2A, SET_BOOKMARK,
	UInt8 Result;
)

IPC_PROTOCOL(A2D, DELETE_BOOKMARK,
	Int32 AccountID;
	UInt8 SlotIndex;
)

IPC_PROTOCOL(D2A, DELETE_BOOKMARK,
	UInt8 Result;
)

IPC_PROTOCOL(A2D, GET_ITEM_LIST,
	Int32 AccountID;
)

IPC_PROTOCOL_STRUCT(IPC_DATA_GET_ITEM_LIST_INDEX,
	UInt8 SlotIndex;
	UInt64 ItemID;
	UInt64 ItemOptions;
	UInt64 ItemPrice;
	Int32 ItemCount;
	Int32 SoldItemCount;
	Timestamp ExpirationDate;
	Timestamp RegistrationDate;
)

IPC_PROTOCOL(D2A, GET_ITEM_LIST,
	UInt8 Result;
	UInt8 ItemCount;
	IPC_DATA_GET_ITEM_LIST_INDEX Items[0];
)

IPC_PROTOCOL(W2A, DISCONNECT_CLIENT,
	Int32 AccountID;
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(A2W, DISCONNECT_CLIENT,
	UInt8 Result;
)

IPC_PROTOCOL(A2D, GET_ITEM_AVERAGE_PRICE,
	UInt64 ItemID;
	UInt64 ItemOptions;
)

IPC_PROTOCOL(D2A, GET_ITEM_AVERAGE_PRICE,
	UInt64 Price;
)

IPC_PROTOCOL(A2D, GET_ITEM_MINIMUM_PRICE,
	UInt64 ItemID;
	UInt64 ItemOptions;
)

IPC_PROTOCOL(D2A, GET_ITEM_MINIMUM_PRICE,
	UInt64 Price;
)

IPC_PROTOCOL(A2D, SEARCH,
	UInt8 CategoryIndex2;
	UInt16 CategoryIndex3;
	UInt8 CategoryIndex4;
	UInt16 CategoryIndex5;
	UInt16 SortOrder;
)

IPC_PROTOCOL_STRUCT(IPC_DATA_SEARCH_RESULT_SLOT,
	UInt64 ItemID;
	UInt64 ItemOptions;
	UInt32 ItemOptionExtended;
	Int16 StackSize;
	UInt8 PriceType;
	Int64 Price;
	UInt32 AccountID;
	Char CharacterName[MAX_CHARACTER_NAME_LENGTH];
)

IPC_PROTOCOL(D2A, SEARCH,
	Int16 ResultCount;
	IPC_DATA_SEARCH_RESULT_SLOT Results[0];
)

IPC_PROTOCOL(W2D, AUCTION_REGISTER_ITEM,
	Int32 AccountID;
	Int32 CharacterIndex;
	UInt8 SlotIndex;
	UInt16 ItemCount;
	UInt64 ItemPrice;
	UInt16 CategoryIndex[5];
	UInt8 SubCategoryIndex[2];
	Timestamp ExpirationTime;
	struct _RTInventoryInfo InventoryInfo;
	UInt8 Data[0];
	// RTItemSlotRef MarketInventorySlots[];
	// RTItemSlotRef InventorySlots[];
)

IPC_PROTOCOL(D2W, AUCTION_REGISTER_ITEM,
	UInt8 Result;
	Timestamp ExpirationDate;
)

IPC_PROTOCOL(W2D, AUCTION_UNREGISTER_ITEM,
	Int32 AccountID;
	UInt8 SlotIndex;
	UInt16 InventorySlotCount;
	UInt16 InventorySlotIndex[0];
)

IPC_PROTOCOL(D2W, AUCTION_UNREGISTER_ITEM,
	UInt8 Result;
	struct _RTItemSlot ItemSlot;
	UInt16 InventorySlotCount;
	UInt16 InventorySlotIndex[0];
)

IPC_PROTOCOL_STRUCT(IPC_W2D_DATA_AUCTION_UPDATE_ITEM_INCREASE,
	UInt16 InventorySlotCount;
	struct _RTInventoryInfo InventoryInfo;
	UInt8 Data[0];
	// RTItemSlotRef MarketInventorySlots[];
	// RTItemSlotRef InventorySlots[];
)

IPC_PROTOCOL_STRUCT(IPC_W2D_DATA_AUCTION_UPDATE_ITEM_DECREASE,
	UInt16 InventorySlotCount;
	UInt16 InventorySlotIndex[0];
)

IPC_PROTOCOL(W2D, AUCTION_UPDATE_ITEM,
	Int32 AccountID;
	Int32 CharacterIndex;
	UInt8 SlotIndex;
	UInt16 ItemCount;
	UInt8 ItemPriceType;
	UInt64 ItemPrice;
	Timestamp ExpirationTime;
	Bool IsDecrease;
	UInt8 Data[0];
)

IPC_PROTOCOL_STRUCT(IPC_D2W_DATA_AUCTION_UPDATE_ITEM_INCREASE,
	UInt8 Padding;
)

IPC_PROTOCOL_STRUCT(IPC_D2W_DATA_AUCTION_UPDATE_ITEM_DECREASE,
	struct _RTItemSlot ItemSlot;
	UInt16 InventorySlotCount;
	UInt16 InventorySlotIndex[0];
)

IPC_PROTOCOL(D2W, AUCTION_UPDATE_ITEM,
	UInt8 Result;
	Timestamp ExpirationDate;
	Bool IsDecrease;
	UInt8 Data[0];
)

IPC_PROTOCOL(W2D, AUCTION_BUY_ITEM,
	Int32 AccountID;
	Int32 CharacterIndex;
	Int32 ItemAccountID;
	UInt8 Unknown1;
	UInt64 ItemID;
	UInt64 ItemOptions;
	UInt32 ItemOptionExtended;
	UInt8 ItemPriceType;
	Int64 ItemPrice;
	Char ItemName[MAX_AUCTION_ITEM_NAME_LENGTH];
	UInt16 InventorySlotCount;
	UInt16 InventorySlotIndex[0];
)

IPC_PROTOCOL(D2W, AUCTION_BUY_ITEM,
	UInt8 Result; 
	UInt64 ItemID;
	UInt64 ItemOptions;
	UInt32 ItemOptionExtended;
	UInt8 ItemPriceType;
	Int64 ItemPrice;
	UInt16 InventorySlotCount;
	UInt16 InventorySlotIndex[0];
)

IPC_PROTOCOL(W2D, AUCTION_PROCEED_ITEM,
	Int32 AccountID;
	UInt8 SlotIndex;
	Bool IsCommissionFree;
)

IPC_PROTOCOL(D2W, AUCTION_PROCEED_ITEM,
	UInt8 Result;
	Int32 SoldItemCount;
	Int64 ReceivedAlz;
)

IPC_PROTOCOL(W2C, CLIENT_CONNECT,
	UInt32 CharacterIndex;
)

IPC_PROTOCOL(W2C, CLIENT_DISCONNECT,
	UInt32 CharacterIndex;
)

#undef IPC_PROTOCOL_ENUM
#undef IPC_PROTOCOL_STRUCT
#undef IPC_PROTOCOL
