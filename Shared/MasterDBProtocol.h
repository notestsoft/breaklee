#pragma once

#include "Constants.h"
#include "GameProtocol.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

typedef struct {
    UInt32 _;
} MASTERDB_DATA_ACCOUNT_ACHIEVEMENT_DATA;

typedef struct {
    UInt32 _;
} MASTERDB_DATA_ACCOUNT_WAREHOUSE_DATA;

typedef struct {
    UInt32 _;
} MASTERDB_DATA_ACCOUNT_MERIT_DATA;

typedef struct {
    Int64 AccountID;

    Char SessionIP[MAX_ADDRESSIP_LENGTH + 1];
    Timestamp SessionTimeout;

    Int32 CharacterSlotID;
    UInt64 CharacterSlotOrder;
    UInt32 CharacterSlotFlags;
    Char CharacterPassword[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 CharacterQuestion;
    Char CharacterAnswer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];

    MASTERDB_DATA_ACCOUNT_ACHIEVEMENT_DATA AchievementData;
    MASTERDB_DATA_ACCOUNT_MERIT_DATA MeritData;
    struct _RTCharacterWarehouseInfo WarehouseData;
    struct _RTCharacterCollectionInfo CollectionData;

    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} MASTERDB_DATA_ACCOUNT;

typedef struct {
    Int64 AccountID;
    Int32 CharacterID;
    Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
    UInt8 Index;

    struct _RTCharacterInfo CharacterData;
    struct _RTCharacterEquipmentInfo EquipmentData;
    struct _RTCharacterInventoryInfo InventoryData;
    struct _RTCharacterSkillSlotInfo SkillSlotData;
    struct _RTCharacterQuickSlotInfo QuickSlotData;
    struct _RTCharacterQuestSlotInfo QuestSlotData;
    struct _RTCharacterQuestFlagInfo QuestFlagData;
    struct _RTCharacterQuestFlagInfo DungeonQuestFlagData;
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
    struct _RTCharacterNewbieSupportInfo NewbieSupportData;

    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} MASTERDB_DATA_CHARACTER;

typedef struct {
    Int32 AccountID;

    Int32 CharacterID;
    Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
    UInt8 Index;

    struct _RTCharacterInfo CharacterData;
    struct _RTCharacterEquipmentInfo EquipmentData;

    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} MASTERDB_DATA_CHARACTER_INDEX;

typedef struct {
    Int32 AccountID;

    Char EquipmentPassword[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 EquipmentQuestion;
    Char EquipmentAnswer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
    Bool EquipmentLocked;

    Char WarehousePassword[MAX_SUBPASSWORD_LENGTH + 1];
    UInt32 WarehouseQuestion;
    Char WarehouseAnswer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
    Bool WarehouseLocked;

    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} MASTERDB_DATA_SUBPASSWORD;

typedef struct {
    Int32 ID;
    Int64 AccountID;
    UInt32 ServiceType;
    Timestamp StartedAt;
    Timestamp ExpiredAt;
    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} MASTERDB_DATA_SERVICE;

#pragma pack(pop)

EXTERN_C_END
