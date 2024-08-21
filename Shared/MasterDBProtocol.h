#pragma once

#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

typedef struct {
    Int32 AccountID;
    Char CharacterPassword[MAX_SUBPASSWORD_LENGTH + 1];
    Int32 CharacterQuestion;
    Char CharacterAnswer[MAX_SUBPASSWORD_ANSWER_LENGTH + 1];
    struct _RTCharacterAccountInfo AccountInfo;
} MASTERDB_DATA_ACCOUNT;

typedef struct {
    Int64 AccountID;
    Int32 CharacterID;
    Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
    UInt8 Index;
    Timestamp CreatedAt;
    Timestamp UpdatedAt;
} MASTERDB_DATA_CHARACTER;

typedef struct {
    Int32 AccountID;

    Int32 CharacterID;
    Char Name[MAX_CHARACTER_NAME_LENGTH + 1];
    UInt8 Index;

    struct _RTCharacterInfo CharacterData;
    struct _RTCharacterEquipmentData EquipmentData;
    struct _RTCharacterOverlordMasteryInfo OverlordMasteryInfo;

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
