
#include "Constants.h"
#include "GameProtocol.h"

#ifndef CLIENT_PROTOCOL_ENUM
#define CLIENT_PROTOCOL_ENUM(...)
#endif

#ifndef CLIENT_PROTOCOL_STRUCT
#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef CLIENT_PROTOCOL
#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __SIGNATURE__, __COMMAND__, __BODY__)
#endif

CLIENT_PROTOCOL_STRUCT(CSC_POSITION,
    UInt16 X;
    UInt16 Y;
)

CLIENT_PROTOCOL(S2C, NFY_ERROR, DEFAULT, 7,
    UInt32 ErrorType;
    UInt16 ErrorCode;
    UInt16 Unknown1;
)

CLIENT_PROTOCOL(C2S, GET_CHARACTERS, DEFAULT, 133,
)

CLIENT_PROTOCOL(S2C, GET_CHARACTERS, DEFAULT, 133,
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
    UInt8 WorldServerID;
    UInt16 PlayerCount;
    UInt8 Unknown1[18];
    UInt8 MaxPlayerLevel;
    UInt8 MinPlayerLevel;
    UInt8 MaxRank;
    UInt8 MinRank;
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

CLIENT_PROTOCOL_STRUCT(S2C_DATA_COLLECTION_SLOT,
    UInt8 TypeID;
    UInt8 CollectionID;
    UInt16 MissionID;
    UInt8 ReceivedReward;
    UInt16 MissionItemCounts[6];
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
    RTEntityID Entity;
    UInt32 WorldIndex;
    UInt32 DungeonIndex;
    CSC_POSITION Position;
    UInt64 Exp;
    UInt64 Alz;
    UInt64 Wexp;
    Int32 Level;
    UInt32 Unknown1;
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
    Timestamp DPDuration;
    UInt32 DP;
    UInt32 SkillLevel;
    UInt32 SkillExp;
    UInt32 SkillPoint;
    UInt64 RestExp;
    UInt64 HonorPoint;
    UInt64 DeathPenaltyExp;
    UInt64 DeathPenaltyHp;
    UInt32 DeathPenaltyMp;
    UInt16 PKLevel;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS ChatServerAddress;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS AuctionServerAddress;
    S2C_DATA_INITIALIZE_SERVER_ADDRESS PartyServerAddress;
    UInt16 UnknownPort;
    UInt8 Nation;
    UInt32 Unknown2;
    UInt32 WarpMask;
    UInt32 MapsMask;
    UInt32 CharacterStyle;
    UInt32 CharacterLiveStyle;
    UInt8 Unknown14[282];
    UInt16 EquipmentSlotCount;
    UInt8 EquipmentLockCount;
    UInt16 InventorySlotCount;
    UInt16 VehicleInventorySlotCount;
    UInt16 SkillSlotCount;
    UInt16 QuickSlotCount;
    UInt16 MercenaryCount;
    UInt16 ItemPeriodCount;
    UInt16 UnknownCount2;
    UInt16 AP;
    UInt32 Axp;
    UInt8 EssenceAbilityCount;
    UInt8 ExtendedEssenceAbilityCount;
    UInt8 BlendedAbilityCount;
    UInt8 ExtendedBlendedAbilityCount;
    UInt8 KarmaAbilityCount;
    UInt8 ExtendedKarmaAbilityCount;
    UInt8 BlessingBeadCount;
    UInt8 PremiumServiceCount;
    UInt16 QuestSlotCount;
    struct _RTCharacterQuestFlagInfo QuestFlagInfo;
    struct _RTCharacterDungeonQuestFlagInfo DungeonQuestFlagInfo;
    UInt8 Unknown15[4097];
    UInt32 DailyQuestCount;
    UInt32 HelpWindow;

    struct {
        UInt32 PetSerial;
        UInt32 PetId;
        UInt32 PetOwnerId;
        UInt64 PetItemKind;
        UInt8 PetLevel;
        UInt32 PetExp;
        Char PetName[MAX_CHARACTER_NAME_LENGTH + 1];
        UInt8 PetSkills[33];
        UInt32 DeleteDate;
        UInt32 Checked;
    } Pet[3];

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
    Int32 ReservedAchievementScore1;
    Int32 ReservedAchievementScore2;
    UInt16 DisplayTitle;
    UInt16 EventTitle;
    UInt16 GuildTitle;
    UInt16 WarTitle;
    UInt32 AchievementCount;
    UInt32 AchievementRewardCount;
    UInt32 UnknownCount5;
    UInt32 CraftCount;
    UInt8 Unknown16;
    UInt32 CraftEnergy;
    UInt8 Unknown17[6];
    UInt8 SortingOrderMask;
    UInt8 Unknown18[2];
    UInt8 RequestCraftCount;
    UInt16 RequestCraftExp;
    UInt8 RequestCraftFlags[1024];
    UInt8 RequestCraftFavoriteFlags[1024];
    UInt16 RequestCraftSortOrder;
    UInt8 SkillCooldownCount;
    UInt16 Unknown2389;
    UInt16 BuffCount;
    UInt32 SpiritRaiseBuffCooldown;
    UInt32 UpgradePoints;
    Timestamp UpgradePointTimestamp;
    UInt32 GoldMeritCount;
    UInt32 GoldMeritExp;
    UInt32 GoldMeritPoint;
    UInt8 IsPlatinumMeritActive;
    UInt32 PlatinumMeritExp;
    UInt32 PlatinumMeritPoint[2];
    UInt8 Unknown21;
    Timestamp CharacterCreationDate;
    UInt16 PlatinumMeritUnknownCount;
    UInt16 PlatinumMeritSlotCount;
    UInt16 PlatinumMeritUnknownCount2;
    UInt16 PlatinumMeritUnknownCount3;
    UInt8 Unknown22B;
    UInt16 Unknown23;
    UInt32 Unknown33;
    UInt64 Unknown3874;
    Timestamp UnknownTimestamp2;
    UInt8 Unknown34;
    Int32 Unknown35;
    UInt64 Unknown36;
    UInt32 ExtendedAchievementRewardCount;
    UInt32 ForceGem;
    UInt32 WarpServiceCount;
    UInt16 Unknown3332[5];
    UInt16 OverlordLevel;
    UInt64 OverlordExp;
    UInt16 OverlordPoint;
    UInt8 OverlordMasteryCount;
    UInt32 UnknownMeritMasteryCount;
    UInt32 HonorMedalExp;
    UInt8 HonorMedalCount;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    Int64 ForceWingExp;
    UInt64 ForceWingUnknown1;
    // TODO: Store the force wing data just in the same memory layout and make the fixed size part of 142 dynamic
    UInt8 ForceWingActivePresetIndex;
    UInt8 ForceWingPresetEnabled[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT];
    Int32 ForceWingPresetTrainingPoints[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT];
    UInt8 ForceWingPresetSlotCount;
    UInt8 ForceWingTrainingSlotCount;
    struct _RTForceWingArrivalSkillSlot ForceWingArrivalSkillSlots[RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_SKILL_COUNT];
    struct _RTForceWingArrivalSkillSlot RestoreForceWingArrivalSkillSlot;
    UInt8 ForceWingTrainingUnlockFlags[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_SIZE];
    UInt8 Unknown2111[21];
    UInt16 SpecialGiftboxPoint;
    UInt8 SpecialGiftboxCount;
    UInt32 Unknown2733[9];
    UInt16 CollectionCount;
    UInt16 TransformCount;
    UInt8 Unknown32[6];
    UInt8 SecretShopRefreshCost;
    Timestamp SecretShopResetTime;
    UInt8 Unknown1111;
    S2C_DATA_SECRET_SHOP_INDEX SecretShops[MAX_SECRET_SHOP_COUNT];
    UInt8 Unknown44[9];
    UInt32 TranscendencePoint;
    UInt32 TranscendenceCount;
    UInt8 Unknown45;
    UInt32 Unknown27A[152];
    UInt16 Unknown28A;
    UInt16 Unknown28B;
    struct { UInt32 Unknown1[4]; Int32 Unknown2[2]; } UnknownSlot[100];
    Int32 Unknown29;
    UInt8 StellarSlotCount;
    Int32 MythRebirth;
    Int32 MythHolyPower;
    Int32 MythLevel;
    UInt64 MythExp;
    Int32 MythPoints;
    Int32 MythUnlockedPageCount;
    UInt8 Poop[13];
    UInt8 MythPropertySlotCount;
    Int32 MythStigmaGrade;
    Int32 MythStigmaExp;
    Timestamp NewbieSupportTimestamp;
    Int32 NewbieSupportSlotCount;
    Timestamp ExplorationStartTimestamp;
    Timestamp ExplorationEndTimestamp;
    Int32 UnknownA5;
    Int32 EventPassMissionCount;
    Int32 EventPassRewardCount;
    Int32 AccountCostumeSlotCount;
    Int32 Unknown1293;
    Int32 CostumePageCount;
    Int32 CostumeUnknownSlotCount;
    Int32 CostumeActivePageIndex;
    Timestamp Unknown1231Timestamp;
    UInt8 Unknown8182[12];
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
    UInt32 SlotIndexItemPrice;
    UInt32 AmountItemPrice;
)

CLIENT_PROTOCOL(C2S, BUY_ITEM, DEFAULT, 161,
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

CLIENT_PROTOCOL(S2C, BUY_ITEM, DEFAULT, 161,
    UInt32 Unknown1;
    UInt64 ItemID;
    UInt64 ItemOptions;
    UInt16 Unknown2;
    UInt16 Unknown3;
    UInt16 Unknown4;
)

CLIENT_PROTOCOL(C2S, SELL_ITEM, DEFAULT, 162,
    UInt16 NpcID;
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
    CSC_POSITION PositionBegin;
    CSC_POSITION PositionEnd;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_ASTRAL,
    UInt16 IsActivation;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_SKILL_GROUP_BATTLE_MODE,
    UInt16 IsActivation;
)

CLIENT_PROTOCOL(C2S, SKILL_TO_CHARACTER, DEFAULT, 175,
    UInt16 SkillIndex;
    UInt8 SlotIndex;
    UInt8 Data[0];
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
    // TODO: Add packet layout
)

CLIENT_PROTOCOL(S2C, ATTACK_TO_CHARACTER, DEFAULT, 177,
    // TODO: Add packet layout
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
    S2C_DATA_ENTITY_DESPAWN_TYPE_DEAD = 0x10,
    S2C_DATA_ENTITY_DESPAWN_TYPE_WARP,
    S2C_DATA_ENTITY_DESPAWN_TYPE_UNKNOWN1,
    S2C_DATA_ENTITY_DESPAWN_TYPE_RETURN,
    S2C_DATA_ENTITY_DESPAWN_TYPE_DISAPPEAR,
    S2C_DATA_ENTITY_DESPAWN_TYPE_NOTIFY_DEAD
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
    UInt32 Unknown2;
    S2C_DATA_NFY_SKILL_TO_MOB_TARGET Data[0];
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

CLIENT_PROTOCOL(S2C, NFY_SKILL_TO_CHARACTER, DEFAULT, 221,
    UInt16 SkillIndex;
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

CLIENT_PROTOCOL(C2S, WARP, DEFAULT, 244,
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
    UInt8 Data[1];
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
        struct { UInt8 _7[34]; UInt32 Level; UInt8 _8[8]; };
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
    UInt32 Unknown1;
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

CLIENT_PROTOCOL_ENUM(
    S2C_DATA_REPUTATION_CHANGE_TYPE_AUTOTRAP_HIT = 0x2,
    S2C_DATA_REPUTATION_CHANGE_TYPE_GUILD_WIN = 0x3,
    S2C_DATA_REPUTATION_CHANGE_TYPE_NATION_KILL = 0x4,
    S2C_DATA_REPUTATION_CHANGE_TYPE_MOBS_KILL = 0x5,
    S2C_DATA_REPUTATION_CHANGE_TYPE_GUILD_KILL = 0x6,
    S2C_DATA_REPUTATION_CHANGE_TYPE_UNKNOWN7 = 0x7,
)

CLIENT_PROTOCOL(C2S, REPUTATION_CHANGE, DEFAULT, 346,
    UInt8 Type;
    UInt8 Data[0];
)

CLIENT_PROTOCOL(S2C, REPUTATION_CHANGE, DEFAULT, 346,
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, NFY_REPUTATION_CHANGE, DEFAULT, 347,
    // TODO: Add packet structure
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

CLIENT_PROTOCOL_STRUCT(S2C_DATA_MOB_ATTACK_TARGET,
    UInt32 CharacterIndex;
    Bool IsDead;
    UInt8 Result;
    UInt32 AppliedDamage;
    UInt64 TargetHP;
    UInt8 Unknown1[33];
)

CLIENT_PROTOCOL(S2C, NFY_MOB_ATTACK_AOE, DEFAULT, 413,
    RTEntityID Entity;
    Bool IsDefaultSkill;
    UInt8 Unknown1;
    UInt64 MobHP;
    UInt32 Unknown2;
    UInt16 TargetCount;
    S2C_DATA_MOB_ATTACK_TARGET Data[0];
)

CLIENT_PROTOCOL(C2S, UNKNOWN_428, DEFAULT, 428,
    Int32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_UNKNOWN_428_SLOT_INDEX,
    UInt8 Unknown1[20];
    UInt8 Unknown2;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_428, DEFAULT, 428,
    UInt8 Unknown1;
    UInt16 Count;
    S2C_DATA_UNKNOWN_428_SLOT_INDEX Slots[0];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_ENVIRONMENT_WORLD_RESTRICTION,
    Int32 Index;
    Int32 StartIndex;
    Int32 Count;
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
    UInt8 MaxSkillRank;
    UInt8 MaxMagicRank;
    Int64 MaxHonorPoint;
    Int64 MinHonorPoint;
    Int32 Unknown8;
    Int32 UnknownCount;
    // S2C_DATA_ENVIRONMENT_WORLD_RESTRICTION Restrictions[UnknownCount];
    UInt8 Unknown19[255];
    UInt32 Unknown20;
    UInt32 Unknown21;
    UInt32 Unknown22;
    UInt32 Unknown23;
    UInt8 Unknown24;
)

CLIENT_PROTOCOL(S2C, NFY_PC_BANG_ALERT, DEFAULT, 480,
    // TODO: Add playload
)

CLIENT_PROTOCOL(C2S, CHECK_DUNGEON_PLAYTIME, DEFAULT, 485,
    UInt32 DungeonID;
)

CLIENT_PROTOCOL(S2C, CHECK_DUNGEON_PLAYTIME, DEFAULT, 485,
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

CLIENT_PROTOCOL(C2S, PUSH_EQUIPMENT_ITEM, DEFAULT, 486,
    UInt32 LeftSlotIndex;
    UInt32 RightSlotIndex;
    UInt32 LeftToRightItemType;
)

CLIENT_PROTOCOL(S2C, PUSH_EQUIPMENT_ITEM, DEFAULT, 486,
    UInt8 Result;
)

CLIENT_PROTOCOL(S2C, UPDATE_SKILL_STATUS, DEFAULT, 760,
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

CLIENT_PROTOCOL(C2S, VERIFY_CREDENTIALS, DEFAULT, 800,
    Int32 CredentialsType;
    Char Credentials[MAX_CREDENTIALS_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_CREDENTIALS, DEFAULT, 800,
    Bool Success;
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

CLIENT_PROTOCOL(C2S, UNKNOWN_985, DEFAULT, 985,
)

CLIENT_PROTOCOL(S2C, UNKNOWN_985, DEFAULT, 985,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, PREMIUM_BENEFIT_INFO, DEFAULT, 999,
    // TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, PREMIUM_BENEFIT_INFO, EXTENDED, 999,
    UInt8 Unknown1;
    UInt32 Count;
    // TODO: Add packet structure
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
    UInt8 Unknown1;
    Int32 UnknownIndex;
    Int32 TextureIndex;
    UInt64 CurrencyPrice;
    UInt16 Unknown2;
    UInt32 ItemPriceCount;
    UInt16 Unknown5;
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
    UInt8 Unknown1;
    UInt8 EventCount;
    UInt32 Unknown2;
    // S2C_DATA_EVENT Events[];
)

// TODO: Add support for additional event action types!
CLIENT_PROTOCOL(C2S, EVENT_ACTION, DEFAULT, 1006,
    Int32 EventIndex;
    Int32 Unknown1;
    UInt8 NpcIndex;
    UInt32 ShopSlotIndex;
    UInt16 InventorySlotCount;
    UInt16 InventorySlotIndex[0];
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
    UInt8 Unknown2;
    UInt16 ItemCount;
    // S2C_DATA_EVENT_ACTION_SHOP_ITEM Items[InventorySlotCount];
)

CLIENT_PROTOCOL(S2C, NFY_EVENT_LIST, DEFAULT, 1008,
    UInt8 Count;
    Int32 EventIndex[];
)

CLIENT_PROTOCOL(C2S, CREATE_SUBPASSWORD, DEFAULT, 1030,
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 Type;
    UInt32 Question;
    Char Answer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
    UInt8 Unknown2[111];
    UInt32 Mode;
)

CLIENT_PROTOCOL(S2C, CREATE_SUBPASSWORD, DEFAULT, 1030,
    UInt32 Success;
    UInt32 Mode;
    UInt32 Type;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, CHECK_SUBPASSWORD, DEFAULT, 1032,
)

CLIENT_PROTOCOL(S2C, CHECK_SUBPASSWORD, DEFAULT, 1032,
    UInt32 IsVerificationRequired;
)

CLIENT_PROTOCOL(C2S, VERIFY_SUBPASSWORD, DEFAULT, 1034,
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 Type;
    UInt32 ExpirationInHours;
    UInt8 Unknown1;
)

CLIENT_PROTOCOL(S2C, VERIFY_SUBPASSWORD, DEFAULT, 1034,
    UInt32 Success;
    UInt8 FailureCount;
    UInt32 Unknown1;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, VERIFY_DELETE_SUBPASSWORD, DEFAULT, 1040,
    UInt32 Type;
    UInt8 Unknown1[65];
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_DELETE_SUBPASSWORD, DEFAULT, 1040,
    UInt32 Success;
    UInt8 FailureCount;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, DELETE_SUBPASSWORD, DEFAULT, 1042,
    UInt32 Type;
)

CLIENT_PROTOCOL(S2C, DELETE_SUBPASSWORD, DEFAULT, 1042,
    UInt32 Success;
    UInt32 Type;
)

CLIENT_PROTOCOL(C2S, ADD_PASSIVE_ABILITY, DEFAULT, 1060,
    UInt16 InventorySlotIndex;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, ADD_PASSIVE_ABILITY, DEFAULT, 1060,
    UInt32 Result;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(C2S, UPGRADE_PASSIVE_ABILITY, DEFAULT, 1061,
    UInt32 AbilityID;
    UInt32 UnknownLevel;
    UInt16 InventorySlotIndex[0];
)

CLIENT_PROTOCOL(S2C, UPGRADE_PASSIVE_ABILITY, DEFAULT, 1061,
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, REMOVE_PASSIVE_ABILITY, DEFAULT, 1062,
    UInt32 AbilityID;
    UInt16 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, REMOVE_PASSIVE_ABILITY, DEFAULT, 1062,
    UInt32 Result;
)

CLIENT_PROTOCOL(C2S, BUY_SKILLBOOK, DEFAULT, 2003,
    UInt8 NpcID;
    UInt16 SlotID;
    UInt16 SkillID;
    UInt16 InventorySlotIndex;
)

CLIENT_PROTOCOL(S2C, BUY_SKILLBOOK, DEFAULT, 2003,
    UInt32 Unknown1;
    UInt32 ItemID;
    UInt32 Unknown3[6];
)

CLIENT_PROTOCOL(S2C, NFY_QUEST_MOB_KILL, DEFAULT, 2006,
    UInt16 MobSpeciesIndex;
    UInt16 SkillIndex;
    UInt16 Unknown1;
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

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PARTY_MEMBER,
    UInt32 CharacterIndex;
    Int32 Level;
    Int32 Unknown1;
    Int32 Unknown2;
    UInt8 Unknown3;
    UInt8 BattleStyleIndex;
    Int32 Unknown5;
    UInt16 OverlordLevel;
    Int32 MythRebirth;
    Int32 MythHolyPower;
    Int32 MythLevel;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    UInt8 NameLength;
    Char Name[RUNTIME_CHARACTER_MAX_NAME_LENGTH];
    UInt8 Unknown6[36];
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INFO, DEFAULT, 2016,
    UInt32 Result;
    UInt32 LeaderCharacterIndex;
    UInt32 Unknown1;
    UInt8 Unknown2;
    UInt8 WorldServerID;
    UInt8 Unknown3;
    UInt32 MemberCount;
    S2C_DATA_PARTY_MEMBER Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
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
// TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, PARTY_LEAVE, DEFAULT, 2019,
// TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, PARTY_DISBAND, DEFAULT, 2020,
// TODO: Add packet structure
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
// TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, PARTY_CHANGE_LEADER, DEFAULT, 2023,
// TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_CHANGE_LEADER, DEFAULT, 2024,
// TODO: Add packet structure
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INVITE_RULE, DEFAULT, 2026,
// TODO: Add packet structure
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PARTY_MEMBER_DATA,
    Int32 Unknown1;
    Int64 MaxHP;
    Int64 CurrentHP;
    Int32 MaxMP;
    Int32 CurrentMP;
    Int32 PositionX;
    Int32 PositionY;
    Int16 MaxSP;
    Int16 CurrentSP;
    Int32 Unknown2;
    Int32 Unknown3;
    UInt8 Unknown4[24];
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_PARTY_UPDATE_MEMBER,
    S2C_DATA_PARTY_MEMBER Info;
    S2C_DATA_PARTY_MEMBER_DATA Data;
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

CLIENT_PROTOCOL_STRUCT(S2C_DATA_NFY_PARTY_INIT_MEMBER,
    UInt32 CharacterIndex;
    Int32 Level;
    Int32 Unknown1;
    Int32 Unknown2;
    UInt8 Unknown3;
    UInt8 BattleStyleIndex;
    Int32 Unknown5;
    UInt16 OverlordLevel;
    Int32 MythRebirth;
    Int32 MythHolyPower;
    Int32 MythLevel;
    UInt8 ForceWingGrade;
    UInt8 ForceWingLevel;
    UInt8 NameLength;
    Char Name[MAX_CHARACTER_NAME_LENGTH];
    UInt8 Unknown6[36];
)

CLIENT_PROTOCOL(S2C, NFY_PARTY_INIT, DEFAULT, 2056,
    Int32 Result;
    UInt32 DungeonIndex;
    UInt16 Unknown1;
    RTEntityID PartyID;
    //RTEntityID PartyLeaderID;
    UInt32 PartyLeaderIndex;
    Int32 Unknown2;
    UInt8 Unknown3;
    UInt8 WorldServerIndex;
    UInt8 Unknown4;
    Int32 MemberCount;
    S2C_DATA_NFY_PARTY_INIT_MEMBER Members[RUNTIME_PARTY_MAX_MEMBER_COUNT];
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

CLIENT_PROTOCOL(C2S, GET_SERVER_STATUS, DEFAULT, 2141,
    UInt8 GroupIndex;
    UInt8 NodeIndex;
)

CLIENT_PROTOCOL(S2C, GET_SERVER_STATUS, DEFAULT, 2141,
    Int32 Result;
)

CLIENT_PROTOCOL(C2S, GET_SPECIAL_EVENT_CHARACTER, DEFAULT, 2156,
)

CLIENT_PROTOCOL(S2C, GET_SPECIAL_EVENT_CHARACTER, DEFAULT, 2156,
    UInt8 IsSet;
    UInt8 Payload[520];
)

CLIENT_PROTOCOL(C2S, VERIFY_CREDENTIALS_SUBPASSWORD, DEFAULT, 2160,
    Char Password[MAX_SUBPASSWORD_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, VERIFY_CREDENTIALS_SUBPASSWORD, DEFAULT, 2160,
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

CLIENT_PROTOCOL(C2S, UNKNOWN_2173, DEFAULT, 2173,
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, UNKNOWN_2173, DEFAULT, 2173,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, UPDATE_QUEST_LIST, DEFAULT, 2175,
    UInt16 QuestID;
    UInt16 SlotIndex;
    UInt8 DisplayNotice;
    UInt8 DisplayOpenNotice;
)

CLIENT_PROTOCOL(C2S, GET_SHOP_LIST, DEFAULT, 2179,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_SHOP_LIST_INDEX,
    UInt32 WorldID;
    UInt8 ShopID;
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
    UInt16 Unknown1;
    UInt16 ShopSlotIndex;
    UInt32 ItemID;
    UInt32 ItemType;
    UInt64 ItemOptions;
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

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_2248, DEFAULT, 2248,
    UInt32 Unknown1;
)

CLIENT_PROTOCOL_STRUCT(C2S_REQUEST_CRAFT_INVENTORY_SLOT,
    Int32 InventorySlotIndex;
    Int32 Count;
)

CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_REGISTER, DEFAULT, 2249,
    UInt32 RequestCode;
    Int32 InventorySlotCount;
    C2S_REQUEST_CRAFT_INVENTORY_SLOT InventorySlots[0];
    // UInt8 Unknown2[4168]; - Maybe AuthCaptcha
)

CLIENT_PROTOCOL(S2C, REQUEST_CRAFT_REGISTER, DEFAULT, 2249,
    UInt8 Result;
)

CLIENT_PROTOCOL(C2S, REQUEST_CRAFT_START, DEFAULT, 2250,
    UInt32 RequestCode;
    UInt8 RequestSlotIndex;
    Int32 Unknown1; // - Maybe AuthCaptcha
    Int32 InventorySlotCount;
    C2S_REQUEST_CRAFT_INVENTORY_SLOT InventorySlots[0];
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
    C2S_REQUEST_CRAFT_INVENTORY_SLOT InventorySlots[0];
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

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_USER_LIST, DEFAULT, 2253,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_UNKNOWN_USER_LIST_INDEX,
    UInt8 Unknown1[18];
    Char CharacterName[MAX_CHARACTER_NAME_LENGTH + 1];
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_USER_LIST, DEFAULT, 2253,
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

CLIENT_PROTOCOL(S2C, NFY_USER_MESSAGE, DEFAULT, 2256,
    UInt32 MessageType;
    UInt32 CharacterIndex;
    UInt8 NameLength;
    Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
    UInt8 Data[0];
)

CLIENT_PROTOCOL(C2S, ROLL_DICE, DEFAULT, 2261,
)

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_SHOP_ITEM_PRICE, DEFAULT, 2308,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_UNKNOWN_SHOP_ITEM_PRICE_INDEX,
    UInt16 SlotIndex;
    RTItem ItemID;
    UInt64 ItemOptions;
    UInt32 Amount;
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_SHOP_ITEM_PRICE, DEFAULT, 2308,
    UInt32 Count;
    S2C_DATA_GET_UNKNOWN_SHOP_ITEM_PRICE_INDEX Data[0];
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

CLIENT_PROTOCOL(C2S, MERGE_INVENTORY, DEFAULT, 2501,
    UInt16 MergeInventorySlotCount;
    UInt16 ResultInventorySlotCount;
    UInt16 Unknown1;
    UInt16 MergeInventorySlotIndex[0]; // MergeSlotCount
    // C2S_DATA_MERGE_INVENTORY_SLOT_RESULT ResultInventorySlots[ResultInventorySlotCount];
)

CLIENT_PROTOCOL(S2C, MERGE_INVENTORY, DEFAULT, 2501,
    UInt8 Result;
)

/*
CLIENT_PROTOCOL(C2S, GET_KEYMAP, DEFAULT, 2502,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_GET_KEYMAP_MACRO,
    UInt8 Length;
    Char Macro[0];
)

CLIENT_PROTOCOL(S2C, GET_KEYMAP, DEFAULT, 2502,
    UInt32 KeyBindingsSize;
    UInt32 ConfigsSize;
    UInt32 MacrosSize;
)
*/

CLIENT_PROTOCOL(S2C, UPDATE_UPGRADE_POINTS, DEFAULT, 2506,
    Int32 UpgradePoint;
    UInt32 Timestamp;
    UInt32 Unknown1;
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_2518, DEFAULT, 2518,
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
    UInt32 Unknown2;
    // S2C_DATA_GET_DUNGEON_REWARD_LIST_ENTRY Data[0];
)

CLIENT_PROTOCOL(S2C, NFY_DUNGEON_COMPLETE_INFO, DEFAULT, 2521,
    UInt32 PayloadLength;
    UInt32 Unknown2;
    UInt32 Unknown3;
)

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_2522, DEFAULT, 2522,
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_2522, DEFAULT, 2522,
    UInt8 Unknown1;
    UInt32 Unknown2[4];
)

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_2528, DEFAULT, 2528,
    UInt32 Unknown1;
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

CLIENT_PROTOCOL(C2S, GET_UNKNOWN_2571, DEFAULT, 2571,
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_UNKNOWN_2571_INDEX,
    UInt32 SlotIndex;
    UInt32 Unknown1;
    UInt32 Unknown2;
)

CLIENT_PROTOCOL(S2C, GET_UNKNOWN_2571, DEFAULT, 2571,
    UInt16 Count;
    S2C_DATA_UNKNOWN_2571_INDEX Data[0];
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

CLIENT_PROTOCOL(C2S, OPEN_GIFTBOX, DEFAULT, 2797,
    UInt8 Index;
)

CLIENT_PROTOCOL_STRUCT(S2C_DATA_OPEN_GIFTBOX_ITEM,
    UInt64 ItemID;
    UInt64 ItemOptions;
)

CLIENT_PROTOCOL(S2C, OPEN_GIFTBOX, DEFAULT, 2797,
    UInt16 Count;
    S2C_DATA_OPEN_GIFTBOX_ITEM Data[0];
)

CLIENT_PROTOCOL(C2S, SORT_INVENTORY, DEFAULT, 2800,
    UInt16 Count;
    UInt16 InventorySlots[1024];
)

CLIENT_PROTOCOL(S2C, SORT_INVENTORY, DEFAULT, 2800,
    UInt8 Success;
)

CLIENT_PROTOCOL(C2S, SORT_WAREHOUSE, DEFAULT, 2802,
    UInt16 Count;
    UInt16 WarehouseSlots[2048];
)

CLIENT_PROTOCOL(S2C, SORT_WAREHOUSE, DEFAULT, 2802,
    UInt8 Success;
)

CLIENT_PROTOCOL_STRUCT(C2S_DATA_OPEN_GIFTBOX_ITEMSLOT,
    UInt16 InventorySlotIndex;
    UInt16 Unknown;
)

CLIENT_PROTOCOL(C2S, OPEN_GIFTBOX_UNKNOWN, DEFAULT, 2819,
    UInt16 Count;
    C2S_DATA_OPEN_GIFTBOX_ITEMSLOT Data[1];
)

CLIENT_PROTOCOL(S2C, OPEN_GIFTBOX_UNKNOWN, DEFAULT, 2819,
    UInt16 Count;
    UInt8 Unknown[43];
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
    UInt16 Unknown1;
    UInt8 TypeID;
    UInt8 CollectionID;
    UInt16 MissionID;
    UInt8 MissionSlotIndex;
    UInt16 MissionItemCounts[RUNTIME_CHARACTER_MAX_COLLECTION_ITEM_COUNT];
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

CLIENT_PROTOCOL(S2C, NFY_UPDATE_TRANSCENDENCE_POINTS, DEFAULT, 2926,
    UInt32 TranscendencePoints;
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

CLIENT_PROTOCOL(S2C, NFY_UNKNOWN_3016, DEFAULT, 3016,
    UInt8 Unknown1;
    Timestamp UnknownTimestamp1;
    Timestamp UnknownTimestamp2;
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
