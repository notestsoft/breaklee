
#include <RuntimeLib/RuntimeLib.h>

#include "Constants.h"
#include "IPCProtocol.h"

#ifndef CLIENT_PROTOCOL_ENUM
#define CLIENT_PROTOCOL_ENUM(...)
#endif

#ifndef CLIENT_PROTOCOL_STRUCT
#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef CLIENT_PROTOCOL
#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __SIGNATURE__, __COMMAND__, __BODY__)
#endif

CLIENT_PROTOCOL(S2C, RUNTIME_NOTIFICATION, DEFAULT, 0,
    UInt8 Data[0];
)

CLIENT_PROTOCOL_STRUCT(CSC_POSITION,
    UInt16 X;
    UInt16 Y;
)

CLIENT_PROTOCOL(S2C, NFY_ERROR, DEFAULT, 7,
    UInt16 ErrorCommand;
    UInt16 ErrorSubCommand;
    UInt16 ErrorCode;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(C2S, GET_CHARACTERS, DEFAULT, 133,
)

CLIENT_PROTOCOL(S2C, GET_CHARACTERS, DEFAULT, 133,
    Int32 IsSubpasswordSet;
    UInt64 BattleFieldEntryFee;
    UInt8 Unknown1;
    UInt8 CanJoinBeginnerGuild;
    struct _RTCharacterAccountInfo AccountInfo;
    // TODO: Replace with CLIENT_PROTOCOL_STRUCT
    IPC_DATA_CHARACTER_INFO Characters[MAX_CHARACTER_COUNT];
)

CLIENT_PROTOCOL(C2S, CREATE_CHARACTER, DEFAULT, 134,
    UInt32 Style;
    UInt8 EnterBeginnerGuild;
    UInt8 CreateSpecialCharacter;
    UInt8 SlotIndex;
    UInt8 NameLength;
    Char Name[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_CHARACTER_STATUS_DATABASE_ERROR = 0x01,
    S2C_CHARACTER_STATUS_DATA_CORRUPTED = 0x02,
    S2C_CHARACTER_STATUS_NAME_DUPLICATE = 0x03,
    S2C_CHARACTER_STATUS_NAME_BLACKLIST = 0x04,
    S2C_CHARACTER_STATUS_SUCCESS = 0xA1,
    S2C_CHARACTER_STATUS_ERROR_DELETE_WITHIN_A_DAY = 0xB1,
    S2C_CHARACTER_STATUS_ERROR_DELETE_GUILD_MASTER = 0xB2,
    S2C_CHARACTER_STATUS_ERROR_JOINING_BEGINNER_GUILD = 0xB3,
)

CLIENT_PROTOCOL(S2C, CREATE_CHARACTER, DEFAULT, 134,
    UInt32 CharacterIndex;
    UInt8 CharacterStatus;
)

CLIENT_PROTOCOL(C2S, DELETE_CHARACTER, DEFAULT, 135,
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(S2C, DELETE_CHARACTER, DEFAULT, 135,
    UInt8 CharacterStatus;
    UInt32 ForceGem;
)

CLIENT_PROTOCOL(C2S, CONNECT, DEFAULT, 140,
    UInt8 ServerID;
    UInt8 WorldServerID;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(S2C, CONNECT, DEFAULT, 140,
    UInt32 XorKey;
    UInt32 AuthKey;
    UInt16 ConnectionID;
    UInt16 XorKeyIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(C2S, VERIFY_LINKS, DEFAULT, 141,
    UInt32 AuthKey;
    UInt16 EntityID;
    UInt8 NodeIndex;
    UInt8 GroupIndex;
    UInt32 ClientMagicKey;
)

CLIENT_PROTOCOL(S2C, VERIFY_LINKS, DEFAULT, 141,
    UInt8 WorldID;
    UInt8 ServerID;
    UInt8 Status;
)

CLIENT_PROTOCOL(C2S, INITIALIZE, DEFAULT, 142,
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_WAR_TIMER,
    UInt32 TimeRemainingTimer;
    UInt32 TimeAttackTimer;
    UInt8 IsTimeAttackEnabled;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_WAR,
    Int32 LobbyEntryOrder;
    S2C_DATA_INITIALIZE_WAR_TIMER LobbyTimer;
    S2C_DATA_INITIALIZE_WAR_TIMER BattleFieldTimer;
    Int32 CapellaScore;
    Int32 ProcyonScore;
    Int32 CapellaPoints;
    Int32 ProcyonPoints;
    Int32 EntryLimitPerNation;
    Int32 CapellaBattleFieldTicketCount;
    Int32 ProcyonBattleFieldTicketCount;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_SERVER_ADDRESS,
    Char Host[MAX_HOSTNAME_LENGTH + 1];
    UInt16 Port;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_SERVER,
    UInt8 ServerID;
    UInt8 WorldServerID;
    UInt16 PlayerCount;
    UInt64 Unknown1;
    UInt64 Unknown2;
    UInt16 Unknown3;
    UInt8 MinPlayerLevel;
    UInt8 MaxPlayerLevel;
    UInt8 MinRank;
    UInt8 MaxRank;
    UInt16 MaxPlayerCount;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS Address;
    UInt32 WorldType;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_QUEST_INDEX,
    UInt16 QuestIndex;
    UInt16 NpcActionFlags;
    UInt8 DisplayNotice;
    UInt8 DisplayOpenNotice;
    UInt8 QuestSlotIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_ESSENCE_ABILITY_SLOT,
    UInt32 AbilityID;
    UInt16 Level;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_BLENDED_ABILITY_SLOT,
    UInt32 AbilityID;
    UInt32 Level;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_PREMIUM_SERVICE_SLOT,
    UInt32 ServiceIndex;
    UInt32 Unknown1;
    RTItemDuration Duration;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_INITIALIZE_BLESSING_BEAD_SLOT,
    UInt32 BlessingBeadID;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_HONOR_MEDAL_SLOT,
    UInt8 CategoryIndex;
    UInt8 GroupIndex;
    UInt32 SlotIndex;
    UInt32 ForceEffectIndex;
    UInt8 IsUnlocked;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_OVERLORD_MASTERY_SLOT,
    UInt8 MasteryIndex;
    UInt8 Level;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NEWBIE_SUPPORT_SLOT,
    UInt8 CategoryType;
    UInt8 ConditionValue1;
    UInt8 ConditionValue2;
    UInt8 RewardIndex;
)

CLIENT_PROTOCOL(S2C, INITIALIZE, EXTENDED, 142,
    S2C_DATA_INITIALIZE_WAR War;
    UInt32 WorldType;
    UInt8 IsWarehousePasswordSet;
    UInt8 IsInventoryPasswordSet;
    UInt8 IsWarehouseLocked;
    UInt8 IsInventoryLocked;
    UInt8 Unknown1A;
    S2C_DATA_INITIALIZE_SERVER Server;

    UInt8 UnknownWar0;
    UInt8 WarType;
    UInt8 UnknownWar1;
    UInt8 UnknownWar2;

    RTEntityID Entity;
    struct _RTCharacterInfo CharacterInfo;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS ChatServerAddress;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS AuctionServerAddress;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS PartyServerAddress;
    UInt16 PartyServerUDPPort;
    struct _RTCharacterStyleInfo CharacterStyleInfo;
    UInt8 UnknownTempInventoryCount;
    struct {
        RTItem Item;
        UInt64 ItemSerial;
        RTItemDuration ItemDuration;
        UInt64 ItemOptions;
    } TempInventory[10];
    struct _RTEquipmentInfo EquipmentInfo;
    struct _RTInventoryInfo InventoryInfo;
    struct _RTVehicleInventoryInfo VehicleInventoryInfo;
    struct _RTSkillSlotInfo SkillSlotInfo;
    struct _RTQuickSlotInfo QuickSlotInfo;
    struct _RTMercenaryInfo MercenaryInfo;
    struct _RTAbilityInfo AbilityInfo;
    struct _RTBlessingBeadInfo BlessingBeadInfo;
    struct _RTPremiumServiceInfo PremiumServiceInfo;
    struct _RTQuestInfo QuestInfo;
    UInt8 DungeonBookmarkEnabled;
    UInt8 DungeonBookmarkInfo[4096];
    struct _RTDailyQuestInfo DailyQuestInfo;
    struct _RTCharacterHelpInfo HelpInfo;
    struct _RTAppearanceInfo AppearanceInfo;
    struct _RTAchievementInfo AchievementInfo;
    struct _RTCraftInfo CraftInfo;
    Int32 Unknown3;
    struct _RTRequestCraftInfo RequestCraftInfo;
    struct _RTCooldownInfo CooldownInfo;
    struct _RTCharacterUpgradeInfo UpgradeInfo;
    struct _RTGoldMeritMasteryInfo GoldMeritMasteryInfo;
    struct _RTPlatinumMeritMasteryInfo PlatinumMeritMasteryInfo;
    struct _RTDiamondMeritMasteryInfo DiamondMeritMasteryInfo;
    struct _RTAchievementExtendedInfo AchievementExtendedInfo;
    UInt32 ForceGem;
    struct _RTWarpServiceInfo WarpServiceInfo;
    Int32 Unknown1;
    Int32 Unknown2;
    Int16 FrontierStoneCount;
    struct _RTOverlordMasteryInfo OverlordMasteryInfo;
    struct _RTHonorMedalInfo HonorMedalInfo;
    struct _RTForceWingInfo ForceWingInfo;
    UInt8 Unknown2111;
    struct _RTGiftBoxInfo GiftBoxInfo;
    struct { UInt32 RewardIndex; UInt64 Timestamp; } GuildRewardSlots[3];
    struct _RTCollectionInfo CollectionInfo;
    struct _RTTransformInfo TransformInfo;
    UInt8 Unknown32[6];
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
    Char Name[0];
)

CLIENT_PROTOCOL(C2S, DEINITIALIZE, DEFAULT, 143,
    UInt8 Reason;
    UInt8 UnknownIndex;
    UInt8 WorldIndex;
    UInt8 Logout;
)

CLIENT_PROTOCOL(S2C, DEINITIALIZE, DEFAULT, 143,
    UInt8 Result;
    UInt8 Reason;
)

CLIENT_PROTOCOL(C2S, GET_WAREHOUSE, DEFAULT, 145,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_WAREHOUSE_SLOT_INDEX,
    RTItem Item;
    UInt64 Unknown1;
    UInt64 ItemOptions;
    UInt16 SlotIndex;
    RTItemDuration ItemDuration;
)

CLIENT_PROTOCOL(S2C, GET_WAREHOUSE, DEFAULT, 145,
    UInt16 Count;
    UInt64 Currency;
    S2C_DATA_GET_WAREHOUSE_SLOT_INDEX Slots[0];
)

CLIENT_PROTOCOL(C2S, SET_QUICKSLOT, DEFAULT, 146,
    UInt16 QuickSlotIndex;
    UInt16 SkillSlotIndex;
)

CLIENT_PROTOCOL(S2C, SET_QUICKSLOT, DEFAULT, 146,
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, SWAP_QUICKSLOT, DEFAULT, 147,
    UInt16 SourceSlotIndex;
    UInt16 TargetSlotIndex;
)

CLIENT_PROTOCOL(S2C, SWAP_QUICKSLOT, DEFAULT, 147,
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, GET_SERVER_TIME, DEFAULT, 148,
)

CLIENT_PROTOCOL(S2C, GET_SERVER_TIME, DEFAULT, 148,
    Timestamp Timestamp;
    Int16 Timezone;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_LOOT_RESULT_SUCCESS = 0x60,
    S2C_DATA_LOOT_RESULT_OWNERSHIP_ERROR,
    S2C_DATA_LOOT_RESULT_UNKNOWN1,
    S2C_DATA_LOOT_RESULT_SLOTINUSE_ERROR,
    S2C_DATA_LOOT_RESULT_ANTIADDICTION_ERROR,
    S2C_DATA_LOOT_RESULT_OUTOFRANGE_ERROR
)

CLIENT_PROTOCOL(C2S, LOOT_INVENTORY_ITEM, DEFAULT, 153,
    RTEntityID Entity;
    UInt16 UniqueKey;
    UInt64 ItemID;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, LOOT_INVENTORY_ITEM, DEFAULT, 153,
    UInt8 Result;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 InventorySlotIndex;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_LOOT_CURRENCY_ITEM_INDEX,
    RTEntityID Entity;
    UInt16 UniqueKey;
)

CLIENT_PROTOCOL(C2S, LOOT_CURRENCY_ITEM, DEFAULT, 154,
    UInt16 Count;
    C2S_DATA_LOOT_CURRENCY_ITEM_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, LOOT_CURRENCY_ITEM, DEFAULT, 154,
    UInt16 Count;
    UInt8 Result;
    UInt64 Currency;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_ITEM_PRICE_INDEX,
    UInt32 SlotIndex;
    UInt32 Count;
)

CLIENT_PROTOCOL(C2S, BUY_ITEM, DEFAULT, 161,
    Int32 RemoteShopItemIndex;
    UInt8 NpcIndex;
    Int16 TabIndex;
    Int16 SlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    Int16 SlotIndex2;
    Int32 Unknown2;
    Int32 ItemCount;
    Int32 ItemPriceCount;
    S2C_DATA_ITEM_PRICE_INDEX ItemPriceList[SERVER_CHARACTER_MAX_BUY_ITEM_PRICE_COUNT];
    UInt8 IsRemoteShop;
    Int32 InventoryIndex[50];
)

CLIENT_PROTOCOL(S2C, BUY_ITEM, DEFAULT, 161,
    UInt32 Unknown1;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 Unknown2;
    UInt16 Unknown3;
    UInt16 Unknown4;
)

CLIENT_PROTOCOL(C2S, SELL_ITEM, DEFAULT, 162,
    UInt8 NpcIndex;
    UInt8 IsRemoteShop;
    UInt16 Unknown1;
    UInt8 Unknown2;
    UInt32 InventoryIndexCount;
    Int32 InventoryIndex[0];
)

CLIENT_PROTOCOL(S2C, SELL_ITEM, DEFAULT, 162,
    UInt64 Currency;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, RECOVER_ITEM, DEFAULT, 166,
    UInt8 RecoverySlotIndex;
    UInt32 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, RECOVER_ITEM, DEFAULT, 166,
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, GET_ITEM_RECOVERY_LIST, DEFAULT, 167,
)

CLIENT_PROTOCOL_STRUCT(S2C_ITEM_RECOVERY_LIST_SLOT,
    UInt64 ItemID;
    UInt64 ItemSerial;
    UInt64 ItemOption;
    UInt64 RecoveryPrice;
    UInt8 RecoverySlotIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
    Timestamp ExpirationTimestamp;
)

CLIENT_PROTOCOL(S2C, GET_ITEM_RECOVERY_LIST, DEFAULT, 167,
    UInt8 Count;
    S2C_ITEM_RECOVERY_LIST_SLOT Slots[0];
)

CLIENT_PROTOCOL(S2C, NFY_RECOVER_ITEM_EXPIRED, DEFAULT, 168,
    Int32 IsSlotExpired[RUNTIME_CHARACTER_MAX_RECOVERY_SLOT_COUNT];
)

CLIENT_PROTOCOL(C2S, WAREHOUSE_CURRENCY_DEPOSIT, DEFAULT, 173,
    UInt32 Unknown1;
    UInt32 Unknown2;
    Int64 Amount;
)

CLIENT_PROTOCOL(S2C, WAREHOUSE_CURRENCY_DEPOSIT, DEFAULT, 173,
	Bool Result;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_TO_MOB_TARGET,
    RTEntityID Entity;
    UInt8 EntityIDType;
    UInt16 Unknown2;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_MOB, DEFAULT, 174,
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt32 Unknown1;
    CSC_POSITION PositionSet;
    UInt8 Unknown2;
    UInt32 Unknown3;
    UInt8 TargetCount;
    CSC_POSITION PositionCharacter;
    CSC_POSITION PositionTarget;
    C2S_DATA_SKILL_TO_MOB_TARGET Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_TO_MOB_TARGET,
    RTEntityID Entity;
    UInt8 EntityIDType;
    UInt8 AttackType;
    UInt32 MobAppliedDamage;
    UInt32 MobTotalDamage;
    UInt32 MobAdditionalDamage;
    UInt64 MobHP;
    CSC_POSITION PositionSet;
    UInt32 BfxType;
    UInt32 BfxDuration;
    UInt8 Unknown3;
)

CLIENT_PROTOCOL(S2C, SKILL_TO_MOB, DEFAULT, 174,
    UInt16 SkillIndex;
    UInt64 CharacterHP;
    UInt32 CharacterMP;
    UInt16 CharacterSP;
    UInt64 AccumulatedExp;
    UInt64 AccumulatedOxp;
    UInt64 ReceivedMxp;
    UInt32 ReceivedSkillExp;
    UInt16 AP;
    UInt64 AXP;
    UInt8 Unknown2[20];
    Int32 Unknown4;
    UInt8 Unknown5;
    UInt64 CharacterMaxHP;
    UInt32 ReflectDamage;
    UInt8 TargetCount;
    S2C_DATA_SKILL_TO_MOB_TARGET Data[0];
)

/*

enum {
    RUNTIME_SKILL_GROUP_PASSIVE = 0,
    RUNTIME_SKILL_GROUP_ATTACK = 1,
    RUNTIME_SKILL_GROUP_ATTACK_UNKNOWN = 3,
    RUNTIME_SKILL_GROUP_MOVEMENT = 5,
    RUNTIME_SKILL_GROUP_UNKNOWN_13 = 13,
    RUNTIME_SKILL_GROUP_BUFF = 24,
    RUNTIME_SKILL_GROUP_DEBUFF = 25,
    RUNTIME_SKILL_GROUP_HEAL = 26,
    RUNTIME_SKILL_GROUP_ASTRAL = 31,
    RUNTIME_SKILL_GROUP_BATTLE_MODE = 32,
    RUNTIME_SKILL_GROUP_WING = 33,
    RUNTIME_SKILL_GROUP_VEHICLE = 38,
    RUNTIME_SKILL_GROUP_COMBO = 40,
    RUNTIME_SKILL_GROUP_UNKNOWN_42 = 42,
    RUNTIME_SKILL_GROUP_TOTEM = 43,
    RUNTIME_SKILL_GROUP_AGGRO = 44
};

*/

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_TO_CHARACTER_TARGET,
    RTEntityID Entity;
    UInt8 EntityIDType;
    UInt8 AttackType;
    UInt8 HitNum;
    Bool IgnoreDamage;
	UInt8 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_ATTACK,
    UInt32 Unknown1;
    CSC_POSITION PositionSet;
    UInt8 Unknown2;
    UInt8 TargetCount;
    CSC_POSITION PositionCharacter;
    CSC_POSITION PositionTarget;
    C2S_DATA_SKILL_TO_CHARACTER_TARGET Data[0];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_MOVEMENT,
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_ASTRAL,
    UInt16 IsActivation;
    UInt32 Unknown2;
    Int32 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_BATTLE_MODE,
    UInt16 IsActivation;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_VEHICLE,
    UInt16 IsActivation;
    Int32 Unknown1;
    Int32 Unknown2;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_CHARACTER, DEFAULT, 175,
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_TO_CHARACTER_TARGET,
    UInt32 CharacterIndex;
    UInt8 AttackType;
    UInt32 AppliedDamage;
    UInt32 TotalDamage;
    UInt32 AdditionalDamage;
    UInt64 HP;
    CSC_POSITION PositionSet;
    UInt32 BfxType;
    UInt32 BfxDuration;
    UInt32 Unknown3;
    UInt8 Unknown4;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_ATTACK,
    UInt64 CharacterHP;
    UInt32 CharacterMP;
    UInt16 CharacterSP;
    UInt64 AccumulatedExp;
    UInt64 AccumulatedOxp;
    UInt64 ReceivedMxp;
    UInt32 ReceivedSkillExp;
    UInt8 Unknown5;
    UInt64 CharacterMaxHP;
    UInt32 ReflectDamage;
    UInt8 TargetCount;
    S2C_DATA_SKILL_TO_CHARACTER_TARGET Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_MOVEMENT,
    UInt32 Unknown1;
    UInt32 CharacterMP;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_ASTRAL,
    UInt32 CurrentMP;
    UInt16 IsActivation;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_BATTLE_MODE,
    UInt32 CurrentMP;
    UInt16 IsActivation;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_VEHICLE,
    UInt32 CurrentMP;
    UInt16 IsActivation;
    Int32 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_WING,
    Int32 CurrentMP;
    Int32 CurrentSP;
)

CLIENT_PROTOCOL(S2C, SKILL_TO_CHARACTER, DEFAULT, 175,
    UInt16 SkillIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(C2S, ATTACK_TO_MOB, DEFAULT, 176,
    RTEntityID Entity;
    UInt8 EntityIDType;
    UInt8 MissMob;
)

CLIENT_PROTOCOL(S2C, ATTACK_TO_MOB, DEFAULT, 176,
    RTEntityID Entity;
    UInt8 EntityIDType;
    UInt64 CharacterHP;
    UInt32 CharacterMP;
    UInt16 CharacterSP;
    UInt8 AttackType;
    UInt64 AccumulatedExp;
    UInt64 AccumulatedOxp;
    UInt64 ReceivedSkillExp;
    UInt32 MobAppliedDamage;
    UInt32 MobTotalDamage;
    UInt32 MobAdditionalDamage;
    UInt64 MobHP;
    UInt32 Unknown1;
    UInt16 AP;
    UInt32 AXP;
    UInt8 Unknown2;
    UInt64 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(C2S, ATTACK_TO_CHARACTER, DEFAULT, 177,
    RTEntityID TargetID;
    UInt32 TargetIndex;
)

CLIENT_PROTOCOL(S2C, ATTACK_TO_CHARACTER, DEFAULT, 177,
    UInt32 TargetCharacterIndex;
    UInt64 CharacterHP;
    UInt32 CharacterMP;
    UInt16 CharacterSP;
    UInt8 AttackType;
    UInt32 AppliedDamage;
    UInt32 TotalDamage;
    UInt32 AdditionalDamage;
    UInt64 TargetHP;
    UInt8 Unknown1;
    UInt64 Unknown2;
    UInt64 CharacterHP2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_BEGIN, DEFAULT, 190,
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
    CSC_POSITION PositionCurrent;
    UInt16 WorldID;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_END, DEFAULT, 191,
    CSC_POSITION Position;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_CHANGE, DEFAULT, 192,
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
    CSC_POSITION PositionCurrent;
    UInt16 WorldID;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_WAYPOINTS, DEFAULT, 193,
    CSC_POSITION PositionCurrent;
    CSC_POSITION PositionNext;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_TILE_POSITION, DEFAULT, 194,
    CSC_POSITION PositionCurrent;
    CSC_POSITION PositionNext;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_MESSAGE_NEARBY_PAYLOAD,
    UInt16 MessageLength;
    Char Message[1];
)

CLIENT_PROTOCOL(C2S, MESSAGE_NEARBY, DEFAULT, 195,
    UInt32 Unknown1;
    UInt16 Unknown2;
    UInt16 PayloadLength;
    UInt8 Payload[0];
)

CLIENT_PROTOCOL(C2S, MESSAGE_PARTY, DEFAULT, 197,
    UInt16 PayloadLength;
    UInt8 Payload[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_GUILD,
    UInt8 GuildNameLength;
    Char GuildName[0]; // GuildNameLength
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT,
    UInt8 EquipmentSlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_PERSONAL_SHOP_MESSAGE,
    UInt8 MessageLength;
    Char Message[0];
    // UInt64 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_PERSONAL_SHOP_INFO,
    UInt64 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_BUFF_SLOT,
    UInt16 SkillIndex;
    UInt8 SkillLevel;
    UInt8 Unknown1[82];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_PET_INFO,
    UInt8 EvolutionLevel : 3; 
    UInt8 NameLength : 5;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_PET_SLOT,
    S2C_DATA_CHARACTERS_SPAWN_PET_INFO Info;
    UInt32 PetSlotIndex;
    Char PetName[0];
    // UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_BLESSING_BEAD_BALOON_SLOT,
    UInt8 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_BALOON_SLOT,
    RTItem Item;
    UInt8 SlotIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_INDEX,
    UInt32 CharacterIndex;
    RTEntityID Entity;
    UInt32 Level;
    UInt16 OverlordLevel;
    UInt32 HolyPower;
    UInt32 Rebirth;
    UInt32 MythLevel;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    UInt64 MaxHP;
    UInt64 CurrentHP;
    Int32 MaxRage;
    Int32 CurrentRage;
    UInt32 Unknown1;
    UInt32 Unknown2;
    UInt32 MovementSpeed;
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
    UInt16 PKState;
    UInt8 Nation;
    UInt32 Unknown3;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt16 Unknown4;
    UInt16 Unknown5;
    UInt8 CharacterExtendedStyle;
    UInt8 IsDead;
    UInt8 EquipmentSlotCount;
    UInt8 CostumeSlotCount;
    UInt8 IsInvisible;
    UInt8 IsPersonalShop;
    UInt32 GuildIndex;
    UInt16 Unknown6;
    UInt16 Unknown7;
    struct {
        UInt8 Unknown8 : 1;
        UInt8 Unknown9 : 1;
        UInt8 Unknown10 : 1;
        UInt8 HasPetInfo : 1;
        UInt8 PetCount : 2;
        UInt8 HasBlessingBeadBaloon : 1;
        UInt8 HasItemBallon : 1;
    };
    UInt8 ActiveBuffCount;
    UInt8 DebuffCount;
    UInt8 GmBuffCount;
    UInt8 PassiveBuffCount;
    UInt8 IsBringer;
    UInt32 Unknown12;
    UInt16 Unknown13;
    UInt16 Unknown14;
    UInt16 DisplayTitle;
    UInt16 EventTitle;
    UInt16 WarTitle;
    UInt16 AbilityTitle;
    UInt8 Unknown39[17];
    UInt8 NameLength;
    Char Name[0]; // Size: NameLength - 1
    // S2C_DATA_CHARACTERS_SPAWN_GUILD Guild;
    // S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT EquipmentSlots[EquipmentSlotCount];
    // S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT CostumeSlots[CostumeSlotCount];
    // S2C_DATA_CHARACTERS_SPAWN_PERSONAL_SHOP_MESSAGE PersonalShopMessage[IsPersonalShop];
    // S2C_DATA_CHARACTERS_SPAWN_PERSONAL_SHOP_INFO PersonalShopInfo[IsPersonalShop];
    // S2C_DATA_CHARACTERS_SPAWN_BUFF_SLOT Buffs[ActiveBuffCount + DebuffCount + GmBuffCount + PassiveBuffCount];
    // S2C_DATA_CHARACTERS_SPAWN_PET_SLOT Pets[PetCount];
    // S2C_DATA_CHARACTERS_SPAWN_BALOON_SLOT ItemBaloon[HasItemBaloon];
)

CLIENT_PROTOCOL(S2C, CHARACTERS_SPAWN, DEFAULT, 200,
    UInt8 Count;
    UInt8 SpawnType;
    //S2C_DATA_CHARACTERS_SPAWN_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, CHARACTERS_DESPAWN, DEFAULT, 201,
    UInt32 CharacterIndex;
    UInt8 DespawnType;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_ENTITY_SPAWN_TYPE_LIST = 0,
    S2C_DATA_ENTITY_SPAWN_TYPE_INIT = 0x30,
    S2C_DATA_ENTITY_SPAWN_TYPE_WARP,
    S2C_DATA_ENTITY_SPAWN_TYPE_MOVE,
    S2C_DATA_ENTITY_SPAWN_TYPE_RESURRECT
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_MOBS_SPAWN_INDEX,
    RTEntityID Entity;
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
    UInt16 MobSpeciesIndex;
    UInt64 MaxHP;
    UInt64 CurrentHP;
    UInt8 IsChasing;
    UInt8 Level;
    UInt8 UnknownNation;
    UInt8 Unknown1;
    UInt16 UnknownAnimationID;
    UInt32 UnknownAnimationTickCount;
    UInt8 Unknown2;
    UInt8 UnknownEvent;
    UInt8 Unknown3;
    UInt32 UnknownCharacterIndex;
    UInt8 Unknown4[12];
    UInt8 Unknown5[22];
)

CLIENT_PROTOCOL(S2C, MOBS_SPAWN, DEFAULT, 202,
    UInt8 Count;
    S2C_DATA_MOBS_SPAWN_INDEX Data[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_ENTITY_DESPAWN_TYPE_DEINIT = 12,
    S2C_DATA_ENTITY_DESPAWN_TYPE_DEAD = 0x10,
    S2C_DATA_ENTITY_DESPAWN_TYPE_WARP,
    S2C_DATA_ENTITY_DESPAWN_TYPE_UNKNOWN1,
    S2C_DATA_ENTITY_DESPAWN_TYPE_RETURN,
    S2C_DATA_ENTITY_DESPAWN_TYPE_DISAPPEAR,
    S2C_DATA_ENTITY_DESPAWN_TYPE_NOTIFY_DEAD,
    S2C_DATA_ENTITY_DESPAWN_TYPE_UNKNOWN2 = 22,
)

CLIENT_PROTOCOL(S2C, MOBS_DESPAWN, DEFAULT, 203,
    RTEntityID Entity;
    UInt8 DespawnType;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_NFY_SPAWN_ITEM_CONTEXT_TYPE_NONE = 0,
    S2C_DATA_NFY_SPAWN_ITEM_CONTEXT_TYPE_MOBS = 1,
    S2C_DATA_NFY_SPAWN_ITEM_CONTEXT_TYPE_USER = 2,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SPAWN_ITEM_INDEX,
    RTEntityID Entity;
    UInt64 ItemOptions;
    UInt32 Unknown1;
    UInt32 SourceIndex;
    UInt64 ItemID;
    UInt16 X;
    UInt16 Y;
    UInt16 UniqueKey;
    UInt8 ContextType;
    RTItemProperty ItemProperty;
)

CLIENT_PROTOCOL(S2C, NFY_SPAWN_ITEM, DEFAULT, 204,
    UInt8 Count;
    S2C_DATA_NFY_SPAWN_ITEM_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, NFY_DESPAWN_ITEM, DEFAULT, 205,
    RTEntityID Entity;
    UInt8 DespawnType;
)

CLIENT_PROTOCOL(S2C, NFY_EQUIP_ITEM, DEFAULT, 206,
    UInt32 CharacterIndex;
    RTItem Item;
    UInt64 ItemOptions;
    UInt16 EquipmentSlotIndex;
    UInt32 Unknown1;
    UInt8 Unknown2;
)

CLIENT_PROTOCOL(S2C, NFY_UNEQUIP_ITEM, DEFAULT, 207,
    UInt32 CharacterIndex;
    UInt16 EquipmentSlotIndex;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_BEGIN, DEFAULT, 210,
    UInt32 CharacterIndex;
    UInt32 TickCount;
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_END, DEFAULT, 211,
    UInt32 CharacterIndex;
    CSC_POSITION Position;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_CHANGE, DEFAULT, 212,
    UInt32 CharacterIndex;
    UInt32 TickCount;
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_MOVEMENT_BEGIN, DEFAULT, 213,
    RTEntityID Entity;
    UInt32 TickCount;
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_MOVEMENT_END, DEFAULT, 214,
    RTEntityID Entity;
    CSC_POSITION Position;
)

CLIENT_PROTOCOL(S2C, MOB_CHASE_BEGIN, DEFAULT, 215,
    RTEntityID Entity;
    UInt32 TickCount;
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_CHASE_END, DEFAULT, 216,
    RTEntityID Entity;
    CSC_POSITION Position;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_MESSAGE_NEARBY_PAYLOAD,
    UInt16 MessageLength;
    Char Message[1];
)

CLIENT_PROTOCOL(S2C, MESSAGE_NEARBY, DEFAULT, 217,
    UInt32 CharacterIndex;
    UInt8 Nation;
    UInt16 PayloadLength;
    UInt8 Payload[0];
)

CLIENT_PROTOCOL(S2C, MESSAGE_SHOUT, DEFAULT, 2232,
    UInt8 Unknown1;
    UInt32 Padding;
)

CLIENT_PROTOCOL(S2C, NFY_MESSAGE_LOUD, DEFAULT, 394,
    UInt32 CharacterIndex;
    UInt8 Nation;
    UInt8 NameLength;
    UInt16 Unknown1;
    //Char *Name;  // Dynamic player name
    //UInt16 MessageLength;
    //UInt8 Payload[0]; // Chat message
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_TO_MOB_TARGET,
    RTEntityID Entity;
    UInt32 EntityIDType;
    UInt8 AttackType;
    UInt64 MobHP;
    UInt32 BfxType;
    UInt32 BfxDuration;
    UInt8 Unknown2;
    UInt32 Unknown3;
    CSC_POSITION PositionSet;
)

CLIENT_PROTOCOL(S2C, NFY_SKILL_TO_MOB, DEFAULT, 220,
    UInt16 SkillIndex;
    UInt8 TargetCount;
    UInt32 CharacterIndex;
    CSC_POSITION PositionSet;
    UInt8 Unknown1;
    UInt64 CharacterHP;
    UInt32 Shield;
    S2C_DATA_NFY_SKILL_TO_MOB_TARGET Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_TO_CHARACTER_TARGET,
    UInt32 CharacterIndex;
    UInt8 AttackType;
    UInt64 HP;
    UInt32 BfxType;
    UInt32 BfxDuration;
    UInt8 Unknown2;
    UInt32 Unknown3;
    CSC_POSITION PositionSet;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_GROUP_ATTACK,
    UInt8 TargetCount;
    UInt32 CharacterIndex;
    CSC_POSITION PositionSet;
    UInt8 HasReflection;
    UInt64 CharacterHP;
    UInt32 Shield;
    S2C_DATA_NFY_SKILL_TO_CHARACTER_TARGET Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_GROUP_MOVEMENT,
    UInt32 CharacterIndex;
    RTEntityID Entity;
    CSC_POSITION PositionEnd;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_GROUP_ASTRAL_WEAPON, 
    UInt32 CharacterIndex;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt8 CharacterExtendedStyle;
    UInt16 IsActivation;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_GROUP_BATTLE_MODE,
    UInt32 CharacterIndex;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt8 CharacterExtendedStyle;
    UInt16 IsActivation;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_GROUP_VEHICLE,
    UInt32 CharacterIndex;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt8 CharacterExtendedStyle;
    UInt16 IsActivation;
    Int32 Unknown1;
)

CLIENT_PROTOCOL(S2C, NFY_SKILL_TO_CHARACTER, DEFAULT, 221,
    UInt16 SkillIndex;
)

CLIENT_PROTOCOL(S2C, NFY_SKILLTOMTON, DEFAULT, 222,
    UInt16 SkillIndex;
    UInt8  Unknown[24];
    UInt32 Damage;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, NFY_ATTACK_TO_MOB, DEFAULT, 225,
    UInt8 AttackType;
    UInt32 CharacterIndex;
    RTEntityID Mob;
    UInt8 MobIDType;
    UInt64 MobHP;
    UInt8 Unknown1;
    UInt64 CharacterHP;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, NFY_ATTACK_TO_CHARACTER, DEFAULT, 226,
    UInt32 CharacterIndex;
    UInt32 TargetIndex;
    UInt8 AttackType;
    UInt64 TargetHP;
    UInt8 Unknown1;
    UInt64 Unknown2;
    UInt64 CharacterHP;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL(S2C, NFY_ATTACK_TO_SELF, DEFAULT, 227,
    Int32 Damage;
)

CLIENT_PROTOCOL(S2C, NFY_MESSAGE_PARTY, DEFAULT, 236,
    UInt32 CharacterIndex;
    UInt16 PayloadLength;
    UInt8 Payload[0];
)

CLIENT_PROTOCOL(C2S, WARP, DEFAULT, 244,
    UInt8 NpcID;
    UInt16 Unknown1;
    UInt16 SlotIndex;
    UInt16 Unknown2;
    UInt8 WarpPositionY;
    UInt8 WarpPositionX;
    UInt8 WarpWorldID;
    UInt8 Unknown3[15];
    UInt16 WarpIndex;
    Int32 IsElite;
)

CLIENT_PROTOCOL(S2C, WARP, DEFAULT, 244,
    CSC_POSITION Position;
    UInt64 AccumulatedExp;
    UInt64 AccumulatedOxp;
    UInt64 ReceivedSkillExp;
    UInt64 Currency;
    RTEntityID Entity;
    UInt8 WorldType;
    UInt32 Unknown2;
    UInt32 WorldID;
    UInt32 DungeonID;
    UInt64 Result;
)

CLIENT_PROTOCOL(C2S, GET_SERVER_POSITION, DEFAULT, 246,
)

CLIENT_PROTOCOL(S2C, GET_SERVER_POSITION, DEFAULT, 246,
    Int32 PositionCurrentX;
    Int32 PositionCurrentY;
)

CLIENT_PROTOCOL(C2S, TRADE_REQUEST, DEFAULT, 260,
    UInt32 TargetIndex;
    RTEntityID TargetID;
)

CLIENT_PROTOCOL(C2S, TRADE_REQUEST_ACCEPT, DEFAULT, 261,
    UInt8 EventType;
)

CLIENT_PROTOCOL(S2C, TRADE_REQUEST_ACCEPT, DEFAULT, 261,
    UInt8 EventType;
)

CLIENT_PROTOCOL(C2S, TRADE_CLOSE, DEFAULT, 262,
    UInt8 EventType;
)

CLIENT_PROTOCOL(S2C, TRADE_CLOSE, DEFAULT, 262,
    UInt8 EventType;
)

CLIENT_PROTOCOL_ENUM(
    S2C_TRADE_EVENT_TRADE_LIMIT_REACHED = 1,
    S2C_TRADE_EVENT_TRADE_END_ACK       = 3,
    S2C_TRADE_EVENT_FAIL_TO_CONTACT     = 30,
    S2C_TRADE_EVENT_CANNOT_REQUEST      = 33,
    S2C_TRADE_EVENT_REQUEST_SUCCESS     = 34,
    S2C_TRADE_EVENT_READY_TO_TRADE      = 35,
    S2C_TRADE_EVENT_INVENTORY_FULL      = 36,
    S2C_TRADE_EVENT_TRADE_NOT_ALLOWED   = 37,
    S2C_TRADE_EVENT_REQUEST_ITEM_LIST   = 38,
    S2C_TRADE_EVENT_TRADE_END           = 39,
    S2C_TRADE_EVENT_ADD_ITEM            = 40,
    S2C_TRADE_EVENT_TRADE_REJECTED      = 42,
    S2C_TRADE_EVENT_CANCEL_SUBMIT       = 43,
    S2C_TRADE_EVENT_CANCEL_TRADE_END    = 44,
    S2C_TRADE_EVENT_REQUEST_ACCEPT      = 45,
    S2C_TRADE_EVENT_REQUEST_DECLINE     = 46,
    S2C_TRADE_EVENT_CANCEL_TRADE        = 47,
)

CLIENT_PROTOCOL(S2C, TRADE_EVENT, DEFAULT, 263,
    UInt32 CharacterIndex;
    UInt8 EventType;
    UInt16 Unknown2;
)

CLIENT_PROTOCOL(C2S, TRADE_ADD_ITEMS, DEFAULT, 264,
    UInt8 InventorySlotCount;
    UInt16 FromInventorySlotIndex[RUNTIME_INVENTORY_PAGE_SIZE];
    UInt16 ToInventorySlotIndex[RUNTIME_INVENTORY_PAGE_SIZE];
)

CLIENT_PROTOCOL(S2C, TRADE_ADD_ITEMS, DEFAULT, 264,
    UInt8 EventType;
)

CLIENT_PROTOCOL(C2S, TRADE_ADD_CURRENCY, DEFAULT, 265,
    UInt64 Currency;
)

CLIENT_PROTOCOL(S2C, TRADE_ADD_CURRENCY, DEFAULT, 265,
    UInt8 EventType;
)

CLIENT_PROTOCOL(C2S, TRADE_INVENTORY_SLOT_LIST, DEFAULT, 269,
    UInt8 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_COMBO_SKILL_SET_RESULT_SUCCESS = 0,
    S2C_DATA_COMBO_SKILL_SET_RESULT_FAILURE = 1,
)

CLIENT_PROTOCOL(S2C, NFY_COMBO_SKILL_SET, DEFAULT, 273,
    UInt8 Result;
    UInt32 CurrentMP;
)

CLIENT_PROTOCOL(C2S, COMBO_SKILL_EVENT, DEFAULT, 274,
    UInt8 SkillSlotIndex;
)

CLIENT_PROTOCOL(S2C, NFY_COMBO_SKILL_EVENT, DEFAULT, 275,
    UInt32 CharacterIndex;
    UInt8 CharacterExtendedStyle;
)

CLIENT_PROTOCOL(S2C, BATTLE_RANK_UP, DEFAULT, 276,
    UInt8 Level;
)

CLIENT_PROTOCOL(C2S, ADD_FORCE_SLOT_OPTION, DEFAULT, 280,
    Int32 ItemSlotIndex;
    Int32 RandomScrollSlotIndex;
    Int32 OptionScrollSlotIndex;
    Int32 Unknown2;
    UInt8 ForceCoreCount;
    Int32 ForceCoreSlotIndices[10];
)

CLIENT_PROTOCOL(S2C, ADD_FORCE_SLOT_OPTION, DEFAULT, 280,
    UInt8 Result;
    UInt64 ItemOptions;
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(C2S, QUEST_BEGIN, DEFAULT, 282,
    UInt16 QuestID;
    UInt8 SlotID;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, QUEST_BEGIN, DEFAULT, 282,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, QUEST_CLEAR, DEFAULT, 283,
    UInt16 QuestIndex;
    UInt16 QuestSlotIndex;
    UInt16 SlotIndex[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_QUEST_CLEAR_RESULT_TYPE_UNKNOWN,
    S2C_DATA_QUEST_CLEAR_RESULT_TYPE_SUCCESS = 4
)

CLIENT_PROTOCOL(S2C, QUEST_CLEAR, DEFAULT, 283,
    UInt8 Result;
    UInt16 ResultType;
    UInt32 Unknown1;
    UInt64 RewardExp;
)

CLIENT_PROTOCOL(C2S, QUEST_CANCEL, DEFAULT, 284,
    UInt16 QuestID;
    UInt8 SlotID;
    UInt8 QuestDungeonCount;
    UInt8 AddItemCount;
    UInt8 DeleteItemCount;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, QUEST_CANCEL, DEFAULT, 284,
)

CLIENT_PROTOCOL(C2S, USE_ITEM, DEFAULT, 285,
    UInt16 InventorySlotIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_USE_ITEM_RESULT_SUCCESS = 0,
    S2C_DATA_USE_ITEM_RESULT_FAILED = 1,
    S2C_DATA_USE_ITEM_RESULT_IS_DEAD = 17,
    S2C_DATA_USE_ITEM_RESULT_IS_NOT_DEAD = 18,
    S2C_DATA_USE_ITEM_RESULT_REJECTED = 19,
    S2C_DATA_USE_ITEM_RESULT_GUILD_INFO_ERROR = 20,
    S2C_DATA_USE_ITEM_RESULT_ITEM_EXPIRED = 21,
    S2C_DATA_USE_ITEM_RESULT_COOLDOWN_1 = 22,
    S2C_DATA_USE_ITEM_RESULT_COOLDOWN_2 = 23,
    S2C_DATA_USE_ITEM_RESULT_COOLDOWN_3 = 24,
    S2C_DATA_USE_ITEM_RESULT_WAR_USAGE_LIMIT = 25,
    S2C_DATA_USE_ITEM_RESULT_WAR_USAGE = 26,
    S2C_DATA_USE_ITEM_RESULT_DUNGEON_USAGE_LIMIT = 27,
)

CLIENT_PROTOCOL(S2C, USE_ITEM, DEFAULT, 285,
    UInt8 Result;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_CHARACTER_UPDATE_TYPE_HPPOTION         = 1,
    S2C_DATA_CHARACTER_UPDATE_TYPE_MPPOTION         = 2,
    S2C_DATA_CHARACTER_UPDATE_TYPE_HP               = 3,
    S2C_DATA_CHARACTER_UPDATE_TYPE_MP               = 4,
    S2C_DATA_CHARACTER_UPDATE_TYPE_SP               = 5,
    S2C_DATA_CHARACTER_UPDATE_TYPE_SP_INCREASE      = 6,
    S2C_DATA_CHARACTER_UPDATE_TYPE_SP_DECREASE      = 7,
    S2C_DATA_CHARACTER_UPDATE_TYPE_EXP              = 8,
    S2C_DATA_CHARACTER_UPDATE_TYPE_RANK             = 9,
    S2C_DATA_CHARACTER_UPDATE_TYPE_LEVEL            = 10,
    S2C_DATA_CHARACTER_UPDATE_TYPE_SP_DECREASE_EX   = 11,
    S2C_DATA_CHARACTER_UPDATE_TYPE_BUFF_POTION      = 12,
    S2C_DATA_CHARACTER_UPDATE_TYPE_REPUTATION       = 13,
    S2C_DATA_CHARACTER_UPDATE_TYPE_GUIDITEMFX       = 14,
    S2C_DATA_CHARACTER_UPDATE_TYPE_RESURRECTION     = 15,
    S2C_DATA_CHARACTER_UPDATE_TYPE_PENALTY_EXP      = 16,
    S2C_DATA_CHARACTER_UPDATE_TYPE_DAMAGE_CELL      = 17,
    S2C_DATA_CHARACTER_UPDATE_TYPE_DEFFICIENCY      = 18,
    S2C_DATA_CHARACTER_UPDATE_TYPE_AUTH_HP_POTION   = 19,
    S2C_DATA_CHARACTER_UPDATE_TYPE_RAGE             = 20,

    S2C_DATA_CHARACTER_UPDATE_TYPE_OVERLORD_LEVEL   = 22,
    S2C_DATA_CHARACTER_UPDATE_TYPE_HONOR_MEDAL      = 23,

    S2C_DATA_CHARACTER_UPDATE_TYPE_BP               = 24,
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_DATA, DEFAULT, 287,
    UInt8 Type;

    union {
        struct { UInt8 _0[34]; UInt64 HPAfterPotion; UInt8 _1[4]; };
        struct { UInt8 _2[38]; UInt64 HP; };
        struct { UInt8 _3[34]; UInt32 MP; UInt8 _4[8]; };
        struct { UInt8 _5[34]; UInt32 SP; UInt8 _6[8]; };
        struct { UInt8 _7[26]; UInt32 MapCode; UInt32 WarpCode; UInt32 Level; UInt8 _8[8]; };
        struct { UInt8 _9[34]; UInt32 Rage; UInt8 _10[8]; };
        struct { UInt8 _11[34]; UInt32 BP; UInt8 _12[8]; };
        struct { UInt8 _13[34]; UInt32 BuffResult; UInt8 _14[8]; };
        struct { UInt8 _15[34]; UInt32 SkillRank; UInt8 _16[8]; };
        struct { UInt8 _17[22]; UInt32 HonorMedalGrade; UInt8 _18[8]; Int64 HonorPoints; UInt8 _19[4]; };
    };
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_CHARACTER_EVENT_TYPE_LEVELUP = 1,
    S2C_DATA_CHARACTER_EVENT_TYPE_RANKUP = 2,
    S2C_DATA_CHARACTER_EVENT_TYPE_OVERLORD_LEVELUP = 3,
    S2C_DATA_CHARACTER_EVENT_TYPE_UNKNOWN = 4,
    S2C_DATA_CHARACTER_EVENT_TYPE_FORCEWING_GRADEUP = 6,
    S2C_DATA_CHARACTER_EVENT_TYPE_FORCEWING_LEVELUP = 7,
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_EVENT, DEFAULT, 288,
    UInt8 Type;
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_START, DEFAULT, 290,
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_START, DEFAULT, 290,
    UInt32 Active;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_END, DEFAULT, 291,
    Int16 Unknown1;
    Int16 Unknown2;
    Int8 Unknown3;
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_END, DEFAULT, 291,
    UInt32 Unknown1;
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_SPAWN, DEFAULT, 292,
    UInt8 IsActive;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_GATE_OPEN, DEFAULT, 293,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_GATE_OPEN, DEFAULT, 293,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_END, DEFAULT, 294,
    UInt32 CharacterIndex;
    UInt16 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, QUEST_DELETE, DEFAULT, 295,
    UInt16 QuestIndex;
)

CLIENT_PROTOCOL(S2C, QUEST_DELETE, DEFAULT, 295,
    UInt16 QuestIndex;
)

CLIENT_PROTOCOL(C2S, QUEST_RESTORE, DEFAULT, 296,
    UInt16 QuestIndex;
)

CLIENT_PROTOCOL(S2C, QUEST_RESTORE, DEFAULT, 296,
    UInt16 QuestIndex;
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_CLEAR_MOBS, DEFAULT, 300,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_SPAWN, DEFAULT, 301,
    UInt32 DungeonTimeout1;
    UInt32 DungeonTimeout2;
    UInt32 Unknown1;
    UInt32 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_302, DEFAULT, 302,
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_PATTERN_PART_COMPLETED, DEFAULT, 303,
    UInt8 PatternPartIndex;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_ACTION, DEFAULT, 310,
    UInt32 TargetIndex;
    UInt16 ActionIndex;
    UInt8 X;
    UInt8 Y;
)

CLIENT_PROTOCOL(S2C, SKILL_TO_ACTION, DEFAULT, 311,
    UInt32 CharacterIndex;
    UInt32 TargetIndex;
    UInt16 ActionIndex;
    UInt8 X;
    UInt8 Y;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_QUEST_ACTION_INDEX,
    UInt32 ActionIndex;
    UInt16 SlotIndex;
)

CLIENT_PROTOCOL(C2S, QUEST_ACTION, DEFAULT, 320,
    UInt16 QuestID;
    UInt32 NpcSetID;
    UInt8 SlotIndex;
    UInt8 ActionCount;
    C2S_DATA_QUEST_ACTION_INDEX Actions[0];
)

CLIENT_PROTOCOL(S2C, QUEST_ACTION, DEFAULT, 320,
    UInt16 QuestID;
    UInt16 NpcFlags;
    UInt32 NpcSetID;
)

CLIENT_PROTOCOL(C2S, ATTACK_BOSS_MOB, DEFAULT, 321,
    RTEntityID Entity;
    // TODO: There is some appendix data here...
)

CLIENT_PROTOCOL(S2C, ATTACK_BOSS_MOB, DEFAULT, 321,
)

CLIENT_PROTOCOL(C2S, CHANGE_STYLE, DEFAULT, 322,
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt32 GuildID;
    UInt32 UnknownFlags;
    UInt8 GuildNameLength;
    Char GuildName[0];
)

CLIENT_PROTOCOL(S2C, CHANGE_STYLE, DEFAULT, 322,
    UInt8 Success;
)

CLIENT_PROTOCOL(S2C, NFY_CHANGE_STYLE, DEFAULT, 323,
    UInt32 CharacterIndex;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt32 GuildID;
    UInt32 UnknownFlags;
    UInt8 GuildNameLength;
    Char GuildName[0];
)

CLIENT_PROTOCOL(C2S, GET_PREMIUM_SERVICE, DEFAULT, 324,
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_NONE           = 0,
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_FREE_IP        = (1 << 0),
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_FREE_TIME      = (1 << 1),
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_FREE_RATE      = (1 << 2),
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_FIXED_RATE     = (1 << 3),
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_PC_ROOM        = (1 << 4),
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_TIME_RATE      = (1 << 5),
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_PC_ROOM_2      = (1 << 6),
    S2C_DATA_PREMIUM_SERVICE_CHARGE_TYPE_PC_ROOM_BEAD   = (1 << 7),
)

CLIENT_PROTOCOL(S2C, GET_PREMIUM_SERVICE, DEFAULT, 324,
    UInt32 ChargeType;
    UInt32 ServiceType;
    Timestamp ExpirationDate;
    UInt32 Unknown2;
    Timestamp UnknownExpirationDate;
)

CLIENT_PROTOCOL(S2C, NFY_PREMIUM_SERVICE, DEFAULT, 325,
    UInt32 ChargeType;
    UInt32 ServiceType;
    Timestamp ExpirationDate;
    UInt32 Unknown2;
    Timestamp UnknownExpirationDate;
)

CLIENT_PROTOCOL(C2S, PVP_REQUEST, DEFAULT, 330,
    RTEntityID TargetID;
    UInt32 TargetIndex;
    UInt8 PvpType;
)

CLIENT_PROTOCOL(S2C, PVP_REQUEST, DEFAULT, 330,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, PVP_RESPONSE, DEFAULT, 332,
    RTEntityID TargetID;
    UInt32 TargetIndex;
    UInt8 PvpType;
    UInt8 PvpResult;
)

CLIENT_PROTOCOL(S2C, PVP_RESPONSE, DEFAULT, 332,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PVP_REQUEST_ACK, DEFAULT, 333,
    UInt32 CharacterIndex;
    UInt8 Unknown1;
    UInt8 Unknown2;
)

CLIENT_PROTOCOL(C2S, PVP_REQUEST_END, DEFAULT, 334,
    UInt8 PvpType;
    UInt8 PvpResult;
)

CLIENT_PROTOCOL(S2C, PVP_REQUEST_END, DEFAULT, 334,
    UInt8 PvpResult;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_REPUTATION_CHANGE_TYPE_AUTOTRAP_HIT = 0x2,
    S2C_DATA_REPUTATION_CHANGE_TYPE_GUILD_WIN = 0x3,
    S2C_DATA_REPUTATION_CHANGE_TYPE_NATION_KILL = 0x4,
    S2C_DATA_REPUTATION_CHANGE_TYPE_MOBS_KILL = 0x5,
    S2C_DATA_REPUTATION_CHANGE_TYPE_GUILD_KILL = 0x6,
    S2C_DATA_REPUTATION_CHANGE_TYPE_UNKNOWN7 = 0x7,
)

CLIENT_PROTOCOL(C2S, PKREQUEST, DEFAULT, 341,
    UInt16 SessionId; 
    UInt16 U0;
    UInt32 CharacterId;
    UInt8 PkType;
)

CLIENT_PROTOCOL(S2C, PKREQUEST, DEFAULT, 341,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PKREQUEST, DEFAULT, 342,
    UInt32 CharacterId;
)

CLIENT_PROTOCOL(C2S, PKENDING, DEFAULT, 343,
    UInt16 SessionId;
    UInt16 U0;
    UInt32 CharacterId;
    UInt8 Result;
    UInt8 PkType;
)

CLIENT_PROTOCOL(S2C, PKENDING, DEFAULT, 343,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PKENDING, DEFAULT, 344,
    UInt32 CharacterId;
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, REPUTATION_CHANGE, DEFAULT, 346,
    UInt8 Type;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, REPUTATION_CHANGE, DEFAULT, 346,
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, NFY_REPUTATION_CHANGE, DEFAULT, 347,
    UInt8 Reason;
    UInt64 HonorPoints;
)

CLIENT_PROTOCOL(C2S, SELECT_TARGET_CHARACTER, DEFAULT, 350,
    RTEntityID TargetID;
)

CLIENT_PROTOCOL(S2C, SELECT_TARGET_CHARACTER, DEFAULT, 350,
    UInt8 Unknown1;
    UInt64 CurrentHP;
    UInt64 MaxHP;
    UInt64 AbsorbHP;
)

CLIENT_PROTOCOL(C2S, DESELECT_TARGET_CHARACTER, DEFAULT, 352,
    RTEntityID TargetID;
)

CLIENT_PROTOCOL(S2C, NFY_TARGET_CHARACTER_INFO, DEFAULT, 353,
    UInt32 CharacterIndex;
    UInt64 CurrentHP;
    UInt64 MaxHP;
    UInt64 AbsorbHP;
)

CLIENT_PROTOCOL_ENUM(
    C2S_PERSONAL_SHOP_OPEN_MODE_SELL = 1,
    C2S_PERSONAL_SHOP_OPEN_MODE_BUY  = 2,
)

CLIENT_PROTOCOL(C2S, PERSONAL_SHOP_OPEN, DEFAULT, 360,
    UInt8 ShopMode;
    struct _RTPersonalShopInfo ShopInfo;
    struct _RTPersonalShopSlot ShopSlots[0];
    // Char* ShopName;
)

CLIENT_PROTOCOL(S2C, PERSONAL_SHOP_OPEN, DEFAULT, 360,
    UInt8 Result;
    UInt64 PremiumShopItemID;
)

CLIENT_PROTOCOL(C2S, PERSONAL_SHOP_CLOSE, DEFAULT, 362,
)

CLIENT_PROTOCOL(S2C, PERSONAL_SHOP_CLOSE, DEFAULT, 362,
)

CLIENT_PROTOCOL(C2S, PERSONAL_SHOP_VIEW, DEFAULT, 364,
    RTEntityID EntityID;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PERSONAL_SHOP_VIEW_SLOT,
    UInt8 ShopSlotIndex;
    UInt64 ItemPrice;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 ItemDuration;
    UInt64 ItemSerial;
)

CLIENT_PROTOCOL(S2C, PERSONAL_SHOP_VIEW, DEFAULT, 364,
    UInt8 Result;
    UInt8 SlotCount;
    // S2C_DATA_PERSONAL_SHOP_VIEW_SLOT ShopSlots[0];
)

CLIENT_PROTOCOL(C2S, PERSONAL_SHOP_EXIT, DEFAULT, 365,
    RTEntityID EntityID;
)

CLIENT_PROTOCOL(S2C, PERSONAL_SHOP_EXIT, DEFAULT, 365,
)

CLIENT_PROTOCOL(C2S, PERSONAL_SHOP_BUY, DEFAULT, 368,
    RTEntityID EntityID;
    UInt64 ItemID;
    UInt16 InventorySlotIndex;
    UInt8 ShopSlotIndex;
    UInt64 ItemPrice;
)

CLIENT_PROTOCOL_ENUM(
    S2C_PERSONAL_SHOP_BUY_RESULT_ALREADY_CLOSED = 1,
    S2C_PERSONAL_SHOP_BUY_RESULT_ALREADY_SOLD = 2,
    S2C_PERSONAL_SHOP_BUY_RESULT_ITEM_EXPIRED = 3,
    S2C_PERSONAL_SHOP_BUY_RESULT_FAIL = 4,
    S2C_PERSONAL_SHOP_BUY_RESULT_WRONG_ITEM_PRICE = 5,
)

CLIENT_PROTOCOL(S2C, PERSONAL_SHOP_BUY, DEFAULT, 368,
    UInt8 Result;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_PARTY_QUEST_BEGIN_RESULT_TYPE_SUCCESS = 0,
    S2C_DATA_PARTY_QUEST_BEGIN_RESULT_TYPE_ALREADY_OPENED = 1,
    S2C_DATA_PARTY_QUEST_BEGIN_RESULT_TYPE_ANTI_ADDICT = 2,
)

CLIENT_PROTOCOL(C2S, PARTY_QUEST_BEGIN, DEFAULT, 371,
    UInt16 QuestID;
    UInt8 QuestSlotIndex;
    Int16 QuestItemSlotIndex;
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_BEGIN, DEFAULT, 371,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_BEGIN, DEFAULT, 375,
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_PARTY_QUEST_CLOSE_RESULT_TYPE_SUCCESS = 0,
    S2C_DATA_PARTY_QUEST_CLOSE_RESULT_TYPE_ALREADY_CLOSED = 1,
    S2C_DATA_PARTY_QUEST_CLOSE_RESULT_TYPE_ALREADY_IN_USE_ITEM_SLOT = 2,
    S2C_DATA_PARTY_QUEST_CLOSE_RESULT_TYPE_ALREADY_IN_USE_TEMP_SLOT = 2,
)

CLIENT_PROTOCOL(C2S, PARTY_QUEST_CLOSE, DEFAULT, 372,
    UInt16 QuestID;
    UInt8 QuestSlotIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_CLOSE, DEFAULT, 372,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_CLOSE, DEFAULT, 376,
    UInt16 QuestID;
    UInt8 QuestSlotIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_PARTY_QUEST_ACTION_RESULT_TYPE_SUCCESS = 0,
    S2C_DATA_PARTY_QUEST_ACTION_RESULT_TYPE_ALREADY_COMPLETED = 1,
    S2C_DATA_PARTY_QUEST_ACTION_RESULT_TYPE_ALREADY_IN_USE_ITEM_SLOT = 2,
    S2C_DATA_PARTY_QUEST_ACTION_RESULT_TYPE_EMPTY_ITEM_SLOT = 3,
    S2C_DATA_PARTY_QUEST_ACTION_RESULT_TYPE_NOT_ENOUGH_CURRENCY = 4,
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_PARTY_QUEST_ACTION_INDEX,
    UInt32 ActionIndex;
    UInt16 SlotIndex;
)

CLIENT_PROTOCOL(C2S, PARTY_QUEST_ACTION, DEFAULT, 373,
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
    UInt32 NpcSetID;
    UInt8 ActionCount;
    C2S_DATA_PARTY_QUEST_ACTION_INDEX Actions[0];
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_ACTION, DEFAULT, 373,
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
    UInt16 NpcFlags;
    UInt32 NpcSetID;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_ACTION, DEFAULT, 377,
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
    UInt16 NpcFlags;
    UInt32 NpcSetID;
)

CLIENT_PROTOCOL(C2S, PARTY_QUEST_LOOT_ITEM, DEFAULT, 374,
    RTEntityID Entity;
    UInt16 UniqueKey;
    UInt64 ItemID;
    Int16 QuestItemSlotIndex;
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_LOOT_ITEM, DEFAULT, 374,
    UInt8 Result;
    UInt64 ItemID;
    UInt64 ItemOptions;
    Int16 QuestItemSlotIndex;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_LOOT_ITEM, DEFAULT, 378,
    UInt64 ItemID;
    UInt64 ItemOptions;
    Int16 QuestItemSlotIndex;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_MISSION_MOB_KILL, DEFAULT, 379,
    UInt16 QuestID;
    UInt16 MobSpeciesIndex;
)

CLIENT_PROTOCOL(C2S, PERSONAL_SHOP_REGISTER_ITEM, DEFAULT, 380,
    RTEntityID EntityID;
    UInt16 InventorySlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt8 ShopSlotIndex;
    UInt64 ItemPrice;
)

CLIENT_PROTOCOL(S2C, PERSONAL_SHOP_REGISTER_ITEM, DEFAULT, 380,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, MOB_SPECIAL_BUFF, DEFAULT, 390,
    RTEntityID MobID;
    UInt64 CurrentHP;
    UInt32 ReceivedDamage;
)

CLIENT_PROTOCOL(C2S, CHANGE_DIRECTION, DEFAULT, 391,
    Float32 Direction;
)

CLIENT_PROTOCOL(S2C, CHANGE_DIRECTION, DEFAULT, 392,
    UInt32 CharacterIndex;
    Float32 Direction;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_BEGIN, DEFAULT, 401,
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_BEGIN, DEFAULT, 403,
    UInt32 CharacterIndex;
    UInt32 TickCount;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_END, DEFAULT, 402,
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_END, DEFAULT, 404,
    UInt32 CharacterIndex;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_CHANGE, DEFAULT, 405,
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_CHANGE, DEFAULT, 406,
    UInt32 CharacterIndex;
    UInt32 TickCount;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(C2S, EXTRACT_ITEM, DEFAULT, 411,
    UInt32 InventorySlotIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_EXTRACT_ITEM_SLOT_INDEX,
    UInt64 ItemID;
    UInt64 Unknown1;
    UInt64 ItemOption;
    UInt16 InventorySlotIndex;
    UInt32 Unnown3;
)

CLIENT_PROTOCOL(S2C, EXTRACT_ITEM, DEFAULT, 411,
    UInt8 Result;
    UInt8 ItemCount;
    S2C_EXTRACT_ITEM_SLOT_INDEX Items[0];
)

CLIENT_PROTOCOL(C2S, CASH_INVENTORY_RECEIVE_ITEM, DEFAULT, 419,
    Int32 TransactionID;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, CASH_INVENTORY_RECEIVE_ITEM, DEFAULT, 419,
    Int32 TransactionID;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 InventorySlotIndex;
    RTItemDuration ItemDuration;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, HEART_BEAT, DEFAULT, 420,
)

CLIENT_PROTOCOL(S2C, HEART_BEAT, DEFAULT, 420,
)

CLIENT_PROTOCOL(C2S, DESTROY_CASH_ITEM, DEFAULT, 422,
    UInt64 ItemID;
    UInt8 InventoryType;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, DESTROY_CASH_ITEM, DEFAULT, 422,
    UInt8 Result;
    UInt32 Unknown1;
    UInt64 Unknown2;
    UInt64 Unknown3;
    UInt64 Unknown4;
    UInt64 Unknown5;
)

CLIENT_PROTOCOL(S2C, NFY_DESTROY_CASH_ITEM, DEFAULT, 423,
    UInt64 ItemID;
    UInt8 InventoryType;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(C2S, GET_CASH_INVENTORY, DEFAULT, 428,
    Int32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_CASH_INVENTORY_SLOT,
    Int32 TransactionID;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt8 ItemDurationIndex;
)

CLIENT_PROTOCOL(S2C, GET_CASH_INVENTORY, DEFAULT, 428,
    UInt8 Result;
    UInt16 ItemCount;
    S2C_DATA_GET_CASH_INVENTORY_SLOT ItemSlots[0];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_GUILD_INITIALIZE_PAYLOAD,
    UInt16 GuildNameLength;
    Char GuildName[0];
)

CLIENT_PROTOCOL(C2S, GUILD_INITIALIZE, DEFAULT, 445,
    UInt32 GuildIndex;
    UInt32 CharacterIndex;
    UInt16 PayloadLength;
//    C2S_DATA_INITIALIZE_GUILD_PAYLOAD Payload;
)

CLIENT_PROTOCOL(S2C, GUILD_INITIALIZE, DEFAULT, 445,
    UInt8 Result;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_ENVIRONMENT_WORLD_RESTRICTION,
    Int32 Index;
    Int32 StartIndex;
    Int32 Count;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_ENVIRONMENT_TAIL,
    UInt8 Unknown19[255];
    UInt32 Unknown20;
    UInt32 Unknown21;
    UInt32 Unknown22;
    UInt32 Unknown23;
    UInt8 Unknown24;
)

CLIENT_PROTOCOL(C2S, GET_SERVER_ENVIRONMENT, DEFAULT, 464,
)

CLIENT_PROTOCOL(S2C, GET_SERVER_ENVIRONMENT, DEFAULT, 464,
    UInt16 MaxLevel;
    UInt8 DummyEnabled;
    UInt8 CashshopEnabled;
    UInt8 NetCafePointEnabled;
//    UInt16 MinChatLevel;
    UInt16 MaxRank;
    UInt16 MinShoutLevel;
    UInt16 MinShoutSkillRank;
    UInt64 MaxInventoryCurrency;
    UInt64 MaxWarehouseCurrency;
    UInt64 MaxAuctionCurrency;
    UInt8 NetCafePremiumEnabled;
    UInt8 GuildBoardEnabled;
    UInt8 NetCafePremiumType;
    UInt8 AgentShopEnabled;
    UInt8 Unknown1;
    UInt8 Unknown2;
    UInt8 Unknown3;
    UInt8 MegaphoneShoutCooldownTime;
    UInt8 MinDummyLevel;
    UInt16 MinAgentShopLevel;
    UInt16 MinPersonalShopLevel;
    UInt8 TPointEnabled;
    UInt8 GuildExpansionEnabled;
    UInt8 LimitMegaphoneShoutEnabled;
    UInt8 MinTalkLevel;
    UInt8 MinTradeTalkLevel;
    Int32 MaxDP;
    Int32 Unknown4;
    Int32 Unknown5;
    UInt8 MinTradeTalkLevel2;
    UInt8 MinTradeTalkLevel3;
    UInt8 Unknown6;
    UInt8 Unknown7;
    Int64 MaxHonorPoint;
    Int64 MinHonorPoint;
    Int32 Unknown8;
    Int32 UnknownCount;
    // S2C_DATA_ENVIRONMENT_WORLD_RESTRICTION Restrictions[UnknownCount];
)

CLIENT_PROTOCOL(S2C, NFY_PC_BANG_ALERT, DEFAULT, 480,
    UInt8 Status;
    UInt32 RemainingTime;
    UInt32 RemainingPoints;
)

CLIENT_PROTOCOL(C2S, CHECK_DUNGEON_PLAYTIME, DEFAULT, 485,
    Int32 DungeonIndex;
)

CLIENT_PROTOCOL(S2C, CHECK_DUNGEON_PLAYTIME, DEFAULT, 485,
    Int32 DungeonIndex;
    Int32 MaxInstanceCount;
    Int32 InstanceCount;
    UInt8 IsAccessible;
    UInt32 Unknown2;
    UInt8 MaxEliteEntryCount;
    UInt8 EliteEntryCount;
    UInt8 Unknown22;
    UInt8 Unknown23;
    UInt8 Unknown24;
    UInt32 Unknown5;
    UInt32 RemainingPlaytimeInSeconds;
    UInt32 WeekdayPlaytimeInSeconds[7];
    UInt8 MaxEntryCount;
    UInt8 EntryCount;
    UInt8 Unknown18;
    UInt8 Unknown19;
    UInt8 Unknown20;
    UInt32 Unknown14;
    UInt32 Unknown15;
)

CLIENT_PROTOCOL(C2S, PUSH_EQUIPMENT_ITEM, DEFAULT, 486,
    UInt32 LeftSlotIndex;
    UInt32 RightSlotIndex;
    UInt32 LeftToRightItemType;
)

CLIENT_PROTOCOL(S2C, PUSH_EQUIPMENT_ITEM, DEFAULT, 486,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, GM_COMMAND, DEFAULT, 490,
    UInt8 GmCommand;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, GM_COMMAND, DEFAULT, 490,
    UInt8 Result;
    UInt8 State;
)

CLIENT_PROTOCOL(C2S, WAR_INTOLOBBY, DEFAULT, 681,
    UInt8 WarChannel;
    UInt32 CharacterIndex;
    UInt8 LobbyConnect;
)

CLIENT_PROTOCOL(S2C, WAR_INTOLOBBY, DEFAULT, 681,
    UInt8 WarChannel;
    UInt8 Result;
    Char WorldHost[65];
    UInt16 WorldPort;
    UInt8 WorldServerID;
    UInt64 EntryFee;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_TO_TARGET_BUFF_TARGET,
    RTEntityID TargetID;
    UInt8 TargetType;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_TO_TARGET_BUFF_TAIL,
    UInt8 Data[5];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_TO_TARGET_BUFF,
    Int32 SlotIndex;
    Int32 Unknown2;
    Int32 Unknown3;
    UInt8 TargetCount;
    // C2S_DATA_SKILL_TO_TARGET_BUFF_TARGET Targets[TargetCount];
    // C2S_DATA_SKILL_TO_TARGET_BUFF_TAIL Tail;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_TO_TARGET_WING,
    UInt16 SlotIndex;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_TARGET, DEFAULT, 701,
    Int16 SkillIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_TO_TARGET_BUFF_TARGET,
    UInt32 TargetIndex;
    UInt8 TargetType;
    UInt32 Result;
    Int32 Unknown2;
    Int32 Unknown3;
    Timestamp Duration;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_TO_TARGET_BUFF_TAIL,
    UInt8 Data[26];
)

CLIENT_PROTOCOL(S2C, SKILL_TO_TARGET_BUFF, DEFAULT, 703,
    Int32 SkillIndex;
    Int16 SkillLevel;
    Int8 BuffType;
    Int8 TargetCount;
    Int32 Unknown1;
    UInt32 ItemID;
    Int64 CurrentHP;
    Int32 CurrentMP;
    Int32 CurrentSP;
    // S2C_DATA_SKILL_TO_TARGET_BUFF_TARGET Targets[TargetCount];
    // S2C_DATA_SKILL_TO_TARGET_BUFF_TAIL Tail;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_TO_TARGET_BUFF_TARGET,
    UInt32 TargetIndex;
    UInt8 TargetType;
    UInt32 Result;
    UInt32 BfxIndex;
    UInt32 Unknown1;
    Timestamp Duration;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_TO_TARGET_BUFF_TAIL,
    UInt8 Data[92];
)

CLIENT_PROTOCOL(S2C, NFY_SKILL_TO_TARGET_BUFF, DEFAULT, 704,
    UInt32 CharacterIndex;
    Int32 SkillIndex;
    Int16 SkillLevel;
    Int32 Unknown1;
    Int8 BuffType;
    Int32 Unknown2;
    Int8 TargetCount;
    UInt32 ItemID;
    UInt32 EffectorItemID;
    UInt64 EffectorItemOptions;
    Int32 SkillTranscendenceLevel;
    Int32 SkillTranscendenceIndex;
)

CLIENT_PROTOCOL(S2C, UPDATE_SKILL_STATUS, DEFAULT, 760,
    Int32 SkillRank;
    Int32 SkillLevel;
    UInt32 SkillLevelMax;
    UInt32 SkillExp;
    UInt32 SkillPoint;
)

CLIENT_PROTOCOL_ENUM(
    C2S_DATA_VERIFY_CREDENTIALS_TYPE_UNKNOWN,
    C2S_DATA_VERIFY_CREDENTIALS_TYPE_PASSWORD
)

CLIENT_PROTOCOL(C2S, VERIFY_CREDENTIALS, DEFAULT, 800,
    Int32 CredentialsType;
    Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_CREDENTIALS, DEFAULT, 800,
    Bool Success;
)

CLIENT_PROTOCOL(C2S, REMOVE_BUFF, DEFAULT, 803,
    Int16 SkillIndex;
)

CLIENT_PROTOCOL(S2C, REMOVE_BUFF, DEFAULT, 803,
    Int16 SkillIndex;
    UInt8 Result;
)

CLIENT_PROTOCOL_ENUM(
    CSC_DATA_PET_COMMAND_TYPE_INFO = 1,

    CSC_DATA_PET_COMMAND_TYPE_UNSEAL = 6,
    CSC_DATA_PET_COMMAND_TYPE_TRAIN = 7,
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_PET_COMMAND_INFO,
    Int32 Unknown1;
    UInt32 CharacterIndex;
    UInt8 Unknown2;
    Int32 InventorySlotIndex;
    Int32 Unknown3;
    Int32 Unknown4;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_PET_COMMAND_UNSEAL,
    Int32 Unknown1;
    Int32 InventorySlotIndex;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_PET_COMMAND_TRAIN,
    Int32 PetIndex;
    UInt8 PetSlotIndex;
    UInt8 InventorySlotCount;
    UInt16 InventorySlots[0];
)

CLIENT_PROTOCOL(C2S, PET_COMMAND, DEFAULT, 905,
    Int32 CommandType;
    UInt8 Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PET_COMMAND_INFO,
    Int32 PetIndex;
    Int32 Unknown1;
    UInt8 Unknown2;
    Int32 InventorySlotIndex;
    Int32 PetIndex2;
    UInt32 PetStyle;
    UInt32 CharacterIndex;
    UInt64 PetItemOptions;
    Int8 PetLevel;
    Int64 PetExp;
    Int32 Unknown3;
    Int16 Unknown4;
    Int64 Unknown5;
    UInt8 PetSlots[RUNTIME_PET_MAX_SLOT_COUNT];
    UInt64 DeletionDate;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PET_COMMAND_UNSEAL,
    UInt64 PetItemOptions;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PET_COMMAND_TRAIN,
    Int32 PetIndex;
    Int32 Unknown1;
    Int64 CurrencyCost;
    UInt8 PetSlotOption;
)

CLIENT_PROTOCOL(S2C, PET_COMMAND, DEFAULT, 905,
    Int32 CommandType;
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_PARAMETERS, DEFAULT, 904,
    Int64 Exp;
    Int32 Level;
    Int32 Str;
    Int32 Dex;
    Int32 Int;
    UInt8 Unknown1[16];
    Int64 MaxHp;
    Int32 MaxMp;
    UInt8 Unknown2[4];
    Int32 Bp;
    Int32 BpMax;
    Int32 BpConsumeDecRate;
    Int32 Atk;
    Int32 MAtk;
    Int32 Def;
    UInt8 Unknown3[5];
    Int32 AtkRate;
    Int32 DefRate;
    Int32 Evasion;
    UInt8 Unknown4[12];
    Int32 CRate;
    Int32 CDmg;
    UInt8 Unknown5[8];
    Int32 HPSteal;
    Int32 MPSteal;
    UInt8 Unknown6[44];
    Int32 MPStealLimit;
    Int32 HPStealLimit;
    Int32 CLimit;
    UInt8 Unknown7[59];
    Int32 BlowR;
    Int32 DownR;
    Int32 StunR;
    Int32 MoveR;
    Int32 CRateR;
    Int32 CDmgR;
    Int32 AmpR;
    Int32 ResistSurprize;
    Int32 ResistSilence;
    UInt8 Unknown8[4];
    Int32 ServerAlzDropRate;
    Int32 ServerAlzBombRate;
    Int32 ServerAxpPercentInc;
    Int32 ServerSkillExpPercentInc;
    UInt8 Unknown9[8];
    Int32 DamageDrop;
    Int32 Hit;
    Int32 Penetration;
    UInt8 Unknown10[24];
    Int32 IgnoreAvoid;
    Int32 IgnoreHit;
    Int32 IgnorePenetration;
    Int32 IgnoreDamagedrop;
    Int32 OffsetIgnoreAvoid;
    Int32 OffsetIgnoreDamageDrop;
    Int32 FinalDmgRateUp;
    Int32 FinalDmgRateDown;
    Int32 HPRecoveryPercent;
    Int32 IgnoreCRateR;
    Int32 AbsoluteDmgRes;
    Int32 IgnoreCDmgR;
    Int32 IgnoreAmpR;
    Int32 IgnoreDownR;
    Int32 IgnoreBlowR;
    Int32 IgnoreStunR;
    Int32 NormalAtkDmgInc;
    Int32 OffsetIgnorePenetration;
    Int32 Speed;
    Int32 PVPResistSkillAmpSword;
    Int32 PVPResistSkillAmpMagic;
    Int32 PVESkillAmpSword;
    Int32 PVESkillAmpMagic;
    Int32 PVEDefense;
    Int32 PVEDamageDrop;
    Int32 PVEIgnorePenetration;
    Int32 PVECriticalDamage;
    Int32 PVEPenetration;
    Int32 PVPSkillAmpSword;
    Int32 PVPSkillAmpMagic;
    Int32 PVPCriticalDamage;
    Int32 AttackPVP;
    Int32 MAttackPVP;
    Int32 AttackPVE;
    Int32 MAttackPVE;
    Int32 PVPAttackRate;
    Int32 PVEAttackRate;
    Int32 PVPCriticalRate;
    Int32 PVECriticalRate;
    Int32 PVPHit;
    Int32 PVEHit;
    Int32 PVPPenetration;
    Int32 PVPDefense;
    Int32 PVEDrate;
    Int32 PVPDrate;
    Int32 PVPAvoid;
    Int32 PVEAvoid;
    Int32 PVPDamageDrop;
    Int32 PVPIgnorePenetration;
    Int32 PVPFDMD;
    Int32 PVEFDMD;
    Int32 PVPNormalAttackDmgInc;
    Int32 PVENormalAttackDmgInc;
    Int32 PVPFDMU;
    Int32 PVEFDMU;
    Int32 PVPAddDmg;
    Int32 PVEAddDmg;
    Int32 PVPOffsetIgnoreAvoid;
    Int32 PVEOffsetIgnoreAvoid;
    Int32 PVPIgnoreResistCriDmg;
    Int32 PVEIgnoreResistCriDmg;
    Int32 PVPOffsetIgnorePenetration;
    Int32 PVEOffsetIgnorePenetration;
    Int32 PVPIgnoreHit;
    Int32 PVEIgnoreHit;
    Int32 PVPIgnoreAvoid;
    Int32 PVEIgnoreAvoid;
    Int32 PVPIgnoreResistAmp;
    Int32 PVEIgnoreResistAmp;
    Int32 PVPIgnoreDmgDrop;
    Int32 PVEIgnoreDmgDrop;
    Int32 PVPIgnoreResistCriRate;
    Int32 PVEIgnoreResistCriRate;
    Int64 CombatAttackBasicTotal;
    Int64 CombatAttackPveTotal;
    Int64 CombatAttackPvpTotal;
    Int64 CombatDefenseBasicTotal;
    Int64 CombatDefensePveTotal;
    Int64 CombatDefensePvpTotal;
    Int64 Unknown11;
    Int64 CombatAttackBasicPassive;
    Int64 CombatDefensePvePassive1;
    Int64 CombatDefensePvpPassive1;
    Int64 CombatDefenseBasicPassive;
    Int64 CombatDefensePvePassive2; 
    Int64 CombatDefensePvpPassive2;
)

CLIENT_PROTOCOL(C2S, UPGRADE_ITEM_LEVEL, DEFAULT, 951,
    UInt32 Unknown1;
    UInt16 InventorySlotIndex;
    UInt8 CoreCount;
    UInt16 SafeCount;
    UInt16 CoreSlotIndices[60];
    UInt16 SafeSlotIndices[0];
)

CLIENT_PROTOCOL(S2C, UPGRADE_ITEM_LEVEL, DEFAULT, 951,
    UInt8 Result;
    UInt64 ItemID;
    UInt64 ItemOption;
    UInt32 ItemDuration;
    UInt8 ConsumedSafeCount;
    UInt32 Unknown6;
    UInt32 Unknown7;
    UInt32 RemainingCoreCount;
)

CLIENT_PROTOCOL(C2S, AUCTION_HOUSE_BUY_ITEM, DEFAULT, 969,
    Int32 AccountID;
    UInt8 Unknown1;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 ItemOptionExtended;
    UInt8 ItemPriceType;
    Int64 ItemPrice;
    Char ItemName[101];
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, AUCTION_HOUSE_BUY_ITEM, DEFAULT, 969,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, AUCTION_HOUSE_REGISTER_ITEM, DEFAULT, 970,
    UInt8 SlotIndex;
    UInt16 ItemCount;
    UInt8 ItemPriceType;
    UInt64 ItemPrice;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, AUCTION_HOUSE_REGISTER_ITEM, DEFAULT, 970,
    UInt8 Result;
    Timestamp ExpirationDate;
)

CLIENT_PROTOCOL(C2S, AUCTION_HOUSE_UNREGISTER_ITEM, DEFAULT, 971,
    UInt8 SlotIndex;
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, AUCTION_HOUSE_UNREGISTER_ITEM, DEFAULT, 971,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, AUCTION_HOUSE_PROCEED_ITEM, DEFAULT, 972,
    UInt8 SlotIndex;
    UInt8 UseCoupon;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, AUCTION_HOUSE_PROCEED_ITEM, DEFAULT, 972,
    UInt8 Result;
    Int32 SoldItemCount;
)

CLIENT_PROTOCOL(C2S, AUCTION_HOUSE_UPDATE_ITEM, DEFAULT, 973,
    UInt8 SlotIndex;
    UInt16 ItemCount;
    UInt8 ItemPriceType;
    UInt64 ItemPrice;
    UInt8 ItemCountDecreased;
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, AUCTION_HOUSE_UPDATE_ITEM, DEFAULT, 973,
    UInt8 Result;
    Timestamp ExpirationDate;
)

CLIENT_PROTOCOL(C2S, SELECT_CHARACTER_IN_GAME, DEFAULT, 985,
)

CLIENT_PROTOCOL(S2C, SELECT_CHARACTER_IN_GAME, DEFAULT, 985,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, PREMIUM_BENEFIT_INFO_LIST, DEFAULT, 999,
    // TODO: Add packet structure
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PREMIUM_BENEFIT_INFO,
    Int16 Unknown1;
    Int32 Unknown2;
    Int32 ServiceID;
    Int32 Type;
    Int16 Unknown3;
    Int16 Unknown4;
    Int32 XP;
    Int32 SkillXP;
    Int32 DropRate;
    Int32 AlzBombRate;
    Int32 WarXP;
    Int32 PetXP;
    Int32 AXP;
    Int32 TPointUp;
    Int32 AlzDropRate;
    Int32 BoxDropRate;
    Int32 ForceWingXP;
    Int32 Inventory1;
    Int32 Inventory2;
    Int32 Inventory3;
    Int32 Inventory4;
    Int32 Inventory5;
    Int32 Inventory6;
    Int32 ExtendPetSlot;
    Int32 Warehouse;
    Int32 SkillXPTDummy;
    Int32 GPSWarpMask;
    UInt8 UnlimitedWarp;
    Int16 AuctionHouseItemBonusSlots;
    Int16 AuctionHouseItemBonusQuantity;
    Int16 AuctionHouseItemBonusPeriod;
    UInt8 AuctionHouseFeeExemption;
    UInt8 AbleToBuyPremiumItemFromShop;
    UInt8 RemoteShop;
    UInt8 AbleToEnterPremiumDungeon;
    UInt8 PremiumDungeonReward;
    UInt8 RemoteWarehouse;
    Int32 CraftMasteryUp;
    Int32 RequestAmityUp;
    UInt8 RequestAmitySlots;
    Int32 DungeonPlayTimeIncreased;
    Int32 Unknown5;
    Int32 Unknown6;
    Int32 Unknown7;
    Int32 Unknown8;
    Int32 Unknown9;
    UInt8 Unknown10;
    UInt8 Unknown11;
    UInt8 Unknown12;
    Int32 MercenarySummoningRestrictionLifted;
    Int32 Unknown13;
    Int32 Unknown14;
    Int32 Unknown15;
    Int32 Unknown16;
)

CLIENT_PROTOCOL(S2C, PREMIUM_BENEFIT_INFO_LIST, EXTENDED, 999,
    UInt8 Result;
    Int32 Count;
    // S2C_DATA_PREMIUM_BENEFIT_INFO Benefits[];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO_DATA,
    RTItem ItemID;
    UInt64 ItemOptions;
    Int32 ExternalID;
    UInt32 TextureItemID;
    UInt8 Unknown3;
    // CString ItemName;
    // CString ItemDescription;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO,
    UInt16 ItemCount;
    // S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO_DATA Items[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_UNKNOWN1_DATA,
    Int32 ItemIndex;
    UInt8 Unknown1;
    RTItem ItemID;
    UInt64 ItemOptions;
    UInt8 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_UNKNOWN2_DATA,
    Int32 ItemIndex;
    RTItem ItemID;
    UInt64 ItemOptions;
    UInt16 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_EVENT_INFO_EVENT,
    Int32 EventIndex;
    Int32 EventType;
    Int32 EventFlags;
    Timestamp EventStartTimestamp;
    Timestamp EventEndTimestamp;
    Timestamp EventUpdateTimestamp;
    UInt32 WorldIndex;
    UInt32 NpcIndex;
    UInt8 Unknown1[142];
    Char Name[0];
    // S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO ItemInfo;
    // UInt16 Unknown1Count;
    // S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_UNKNOWN1_DATA Unknown1[Unknown1Count];
    // UInt16 Unknown2Count;
    // S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_UNKNOWN2_DATA Unknown1[Unknown1Count];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_EVENT_INFO_HEADER,
    UInt8 EventCount;
    // S2C_DATA_NFY_EVENT_INFO_EVENT Events[0];
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_INFO, EXTENDED, 1002,
    UInt8 HasEventInfo;
    // S2C_DATA_NFY_EVENT_INFO_HEADER Header;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_LABEL_HEADER,
    UInt16 Count;
    // CString Label;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_UNKNOWN1_HEADER,
    UInt16 Count;
    // S2C_DATA_EVENT_UNKNOWN1_DATA Data;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_UNKNOWN2_HEADER,
    UInt16 Count;
    // S2C_DATA_EVENT_UNKNOWN2_DATA Data;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_SHOP_HEADER,
    UInt16 ItemCount;
    // S2C_DATA_EVENT_SHOP_ITEM_SLOT ItemSlots[];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_UNKNOWN3_HEADER,
    UInt8 Count;
    // TODO: Add packet payload
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_HEADER,
    UInt16 Count;
    // S2C_DATA_EVENT_BINGO_BODY Bingo[];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_BODY,
    UInt16 RotationCount;
    UInt16 UnknownCount2;
    UInt16 UnknownCount3;
    UInt16 UnknownCount4;
    // S2C_DATA_EVENT_BINGO_BODY_BOARD Boards[]
    // S2C_DATA_EVENT_BINGO_BODY_ROTATION Rotations[]
    // S2C_DATA_EVENT_BINGO_BODY_SLOT Slots[]
    // S2C_DATA_EVENT_BINGO_BODY_REWARD_SLOT RewardSlots[]
    // S2C_DATA_EVENT_BINGO_BODY_REWARD_ITEM RewardItems[]
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_BODY_BOARD,
    UInt32 BoardIndex;
    UInt32 Unknown1;
    UInt8 Unknown2;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_BODY_ROTATION,
    UInt8 PeriodIndex;
    Timestamp StartTimestamp;
    Timestamp EndTimestamp;
    UInt32 BoardIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_BODY_SLOT,
    UInt32 BoardIndex;
    UInt8 SlotIndex;
    UInt32 Unknown1;
    RTItem ItemID;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_BODY_REWARD_SLOT,
    UInt32 BoardIndex;
    UInt8 SlotIndex;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_BODY_REWARD_ITEM,
    UInt32 UnknownIndex;
    UInt32 BoardIndex;
    RTItem ItemID;
    UInt64 ItemOptions;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_INITIALIZE_HEADER,
    UInt8 Count;
    // S2C_DATA_EVENT_BINGO_INITIALIZE_BODY Bingo[];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_INITIALIZE_BODY,
    UInt16 PayloadLength;
    UInt8 UnknownA[19];
    UInt8 SlotCount;
    // S2C_DATA_EVENT_BINGO_INITIALIZE_BODY_SLOT Slots[];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_BINGO_INITIALIZE_BODY_SLOT,
    UInt8 SlotIndex;
    UInt64 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_PERIOD_HEADER,
    UInt8 Count;
    // S2C_DATA_EVENT_PERIOD_BODY Body[];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_PERIOD_BODY,
    UInt16 Unknown1;
    Timestamp StartDate;
    Timestamp EndDate;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_DUNGEON_REWARD_HEADER,
    UInt16 Count;
    // S2C_DATA_EVENT_DUNGEON_REWARD_BODY Body[];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_SHOP_ITEM,
    UInt32 ShopSlotIndex;
    RTItem ItemID;
    UInt64 ItemOptions;
    UInt32 ItemDuration;
    UInt8 Unknown1;
    Int32 TextureIndex;
    UInt64 CurrencyPrice;
    UInt16 TPointPrice;
    UInt32 ItemPriceCount;
    UInt16 ItemPurchaseLimit;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_SHOP_ITEM_PRICE,
    RTItem ItemID;
    UInt64 ItemOptions;
    UInt32 ItemCount;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT,
    UInt32 EventIndex;
)

CLIENT_PROTOCOL(C2S, GET_EVENT_LIST, DEFAULT, 1003,
)

CLIENT_PROTOCOL(S2C, GET_EVENT_LIST, DEFAULT, 1003,
    UInt8 Result;
    UInt8 EventCount;
    UInt32 Unknown2;
    // S2C_DATA_EVENT Events[];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_EVENT_ACTION_ITEM_PRICE,
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

// TODO: Add support for additional event action types!
CLIENT_PROTOCOL(C2S, EVENT_ACTION, DEFAULT, 1006,
    Int32 EventIndex;
    Int32 Unknown1;
    UInt8 NpcIndex;
    UInt16 ShopSlotIndex;
    UInt16 ItemPriceCount;
    UInt16 InventorySlotCount;
    UInt8 Data[0];
    // EVENT_ACTION_ITEM_PRICE ItemPrices[]
    // UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_EVENT_ACTION_SHOP_ITEM,
    RTItem Item;
    UInt64 ItemOptions;
    UInt16 SlotIndex;
    RTItemDuration ItemDuration;
)

CLIENT_PROTOCOL(S2C, EVENT_ACTION, DEFAULT, 1006,
    Int32 EventIndex;
    Int32 Unknown1;
    UInt8 Result;
    UInt16 ItemCount;
    // S2C_DATA_EVENT_ACTION_SHOP_ITEM Items[InventorySlotCount];
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_LIST, DEFAULT, 1008,
    UInt8 Count;
    Int32 EventIndex[];
)

CLIENT_PROTOCOL(C2S, CREATE_SUBPASSWORD, DEFAULT, 1030,
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
    Int32 Type;
    Int32 Question;
    Char Answer[MAX_SUBPASSWORD_ANSWER_LENGTH];
    Int32 IsChange;
)

CLIENT_PROTOCOL(S2C, CREATE_SUBPASSWORD, DEFAULT, 1030,
    Int32 Success;
    Int32 IsChange;
    Int32 Type;
    Int32 IsLocked;
)

CLIENT_PROTOCOL(C2S, CHECK_SUBPASSWORD, DEFAULT, 1032,
)

CLIENT_PROTOCOL(S2C, CHECK_SUBPASSWORD, DEFAULT, 1032,
    Int32 IsVerificationRequired;
)

CLIENT_PROTOCOL(C2S, VERIFY_SUBPASSWORD, DEFAULT, 1034,
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
    Int32 Type;
    Int8 ExpirationInHours;
    Int32 IsLocked;
)

CLIENT_PROTOCOL(S2C, VERIFY_SUBPASSWORD, DEFAULT, 1034,
    Int32 Success;
    Int8 FailureCount;
    Int32 IsLocked;
    Int32 Type;
)

CLIENT_PROTOCOL(C2S, VERIFY_DELETE_SUBPASSWORD, DEFAULT, 1040,
    Int32 Type;
    UInt8 Unknown1[65];
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_DELETE_SUBPASSWORD, DEFAULT, 1040,
    Int32 Success;
    Int8 FailureCount;
    Int32 Type;
)

CLIENT_PROTOCOL(C2S, DELETE_SUBPASSWORD, DEFAULT, 1042,
    Int32 Type;
)

CLIENT_PROTOCOL(S2C, DELETE_SUBPASSWORD, DEFAULT, 1042,
    Int32 Success;
    Int32 Type;
)

CLIENT_PROTOCOL_ENUM(
    CSC_DATA_ABILITY_TYPE_ESSENCE = 1,
    CSC_DATA_ABILITY_TYPE_BLENDED = 2,
    CSC_DATA_ABILITY_TYPE_KARMA = 3,
)

CLIENT_PROTOCOL(C2S, ADD_ABILITY, DEFAULT, 1060,
    UInt16 InventorySlotIndex;
    UInt32 AbilityType;
    UInt16 MaterialSlotIndex[0];
)

CLIENT_PROTOCOL(S2C, ADD_ABILITY, DEFAULT, 1060,
    UInt32 Result;
    UInt32 AbilityType;
)

CLIENT_PROTOCOL(C2S, UPGRADE_ABILITY, DEFAULT, 1061,
    UInt32 AbilityID;
    UInt32 AbilityType;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, UPGRADE_ABILITY, DEFAULT, 1061,
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, REMOVE_ABILITY, DEFAULT, 1062,
    UInt32 AbilityID;
    UInt16 Unknown1;
    UInt32 AbilityType;
)

CLIENT_PROTOCOL(S2C, REMOVE_ABILITY, DEFAULT, 1062,
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, SET_CHARACTER_SLOT_ORDER, DEFAULT, 2001,
    UInt64 CharacterSlotOrder;
)

CLIENT_PROTOCOL(S2C, SET_CHARACTER_SLOT_ORDER, DEFAULT, 2001,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, BUY_SKILLBOOK, DEFAULT, 2003,
    UInt8 NpcID;
    UInt16 SlotID;
    UInt16 SkillID;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, BUY_SKILLBOOK, DEFAULT, 2003,
    Int32 Result;
    UInt64 ItemID;
    UInt64 ItemSerial;
    UInt64 ItemOptions;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_INFO, DEFAULT, 2004,
    UInt8 Unknown1;
    UInt16 Unknown2;
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_MOB_KILL, DEFAULT, 2006,
    Int16 MobSpeciesIndex;
    Int32 SkillIndex;
)

CLIENT_PROTOCOL(C2S, PARTY_INVITE, DEFAULT, 2011,
    UInt8 Unknown1;
    UInt32 CharacterIndex;
    UInt8 WorldServerID;
    UInt8 NameLength;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, PARTY_INVITE, DEFAULT, 2011,
    UInt8 Result;
    UInt32 CharacterIndex;
    UInt8 WorldServerID;
    UInt8 CharacterType;
    Int32 Level;
    UInt8 NameLength;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INVITE, DEFAULT, 2012,
    UInt32 CharacterIndex;
    UInt8 WorldServerID;
    UInt8 CharacterType;
    Int32 Level;
    UInt8 NameLength;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
)

CLIENT_PROTOCOL(C2S, PARTY_INVITE_CONFIRM, DEFAULT, 2013,
    UInt32 CharacterIndex;
    UInt8 WorldServerID;
    Int32 IsAccept;
)

CLIENT_PROTOCOL(S2C, PARTY_INVITE_CONFIRM, DEFAULT, 2013,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INVITE_TIMEOUT, DEFAULT, 2014,
    UInt32 IsAccept;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INFO, DEFAULT, 2016,
    UInt32 Result;
    UInt32 LeaderCharacterIndex;
    UInt32 Unknown1;
    UInt8 Unknown2;
    UInt8 WorldServerID;
    UInt8 Unknown3;
    UInt32 MemberCount;
    struct _RTPartyMemberInfo Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
)

CLIENT_PROTOCOL(C2S, PARTY_INVITE_CANCEL, DEFAULT, 2017,
    UInt32 CharacterIndex;
    UInt8 WorldServerID;
)

CLIENT_PROTOCOL(S2C, PARTY_INVITE_CANCEL, DEFAULT, 2017,
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INVITE_CANCEL, DEFAULT, 2018,
)

CLIENT_PROTOCOL(C2S, PARTY_LEAVE, DEFAULT, 2019,
)

CLIENT_PROTOCOL(S2C, PARTY_LEAVE, DEFAULT, 2019,
    Int32 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_LEAVE, DEFAULT, 2020,
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(C2S, PARTY_EXPEL_MEMBER, DEFAULT, 2021,
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, PARTY_EXPEL_MEMBER, DEFAULT, 2021,
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_EXPEL_MEMBER, DEFAULT, 2022,
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(C2S, PARTY_CHANGE_LEADER, DEFAULT, 2023,
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(S2C, PARTY_CHANGE_LEADER, DEFAULT, 2023,
    Int32 Result;
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_CHANGE_LEADER, DEFAULT, 2024,
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_CHANGE_RULE, DEFAULT, 2025,
    Int32 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INVITE_RULE, DEFAULT, 2026,
// TODO: Add packet structure
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PARTY_UPDATE_MEMBER,
    struct _RTPartyMemberInfo Info;
    struct _RTPartyMemberData Data;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_UPDATE, DEFAULT, 2028,
    UInt32 MemberCount;
    S2C_DATA_PARTY_UPDATE_MEMBER Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
    UInt8 Unknown1[24];
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_PICK_UP_CURRENCY, DEFAULT, 2035,
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(C2S, ENTER_DUNGEON_GATE, DEFAULT, 2029,
    UInt32 DungeonID;
    UInt32 Unknown1;
    UInt32 NpcID;
    UInt32 Unknown3;
    Int32 Unknown4;
    UInt32 WorldID;
    UInt8 Unknown5;
    UInt16 DungeonBoostLevel;
)

CLIENT_PROTOCOL(S2C, ENTER_DUNGEON_GATE, DEFAULT, 2029,
    UInt32 Result;
    UInt32 DungeonID;
    UInt32 Unknown1;
    UInt32 NpcID;
    UInt32 Unknown3;
    Int32 Unknown4;
    UInt32 WorldID;
    UInt8 Unknown5;
    UInt16 DungeonBoostLevel;
)

CLIENT_PROTOCOL(C2S, PARTY_CHANGE_LOOTING_RULE, DEFAULT, 2033,
    Int32 NewRule;
    Int32 OldRule;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_CHANGE_LOOTING_RULE, DEFAULT, 2034,
    Int32 NewRule;
    Int32 OldRule;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INIT, DEFAULT, 2056,
    Int32 Result;
    UInt32 DungeonIndex;
    UInt16 Unknown1;
    RTEntityID PartyID;
    UInt32 PartyLeaderIndex;
    Int32 Unknown2;
    UInt8 Unknown3;
    UInt8 WorldServerIndex;
    UInt8 Unknown4;
    Int32 MemberCount;
    struct _RTPartyMemberInfo Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
    UInt8 Padding[100];
    UInt8 Unknown5;
    UInt32 SoloDungeonIndex;
    Timestamp SoloDungeonTimeout;
    UInt8 Unknown6[3];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_MESSAGE_BROADCAST_HEADER,
    UInt8 Unknown1[5];
    UInt16 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_MESSAGE_BROADCAST_BODY,
    Char Message[109];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_MESSAGE_BROADCAST_PAYLOAD,
    S2C_DATA_NFY_MESSAGE_BROADCAST_HEADER MessageHeader;
    UInt16 MessagePayloadLength;
    S2C_DATA_NFY_MESSAGE_BROADCAST_BODY Body;
)

CLIENT_PROTOCOL(S2C, NFY_MESSAGE_BROADCAST, DEFAULT, 2091,
    S2C_DATA_NFY_MESSAGE_BROADCAST_PAYLOAD Payload;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_CHARACTER_STATUS_BUFF,
    Int32 SkillIndex;
    Int32 SkillLevel;
    Int32 Duration;
    UInt8 Unknown1[10];
    Int32 SkillTranscendenceLevel;
    Int32 SkillTranscendenceIndex;
    UInt8 Unknown2[66];
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_STATUS, DEFAULT, 2110,
    Int64 CurrentHP;
    Int32 CurrentMP;
    Int64 CurrentShield;
    struct _RTCharacterBattleModeInfo BattleModeInfo;
    struct _RTBuffInfo BuffInfo;
    // UInt8[30] BuffSlot[] for EffectorBuffCount, UnknownBuffPotionCount1, ..., UnknownBuffPotionCount2
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_MOVEMENT_SPEED, DEFAULT, 2111,
    UInt32 CharacterIndex;
    UInt32 MovementSpeed;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SERVER_LIST_WORLD,
    UInt8 ServerID;
    UInt8 WorldID;
    UInt16 PlayerCount;
    UInt16 LobbyPlayerCount;
    UInt16 Unknown1;
    UInt16 CapellaPlayerCount;
    UInt16 ProcyonPlayerCount;
    UInt32 Unknown2;
    UInt16 CapellaPlayerCount2;
    UInt16 ProcyonPlayerCount2;
    UInt16 Unknown3;
    UInt8 MinLevel;
    UInt8 MaxLevel;
    UInt8 MinRank;
    UInt8 MaxRank;
    UInt16 MaxPlayerCount;
    Char WorldHost[65];
    UInt16 WorldPort;
    UInt64 WorldType;
)

CLIENT_PROTOCOL(C2S, GET_SERVER_LIST, DEFAULT, 2112,
)

CLIENT_PROTOCOL(S2C, GET_SERVER_LIST, DEFAULT, 2112,
    UInt8 Unknown1;
    UInt8 WorldCount;
    // S2C_DATA_SERVER_LIST_WORLD Worlds[0]; /* [WorldCount] */
)

CLIENT_PROTOCOL_ENUM(
    CSC_DUNGEON_PAUSE_RESULT_SUCCESS        = 0,
    CSC_DUNGEON_PAUSE_RESULT_FAILED         = 1,
    CSC_DUNGEON_PAUSE_RESULT_FAILED_TIMEOUT = 2,
)

CLIENT_PROTOCOL(C2S, DUNGEON_PAUSE, DEFAULT, 2137,
)

CLIENT_PROTOCOL(S2C, DUNGEON_PAUSE, DEFAULT, 2137,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, DUNGEON_RESUME, DEFAULT, 2138,
)

CLIENT_PROTOCOL(S2C, DUNGEON_RESUME, DEFAULT, 2138,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, GET_SERVER_STATUS, DEFAULT, 2141,
    UInt8 GroupIndex;
    UInt8 NodeIndex;
)

CLIENT_PROTOCOL(S2C, GET_SERVER_STATUS, DEFAULT, 2141,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, REGISTER_MERCENARY_CARD, DEFAULT, 2142,
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL_ENUM(
    S2C_REGISTER_MERCENARY_CARD_RESULT_SUCCESS = 0,
    S2C_REGISTER_MERCENARY_CARD_RESULT_ERROR = 1,
    S2C_REGISTER_MERCENARY_CARD_RESULT_ALREADY = 30002,
)

CLIENT_PROTOCOL(S2C, REGISTER_MERCENARY_CARD, DEFAULT, 2142,
    Int32 Result;
    UInt8 Rank;
    UInt8 Level;
)

CLIENT_PROTOCOL(C2S, GET_SPECIAL_EVENT_CHARACTER, DEFAULT, 2156,
)

CLIENT_PROTOCOL(S2C, GET_SPECIAL_EVENT_CHARACTER, DEFAULT, 2156,
    UInt8 IsSet;
    Int32 Data[130];
    //UInt8 Payload[520];
)

CLIENT_PROTOCOL(C2S, VERIFY_CREDENTIALS_SUBPASSWORD, DEFAULT, 2160,
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_CREDENTIALS_SUBPASSWORD, DEFAULT, 2160,
    Int8 Success;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, LOCK_EQUIPMENT, DEFAULT, 2162,
    Int16 EquipmentSlotIndex;
    UInt8 IsLocked;
)

CLIENT_PROTOCOL(S2C, LOCK_EQUIPMENT, DEFAULT, 2162,
    Int32 Success;
)

CLIENT_PROTOCOL_ENUM(
    CSC_DATA_STORAGE_TYPE_INVENTORY = 0,
    CSC_DATA_STORAGE_TYPE_EQUIPMENT = 1,
    CSC_DATA_STORAGE_TYPE_WAREHOUSE = 2,
)

CLIENT_PROTOCOL_STRUCT(CSC_DATA_ITEM_SLOT_INDEX,
    Int32 StorageType;
    UInt32 Index;
)

CLIENT_PROTOCOL(C2S, MOVE_INVENTORY_ITEM, DEFAULT, 2165,
    CSC_DATA_ITEM_SLOT_INDEX Source;
    CSC_DATA_ITEM_SLOT_INDEX Destination;
    UInt16 Unknown1;
    UInt16 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, MOVE_INVENTORY_ITEM, DEFAULT, 2165,
    UInt8 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, SWAP_INVENTORY_ITEM, DEFAULT, 2166,
    CSC_DATA_ITEM_SLOT_INDEX Source1;
    CSC_DATA_ITEM_SLOT_INDEX Destination1;
    CSC_DATA_ITEM_SLOT_INDEX Source2;
    CSC_DATA_ITEM_SLOT_INDEX Destination2;
    UInt16 Unknown1;
    UInt16 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, SWAP_INVENTORY_ITEM, DEFAULT, 2166,
    UInt8 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, MOVE_INVENTORY_ITEM_LIST, DEFAULT, 2167,
    UInt16 ItemCount;
    Int32 Unknown1[3];
    UInt8 Data[0];
//    CSC_DATA_ITEM_SLOT_INDEX Source[];
//    CSC_DATA_ITEM_SLOT_INDEX Destination[];
)

CLIENT_PROTOCOL(S2C, MOVE_INVENTORY_ITEM_LIST, DEFAULT, 2167,
    UInt16 Count;
)

CLIENT_PROTOCOL(C2S, DROP_INVENTORY_ITEM, DEFAULT, 2168,
    Int32 StorageType;
    Int32 InventorySlotIndex;
    UInt16 Unknown1;
    UInt16 Unknown2;
    Int64 Unknown3;
)

CLIENT_PROTOCOL(S2C, DROP_INVENTORY_ITEM, DEFAULT, 2168,
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, CONVERT_ITEM, DEFAULT, 2170,
    UInt32 SourceSlotIndex;
    UInt32 TargetSlotIndex;
)

CLIENT_PROTOCOL(S2C, CONVERT_ITEM, DEFAULT, 2170,
    UInt32 Result;
    RTItem Item;
    UInt64 Unknown2;
    UInt64 ItemOptions;
    UInt32 InventorySlotIndex;
    UInt16 Unknown3;
)

CLIENT_PROTOCOL(C2S, UPDATE_HELP_INFO, DEFAULT, 2173,
    UInt32 HelpWindow;
)

CLIENT_PROTOCOL(S2C, UPDATE_HELP_INFO, DEFAULT, 2173,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, UPDATE_QUEST_LIST, DEFAULT, 2175,
    UInt16 QuestID;
    UInt16 SlotIndex;
    UInt8 DisplayNotice;
    UInt8 DisplayOpenNotice;
)

CLIENT_PROTOCOL(C2S, CLOSE_AUCTION_HOUSE, DEFAULT, 2177,
)

CLIENT_PROTOCOL(S2C, CLOSE_AUCTION_HOUSE, DEFAULT, 2177,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, GET_SHOP_LIST, DEFAULT, 2179,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_SHOP_LIST_INDEX,
    UInt32 WorldIndex;
    UInt8 NpcIndex;
    UInt16 ShopIndex;
    UInt8 IsCouponShop;
)

CLIENT_PROTOCOL(S2C, GET_SHOP_LIST, DEFAULT, 2179,
    UInt16 Count;
    S2C_DATA_GET_SHOP_LIST_INDEX Shops[0];
)

CLIENT_PROTOCOL(C2S, GET_SHOP_DATA, DEFAULT, 2180,
    UInt16 ShopIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_SHOP_DATA_INDEX,
    UInt16 TabIndex;
    UInt16 SlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 ItemDuration;
    UInt8 MinLevel;
    UInt8 MaxLevel;
    UInt8 MinGuildLevel;
    Int8 MinHonorRank;
    Int8 MaxHonorRank;
    UInt8 IsPremiumOnly;
    UInt8 IsWinningOnly;
    Int16 DailyPurchaseLimit;
    Int16 WeeklyPurchaseLimit;
    UInt64 PriceAlz;
    UInt64 PriceWexp;
    UInt16 PriceAP;
    UInt32 PriceDP;
    UInt32 PriceCash;
    Int16 ShopPricePoolIndex;
    UInt32 PriceGem;
)

CLIENT_PROTOCOL(S2C, GET_SHOP_DATA, DEFAULT, 2180,
    UInt16 ShopIndex;
    UInt16 Count;
    S2C_DATA_GET_SHOP_DATA_INDEX Data[0];
)

CLIENT_PROTOCOL(C2S, GET_CASH_BALANCE, DEFAULT, 2181,
)

CLIENT_PROTOCOL(S2C, GET_CASH_BALANCE, DEFAULT, 2182,
    UInt32 Amount;
)

CLIENT_PROTOCOL(S2C, NFY_WARP_HERO_LEAP, DEFAULT, 2202,
    UInt16 PositionX;
    UInt16 PositionY;
)

CLIENT_PROTOCOL(C2S, CANCEL_BATTLE_MODE_SYNERGY, DEFAULT, 2203,
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(S2C, NFY_CANCEL_BATTLE_MODE_SYNERGY, DEFAULT, 2204,
)

CLIENT_PROTOCOL(C2S, MESSAGE_SHOUT, DEFAULT, 2232,
    UInt32 Unknown1;
    UInt8 WorldSvrIdx;
    UInt32 Unknown2;
    UInt8 Unknown3;
    UInt16 PayloadLength;
    UInt8 Payload[0];
)

CLIENT_PROTOCOL(C2S, GUILD_CREATE, DEFAULT, 2233,
    Int32 Unknown1;
    Char GuildName[MAX_GUILDNAME_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, GUILD_CREATE, DEFAULT, 2233,
    Int32 Result;
    Int32 Unknown1;
    Char GuildName[MAX_GUILDNAME_LENGTH + 1];
    Int32 GuildIndex;
    Int64 GuildCurrency;
)

CLIENT_PROTOCOL(C2S, OPEN_AUCTION_HOUSE, DEFAULT, 2241,
)

CLIENT_PROTOCOL(S2C, NFY_ACHIEVEMENT_INFO, DEFAULT, 2224,
    UInt16 AchievementIndex;
    Int32 AchievementState;
    RTShortDate AchievementDate;
    UInt32 CategoryIndex;
    Int32 CategoryScore;
    UInt16 TitleIndex;
    UInt8 DialogType;
    Int32 Unknown1;
    UInt8 ForceGemReward;
)

CLIENT_PROTOCOL(S2C, NFY_CRAFT_ENERGY, DEFAULT, 2248,
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_REQUEST_CRAFT_REGISTER_TAIL,
    UInt8 Unknown2[4168];
)

CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_REGISTER, DEFAULT, 2249,
    UInt32 RequestCode;
    Int32 InventorySlotCount;
    struct _RTRequestCraftInventorySlot InventorySlots[0];
    // C2S_REQUEST_CRAFT_REGISTER_TAIL Tail; - Maybe AuthCaptcha
)

CLIENT_PROTOCOL(S2C, REQUEST_CRAFT_REGISTER, DEFAULT, 2249,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_START, DEFAULT, 2250,
    UInt32 RequestCode;
    UInt8 RequestSlotIndex;
    Int32 Unknown1; // - Maybe AuthCaptcha
    Int32 InventorySlotCount;
    struct _RTRequestCraftInventorySlot InventorySlots[0];
    // UInt8 Unknown2[4168]; - Maybe AuthCaptcha
)

CLIENT_PROTOCOL(S2C, REQUEST_CRAFT_START, DEFAULT, 2250,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_END, DEFAULT, 2251,
    UInt32 RequestCode;
    UInt8 RequestSlotIndex;
    Int32 Unknown1; // - Maybe AuthCaptcha
    Int32 InventorySlotCount;
    struct _RTRequestCraftInventorySlot InventorySlots[0];
    // UInt8 Unknown2[4168]; - Maybe AuthCaptcha
)

CLIENT_PROTOCOL(S2C, REQUEST_CRAFT_END, DEFAULT, 2251,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, REQUEST_CRAFT_UPDATE, DEFAULT, 2252,
    UInt32 ItemID;
    UInt8 RequestSlotIndex;
    UInt32 RequestCode;
    UInt32 RequestCraftExp;
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, GET_ACHIEVEMENT_TIMELINE, DEFAULT, 2253,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_ACHIEVEMENT_TIMELINE_INDEX,
    UInt8 Unknown1[18];
    Char CharacterName[MAX_CHARACTER_NAME_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, GET_ACHIEVEMENT_TIMELINE, DEFAULT, 2253,
    UInt32 CharacterCount;
    UInt32 AccountCount;
    UInt32 BuddyCount;
    UInt32 Unknown1[4];
    // S2C_DATA_GET_ACHIEVEMENT_TIMELINE_INDEX[CharacterCount];
    // S2C_DATA_GET_ACHIEVEMENT_TIMELINE_INDEX[AccountCount];
    // S2C_DATA_GET_ACHIEVEMENT_TIMELINE_INDEX[BuddyCount];
)

CLIENT_PROTOCOL(S2C, NFY_ACHIEVEMENT_EVENT, DEFAULT, 2255,
    UInt32 CharacterIndex;
    UInt16 AchivementIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_USER_MESSAGE_ROLL_DICE,
    UInt32 Value;
)

CLIENT_PROTOCOL(S2C, NFY_USER_MESSAGE, DEFAULT, 2256,
    UInt32 MessageType;
    UInt32 CharacterIndex;
    UInt8 NameLength;
    Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
    UInt8 Data[0];
)

CLIENT_PROTOCOL(C2S, ROLL_DICE, DEFAULT, 2261,
)

CLIENT_PROTOCOL(S2C, NFY_SKILL_REMOVE_BUFF, DEFAULT, 2307,
    Int32 EntityType;
    UInt32 EntityID;
    Int32 BuffType;
    Int32 BuffSlotIndex;
    Int32 SkillIndex;
)

CLIENT_PROTOCOL(C2S, GET_SHOP_ITEM_PRICE_POOL, DEFAULT, 2308,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_SHOP_ITEM_PRICE_POOL_INDEX,
    Int16 PoolIndex;
    RTItem ItemID;
    UInt64 ItemOptions;
    Int32 ItemCount;
)

CLIENT_PROTOCOL(S2C, GET_SHOP_ITEM_PRICE_POOL, DEFAULT, 2308,
    Int16 Count;
    S2C_DATA_GET_SHOP_ITEM_PRICE_POOL_INDEX Data[0];
)

CLIENT_PROTOCOL(C2S, UPGRADE_EXTREME_LEVEL, DEFAULT, 2309,
    UInt16 TargetSlotIndex;
    UInt16 InventorySlotCount;
    UInt16 PayloadLength;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_UPGRADE_EXTREME_LEVEL_RESULT_SUCCESS = 3,
    S2C_UPGRADE_EXTREME_LEVEL_RESULT_RESET = 4,
    S2C_UPGRADE_EXTREME_LEVEL_RESULT_BROKEN = 5,
    S2C_UPGRADE_EXTRENE_LEVEL_RESULT_INSUFFICIENT_CORES = 6,
    S2C_UPGRADE_EXTRENE_LEVEL_RESULT_INSUFFICIENT_CURRENCY = 7,
)

CLIENT_PROTOCOL(S2C, UPGRADE_EXTREME_LEVEL, DEFAULT, 2309,
    UInt8 Result;
    UInt64 Currency;
)

CLIENT_PROTOCOL(C2S, UPGRADE_EXTREME_REPAIR, DEFAULT, 2310,
    UInt16 TargetSlotIndex;
    UInt16 SourceSlotIndex;
)

CLIENT_PROTOCOL_ENUM(
    S2C_UPGRADE_EXTREME_REPAIR_RESULT_ERROR = 0,
    S2C_UPGRADE_EXTREME_REPAIR_RESULT_SUCCESS = 2,
    S2C_UPGRADE_EXTREME_REPAIR_RESULT_FAILED = 3,
    S2C_UPGRADE_EXTREME_REPAIR_RESULT_BROKEN = 4,
)

CLIENT_PROTOCOL(S2C, UPGRADE_EXTREME_REPAIR, DEFAULT, 2310,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, TAKE_NEWBIE_REWARD, DEFAULT, 2319,
    UInt8 CategoryType;
    UInt8 ConditionValue1;
    UInt8 ConditionValue2;
    UInt8 RewardIndex;
    UInt8 InventorySlotCount;
    UInt16 InventorySlotIndex[50];
)

CLIENT_PROTOCOL(S2C, TAKE_NEWBIE_REWARD, DEFAULT, 2319,
    UInt8 Success;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_MERGE_INVENTORY_SLOT_RESULT,
    UInt16 SlotIndex;
    UInt32 StackSize;
)

CLIENT_PROTOCOL(C2S, SPLIT_INVENTORY, DEFAULT, 2500,
    UInt8 Unknown1;
    UInt16 InventorySlotIndex;
    UInt16 SplitInventorySlotCount;
    UInt16 StackSize;
    UInt16 Unknown2;
    UInt16 PayloadLength;
    UInt16 SplitInventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, SPLIT_INVENTORY, DEFAULT, 2500,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, MERGE_INVENTORY, DEFAULT, 2501,
    UInt16 MergeInventorySlotCount;
    UInt16 ResultInventorySlotCount;
    UInt16 PayloadLength;
    UInt16 MergeInventorySlotIndex[0]; // MergeInventorySlotCount
    // C2S_DATA_MERGE_INVENTORY_SLOT_RESULT ResultInventorySlotIndex[ResultInventorySlotCount];
)

CLIENT_PROTOCOL(S2C, MERGE_INVENTORY, DEFAULT, 2501,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, GET_SETTINGS, DEFAULT, 2502,
)

CLIENT_PROTOCOL(S2C, GET_SETTINGS, DEFAULT, 2502,
    UInt32 HotKeysDataLength;
    UInt32 OptionsDataLength;
    UInt32 MacrosDataLength;
    UInt8 Data[0]; // HotKeys, Settings, Macros
)

CLIENT_PROTOCOL(C2S, SET_SETTINGS_HOTKEYS, DEFAULT, 2503,
    UInt32 DataLength;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(C2S, SET_SETTINGS_OPTIONS, DEFAULT, 2504,
    UInt32 DataLength;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(C2S, SET_SETTINGS_MACROS, DEFAULT, 2505,
    UInt32 DataLength;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, UPDATE_UPGRADE_POINTS, DEFAULT, 2506,
    struct _RTCharacterUpgradeInfo UpgradeInfo;
)

CLIENT_PROTOCOL(S2C, NFY_BUDDY_POINTS, DEFAULT, 2518,
    UInt32 Unknown1[6];
)

CLIENT_PROTOCOL(C2S, GET_DUNGEON_REWARD_LIST, DEFAULT, 2520,
    UInt32 DungeonID;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY_INDEX,
    UInt8 Unknown1;
    UInt32 ItemID;
    UInt32 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
    UInt32 Unknown5;
    UInt8 Unknown6;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY,
    UInt8 Position;
    UInt32 RepeatCount;
    UInt32 RewardCount;
    // S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, GET_DUNGEON_REWARD_LIST, DEFAULT, 2520,
    UInt32 PayloadLength;
    UInt32 DungeonID;
    UInt16 Unknown1;
    UInt16 Unknown2;
    // S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY Data[0];
)

CLIENT_PROTOCOL(S2C, NFY_DUNGEON_COMPLETE_INFO, DEFAULT, 2521,
    UInt32 PayloadLength;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL(C2S, GET_DUNGEON_EXPIRATION_TIME, DEFAULT, 2522,
)

CLIENT_PROTOCOL(S2C, GET_DUNGEON_EXPIRATION_TIME, DEFAULT, 2522,
    UInt8 Unknown1;
    UInt32 Unknown2[4];
)

CLIENT_PROTOCOL(S2C, NFY_GUILD_RANKING_WEXP_BONUS, DEFAULT, 2528,
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, UPGRADE_CHAOS_LEVEL, DEFAULT, 2534,
    Int32 Unknown1;
    UInt16 InventorySlotIndex;
    UInt16 CoreCount;
    UInt16 SafeCount;
    UInt16 CoreSlotIndices[0];
)

CLIENT_PROTOCOL(S2C, UPGRADE_CHAOS_LEVEL, DEFAULT, 2534,
    UInt8 Result;
    UInt8 ConsumedSafeCount;
    Int32 RemainingCoreCount;
)

CLIENT_PROTOCOL(S2C, NFY_DUNGEON_TIMER, DEFAULT, 2542,
    UInt32 TimerType;
    UInt32 Interval;
)

CLIENT_PROTOCOL(C2S, MERIT_MEDAL_EVALUATION, DEFAULT, 2543,
    Int16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, MERIT_MEDAL_EVALUATION, DEFAULT, 2543,
    UInt8 Success;
    UInt64 ItemID;
    UInt64 ItemOptions;
)

CLIENT_PROTOCOL(C2S, MERIT_MEDAL_RESET, DEFAULT, 2544,
    Int16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, MERIT_MEDAL_RESET, DEFAULT, 2544,
    UInt8 Success;
    UInt64 ItemID;
    UInt64 ItemOptions;
)

CLIENT_PROTOCOL(C2S, MERIT_MEDAL_REGISTRATION, DEFAULT, 2545,
    UInt8 InventorySlotCount;
    UInt32 PayloadLength;
    UInt32 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, MERIT_MEDAL_REGISTRATION, DEFAULT, 2545,
    UInt8 Success;
    UInt32 GoldMeritPoints;
    UInt32 PlatinumMeritPoints;
    UInt32 DiamondMeritPoints;
)

CLIENT_PROTOCOL(C2S, MERIT_MASTERY_TRAIN, DEFAULT, 2547,
    Int32 MasteryIndex;
    Int32 TargetLevel;
)

CLIENT_PROTOCOL(S2C, MERIT_MASTERY_TRAIN, DEFAULT, 2547,
    UInt8 Success;
    UInt32 MeritPoints;
)

CLIENT_PROTOCOL_ENUM(
    S2C_NFY_DUNGEON_TIMER_INFO_TYPE_DROP = 0,
    S2C_NFY_DUNGEON_TIMER_INFO_TYPE_MOB = 1,
)

CLIENT_PROTOCOL(S2C, NFY_DUNGEON_TIMER_INFO, DEFAULT, 2552,
    UInt8 Type;
    union {
        struct { UInt64 ItemID; UInt16 ItemCount; } Drop;
        struct { UInt32 Unknown1; UInt64 CurrentHP; UInt64 MaxHP; } Mob;
    };
)

CLIENT_PROTOCOL(C2S, UNKNOWN_2559, DEFAULT, 2559,
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_2559, DEFAULT, 2559,
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(C2S, UNKNOWN_2566, DEFAULT, 2566,
)

CLIENT_PROTOCOL(S2C, UNKNOWN_2566, DEFAULT, 2566,
    UInt16 Unknown1;
)

CLIENT_PROTOCOL_ENUM(
    CSC_PURCHASE_PREMIUM_SERVICE_TYPE_PARTY_MEMBER_WARP     = 4,
    CSC_PURCHASE_PREMIUM_SERVICE_TYPE_HONOR_MEDAL_RESET     = 17,
    CSC_PURCHASE_PREMIUM_SERVICE_TYPE_GIFTBOX_COMPLETE      = 23,
    CSC_PURCHASE_PREMIUM_SERVICE_TYPE_MYTH_RESURRECT        = 32,
    CSC_PURCHASE_PREMIUM_SERVICE_TYPE_ANIMAMASTERY_RESET    = 41,
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_PURCHASE_PREMIUM_SERVICE_GIFTBOX,
    Int32 SlotIndex;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_PURCHASE_PREMIUM_SERVICE_HONOR_MEDAL_RESET,
    Int32 GroupIndex;
)

CLIENT_PROTOCOL_STRUCT(CSC_DATA_PURCHASE_PREMIUM_SERVICE_TYPE_MYTH_RESURRECT,
    Int32 MythLevel;
    Int32 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(C2S, PURCHASE_PREMIUM_SERVICE, DEFAULT, 2569,
    Int32 ServiceType;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, PURCHASE_PREMIUM_SERVICE, DEFAULT, 2569,
    Int32 Result;
    Int32 ServiceType;
    Int32 ForceGemCount;
    Int32 Unknown1;
)

CLIENT_PROTOCOL(C2S, GET_FORCEGEM_PRICE_POOL, DEFAULT, 2571,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_FORCEGEM_PRICE,
    Int32 PoolIndex;
    Int32 PriceIndex; 
    Int32 Price;
)

CLIENT_PROTOCOL(S2C, GET_FORCEGEM_PRICE_POOL, DEFAULT, 2571,
    UInt16 Count;
    S2C_DATA_GET_FORCEGEM_PRICE Data[0];
)

CLIENT_PROTOCOL(C2S, GET_CASHSHOP_LIST, DEFAULT, 2627,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_CASHSHOP_LIST_GROUP_INDEX,
    UInt32 Unknown[3];
)

CLIENT_PROTOCOL(S2C, GET_CASHSHOP_LIST, DEFAULT, 2627,
    UInt8 IsActive;
    UInt16 GroupCount;
    S2C_DATA_GET_CASHSHOP_LIST_GROUP_INDEX Groups[0];
)

CLIENT_PROTOCOL(C2S, CHANGE_CHARACTER_SKILL_LEVEL, DEFAULT, 2646,
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt16 CurrentSkillLevel;
    UInt16 TargetSkillLevel;
)

CLIENT_PROTOCOL(S2C, CHANGE_CHARACTER_SKILL_LEVEL, DEFAULT, 2646,
)

CLIENT_PROTOCOL(C2S, ADD_CHARACTER_STATS, DEFAULT, 2648,
    Int32 Stat[3];
    UInt32 StatDelta[3];
)

CLIENT_PROTOCOL(S2C, ADD_CHARACTER_STATS, DEFAULT, 2648,
    Int32 Result;
    UInt32 Stat[3];
)

CLIENT_PROTOCOL(C2S, REMOVE_CHARACTER_STATS, DEFAULT, 2649,
    Int32 Stat[3];
    Int32 StatDelta[3];
    UInt8 StatFlag[3];
    UInt16 InventorySlots[0];
)

CLIENT_PROTOCOL(S2C, REMOVE_CHARACTER_STATS, DEFAULT, 2649,
    Int32 Result;
    UInt32 Stat[3];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_LOOT_GROUP_ITEM_INDEX,
    RTEntityID Entity;
    UInt16 UniqueKey;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(C2S, LOOT_GROUP_ITEM, DEFAULT, 2753,
    UInt8 ItemCount;
    C2S_DATA_LOOT_GROUP_ITEM_INDEX Items[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_LOOT_GROUP_ITEM_RESULT_SUCCESS = 96,
    S2C_DATA_LOOT_GROUP_ITEM_RESULT_OWNERSHIP_ERROR = 97,
    S2C_DATA_LOOT_GROUP_ITEM_RESULT_TIMEOUT_ERROR = 100,
    S2C_DATA_LOOT_GROUP_ITEM_RESULT_DISTANCE_ERROR = 101,
    S2C_DATA_LOOT_GROUP_ITEM_RESULT_PARTY_LEADER_UNAVAILABLE_ERROR = 106,
    S2C_DATA_LOOT_GROUP_ITEM_RESULT_PARTY_BINDING_ERROR = 107,
    S2C_DATA_LOOT_GROUP_ITEM_RESULT_PARTY_LEADER_LOOTING_ERROR = 108,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_LOOT_GROUP_ITEM_INDEX,
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 InventorySlotIndex;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, LOOT_GROUP_ITEM, DEFAULT, 2753,
    Int32 Result;
    UInt8 ItemCount;
    S2C_DATA_LOOT_GROUP_ITEM_INDEX Items[0];
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_2756, DEFAULT, 2756,
    UInt32 Unknown1[9];
)

CLIENT_PROTOCOL(C2S, ADD_OVERLORD_MASTERY_SLOT, DEFAULT, 2763,
    UInt8 MasteryIndex;
)

CLIENT_PROTOCOL(S2C, ADD_OVERLORD_MASTERY_SLOT, DEFAULT, 2763,
    UInt8 MasteryIndex;
    UInt8 Level;
)

CLIENT_PROTOCOL(C2S, DIVINE_UPGRADE_ITEM_LEVEL, DEFAULT, 2765,
    UInt16 InventorySlotIndex;
    UInt16 CoreCount;
    UInt16 SafeCount;
    UInt16 CoreSlotIndices[0];
)

CLIENT_PROTOCOL(S2C, DIVINE_UPGRADE_ITEM_LEVEL, DEFAULT, 2765,
    UInt8 Result;
    UInt8 ResultLevel;
    UInt8 ConsumedSafeguardCount;
    UInt32 RemainingSafeguardCount;
)

CLIENT_PROTOCOL(C2S, DIVINE_UPGRADE_SEAL, DEFAULT, 2768,
    Int16 TargetSlotIndex;
    Int16 SourceSlotIndex;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DIVINE_UPGRADE_SEAL_RESULT_ERROR    = 0,
    S2C_DIVINE_UPGRADE_SEAL_RESULT_SEAL     = 1,
    S2C_DIVINE_UPGRADE_SEAL_RESULT_UNSEAL   = 2,
)

CLIENT_PROTOCOL(S2C, DIVINE_UPGRADE_SEAL, DEFAULT, 2768,
    UInt8 Result;
    UInt64 ResultSerial;
)

CLIENT_PROTOCOL(C2S, EXTREME_UPGRADE_SEAL, DEFAULT, 2769,
    Int32 TargetSlotIndex;
    Int32 SourceSlotIndex;
)

CLIENT_PROTOCOL(S2C, EXTREME_UPGRADE_SEAL, DEFAULT, 2769,
    Int32 IsUnsealing;
    RTItem ItemID;
)

CLIENT_PROTOCOL(C2S, HONOR_MEDAL_UNLOCK_SLOT, DEFAULT, 2770,
    UInt8 CategoryIndex;
    UInt8 GroupIndex;
    UInt32 SlotIndex;
    UInt16 MaterialSlotCount1;
    UInt16 MaterialSlotCount2;
    UInt16 MaterialSlotIndex[0]; // MaterialSlotCount1 + MaterialSlotCount2
)

CLIENT_PROTOCOL(S2C, HONOR_MEDAL_UNLOCK_SLOT, DEFAULT, 2770,
    UInt8 CategoryIndex;
    UInt8 GroupIndex;
    UInt32 SlotIndex;
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, HONOR_MEDAL_ROLL_SLOT, DEFAULT, 2771,
    UInt8 CategoryIndex;
    UInt8 GroupIndex;
    UInt32 SlotIndex;
)

CLIENT_PROTOCOL(S2C, HONOR_MEDAL_ROLL_SLOT, DEFAULT, 2771,
    UInt8 CategoryIndex;
    UInt8 GroupIndex;
    UInt32 SlotIndex;
    UInt32 Unknown1;
    UInt32 ForceEffectIndex;
    Int64 WExp;
    Int16 AP;
)

CLIENT_PROTOCOL(C2S, HONOR_MEDAL_SEALING, DEFAULT, 2772,
    Int8 GroupIndex;
    UInt32 InventorySlotIndex;
)

CLIENT_PROTOCOL_ENUM(
    S2C_CHAOS_HONOR_MEDAL_SEAL_RESULT_ERROR = 0,
    S2C_CHAOS_HONOR_MEDAL_SEAL_RESULT_SEAL = 1,
    S2C_CHAOS_HONOR_MEDAL_SEAL_RESULT_UNSEAL = 2,
)

CLIENT_PROTOCOL(S2C, HONOR_MEDAL_SEALING, DEFAULT, 2772,
    Int32 Result;
    UInt64 ItemSerial;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_USE_ITEM_SAVER_INVENTORY_SLOT,
    Int16 InventorySlotIndex;
    Int16 Amount;
)

CLIENT_PROTOCOL(C2S, USE_ITEM_SAVER, DEFAULT, 2773,
    UInt16 InventorySlotCount;
    C2S_DATA_USE_ITEM_SAVER_INVENTORY_SLOT InventorySlots[0]; // InventorySlotCount
)

CLIENT_PROTOCOL(C2S, CHAOS_UPGRADE_SEAL, DEFAULT, 2787,
    Int16 TargetSlotIndex;
    Int16 SourceSlotIndex;
)

CLIENT_PROTOCOL_ENUM(
    S2C_CHAOS_UPGRADE_SEAL_RESULT_ERROR     = 0,
    S2C_CHAOS_UPGRADE_SEAL_RESULT_SEAL      = 1,
    S2C_CHAOS_UPGRADE_SEAL_RESULT_UNSEAL    = 2,
)

CLIENT_PROTOCOL(S2C, CHAOS_UPGRADE_SEAL, DEFAULT, 2787,
    UInt8 Result;
    UInt64 ResultSerial;
)

CLIENT_PROTOCOL(S2C, NFY_FORCE_WING_GRADE, DEFAULT, 2788,
    UInt32 CharacterIndex;
    UInt8 ForceWingGrade;
)

CLIENT_PROTOCOL(S2C, NFY_FORCE_WING_UPDATE, DEFAULT, 2789,
    UInt8 Status;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    Int64 ForceWingExp;
    Int64 Unknown1;
    UInt32 TrainingPointCount;
)

CLIENT_PROTOCOL(C2S, FORCE_WING_LEVEL_UP, DEFAULT, 2790,
    Int32 ItemStackCount1;
    UInt16 InventorySlotCount1;
    Int32 ItemStackCount2;
    UInt16 InventorySlotCount2;
    UInt16 InventorySlotIndex[0];
//    UInt16 InventorySlotIndex2[0];
)

CLIENT_PROTOCOL(S2C, FORCE_WING_LEVEL_UP, DEFAULT, 2790,
    UInt8 Success;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    Int64 ForceWingExp;
    Int64 Unknown1;
    UInt32 AddedTrainingPointCount;
)

CLIENT_PROTOCOL(S2C, NFY_FORCE_WING_EXP, DEFAULT, 2791,
    Int64 ForceWingExp;
)

CLIENT_PROTOCOL(C2S, SET_FORCEWING_PRESET_SLOT, DEFAULT, 2792,
    UInt8 PresetPageIndex;
    UInt8 PresetSlotIndex;
    UInt8 TrainingSlotIndex;
)

CLIENT_PROTOCOL(S2C, SET_FORCEWING_PRESET_SLOT, DEFAULT, 2792,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, ADD_FORCEWING_TRAINING_LEVEL, DEFAULT, 2793,
    UInt8 PresetPageIndex;
    UInt8 PresetSlotIndex;
    UInt8 TrainingSlotIndex;
    Int32 AddedTrainingLevel;
)

CLIENT_PROTOCOL(S2C, ADD_FORCEWING_TRAINING_LEVEL, DEFAULT, 2793,
    Int32 Result;
    Int32 RemainingTrainingPointCount;
    UInt8 TrainingLevel;
)

CLIENT_PROTOCOL(C2S, SET_ACTIVE_FORCEWING_PRESET, DEFAULT, 2794,
    UInt8 PresetPageIndex;
)

CLIENT_PROTOCOL(S2C, SET_ACTIVE_FORCEWING_PRESET, DEFAULT, 2794,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, OPEN_GIFTBOX_ROLL, DEFAULT, 2797,
    UInt8 SlotIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_OPEN_GIFTBOX_ROLL_SLOT,
    UInt8 SlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
)

CLIENT_PROTOCOL(S2C, OPEN_GIFTBOX_ROLL, DEFAULT, 2797,
    UInt8 Count;
    S2C_DATA_OPEN_GIFTBOX_ROLL_SLOT Data[0];
)

CLIENT_PROTOCOL(C2S, SORT_INVENTORY, DEFAULT, 2800,
    UInt16 Count;
    UInt16 InventorySlots[1024];
)

CLIENT_PROTOCOL(S2C, SORT_INVENTORY, DEFAULT, 2800,
    UInt8 Success;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GIFTBOX_PRICE_POOL,
    Int32 SlotIndex;
    Int32 PriceCount;
    // Int32 PriceList[0];
)

CLIENT_PROTOCOL(S2C, GIFTBOX_PRICE_POOL_LIST, DEFAULT, 2801,
    Int32 Count;
    S2C_DATA_GIFTBOX_PRICE_POOL Data[0];
)

CLIENT_PROTOCOL(C2S, SORT_WAREHOUSE, DEFAULT, 2802,
    UInt16 Count;
    UInt16 WarehouseSlots[2048];
)

CLIENT_PROTOCOL(S2C, SORT_WAREHOUSE, DEFAULT, 2802,
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, GUILD_DUNGEON_ENTRANCE_COUNT, DEFAULT, 2808,
)

CLIENT_PROTOCOL(S2C, GUILD_DUNGEON_ENTRANCE_COUNT, DEFAULT, 2808,
    Int32 DungeonIndex;
    UInt8 Unknown1[26];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_OPEN_GIFTBOX_RECEIVE_SLOT,
    UInt16 InventorySlotIndex;
    UInt16 StackSize;
)

CLIENT_PROTOCOL(C2S, OPEN_GIFTBOX_RECEIVE, DEFAULT, 2819,
    UInt8 SlotIndex;
    UInt16 Count;
    C2S_DATA_OPEN_GIFTBOX_RECEIVE_SLOT Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_OPEN_GIFTBOX_RECEIVE_ITEMSLOT,
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 InventorySlotIndex;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_OPEN_GIFTBOX_RECEIVE_SLOT,
    UInt8 SlotIndex;
    UInt64 ElapsedTimeInSeconds;
    UInt64 TotalTimeInSeconds;
    UInt8 ItemCount;
    //S2C_DATA_OPEN_GIFTBOX_RECEIVE_ITEMSLOT Data[0];
)

CLIENT_PROTOCOL(S2C, OPEN_GIFTBOX_RECEIVE, DEFAULT, 2819,
    UInt8 Count;
    //S2C_DATA_OPEN_GIFTBOX_RECEIVE_SLOT Data[0];
)

CLIENT_PROTOCOL(C2S, UNKNOWN_2828, DEFAULT, 2828,
)

CLIENT_PROTOCOL(S2C, UNKNOWN_2828, DEFAULT, 2828,
    UInt8 Unknown1;
    UInt32 Unknown2[5];
)

CLIENT_PROTOCOL(C2S, REGISTER_COLLECTION_ITEM, DEFAULT, 2830,
    UInt8 TypeID;
    UInt8 CollectionID;
    UInt16 Unknown1;
    UInt16 MissionID;
    UInt8 MissionSlotIndex;
    UInt32 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, REGISTER_COLLECTION_ITEM, DEFAULT, 2830,
    UInt8 Success;
    UInt16 MissionID2;
    struct _RTCollectionSlot CollectionSlot;
)

CLIENT_PROTOCOL(C2S, RECEIVE_COLLECTION_REWARD, DEFAULT, 2831,
    UInt8 TypeID;
    UInt8 CollectionID;
    UInt16 Unknown1;
    UInt16 MissionID;
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_RECEIVE_COLLECTION_REWARD_CURRENCY,
    UInt32 Currency;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_RECEIVE_COLLECTION_REWARD_ITEM,
    RTItem ItemID;
    UInt32 Unknown5;
    UInt32 Unknown6;
    UInt64 ItemOptions;
    UInt16 InventorySlotIndex;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL(S2C, RECEIVE_COLLECTION_REWARD, DEFAULT, 2831,
    UInt8 Success;
    UInt16 Unknown1;
    UInt32 RewardType;
    UInt16 InventorySlotCount;
)

CLIENT_PROTOCOL(C2S, MERIT_MASTERY_GRADE_UP, DEFAULT, 2832,
    UInt16 InventorySlotCount;
    UInt16 InventorySlots[0];
)

CLIENT_PROTOCOL(S2C, MERIT_MASTERY_GRADE_UP, DEFAULT, 2832,
    Int8 Result;
)

CLIENT_PROTOCOL(C2S, MERIT_MASTERY_OPEN_SLOT, DEFAULT, 2833,
    Int32 MasteryIndex;
    UInt16 MaterialSlotCount1;
    UInt16 MaterialSlotCount2;
    UInt16 MaterialSlotIndex[0]; // MaterialSlotCount1 + MaterialSlotCount2
)

CLIENT_PROTOCOL(S2C, MERIT_MASTERY_OPEN_SLOT, DEFAULT, 2833,
    Int8 Result;
    Int32 MasteryIndex;
    Timestamp UnlockTime;
)

CLIENT_PROTOCOL(C2S, MERIT_MASTERY_OPEN_SLOT_CANCEL, DEFAULT, 2835,
    Int32 MasteryIndex;
)

CLIENT_PROTOCOL(S2C, MERIT_MASTERY_OPEN_SLOT_CANCEL, DEFAULT, 2835,
    Int8 Result;
)

CLIENT_PROTOCOL_STRUCT(C2S_FORCE_WING_INVENTORY_SLOT,
    UInt16 ItemStackSize;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(C2S, FORCE_WING_GRADE_UP, DEFAULT, 2837,
    UInt16 InventorySlotCount;
    C2S_FORCE_WING_INVENTORY_SLOT InventorySlots[0];
)

CLIENT_PROTOCOL(S2C, FORCE_WING_GRADE_UP, DEFAULT, 2837,
    UInt8 Success;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    Int64 ForceWingExp;
    Int64 Unknown1;
    UInt32 AddedTrainingPointCount;
)

CLIENT_PROTOCOL(C2S, MERIT_MASTERY_SET_PAGE_INDEX, DEFAULT, 2838,
    UInt8 MemorizeIndex;
)

CLIENT_PROTOCOL(S2C, MERIT_MASTERY_SET_PAGE_INDEX, DEFAULT, 2838,
    UInt8 Result;
    Int32 MeritPoints;
)

CLIENT_PROTOCOL(C2S, MERIT_MASTERY_GRANT_SPECIAL_MASTERY, DEFAULT, 2839,
    Int32 CategoryIndex;
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, MERIT_MASTERY_GRANT_SPECIAL_MASTERY, DEFAULT, 2839,
    UInt8 Result;
    struct _RTPlatinumMeritSpecialMasterySlot SpecialMasterySlots[2];
)

CLIENT_PROTOCOL_ENUM(
    C2S_DATA_MERIT_MASTERY_SEAL_SPECIAL_MASTERY_RESULT_FAIL           = 0,
    C2S_DATA_MERIT_MASTERY_SEAL_SPECIAL_MASTERY_RESULT_SUCCESS_SEAL   = 1,
    C2S_DATA_MERIT_MASTERY_SEAL_SPECIAL_MASTERY_RESULT_SUCCESS_UNSEAL = 2,
)

CLIENT_PROTOCOL(C2S, MERIT_MASTERY_SEAL_SPECIAL_MASTERY, DEFAULT, 2840,
    UInt16 InventorySlotIndex;
    UInt8 MemorizeIndex;
    Int32 CategoryIndex;
)

CLIENT_PROTOCOL(S2C, MERIT_MASTERY_SEAL_SPECIAL_MASTERY, DEFAULT, 2840,
    UInt8 Result;
    UInt64 ItemOptions;
    struct _RTPlatinumMeritSpecialMasterySlot SpecialMasterySlots[2];
)

CLIENT_PROTOCOL(S2C, NFY_UPDATE_TRANSCENDENCE_POINTS, DEFAULT, 2926,
    UInt32 TranscendencePoints;
)

CLIENT_PROTOCOL(C2S, STELLAR_LINK_IMPRINT_SLOT, DEFAULT, 2936,
    UInt8 GroupID;
    UInt8 SlotLine;
    UInt8 SlotIndex;
    UInt32 MaterialCount;
    UInt16 MaterialSlotCount1;
    UInt16 MaterialSlotCount2;
    UInt16 MaterialSlotIndex[0]; // MaterialSlotCount1 + MaterialSlotCount2
)

CLIENT_PROTOCOL(S2C, STELLAR_LINK_IMPRINT_SLOT, DEFAULT, 2936,
    UInt8 GroupID;
    UInt8 SlotLine;
    UInt8 SlotIndex;
    UInt8 StellarLinkGrade;
    UInt32 StellarForceEffect; // See RuntimeLib/Force.h
    UInt32 StellarForceValue;
    UInt32 StellarForceValueType;
    UInt32 ErrorCode;
)

CLIENT_PROTOCOL(C2S, STELLAR_LINK_TRANSFORM_LINK, DEFAULT, 2937,
    UInt8 GroupID;
    UInt8 SlotLine;
    UInt8 SlotIndex;
    UInt8 StellarLinkGrade;
    UInt16 MaterialSlotCount1;
    UInt16 MaterialSlotCount2;
    UInt16 MaterialSlotIndex[0]; // MaterialSlotCount1 + MaterialSlotCount2
)

CLIENT_PROTOCOL(S2C, STELLAR_LINK_TRANSFORM_LINK, DEFAULT, 2937,
    UInt8 GroupID;
    UInt8 SlotLine;
    UInt8 SlotIndex;
    UInt8 StellarLinkGrade;
    UInt32 StellarForceEffect; // See RuntimeLib/Force.h
    Int32 StellarForceValue;
    UInt32 StellarForceValueType;
    UInt32 ErrorCode;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_DUNGEON_DROP_LIST_ITEM,
    UInt32 CharacterIndex;
    UInt8 CharacterNameLength;
    Char CharacterName[MAX_CHARACTER_NAME_LENGTH];
    UInt8 Unknown1;
    UInt64 ItemID;
    UInt64 ItemOptions;
    Timestamp Timestamp;
)

CLIENT_PROTOCOL(S2C, NFY_DUNGEON_DROP_LIST, DEFAULT, 2849,
    Int32 DungeonIndex;
    S2C_DATA_NFY_DUNGEON_DROP_LIST_ITEM Items[MAX_DUNGEON_DROP_LIST_ITEM_COUNT];
)

CLIENT_PROTOCOL(C2S, MYTH_ROLL_SLOT, DEFAULT, 2959,
    UInt8 MasteryIndex;
    UInt8 SlotIndex;
)

CLIENT_PROTOCOL(S2C, MYTH_ROLL_SLOT, DEFAULT, 2959,
    UInt32 MythPoints;
    UInt32 HolyPower;
    struct _RTMythMasterySlot MasterySlot;
    UInt32 StigmaGrade;
    UInt32 StigmaXP;
    UInt32 ErrorCode;
)

CLIENT_PROTOCOL(C2S, MYTH_FINISH_ROLL_SLOT, DEFAULT, 2960,
    UInt8 IsRollback; //1 = revert to temp slot
    UInt8 MasteryIndex;
    UInt8 SlotIndex;
    UInt32 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, MYTH_FINISH_ROLL_SLOT, DEFAULT, 2960,
    struct _RTMythMasterySlot MasterySlot;
    UInt32 HolyPower;
    UInt32 ErrorCode;
)

CLIENT_PROTOCOL(C2S, MYTH_OPEN_LOCK, DEFAULT, 2961,
    UInt8 MasteryIndex;
    UInt8 LockGroupIndex;
)

CLIENT_PROTOCOL(S2C, MYTH_OPEN_LOCK, DEFAULT, 2961,
    UInt8 MasteryIndex;
    UInt8 LockGroupIndex;
    UInt32 ErrorCode; //not 100% but fairly confident
)

CLIENT_PROTOCOL(S2C, NFY_MYTH_XP, DEFAULT, 2963,
    UInt64 MythXP;
)

CLIENT_PROTOCOL(C2S, ROLL_FORCEWING_ARRIVAL_SKILL, DEFAULT, 2965,
    UInt8 SkillSlotIndex;
    UInt32 Unknown1;
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, ROLL_FORCEWING_ARRIVAL_SKILL, DEFAULT, 2965,
    Int32 Result;
    struct _RTForceWingArrivalSkillSlot SkillSlot;
)

CLIENT_PROTOCOL(C2S, CHANGE_FORCEWING_ARRIVAL_SKILL, DEFAULT, 2966,
)

CLIENT_PROTOCOL(S2C, CHANGE_FORCEWING_ARRIVAL_SKILL, DEFAULT, 2966,
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, SET_FORCEWING_TRAINING_SLOT_FLAGS, DEFAULT, 2968,
    UInt8 Flags[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_SIZE];
)

CLIENT_PROTOCOL(C2S, GET_SHOP_ROTATION, DEFAULT, 2993,
)

CLIENT_PROTOCOL(S2C, GET_SHOP_ROTATION, DEFAULT, 2993,
    UInt8 Result;
    Int64 Rotations[4];
)
CLIENT_PROTOCOL(C2S, GET_EVENT_PASS_REWARD_LIST, DEFAULT, 2999,
    UInt8 Result;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_EVENT_PASS_REWARD_LIST_SLOT,
    Int32 EventPassLevel;
    Int32 EventPassType;
    RTItem Item;
    UInt64 ItemDuration;
    Int32 ItemAmount;
    Int32 Unknown2;
    Int32 Unknown3;
)

CLIENT_PROTOCOL(S2C, GET_EVENT_PASS_REWARD_LIST, DEFAULT, 2999,
    UInt32 Unknown1;
    UInt32 RewardSlotCount;
    S2C_DATA_GET_EVENT_PASS_REWARD_LIST_SLOT RewardSlots[1];
)

CLIENT_PROTOCOL(C2S, GET_EVENT_PASS_SEASON, DEFAULT, 3000,
)

CLIENT_PROTOCOL(S2C, GET_EVENT_PASS_SEASON, DEFAULT, 3000,
    UInt32 ExpirationDate;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_PASS_PROGRESS, DEFAULT, 3001,
    UInt32 GroupID;
    UInt32 MissionID;
    UInt32 Progress;
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_PASS_RESUME, DEFAULT, 3002,
    UInt32 Unknown1;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL_ENUM(
    C2S_SET_ITEM_PROTECTION_STORAGE_TYPE_INVENTORY = 0,
    C2S_SET_ITEM_PROTECTION_STORAGE_TYPE_EQUIPMENT = 2,
)

CLIENT_PROTOCOL(C2S, SET_ITEM_PROTECTION, DEFAULT, 3011,
    UInt8 StorageType;
    UInt16 InventorySlotIndex;
    UInt16 Unknown2;
    UInt8 IsLocked;
)

CLIENT_PROTOCOL(S2C, SET_ITEM_PROTECTION, DEFAULT, 3011,
    Int32 Success;
)

CLIENT_PROTOCOL(C2S, COSTUME_WAREHOUSE_REGISTER, DEFAULT, 3017,
    Int32 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, COSTUME_WAREHOUSE_REGISTER, DEFAULT, 3017,
    Int32 Success;
    Int32 MillagePoints;
)

CLIENT_PROTOCOL(C2S, COSTUME_WAREHOUSE_APPLY, DEFAULT, 3018,
    Int32 PresetIndex;
    UInt32 ItemIDs[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT];
)

CLIENT_PROTOCOL(S2C, COSTUME_WAREHOUSE_APPLY, DEFAULT, 3018,
    Int32 Success;
)

CLIENT_PROTOCOL(C2S, COSTUME_WAREHOUSE_LIST, DEFAULT, 3019,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_COSTUME_WAREHOUSE_ITEM,
    UInt32 ItemID;
    Int32 PriceMillage;
    Int32 PriceForceGem;
    Int32 RewardMillage;
)

CLIENT_PROTOCOL(S2C, COSTUME_WAREHOUSE_LIST, DEFAULT, 3019,
    Int32 IsActive;
    Int32 Unknown1;
    Int32 ItemCount;
    S2C_DATA_COSTUME_WAREHOUSE_ITEM Items[0];
)

CLIENT_PROTOCOL(C2S, COSTUME_WAREHOUSE_SHOPPING, DEFAULT, 3020,
    UInt32 ItemID;
)

CLIENT_PROTOCOL(S2C, COSTUME_WAREHOUSE_SHOPPING, DEFAULT, 3020,
    Int32 Result;
)

CLIENT_PROTOCOL_ENUM(
    C2S_DATA_COSTUME_WAREHOUSE_PURCHASE_PRICE_TYPE_MILLAGE = 0,
    C2S_DATA_COSTUME_WAREHOUSE_PURCHASE_PRICE_TYPE_FORCEGEM = 1,
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_COSTUME_WAREHOUSE_PURCHASE_SLOT,
    UInt32 ItemID;
    UInt8 PriceType;
)

CLIENT_PROTOCOL(C2S, COSTUME_WAREHOUSE_PURCHASE, DEFAULT, 3021,
    UInt8 Unknown1;
    Int32 ItemCount;
    C2S_DATA_COSTUME_WAREHOUSE_PURCHASE_SLOT ItemSlots[0];
//    UInt32 Unknown2;
//    UInt32 Unknown3;
)

CLIENT_PROTOCOL(S2C, COSTUME_WAREHOUSE_PURCHASE, DEFAULT, 3021,
    Int32 Success;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_3016, DEFAULT, 3016,
    UInt8 Unknown1;
    Timestamp UnknownTimestamp1;
    Timestamp UnknownTimestamp2;
)

CLIENT_PROTOCOL(C2S, MULTI_CONVERT_ITEM, DEFAULT, 3024,
    Int32 SourceItemCount;
    Int32 TargetItemCount;
    UInt16 SourceInventorySlots[0];
    // UInt16 TargetInventorySlots[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_MULTI_CONVERT_ITEM_SLOT_INDEX,
    UInt64 ItemID;
    UInt64 Unknown1;
    UInt64 ItemOptions;
    UInt16 InventorySlotIndex;
    UInt32 Unnown3;
)

CLIENT_PROTOCOL(S2C, MULTI_CONVERT_ITEM, DEFAULT, 3024,
    Int32 Result;
    Int32 ResultSlotCount;
    S2C_MULTI_CONVERT_ITEM_SLOT_INDEX ResultSlots[0];
)

CLIENT_PROTOCOL(C2S, DIAMOND_MERIT_MASTERY_GRADE_UP, DEFAULT, 3034,
    UInt16 InventorySlotCount;
    UInt16 InventorySlots[0];
)

CLIENT_PROTOCOL(S2C, DIAMOND_MERIT_MASTERY_GRADE_UP, DEFAULT, 3034,
    Int8 Result;
)

CLIENT_PROTOCOL(C2S, DIAMOND_MERIT_MASTERY_OPEN_SLOT, DEFAULT, 3035,
    Int32 MasteryIndex;
    UInt16 MaterialSlotCount1;
    UInt16 MaterialSlotCount2;
    UInt16 MaterialSlotIndex[0]; // MaterialSlotCount1 + MaterialSlotCount2
)

CLIENT_PROTOCOL(S2C, DIAMOND_MERIT_MASTERY_OPEN_SLOT, DEFAULT, 3035,
    Int8 Result;
    Int32 MasteryIndex;
    Timestamp UnlockTime;
)

CLIENT_PROTOCOL(C2S, DIAMOND_MERIT_MASTERY_SET_PAGE_INDEX, DEFAULT, 3039,
    UInt8 MemorizeIndex;
)

CLIENT_PROTOCOL(S2C, DIAMOND_MERIT_MASTERY_SET_PAGE_INDEX, DEFAULT, 3039,
    UInt8 Result;
    Int32 MeritPoints;
)

CLIENT_PROTOCOL(C2S, DIAMOND_MERIT_MASTERY_GRANT_SPECIAL_MASTERY, DEFAULT, 3040,
    Int32 CategoryIndex;
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, DIAMOND_MERIT_MASTERY_GRANT_SPECIAL_MASTERY, DEFAULT, 3040,
    UInt8 Result;
    Int32 Points;
    struct _RTDiamondMeritSpecialMasterySlot SpecialMasterySlots[3];
)

CLIENT_PROTOCOL(C2S, COSTUME_WAREHOUSE_NAME_LIST, DEFAULT, 3046,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_COSTUME_WAREHOUSE_NAME_ITEM,
    UInt32 ItemID;
    Char Name[MAX_USERNAME_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, COSTUME_WAREHOUSE_NAME_LIST, DEFAULT, 3046,
    Int32 ItemCount;
    S2C_DATA_COSTUME_WAREHOUSE_NAME_ITEM Items[0]; // ItemCount
)

CLIENT_PROTOCOL(C2S, COSTUME_WAREHOUSE_NAME_SET, DEFAULT, 3048,
    Int32 InventorySlotIndex;
    UInt32 ItemID;
    Char Name[MAX_USERNAME_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, COSTUME_WAREHOUSE_NAME_SET, DEFAULT, 3048,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, ANIMA_MASTERY_UNSEALING, DEFAULT, 3050,
    UInt8 StorageSlotIndex;
    Int32 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, ANIMA_MASTERY_UNSEALING, DEFAULT, 3050,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, ANIMA_MASTERY_SEALING, DEFAULT, 3051,
    UInt8 CategoryIndex;
    UInt8 StorageSlotIndex;
    Int32 TargetInventorySlotIndex;
    Int32 SourceInventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, ANIMA_MASTERY_SEALING, DEFAULT, 3051,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, ANIMA_MASTERY_TRAIN_SLOT, DEFAULT, 3052,
    Int8 CategoryIndex;
    Int8 MasterySlotIndex;
    Int32 ConversionKitSlotIndex;
    UInt16 MaterialSlotCount;
    UInt16 MaterialSlotIndex[0];
)

CLIENT_PROTOCOL(S2C, ANIMA_MASTERY_TRAIN_SLOT, DEFAULT, 3052,
    Int32 Success;
    UInt8 ForceEffectOrder;
)

CLIENT_PROTOCOL(C2S, ANIMA_MASTERY_RESET_SLOT, DEFAULT, 3053,
    Int8 CategoryIndex;
    Int8 StorageIndex;
    Int32 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, ANIMA_MASTERY_RESET_SLOT, DEFAULT, 3053,
    Int32 Success;
)

CLIENT_PROTOCOL(C2S, ANIMA_MASTERY_UNLOCK_CATEGORY, DEFAULT, 3054,
    Int8 CategoryIndex;
)

CLIENT_PROTOCOL(S2C, ANIMA_MASTERY_UNLOCK_CATEGORY, DEFAULT, 3054,
    Int32 Success;
)

CLIENT_PROTOCOL(C2S, ANIMA_MASTERY_SET_ACTIVE_STORAGE_INDEX, DEFAULT, 3055,
    Int8 CategoryIndex;
    Int8 StorageSlotIndex;
)

CLIENT_PROTOCOL(S2C, ANIMA_MASTERY_SET_ACTIVE_STORAGE_INDEX, DEFAULT, 3055,
    Int32 Success;
)

CLIENT_PROTOCOL(C2S, ANIMA_MASTERY_SET_ACTIVE_PRESET_INDEX, DEFAULT, 3056,
    Int8 PresetIndex;
)

CLIENT_PROTOCOL(S2C, ANIMA_MASTERY_SET_ACTIVE_PRESET_INDEX, DEFAULT, 3056,
    Int32 Success;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_5305, DEFAULT, 5305,
    UInt8 Unknown1[6];
)

CLIENT_PROTOCOL(C2S, UNKNOWN_5383, DEFAULT, 5383,
)

CLIENT_PROTOCOL(S2C, UNKNOWN_5383, DEFAULT, 5383,
    UInt8 Unknown1[8];
)

#undef CLIENT_PROTOCOL_ENUM
#undef CLIENT_PROTOCOL_STRUCT
#undef CLIENT_PROTOCOL
