
#include "Constants.h"
#include "GameProtocol.h"

#ifndef CLIENT_PROTOCOL_ENUM
#define CLIENT_PROTOCOL_ENUM(...)
#endif

#ifndef CLIENT_PROTOCOL_STRUCT
#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef CLIENT_PROTOCOL
#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __COMMAND__, __VERSION__, __BODY__)
#endif

CLIENT_PROTOCOL_STRUCT(S2C_POSITION,
    UInt16 X;
    UInt16 Y;
)

CLIENT_PROTOCOL(S2C, NFY_ERROR, 7, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 ErrorType;
    UInt16 ErrorCode;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(C2S, GET_CHARACTERS, 133, 13130,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_CHARACTERS, 133, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 IsSubpasswordSet;
    UInt8 Unknown1[12];
    UInt8 Unknown2;
    UInt32 CharacterSlotID;
    UInt64 CharacterSlotOrder;
    UInt32 Unknown3;
    UInt32 Unknown4;
    UInt32 Unknown5;
    // TODO: Replace with CLIENT_PROTOCOL_STRUCT
    GAME_DATA_CHARACTER_INDEX Characters[MAX_CHARACTER_COUNT];
)

CLIENT_PROTOCOL(C2S, CREATE_CHARACTER, 134, 13130,
    C2S_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(S2C, CREATE_CHARACTER, 134, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt8 CharacterStatus;
)

CLIENT_PROTOCOL(C2S, DELETE_CHARACTER, 135, 13130,
    C2S_DATA_SIGNATURE;
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(S2C, DELETE_CHARACTER, 135, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 CharacterStatus;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, CONNECT, 140, 13130,
    C2S_DATA_SIGNATURE;
    UInt8 ServerID;
    UInt8 ChannelID;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(S2C, CONNECT, 140, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 XorKey;
    UInt32 AuthKey;
    UInt16 ConnectionID;
    UInt16 XorKeyIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(C2S, INITIALIZE, 142, 13130,
    C2S_DATA_SIGNATURE;
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SECRET_SHOP_INDEX,
    UInt16 SlotIndex;
    UInt32 ShopItemID;
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
    UInt8 WorldID;
    UInt16 PlayerCount;
    UInt8 Unknown1[22];
    UInt16 MaxPlayerCount;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS Address;
    UInt32 WorldType;
    UInt32 Unknown2;
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

CLIENT_PROTOCOL_STRUCT(S2C_DATA_OVERLORD_MASTERY_SLOT,
    UInt8 MasteryIndex;
    UInt8 Level;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_COLLECTION_SLOT,
    UInt8 TypeID;
    UInt8 CollectionID;
    UInt16 MissionID;
    UInt8 ReceivedReward;
    UInt16 MissionItemCounts[6];
)

CLIENT_PROTOCOL(S2C, INITIALIZE, 142, 13130,
    S2C_DATA_SIGNATURE_EXTENDED;
    S2C_DATA_INITIALIZE_WAR War;
    UInt32 WorldType;
    UInt8 IsWarehousePasswordSet;
    UInt8 IsInventoryPasswordSet;
    UInt8 IsWarehouseLocked;
    UInt8 IsInventoryLocked;
    UInt8 Unknown1A;
    S2C_DATA_INITIALIZE_SERVER Server;
    RTEntityID Entity;
    UInt32 WorldIndex;
    UInt32 DungeonIndex;
    S2C_POSITION Position;
    UInt64 Exp;
    UInt64 Alz;
    UInt64 Wexp;
    Int32 Level;
    UInt32 Unknown4;
    UInt32 STR;
    UInt32 DEX;
    UInt32 INT;
    UInt32 PNT;
    Int32 SkillRank;
    Int32 MagicSkillRank;
    UInt64 BaseHP;
    UInt64 CurrentHP;
    UInt32 MaxMP;
    UInt32 CurrentMP;
    UInt16 MaxSP;
    UInt16 CurrentSP;
    UInt32 MaxRage;
    UInt32 CurrentRage;
    UInt32 MaxBP;
    UInt32 CurrentBP;
    UInt32 CharacterIndex;
    UInt32 Unknown6;
    UInt8 Unknown7[3];
    UInt8 UnknownSkillRank;
    UInt32 SkillLevel;
    UInt32 SkillExp;
    UInt32 SkillPoint;
    UInt64 UnknownExp;
    UInt64 HonorPoint;
    UInt64 DeathPenaltyExp;
    UInt32 DeathPenaltyHp;
    UInt32 DeathPenaltyMp;
    UInt16 PKLevel;
    UInt8 Unknown11[4];
    S2C_DATA_INITIALIZE_SERVER_ADDRESS ChatServerAddress;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS AuctionServerAddress;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS PartyServerAddress;
    UInt16 UnknownPort;
    UInt8 Nation;
    UInt32 Unknown13;
    UInt32 WarpMask;
    UInt32 MapsMask;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt8 Unknown14[282];
    UInt16 EquipmentSlotCount;
    UInt8 EquipmentLockCount;
    UInt32 InventorySlotCount;
    UInt16 SkillSlotCount;
    UInt16 QuickSlotCount;
    UInt16 MercenaryCount;
    UInt16 UnknownCount1;
    UInt16 UnknownCount2;
    UInt16 AP;
    UInt32 Axp;
    UInt8 EssenceAbilityCount;
    UInt8 UnknownDailyMissionCount;
    UInt16 BlendedAbilityCount;
    UInt16 PremiumServiceCount;
    UInt16 BlessingBeadCount;
    UInt16 QuestSlotCount;
    struct _RTCharacterQuestFlagInfo QuestFlagInfo;
    struct _RTCharacterDungeonQuestFlagInfo DungeonQuestFlagInfo;
    UInt8 Unknown15[4097];
    UInt32 UnknownDailyQuestCount;
    
    struct {
        UInt32 Unknown1;
        UInt32 PetSerial;
        UInt32 Unknown3;
        UInt32 Unknown4;
        UInt64 PetItemKind;
        UInt32 Unknown5;
        UInt16 Unknown6;
        Char PetName[MAX_CHARACTER_NAME_LENGTH + 1];
    } Pet;

    UInt8 Unknown16[206];

    UInt8 EquipmentAppearanceCount;
    UInt16 InventoryAppearanceCount;
    Int32 AllAchievementScore;
    Int32 NormalAchievementScore;
    Int32 QuestAchievementScore;
    Int32 DungeonAchievementScore;
    Int32 ItemsAchievementScore;
    Int32 PvpAchievementScore;
    Int32 WarAchievementScore;
    Int32 HuntingAchievementScore;
    Int32 CraftAchievementScore;
    Int32 CommunityAchievementScore;
    Int32 SharedAchievementScore;
    Int32 SpecialAchievementScore;
    UInt16 DisplayTitle;
    UInt16 EventTitle;
    UInt16 GuildTitle;
    UInt16 WarTitle;
    UInt32 AchievementCount;
    UInt32 TitleCount;
    UInt32 UnknownCount5;
    UInt32 CraftCount;
    UInt8 Unknown199;
    UInt32 CraftEnergy;
    UInt8 Unknown17[6];
    UInt8 SortingOrderMask;
    UInt16 UnknownBasicCraftCount;
    UInt16 BasicCraftExp;
    // UInt16 UnknownBasicCraftExp;
    // UInt8 BasicCraftFlags[256];
    // UInt8 Unknown19[1796];
    UInt8 Unknown19[2072];
    // UInt32 SkillCooldownCount;
    // UInt8 Unknown65[14];
    UInt32 GoldMeritCount;
    UInt32 GoldMeritExp;
    UInt32 GoldMeritPoint;
    UInt8 PlatinumMeritCount;
    UInt32 PlatinumMeritExp;
    UInt32 PlatinumMeritPoint[2];
    UInt8 Unknown21;
    Timestamp CharacterCreationDate;
    UInt8 Unknown22[9];
    UInt32 ExtendedTitleCount;
    UInt64 ForceGem;
    UInt8 Unknown33[10];
    UInt16 OverlordLevel;
    UInt64 OverlordExp;
    UInt16 OverlordPoint;
    UInt8 OverlordMasteryCount;
    UInt32 UnknownMeritMasteryCount;
    // UInt32 HonorMedalCount;
    // UInt32 HonorMedalExp;
    UInt8 Unknown84[4];
    UInt8 HonorMedalCount;
    UInt8 ForceWingRank;
    UInt8 ForceWingLevel;
    UInt64 ForceWingExp;
    UInt8 Unknown2422s[14];
    UInt16 ForceWingTrainingPoints;
    UInt8 Unknown2422e[19];
    UInt16 ForceWingTrainingSlotCount;
    UInt16 UnknownSomething;
    UInt8 Unknown24[107];
    UInt16 SpecialGiftboxPoint;
    UInt8 SpecialGiftboxCount;
    UInt32 Unknown2733[9];
    UInt16 CollectionCount;
    UInt16 TransformCount;
    UInt8 Unknown32[7];
    // Timestamp SecretShopResetTime;
    // UInt8 SecretShopRefreshCost;
    UInt8 UnknownSecretShopRefreshCost;
    Timestamp UnknownSecretShopResetTime;
    S2C_DATA_SECRET_SHOP_INDEX SecretShops[MAX_SECRET_SHOP_COUNT];
    UInt8 Unknown44[9];
    UInt32 TranscendencePoint;
    UInt32 TranscendenceCount;
    UInt8 U_UPDATEINV[3134];
    //UInt32 EventPassMissionCount;
    //UInt32 ;
    UInt8 Unknown45[48];
    UInt8 NameLength;
    Char Name[0];
)

CLIENT_PROTOCOL(C2S, DEINITIALIZE, 143, 13130,
    C2S_DATA_SIGNATURE;
    UInt16 UnknownIndex;
    UInt8 WorldIndex;
    UInt8 Logout;
)

CLIENT_PROTOCOL(S2C, DEINITIALIZE, 143, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(C2S, GET_WAREHOUSE, 145, 13130,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_WAREHOUSE_SLOT_INDEX,
    RTItem ItemID;
    UInt32 Unknown1;
    UInt32 Unknown2;
    UInt64 ItemOptions;
    UInt32 Unknown3;
    UInt16 SlotIndex;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, GET_WAREHOUSE, 145, 13130,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    UInt64 Currency;
    S2C_DATA_GET_WAREHOUSE_SLOT_INDEX Slots[0];
)

CLIENT_PROTOCOL(C2S, SET_QUICKSLOT, 146, 13130,
    C2S_DATA_SIGNATURE;
    UInt16 QuickSlotIndex;
    UInt16 SkillSlotIndex;
)

CLIENT_PROTOCOL(S2C, SET_QUICKSLOT, 146, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, SWAP_QUICKSLOT, 147, 13130,
    C2S_DATA_SIGNATURE;
    UInt16 SourceSlotIndex;
    UInt16 TargetSlotIndex;
)

CLIENT_PROTOCOL(S2C, SWAP_QUICKSLOT, 147, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, GET_SERVER_TIME, 148, 13130,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_SERVER_TIME, 148, 13130,
    S2C_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(C2S, LOOT_INVENTORY_ITEM, 153, 13130,
    C2S_DATA_SIGNATURE;
    RTEntityID Entity;
    UInt16 UniqueKey;
    UInt64 ItemID;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, LOOT_INVENTORY_ITEM, 153, 13130,
    S2C_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(C2S, LOOT_CURRENCY_ITEM, 154, 13130,
    C2S_DATA_SIGNATURE;
    UInt16 Count;
    C2S_DATA_LOOT_CURRENCY_ITEM_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, LOOT_CURRENCY_ITEM, 154, 13130,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    UInt8 Result;
    UInt64 Currency;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_ITEM_PRICE_INDEX,
    UInt32 SlotIndexItemPrice;
    UInt32 AmountItemPrice;
)

CLIENT_PROTOCOL(C2S, BUY_ITEM, 161, 13130,
    C2S_DATA_SIGNATURE;
    UInt32 TabIndex;
    UInt16 NpcID;
    UInt8 Unknown1;
    UInt16 SlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 SlotIndex2;
    UInt32 Unknown2;
    UInt32 InventoryIndexCount;
    UInt32 ItemPriceCount;
    S2C_DATA_ITEM_PRICE_INDEX ItemPriceTable[522];
    UInt8 Unknown3;
    Int32 InventoryIndex[50]; // TODO: Check if the appended data after inventory indices are useful
)

CLIENT_PROTOCOL(S2C, BUY_ITEM, 161, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 Unknown2;
    UInt16 Unknown3;
    UInt16 Unknown4;
)

CLIENT_PROTOCOL(C2S, SELL_ITEM, 162, 13130,
    C2S_DATA_SIGNATURE;
    UInt16 NpcID;
    UInt16 Unknown1;
    UInt8 Unknown2;
    UInt32 InventoryIndexCount;
    Int32 InventoryIndex[0];
)

CLIENT_PROTOCOL(S2C, SELL_ITEM, 162, 13130,
    S2C_DATA_SIGNATURE;
    UInt64 Currency;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, RECOVER_ITEM, 166, 13130,
    C2S_DATA_SIGNATURE;
    UInt8 RecoverySlotIndex;
    UInt32 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, RECOVER_ITEM, 166, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, GET_ITEM_RECOVERY_LIST, 167, 13130,
    C2S_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(S2C, GET_ITEM_RECOVERY_LIST, 167, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Count;
    S2C_ITEM_RECOVERY_LIST_SLOT Slots[0];
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_TO_MOB_TARGET,
    RTEntityID Entity;
    UInt8 EntityIDType;
    UInt16 Unknown2;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_MOB, 174, 13130,
    C2S_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt32 Unknown1;
    S2C_POSITION PositionSet;
    UInt8 Unknown2;
    UInt32 Unknown3;
    UInt8 TargetCount;
    S2C_POSITION PositionCharacter;
    S2C_POSITION PositionTarget;
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
    S2C_POSITION PositionSet;
    UInt32 BfxType;
    UInt32 BfxDuration;
    UInt8 Unknown3;
)

CLIENT_PROTOCOL(S2C, SKILL_TO_MOB, 174, 13130,
    S2C_DATA_SIGNATURE;
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
    RUNTIME_SKILL_GROUP_UNKNOWN_32 = 32,
    RUNTIME_SKILL_GROUP_WING = 33,
    RUNTIME_SKILL_GROUP_VEHICLE = 38,
    RUNTIME_SKILL_GROUP_COMBO = 40,
    RUNTIME_SKILL_GROUP_UNKNOWN_42 = 42,
    RUNTIME_SKILL_GROUP_TOTEM = 43,
    RUNTIME_SKILL_GROUP_AGGRO = 44,
    RUNTIME_SKILL_GROUP_COUNT = 50
};
*/

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_MOVEMENT,
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_ASTRAL,
    UInt16 Unknown1;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_BATTLE_MODE,
    UInt16 IsActivation;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_CHARACTER, 175, 13130,
    C2S_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_MOVEMENT,
    UInt32 Unknown1;
    UInt32 CharacterMP;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_ASTRAL,
    UInt16 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_SKILL_GROUP_BATTLE_MODE,
    UInt32 CurrentMP;
    UInt16 IsActivation;
)

CLIENT_PROTOCOL(S2C, SKILL_TO_CHARACTER, 175, 13130,
    S2C_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(C2S, ATTACK_TO_MOB, 176, 13130,
    C2S_DATA_SIGNATURE;
    RTEntityID Entity;
    UInt8 EntityIDType;
    UInt8 MissMob;
)

CLIENT_PROTOCOL(S2C, ATTACK_TO_MOB, 176, 13130,
    S2C_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(C2S, ATTACK_TO_CHARACTER, 177, 0000,
    C2S_DATA_SIGNATURE;
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, ATTACK_TO_CHARACTER, 177, 0000,
    S2C_DATA_SIGNATURE;
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(C2S, MOVEMENT_BEGIN, 190, 13130,
    C2S_DATA_SIGNATURE;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
    S2C_POSITION PositionCurrent;
    UInt16 WorldID;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_END, 191, 13130,
    C2S_DATA_SIGNATURE;
    S2C_POSITION Position;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_CHANGE, 192, 13130,
    C2S_DATA_SIGNATURE;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
    S2C_POSITION PositionCurrent;
    UInt16 WorldID;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_WAYPOINTS, 193, 13130,
    C2S_DATA_SIGNATURE;
    S2C_POSITION PositionCurrent;
    S2C_POSITION PositionNext;
)

CLIENT_PROTOCOL(C2S, MOVEMENT_TILE_POSITION, 194, 13130,
    C2S_DATA_SIGNATURE;
    S2C_POSITION PositionCurrent;
    S2C_POSITION PositionNext;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_EQUIPMENT_SLOT,
    UInt8 EquipmentSlotIndex;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_BUFF_SLOT,
    UInt16 SkillIndex;
    UInt8 SkillLevel;
    UInt8 Unknown1[82];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_BATTLE_MODE,
    UInt8 Unknown0 : 1;
    UInt8 AstralWeapon : 1;
    UInt8 Unknown2 : 1;
    UInt8 Aura : 1;
    UInt8 BattleMode1 : 1;
    UInt8 BattleMode2 : 1;
    UInt8 BattleMode3 : 1;
    UInt8 Combo : 1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_STYLE_EXTENSION,
    UInt8 Unknown0 : 1;
    UInt8 Unknown1 : 1;
    UInt8 Unknown2 : 1;
    UInt8 Unknown3 : 1;
    UInt8 HasPetStyle1 : 1;
    UInt8 HasPetInfo : 1;
    UInt8 HasBlessingBeadBalloon : 1;
    UInt8 HasItemBallon : 1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_PET_FLAGS,
    UInt8 EvolutionLevel : 3; 
    UInt8 NameLength : 5;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_PET_SLOT,
    S2C_DATA_CHARACTERS_SPAWN_PET_FLAGS PetFlags;
    UInt32 PetSlotIndex;
    Char PetName[0];
    // UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_BALOON_SLOT,
    RTItem Item;
    UInt8 SlotIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_GUILD,
    UInt8 GuildNameLength;
    Char GuildName[0]; // GuildNameLength
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_CHARACTERS_SPAWN_INDEX,
    UInt32 CharacterIndex;
    RTEntityID Entity;
    UInt32 Level;
    UInt16 OverlordLevel;
    UInt32 HolyPower;
    UInt32 Rebirth;
    UInt32 MythLevel;
    UInt8 ForceWingRank;
    UInt8 ForceWingLevel;
    UInt64 MaxHP;
    UInt64 CurrentHP;
    Int32 MaxRage;
    Int32 CurrentRage;
    UInt8 Unknown2[8];
    UInt32 MovementSpeed;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
    UInt16 PKLevel;
    UInt8 Nation;
    UInt32 Unknown3;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt32 SkillEffectIndex;
    UInt8 BattleMode;
    UInt8 IsDead;
    UInt8 EquipmentSlotCount;
    UInt8 CostumeCount;
    UInt8 IsPersonalShop;
    UInt32 GuildIndex;
    UInt8 Unknown4[5];
    S2C_DATA_CHARACTERS_STYLE_EXTENSION CharacterStyleExtension;
    UInt8 Unknown5[2];
    UInt8 GmBuffCount;
    UInt8 PassiveBuffCount;
    UInt8 ActiveBuffCount;
    UInt8 DebuffCount;
    UInt8 UnknownBuffCount;
    UInt8 Bringer;
    UInt16 DisplayTitle;
    UInt16 EventTitle;
    UInt16 GuildTitle;
    UInt16 WarTitle;
    UInt8 Unknown6[5];
    UInt8 NameLength;
    Char Name[0]; // Size: NameLength - 1
    /*
    Equipments: Equipment[EquipmentCount];
    Costumes: Equipment[CostumeCount];
    GmBuffData: Buff[GmBuffCount];
    PassiveBuffData: Buff[PassiveBuffCount];
    ActiveBuffData: Buff[ActiveBuffCount];
    DebuffData: Buff[DebuffCount];
    UnknownBuffData: Buff[UnknownBuffCount];

    if (StyleExtension.HasBlessingBeadBalloon == 1) {
        BlessingBeadBalloon: BlessingBeadBalloon;
    }
    */
)

CLIENT_PROTOCOL(S2C, CHARACTERS_SPAWN, 200, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Count;
    UInt8 SpawnType;
    //S2C_DATA_CHARACTERS_SPAWN_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, CHARACTERS_DESPAWN, 201, 13130,
    S2C_DATA_SIGNATURE;
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
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
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

CLIENT_PROTOCOL(S2C, MOBS_SPAWN, 202, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Count;
    S2C_DATA_MOBS_SPAWN_INDEX Data[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_ENTITY_DESPAWN_TYPE_DEAD = 0x10,
    S2C_DATA_ENTITY_DESPAWN_TYPE_WARP,
    S2C_DATA_ENTITY_DESPAWN_TYPE_UNKNOWN1,
    S2C_DATA_ENTITY_DESPAWN_TYPE_RETURN,
    S2C_DATA_ENTITY_DESPAWN_TYPE_DISAPPEAR,
    S2C_DATA_ENTITY_DESPAWN_TYPE_NOTIFY_DEAD
)

CLIENT_PROTOCOL(S2C, MOBS_DESPAWN, 203, 13130,
    S2C_DATA_SIGNATURE;
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
    UInt32 SourceIndex;
    UInt64 ItemID;
    UInt16 X;
    UInt16 Y;
    UInt16 UniqueKey;
    UInt8 ContextType;
    RTItemProperty ItemProperty;
)

CLIENT_PROTOCOL(S2C, NFY_SPAWN_ITEM, 204, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Count;
    S2C_DATA_NFY_SPAWN_ITEM_INDEX Data[0];
)

CLIENT_PROTOCOL(S2C, NFY_DESPAWN_ITEM, 205, X596,
    S2C_DATA_SIGNATURE;
    RTEntityID Entity;
    UInt8 DespawnType;
)

CLIENT_PROTOCOL(S2C, NFY_EQUIP_ITEM, 206, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    RTItem Item;
    UInt64 ItemOptions;
    UInt16 EquipmentSlotIndex;
    UInt32 Unknown1;
    UInt8 Unknown2;
)

CLIENT_PROTOCOL(S2C, NFY_UNEQUIP_ITEM, 207, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt16 EquipmentSlotIndex;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_BEGIN, 210, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt32 TickCount;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_END, 211, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    S2C_POSITION Position;
)

CLIENT_PROTOCOL(S2C, MOVEMENT_CHANGE, 212, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt32 TickCount;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_MOVEMENT_BEGIN, 213, X596,
    S2C_DATA_SIGNATURE;
    RTEntityID Entity;
    UInt32 TickCount;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_MOVEMENT_END, 214, X596,
    S2C_DATA_SIGNATURE;
    RTEntityID Entity;
    S2C_POSITION Position;
)

CLIENT_PROTOCOL(S2C, MOB_CHASE_BEGIN, 215, X596,
    S2C_DATA_SIGNATURE;
    RTEntityID Entity;
    UInt32 TickCount;
    S2C_POSITION PositionBegin;
    S2C_POSITION PositionEnd;
)

CLIENT_PROTOCOL(S2C, MOB_CHASE_END, 216, X596,
    S2C_DATA_SIGNATURE;
    RTEntityID Entity;
    S2C_POSITION Position;
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
    S2C_POSITION PositionSet;
)

CLIENT_PROTOCOL(S2C, NFY_SKILL_TO_MOB, 220, X596,
    S2C_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 TargetCount;
    UInt32 CharacterIndex;
    S2C_POSITION PositionSet;
    UInt8 Unknown1;
    UInt64 CharacterHP;
    UInt32 Unknown2;
    S2C_DATA_NFY_SKILL_TO_MOB_TARGET Data[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_GROUP_ASTRAL_WEAPON,
    UInt32 CharacterIndex;
    RTEntityID Entity;
    S2C_POSITION Position; 
    UInt16 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_SKILL_GROUP_BATTLE_MODE,
    UInt32 CharacterIndex;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt8 StyleExtension;
    UInt16 IsActivation;
)

CLIENT_PROTOCOL(S2C, NFY_SKILL_TO_CHARACTER, 221, X596,
    S2C_DATA_SIGNATURE;
    UInt16 SkillIndex;
)

CLIENT_PROTOCOL(S2C, NFY_ATTACK_TO_MOB, 225, X596,
    S2C_DATA_SIGNATURE;
    UInt8 AttackType;
    UInt32 CharacterIndex;
    RTEntityID Mob;
    UInt8 MobIDType;
    UInt64 MobHP;
    UInt8 Unknown1;
    UInt64 CharacterHP;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(C2S, WARP, 244, X596,
    C2S_DATA_SIGNATURE;
    UInt8 NpcID;
    UInt16 Unknown1;
    UInt16 SlotIndex;
    UInt16 Unknown2;
    UInt8 WarpPositionY;
    UInt8 WarpPositionX;
    UInt8 WarpWorldID;
    UInt8 Unknown3[15];
    Int32 WarpIndex;
    UInt8 Unknown4;
)

CLIENT_PROTOCOL(S2C, WARP, 244, X596,
    S2C_DATA_SIGNATURE;
    S2C_POSITION Position;
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

CLIENT_PROTOCOL(S2C, BATTLE_RANK_UP, 276, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Level;
)

CLIENT_PROTOCOL(C2S, ADD_FORCE_SLOT_OPTION, 280, 0000,
    C2S_DATA_SIGNATURE;
    Int32 ItemSlotIndex;
    Int32 Unknown1;
    Int32 OptionScrollSlotIndex;
    Int32 Unknown2;
    UInt8 ForceCoreCount;
    Int32 ForceCoreSlotIndices[10];
)

CLIENT_PROTOCOL(S2C, ADD_FORCE_SLOT_OPTION, 280, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1[10];
)

CLIENT_PROTOCOL(C2S, QUEST_BEGIN, 282, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt8 SlotID;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, QUEST_BEGIN, 282, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, QUEST_CLEAR, 283, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuestIndex;
    UInt16 QuestSlotIndex;
    UInt16 SlotIndex[0];
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_QUEST_CLEAR_RESULT_TYPE_UNKNOWN,
    S2C_DATA_QUEST_CLEAR_RESULT_TYPE_SUCCESS = 4
)

CLIENT_PROTOCOL(S2C, QUEST_CLEAR, 283, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt16 ResultType;
    UInt32 Unknown1;
    UInt64 RewardExp;
)

CLIENT_PROTOCOL(C2S, QUEST_CANCEL, 284, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt8 SlotID;
    UInt8 QuestDungeonCount;
    UInt8 AddItemCount;
    UInt8 DeleteItemCount;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, QUEST_CANCEL, 284, 0000,
    S2C_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(C2S, USE_ITEM, 285, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 InventorySlotIndex;
    UInt8 Data[1];
)

CLIENT_PROTOCOL(S2C, USE_ITEM, 285, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

// NOTE: This table has to be in sync with (RUNTIME_EVENT_CHARACTER_DATA_TYPE_...)
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
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_DATA, 287, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Type;

    union {
        struct { UInt8 _0[34]; UInt64 HPAfterPotion; UInt8 _1[4]; };
        struct { UInt8 _2[38]; UInt64 HP; };
        struct { UInt8 _3[34]; UInt32 MP; UInt8 _4[8]; };
        struct { UInt8 _5[34]; UInt32 SP; UInt8 _6[8]; };
        struct { UInt8 _7[34]; UInt32 Level; UInt8 _8[8]; };
        struct { UInt8 _9[34]; UInt32 Rage; UInt8 _10[8]; };
    };
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_CHARACTER_EVENT_TYPE_LEVELUP = 1,
    S2C_DATA_CHARACTER_EVENT_TYPE_RANKUP = 2,
    S2C_DATA_CHARACTER_EVENT_TYPE_OVERLORD_LEVELUP = 3,
    S2C_DATA_CHARACTER_EVENT_TYPE_UNKNOWN = 4,
)

CLIENT_PROTOCOL(S2C, NFY_CHARACTER_EVENT, 288, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Type;
    UInt32 CharacterIndex;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_START, 290, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_START, 290, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_END, 291, X596,
    C2S_DATA_SIGNATURE;
    Int16 Unknown1;
    Int16 Unknown2;
    Int8 Unknown3;
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_END, 291, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_SPAWN, 292, X596,
    C2S_DATA_SIGNATURE;
    UInt8 IsActive;
)

CLIENT_PROTOCOL(C2S, QUEST_DUNGEON_GATE_OPEN, 293, 0000,
    C2S_DATA_SIGNATURE;
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, QUEST_DUNGEON_GATE_OPEN, 293, 0000,
    S2C_DATA_SIGNATURE;
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_END, 294, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt16 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_CLEAR_MOBS, 300, X596,
    S2C_DATA_SIGNATURE;
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_SPAWN, 301, X596,
    S2C_DATA_SIGNATURE;
    UInt32 DungeonTimeout1;
    UInt32 DungeonTimeout2;
    UInt32 Unknown1;
    UInt32 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_302, 302, X596,
    S2C_DATA_SIGNATURE;
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_DUNGEON_PATTERN_PART_COMPLETED, 303, X596,
    S2C_DATA_SIGNATURE;
    UInt8 PatternPartIndex;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_QUEST_ACTION_INDEX,
    UInt32 ActionIndex;
    UInt16 SlotIndex;
)

CLIENT_PROTOCOL(C2S, QUEST_ACTION, 320, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt32 NpcSetID;
    UInt8 SlotIndex;
    UInt8 ActionCount;
    C2S_DATA_QUEST_ACTION_INDEX Actions[0];
)

CLIENT_PROTOCOL(S2C, QUEST_ACTION, 320, X596,
    S2C_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt16 NpcFlags;
    UInt32 NpcSetID;
)

CLIENT_PROTOCOL(C2S, ATTACK_BOSS_MOB, 321, 0000,
    C2S_DATA_SIGNATURE;
    RTEntityID Entity;
    // TODO: There is some appendix data here...
)

CLIENT_PROTOCOL(S2C, ATTACK_BOSS_MOB, 321, 0000,
    S2C_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(C2S, CHANGE_STYLE, 322, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt32 GuildID;
    UInt32 UnknownFlags;
    UInt8 GuildNameLength;
    Char GuildName[0];
)

CLIENT_PROTOCOL(S2C, CHANGE_STYLE, 322, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
)

CLIENT_PROTOCOL(S2C, NFY_CHANGE_STYLE, 323, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt32 GuildID;
    UInt32 UnknownFlags;
    UInt8 GuildNameLength;
    Char GuildName[0];
)

CLIENT_PROTOCOL(C2S, GET_PREMIUM_SERVICE, 324, 13130,
    C2S_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(S2C, GET_PREMIUM_SERVICE, 324, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 ChargeType;
    UInt32 ServiceType;
    Timestamp ExpirationDate;
    UInt32 Unknown2;
    Timestamp UnknownExpirationDate;
)

CLIENT_PROTOCOL(S2C, NFY_PREMIUM_SERVICE, 325, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 ChargeType;
    UInt32 ServiceType;
    Timestamp ExpirationDate;
    UInt32 Unknown2;
    Timestamp UnknownExpirationDate;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_REPUTATION_CHANGE_TYPE_AUTOTRAP_HIT = 0x2,
    S2C_DATA_REPUTATION_CHANGE_TYPE_GUILD_WIN = 0x3,
    S2C_DATA_REPUTATION_CHANGE_TYPE_NATION_KILL = 0x4,
    S2C_DATA_REPUTATION_CHANGE_TYPE_MOBS_KILL = 0x5,
    S2C_DATA_REPUTATION_CHANGE_TYPE_GUILD_KILL = 0x6,
    S2C_DATA_REPUTATION_CHANGE_TYPE_UNKNOWN7 = 0x7,
)

CLIENT_PROTOCOL(C2S, REPUTATION_CHANGE, 346, X596,
    C2S_DATA_SIGNATURE;
    UInt8 Type;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, REPUTATION_CHANGE, 346, X596,
    C2S_DATA_SIGNATURE;
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, NFY_REPUTATION_CHANGE, 347, X596,
    C2S_DATA_SIGNATURE;
    // TODO: Add packet structure
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_PARTY_QUEST_BEGIN_RESULT_TYPE_SUCCESS = 0,
    S2C_DATA_PARTY_QUEST_BEGIN_RESULT_TYPE_ALREADY_OPENED = 1,
    S2C_DATA_PARTY_QUEST_BEGIN_RESULT_TYPE_ANTI_ADDICT = 2,
)

CLIENT_PROTOCOL(C2S, PARTY_QUEST_BEGIN, 371, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt8 QuestSlotIndex;
    Int16 QuestItemSlotIndex;
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_BEGIN, 371, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_BEGIN, 375, X596,
    S2C_DATA_SIGNATURE;
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
)

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_PARTY_QUEST_CLOSE_RESULT_TYPE_SUCCESS = 0,
    S2C_DATA_PARTY_QUEST_CLOSE_RESULT_TYPE_ALREADY_CLOSED = 1,
    S2C_DATA_PARTY_QUEST_CLOSE_RESULT_TYPE_ALREADY_IN_USE_ITEM_SLOT = 2,
    S2C_DATA_PARTY_QUEST_CLOSE_RESULT_TYPE_ALREADY_IN_USE_TEMP_SLOT = 2,
)

CLIENT_PROTOCOL(C2S, PARTY_QUEST_CLOSE, 372, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt8 QuestSlotIndex;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_CLOSE, 372, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_CLOSE, 376, X596,
    S2C_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(C2S, PARTY_QUEST_ACTION, 373, X596,
    C2S_DATA_SIGNATURE;
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
    UInt32 NpcSetID;
    UInt8 ActionCount;
    C2S_DATA_PARTY_QUEST_ACTION_INDEX Actions[0];
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_ACTION, 373, X596,
    S2C_DATA_SIGNATURE;
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
    UInt16 NpcFlags;
    UInt32 NpcSetID;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_ACTION, 377, X596,
    S2C_DATA_SIGNATURE;
    UInt8 QuestSlotIndex;
    UInt16 QuestID;
    UInt16 NpcFlags;
    UInt32 NpcSetID;
)

CLIENT_PROTOCOL(C2S, PARTY_QUEST_LOOT_ITEM, 374, X596,
    C2S_DATA_SIGNATURE;
    RTEntityID Entity;
    UInt16 UniqueKey;
    UInt64 ItemID;
    Int16 QuestItemSlotIndex;
)

CLIENT_PROTOCOL(S2C, PARTY_QUEST_LOOT_ITEM, 374, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt64 ItemID;
    UInt64 ItemOptions;
    Int16 QuestItemSlotIndex;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_LOOT_ITEM, 378, X596,
    S2C_DATA_SIGNATURE;
    UInt64 ItemID;
    UInt64 ItemOptions;
    Int16 QuestItemSlotIndex;
    UInt32 ItemDuration;
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_QUEST_MISSION_MOB_KILL, 379, X596,
    S2C_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt16 MobSpeciesIndex;
)

CLIENT_PROTOCOL(C2S, CHANGE_DIRECTION, 391, X596,
    C2S_DATA_SIGNATURE;
    Float32 Direction;
)

CLIENT_PROTOCOL(S2C, CHANGE_DIRECTION, 392, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    Float32 Direction;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_BEGIN, 401, X596,
    C2S_DATA_SIGNATURE;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_BEGIN, 403, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    Timestamp TickCount;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_END, 402, X596,
    C2S_DATA_SIGNATURE;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_END, 404, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
)

CLIENT_PROTOCOL(C2S, KEY_MOVEMENT_CHANGE, 405, X596,
    C2S_DATA_SIGNATURE;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    Float32 PositionCurrentX;
    Float32 PositionCurrentY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(S2C, KEY_MOVEMENT_CHANGE, 406, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterIndex;
    Timestamp TickCount;
    Float32 PositionStartX;
    Float32 PositionStartY;
    Float32 PositionEndX;
    Float32 PositionEndY;
    UInt8 Direction;
)

CLIENT_PROTOCOL(C2S, EXTRACT_ITEM, 411, X596,
    C2S_DATA_SIGNATURE;
    UInt32 InventorySlotIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_EXTRACT_ITEM_SLOT_INDEX,
    UInt64 ItemID;
    UInt64 Unknown1;
    UInt64 ItemOption;
    UInt16 InventorySlotIndex;
    UInt32 Unnown3;
)

CLIENT_PROTOCOL(S2C, EXTRACT_ITEM, 411, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt8 ItemCount;
    S2C_EXTRACT_ITEM_SLOT_INDEX Items[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_MOB_ATTACK_TARGET,
    UInt32 CharacterIndex;
    Bool IsDead;
    UInt8 Result;
    UInt32 AppliedDamage;
    UInt64 TargetHP;
    UInt8 Unknown1[33];
)

CLIENT_PROTOCOL(S2C, NFY_MOB_ATTACK_AOE, 413, X596,
    S2C_DATA_SIGNATURE;
    RTEntityID Entity;
    Bool IsDefaultSkill;
    UInt8 Unknown1;
    UInt64 MobHP;
    UInt32 Unknown2;
    UInt16 TargetCount;
    S2C_DATA_MOB_ATTACK_TARGET Data[0];
)

CLIENT_PROTOCOL(C2S, UNKNOWN_428, 428, X596,
    C2S_DATA_SIGNATURE;
    Int32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_UNKNOWN_428_SLOT_INDEX,
    UInt8 Unknown1[20];
    UInt8 Unknown2;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_428, 428, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1;
    UInt16 Count;
    S2C_DATA_UNKNOWN_428_SLOT_INDEX Slots[0];
)

CLIENT_PROTOCOL(C2S, GET_SERVER_ENVIRONMENT, 464, 13130,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_SERVER_ENVIRONMENT, 464, 13130,
    S2C_DATA_SIGNATURE;
    UInt16 MaxLevel;
    UInt8 DummyEnabled;
    UInt8 CashshopEnabled;
    UInt8 NetCafePointEnabled;
    UInt16 MinChatLevel;
    UInt16 MinShoutLevel;
    UInt16 MinShoutSkillRank;
    UInt64 MaxInventoryCurrency;
    UInt64 MaxWarehouseCurrency;
    UInt64 MaxAuctionCurrency;
    UInt8 NetCafePremiumEnabled;
    UInt8 GuildBoardEnabled;
    UInt8 NetCafePremiumType;
    UInt8 Unknown1;
    UInt8 Unknown2;
    UInt8 Unknown3;
    UInt8 AgentShopEnabled;
    UInt16 MegaphoneShoutCooldownTime;
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
    UInt16 Unknown6;
    UInt16 Unknown7;
    Int64 MaxHonorPoint;
    Int64 MinHonorPoint;
    Int32 Unknown8;
    Int32 Unknown9;
    Int32 Unknown10;
    Int32 Unknown11;
    Int32 Unknown12;
    Int32 Unknown13;
    Int32 Unknown14;
    Int32 Unknown15;
    Int32 Unknown16;
    Int32 Unknown17;
    Int32 Unknown18;
    UInt8 Unknown19[255];
    UInt32 Unknown20;
    UInt32 Unknown21;
    UInt32 Unknown22;
    UInt32 Unknown23;
    UInt8 Unknown24;
)

CLIENT_PROTOCOL(S2C, NFY_PC_BANG_ALERT, 480, 0000,
    S2C_DATA_SIGNATURE;
    // TODO: Add playload
)

CLIENT_PROTOCOL(C2S, CHECK_DUNGEON_PLAYTIME, 485, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 DungeonID;
)

CLIENT_PROTOCOL(S2C, CHECK_DUNGEON_PLAYTIME, 485, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 DungeonID;
    Int32 MaxInstanceCount;
    Int32 InstanceCount;
    UInt32 Unknown1[3];
    UInt16 Unknown2;
    UInt16 RemainingPlayTimeInSeconds;
    UInt32 Unknown3[7];
    UInt16 Unknown4;
    UInt16 MaxEntryCount;
    UInt16 Unknown5;
    UInt32 Unknown6;
)

CLIENT_PROTOCOL(C2S, PUSH_EQUIPMENT_ITEM, 486, X596,
    C2S_DATA_SIGNATURE;
    UInt32 LeftSlotIndex;
    UInt32 RightSlotIndex;
    UInt32 LeftToRightItemType;
)

CLIENT_PROTOCOL(S2C, PUSH_EQUIPMENT_ITEM, 486, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, UPDATE_SKILL_STATUS, 760, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 SkillRank;
    UInt32 SkillLevel;
    UInt32 SkillLevelMax;
    UInt32 SkillExp;
    UInt32 SkillPoint;
)

CLIENT_PROTOCOL_ENUM(
    C2S_DATA_VERIFY_CREDENTIALS_TYPE_UNKNOWN,
    C2S_DATA_VERIFY_CREDENTIALS_TYPE_PASSWORD
)

CLIENT_PROTOCOL(C2S, VERIFY_CREDENTIALS, 800, 13130,
    C2S_DATA_SIGNATURE;
    Int32 CredentialsType;
    Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_CREDENTIALS, 800, 13130,
    S2C_DATA_SIGNATURE;
    Bool Success;
)

CLIENT_PROTOCOL(C2S, UPGRADE_ITEM_LEVEL, 951, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt16 InventorySlotIndex;
    UInt8 CoreCount;
    UInt16 SafeCount;
    UInt16 CoreSlotIndices[60];
    UInt16 SafeSlotIndices[0];
)

CLIENT_PROTOCOL(S2C, UPGRADE_ITEM_LEVEL, 951, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt64 ItemID;
    UInt64 ItemOption;
    UInt32 ItemDuration;
    UInt8 ConsumedSafeCount;
    UInt32 Unknown6;
    UInt32 Unknown7;
    UInt32 RemainingCoreCount;
)

CLIENT_PROTOCOL(C2S, UNKNOWN_985, 985, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_985, 985, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, PREMIUM_BENEFIT_INFO, 999, 0000,
    C2S_DATA_SIGNATURE;
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, PREMIUM_BENEFIT_INFO, 999, 0000,
    S2C_DATA_SIGNATURE_EXTENDED;
    UInt8 Unknown1;
    UInt32 Count;
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(C2S, CREATE_SUBPASSWORD, 1030, 0000,
    C2S_DATA_SIGNATURE;
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 Type;
    UInt32 Question;
    Char Answer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
    UInt8 Unknown2[111];
    UInt32 Mode;
)

CLIENT_PROTOCOL(S2C, CREATE_SUBPASSWORD, 1030, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Success;
    UInt32 Mode;
    UInt32 Type;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, CHECK_SUBPASSWORD, 1032, 13130,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, CHECK_SUBPASSWORD, 1032, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 IsVerificationRequired;
)

CLIENT_PROTOCOL(C2S, VERIFY_SUBPASSWORD, 1034, 0000,
    C2S_DATA_SIGNATURE;
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 Type;
    UInt32 ExpirationInHours;
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(S2C, VERIFY_SUBPASSWORD, 1034, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Success;
    UInt8 FailureCount;
    UInt32 Unknown1;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, VERIFY_DELETE_SUBPASSWORD, 1040, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 Type;
    UInt8 Unknown1[65];
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_DELETE_SUBPASSWORD, 1040, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Success;
    UInt8 FailureCount;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, DELETE_SUBPASSWORD, 1042, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 Type;
)

CLIENT_PROTOCOL(S2C, DELETE_SUBPASSWORD, 1042, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Success;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, ADD_PASSIVE_ABILITY, 1060, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 InventorySlotIndex;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, ADD_PASSIVE_ABILITY, 1060, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, UPGRADE_PASSIVE_ABILITY, 1061, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 AbilityID;
    UInt32 UnknownLevel;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, UPGRADE_PASSIVE_ABILITY, 1061, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, REMOVE_PASSIVE_ABILITY, 1062, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 AbilityID;
    UInt16 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, REMOVE_PASSIVE_ABILITY, 1062, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, BUY_SKILLBOOK, 2003, 0000,
    C2S_DATA_SIGNATURE;
    UInt8 NpcID;
    UInt16 SlotID;
    UInt16 SkillID;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, BUY_SKILLBOOK, 2003, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt32 ItemID;
    UInt32 Unknown3[6];
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_MOB_KILL, 2006, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 MobSpeciesIndex;
    UInt16 SkillIndex;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(C2S, ENTER_DUNGEON_GATE, 2029, 0000,
    C2S_DATA_SIGNATURE;
    UInt32 DungeonID;
    UInt32 Unknown1;
    UInt32 NpcID;
    UInt32 Unknown3;
    Int32 Unknown4;
    UInt32 WorldID;
    UInt8 Unknown5;
    UInt16 DungeonBoostLevel;
)

CLIENT_PROTOCOL(S2C, ENTER_DUNGEON_GATE, 2029, 0000,
    S2C_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(S2C, NFY_MESSAGE_BROADCAST, 2091, X596,
    S2C_DATA_SIGNATURE;
    S2C_DATA_NFY_MESSAGE_BROADCAST_PAYLOAD Payload;
)

CLIENT_PROTOCOL(C2S, GET_SPECIAL_EVENT_CHARACTER, 2156, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_SPECIAL_EVENT_CHARACTER, 2156, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 IsSet;
    UInt8 Payload[520];
)

CLIENT_PROTOCOL(C2S, VERIFY_CREDENTIALS_SUBPASSWORD, 2160, 13130,
    C2S_DATA_SIGNATURE;
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_CREDENTIALS_SUBPASSWORD, 2160, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_ENUM(
    CSC_DATA_STORAGE_TYPE_INVENTORY = 0,
    CSC_DATA_STORAGE_TYPE_EQUIPMENT = 1,
    CSC_DATA_STORAGE_TYPE_WAREHOUSE = 2,
)

CLIENT_PROTOCOL_STRUCT(CSC_DATA_ITEM_SLOT_INDEX,
    UInt32 StorageType;
    UInt32 Index;
)

CLIENT_PROTOCOL(C2S, MOVE_INVENTORY_ITEM, 2165, 0000,
    C2S_DATA_SIGNATURE;
    CSC_DATA_ITEM_SLOT_INDEX Source;
    CSC_DATA_ITEM_SLOT_INDEX Destination;
    UInt16 Unknown1;
    UInt16 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, MOVE_INVENTORY_ITEM, 2165, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, SWAP_INVENTORY_ITEM, 2166, X596,
    C2S_DATA_SIGNATURE;
    CSC_DATA_ITEM_SLOT_INDEX Source1;
    CSC_DATA_ITEM_SLOT_INDEX Destination1;
    CSC_DATA_ITEM_SLOT_INDEX Source2;
    CSC_DATA_ITEM_SLOT_INDEX Destination2;
    UInt16 Unknown1;
    UInt16 Unknown2;
    UInt32 Unknown3;
    UInt32 Unknown4;
)

CLIENT_PROTOCOL(S2C, SWAP_INVENTORY_ITEM, 2166, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, MOVE_INVENTORY_ITEM_LIST, 2167, X596,
    C2S_DATA_SIGNATURE;
    UInt16 ItemCount;
    UInt32 Unknown1[3];
    UInt8 Data[0];
//    CSC_DATA_ITEM_SLOT_INDEX Source[];
//    CSC_DATA_ITEM_SLOT_INDEX Destination[];
)

CLIENT_PROTOCOL(S2C, MOVE_INVENTORY_ITEM_LIST, 2167, X596,
    S2C_DATA_SIGNATURE;
    UInt16 Result;
)

CLIENT_PROTOCOL(C2S, CONVERT_ITEM, 2170, X596,
    C2S_DATA_SIGNATURE;
    UInt32 SourceSlotIndex;
    UInt32 TargetSlotIndex;
)

CLIENT_PROTOCOL(S2C, CONVERT_ITEM, 2170, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Result;
    RTItem Item;
    UInt64 Unknown2;
    UInt64 ItemOptions;
    UInt32 InventorySlotIndex;
    UInt16 Unknown3;
)

CLIENT_PROTOCOL(C2S, UNKNOWN_2173, 2173, X596,
    C2S_DATA_SIGNATURE;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_2173, 2173, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, UPDATE_QUEST_LIST, 2175, X596,
    C2S_DATA_SIGNATURE;
    UInt16 QuestID;
    UInt16 SlotIndex;
    UInt8 DisplayNotice;
    UInt8 DisplayOpenNotice;
)

CLIENT_PROTOCOL(C2S, GET_SHOP_LIST, 2179, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_SHOP_LIST_INDEX,
    UInt32 WorldID;
    UInt8 ShopID;
    UInt16 ShopIndex;
    UInt8 IsCouponShop;
)

CLIENT_PROTOCOL(S2C, GET_SHOP_LIST, 2179, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    S2C_DATA_GET_SHOP_LIST_INDEX Shops[0];
)

CLIENT_PROTOCOL(C2S, GET_SHOP_DATA, 2180, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 ShopIndex;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_SHOP_DATA_INDEX,
    UInt16 Unknown1;
    UInt16 ShopSlotIndex;
    UInt32 ItemID;
    UInt32 ItemType;
    UInt32 ItemOptions;
    UInt32 Unknown2;
    UInt32 ItemDuration;
    UInt8 Unknown5[3];
    Int8 MinHonorRank;
    Int8 MaxHonorRank;
    Int8 Unknown6[6];
    UInt64 AlzPrice;
    UInt64 WexpPrice;
    UInt16 ApPrice;
    UInt16 DpPrice;
    UInt32 Unknown8;
    UInt64 CashPrice;
)

CLIENT_PROTOCOL(S2C, GET_SHOP_DATA, 2180, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 ShopIndex;
    UInt16 Count;
    S2C_DATA_GET_SHOP_DATA_INDEX Data[0];
)

CLIENT_PROTOCOL(C2S, GET_CASH_BALANCE, 2181, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_CASH_BALANCE, 2182, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Amount;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_2248, 2248, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_USER_LIST, 2253, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_UNKNOWN_USER_LIST_INDEX,
    UInt8 Unknown1[18];
    Char CharacterName[MAX_CHARACTER_NAME_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_USER_LIST, 2253, X596,
    S2C_DATA_SIGNATURE;
    UInt32 CharacterCount;
    UInt32 AccountCount;
    UInt32 BuddyCount;
    UInt32 Unknown1[4];
    // S2C_DATA_GET_UNKNOWN_USER_LIST_INDEX[CharacterCount];
    // S2C_DATA_GET_UNKNOWN_USER_LIST_INDEX[AccountCount];
    // S2C_DATA_GET_UNKNOWN_USER_LIST_INDEX[BuddyCount];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_USER_MESSAGE_ROLL_DICE,
    UInt32 Value;
)

CLIENT_PROTOCOL(S2C, NFY_USER_MESSAGE, 2256, X596,
    S2C_DATA_SIGNATURE;
    UInt32 MessageType;
    UInt32 CharacterIndex;
    UInt8 NameLength;
    Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
    UInt8 Data[0];
)

CLIENT_PROTOCOL(C2S, ROLL_DICE, 2261, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_SHOP_ITEM_PRICE, 2308, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_UNKNOWN_SHOP_ITEM_PRICE_INDEX,
    UInt16 SlotIndex;
    RTItem ItemID;
    UInt64 ItemOptions;
    UInt32 Amount;
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_SHOP_ITEM_PRICE, 2308, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Count;
    S2C_DATA_GET_UNKNOWN_SHOP_ITEM_PRICE_INDEX Data[0];
)
/*
CLIENT_PROTOCOL(C2S, GET_KEYMAP, 2502, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_KEYMAP_MACRO,
    UInt8 Length;
    Char Macro[0];
)

CLIENT_PROTOCOL(S2C, GET_KEYMAP, 2502, X596,
    S2C_DATA_SIGNATURE;
    UInt32 KeyBindingsSize;
    UInt32 ConfigsSize;
    UInt32 MacrosSize;
)
*/
CLIENT_PROTOCOL(S2C, UPDATE_UPGRAGE_POINTS, 2506, 0000,
    S2C_DATA_SIGNATURE;
    Int32 UpgradePoint;
    UInt32 Timestamp;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_2518, 2518, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1[6];
)

CLIENT_PROTOCOL(C2S, GET_DUNGEON_REWARD_LIST, 2520, 0000,
    C2S_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(S2C, GET_DUNGEON_REWARD_LIST, 2520, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 PayloadLength;
    UInt32 DungeonID;
    UInt32 Unknown2;
    // S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY Data[0];
)

CLIENT_PROTOCOL(S2C, NFY_DUNGEON_COMPLETE_INFO, 2521, 0000,
    S2C_DATA_SIGNATURE;
    UInt32 PayloadLength;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_2522, 2522, 0000,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_2522, 2522, 0000,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1;
    UInt32 Unknown2[4];
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_2528, 2528, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, UNKNOWN_2559, 2559, X596,
    C2S_DATA_SIGNATURE;
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_2559, 2559, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(C2S, UNKNOWN_2566, 2566, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_2566, 2566, X596,
    S2C_DATA_SIGNATURE;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_2571, 2571, 13130,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_UNKNOWN_2571_INDEX,
    UInt32 SlotIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_2571, 2571, 13130,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    S2C_DATA_UNKNOWN_2571_INDEX Data[0];
)

CLIENT_PROTOCOL(C2S, GET_CASHSHOP_LIST, 2627, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_CASHSHOP_LIST_GROUP_INDEX,
    UInt32 Unknown[3];
)

CLIENT_PROTOCOL(S2C, GET_CASHSHOP_LIST, 2627, X596,
    S2C_DATA_SIGNATURE;
    UInt8 IsActive;
    UInt16 GroupCount;
    S2C_DATA_GET_CASHSHOP_LIST_GROUP_INDEX Groups[0];
)

CLIENT_PROTOCOL(C2S, CHANGE_CHARACTER_SKILL_LEVEL, 2646, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt16 CurrentSkillLevel;
    UInt16 TargetSkillLevel;
)

CLIENT_PROTOCOL(S2C, CHANGE_CHARACTER_SKILL_LEVEL, 2646, 0000,
    S2C_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(C2S, ADD_CHARACTER_STATS, 2648, X596,
    C2S_DATA_SIGNATURE;
    Int32 Stat[3];
    UInt32 StatDelta[3];
)

CLIENT_PROTOCOL(S2C, ADD_CHARACTER_STATS, 2648, X596,
    S2C_DATA_SIGNATURE;
    Int32 Result;
    UInt32 Stat[3];
)

CLIENT_PROTOCOL(C2S, REMOVE_CHARACTER_STATS, 2649, X596,
    C2S_DATA_SIGNATURE;
    Int32 Stat[3];
    Int32 StatDelta[3];
    UInt8 StatFlag[3];
    UInt16 InventorySlots[0];
)

CLIENT_PROTOCOL(S2C, REMOVE_CHARACTER_STATS, 2649, X596,
    S2C_DATA_SIGNATURE;
    Int32 Result;
    UInt32 Stat[3];
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_2756, 2756, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1[9];
)

CLIENT_PROTOCOL(C2S, ADD_OVERLORD_MASTERY_SLOT, 2763, X596,
    C2S_DATA_SIGNATURE;
    UInt8 MasteryIndex;
)

CLIENT_PROTOCOL(S2C, ADD_OVERLORD_MASTERY_SLOT, 2763, X596,
    S2C_DATA_SIGNATURE;
    UInt8 MasteryIndex;
    UInt8 Level;
)

CLIENT_PROTOCOL(C2S, OPEN_GIFTBOX, 2797, X596,
    C2S_DATA_SIGNATURE;
    UInt8 Index;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_OPEN_GIFTBOX_ITEM,
    UInt64 ItemID;
    UInt64 ItemOptions;
)

CLIENT_PROTOCOL(S2C, OPEN_GIFTBOX, 2797, X596,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    S2C_DATA_OPEN_GIFTBOX_ITEM Data[0];
)

CLIENT_PROTOCOL(C2S, SORT_INVENTORY, 2800, CHECK_OST,
    C2S_DATA_SIGNATURE;
    UInt16 Count;
    UInt8 UnknownSlotIndices[1];
)

CLIENT_PROTOCOL(S2C, SORT_INVENTORY, 2800, CHECK_OST,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_OPEN_GIFTBOX_ITEMSLOT,
    UInt16 InventorySlotIndex;
    UInt16 Unknown;
)

CLIENT_PROTOCOL(C2S, OPEN_GIFTBOX_UNKNOWN, 2819, 0000,
    C2S_DATA_SIGNATURE;
    UInt16 Count;
    C2S_DATA_OPEN_GIFTBOX_ITEMSLOT Data[1];
)

CLIENT_PROTOCOL(S2C, OPEN_GIFTBOX_UNKNOWN, 2819, 0000,
    S2C_DATA_SIGNATURE;
    UInt16 Count;
    UInt8 Unknown[43];
)

CLIENT_PROTOCOL(C2S, UNKNOWN_2828, 2828, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_2828, 2828, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1;
    UInt32 Unknown2[5];
)

CLIENT_PROTOCOL(C2S, REGISTER_COLLECTION_ITEM, 2830, X596,
    C2S_DATA_SIGNATURE;
    UInt8 TypeID;
    UInt8 CollectionID;
    UInt16 Unknown1;
    UInt16 MissionID;
    UInt8 MissionSlotIndex;
    UInt32 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, REGISTER_COLLECTION_ITEM, 2830, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
    UInt16 Unknown1;
    UInt8 TypeID;
    UInt8 CollectionID;
    UInt16 MissionID;
    UInt8 MissionSlotIndex;
    UInt16 MissionItemCounts[RUNTIME_CHARACTER_MAX_COLLECTION_ITEM_COUNT];
)

CLIENT_PROTOCOL(C2S, RECEIVE_COLLECTION_REWARD, 2831, X596,
    C2S_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(S2C, RECEIVE_COLLECTION_REWARD, 2831, X596,
    S2C_DATA_SIGNATURE;
    UInt8 Success;
    UInt16 Unknown1;
    UInt32 RewardType;
    UInt16 InventorySlotCount;
)

CLIENT_PROTOCOL(S2C, NFY_UPDATE_TRANSCENDENCE_POINTS, 2926, X596,
    S2C_DATA_SIGNATURE;
    UInt32 TranscendencePoints;
)

CLIENT_PROTOCOL(C2S, GET_EVENT_PASS_REWARD_LIST, 2999, X596,
    C2S_DATA_SIGNATURE;
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

CLIENT_PROTOCOL(S2C, GET_EVENT_PASS_REWARD_LIST, 2999, X596,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt32 RewardSlotCount;
    S2C_DATA_GET_EVENT_PASS_REWARD_LIST_SLOT RewardSlots[1];
)

CLIENT_PROTOCOL(C2S, GET_EVENT_PASS_SEASON, 3000, X596,
    C2S_DATA_SIGNATURE;
)

CLIENT_PROTOCOL(S2C, GET_EVENT_PASS_SEASON, 3000, X596,
    S2C_DATA_SIGNATURE;
    UInt32 ExpirationDate;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_PASS_PROGRESS, 3001, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 GroupID;
    UInt32 MissionID;
    UInt32 Progress;
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_PASS_RESUME, 3002, 13130,
    S2C_DATA_SIGNATURE;
    UInt32 Unknown1;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_3016, 3016, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1;
    Timestamp UnknownTimestamp1;
    Timestamp UnknownTimestamp2;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_5305, 5305, 13130,
    S2C_DATA_SIGNATURE;
    UInt8 Unknown1[6];
)

#undef CLIENT_PROTOCOL_ENUM
#undef CLIENT_PROTOCOL_STRUCT
#undef CLIENT_PROTOCOL
