#include "MasterDB.h"

static StatementRef kStatementTable[MASTERDB_STATEMENT_COUNT];

// TODO: Migrate missing data for existing accounts and characters when new tables are added
Void MasterDBMigrate(
    DatabaseRef Database
) {
#define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__, __SCOPE__) \
    DatabaseCreateDataTable(Database, #__SCOPE__, #__NAME__);
#include "RuntimeLib/CharacterDataDefinition.h"
}

Void MasterDBPrepareStatements(
    DatabaseRef Database
) {
#define STATEMENT(__NAME__, __QUERY__)						                \
	Info("Create Statement: %s", #__NAME__);	\
	kStatementTable[__NAME__] = DatabaseCreateStatement(	                \
    	Database,											                \
    	__QUERY__											                \
	);
#include "MasterDBStatements.h"
}

StatementRef MasterDBGetStatement(
    DatabaseRef Database,
    Int32 StatementID
) {
    assert(0 <= StatementID && StatementID < MASTERDB_STATEMENT_COUNT);

    return kStatementTable[StatementID];
}

Bool MasterDBGetOrCreateAccount(
    DatabaseRef Database,
    Int64 AccountID,
    MASTERDB_DATA_ACCOUNT* Account
) {
    if (MasterDBSelectAccountByID(Database, AccountID, Account)) return true;

    if (!DatabaseBeginTransaction(Database)) goto error;
    {
        if (!MasterDBInsertAccount(Database, AccountID)) goto error;
        if (!MasterDBInsertSubpassword(Database, Account->AccountID)) goto error;

        Account->AccountID = DatabaseGetLastInsertID(Database);
    }
    if (!DatabaseCommitTransaction(Database)) goto error;

    if (!MasterDBSelectAccountByID(Database, AccountID, Account)) goto error;

    return true;

error:
    DatabaseRollbackTransaction(Database);
    return false;
}

Bool MasterDBInsertAccount(
    DatabaseRef Database,
    Int64 AccountID
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_INSERT_ACCOUNT);
    StatementBindParameterInt64(Statement, 0, AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBSelectAccountByID(
    DatabaseRef Database,
    Int64 AccountID,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_ACCOUNT_BY_ID);
    StatementBindParameterInt64(Statement, 0, AccountID);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt64(Statement, 0, &Data->AccountID);
    StatementReadResultString(Statement, 1, MAX_ADDRESSIP_LENGTH, Data->SessionIP);
    StatementReadResultTimestamp(Statement, 2, &Data->SessionTimeout);
    StatementReadResultInt32(Statement, 3, &Data->CharacterSlotID);
    StatementReadResultUInt64(Statement, 4, &Data->CharacterSlotOrder);
    StatementReadResultUInt32(Statement, 5, &Data->CharacterSlotFlags);
    StatementReadResultString(Statement, 6, MAX_SUBPASSWORD_LENGTH, Data->CharacterPassword);
    StatementReadResultUInt32(Statement, 7, &Data->CharacterQuestion);
    StatementReadResultString(Statement, 8, MAX_SUBPASSWORD_ANSWER_LENGTH, Data->CharacterAnswer);
    StatementReadResultTimestamp(Statement, 9, &Data->CreatedAt);
    StatementReadResultTimestamp(Statement, 10, &Data->UpdatedAt);

    StatementFlushResults(Statement);

    return true;
}

Bool MasterDBUpdateAccountSession(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_SESSION);
    StatementBindParameterString(Statement, 0, Data->SessionIP);
    StatementBindParameterUInt64(Statement, 1, Data->SessionTimeout);
    StatementBindParameterInt64(Statement, 2, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateAccountCharacterSlot(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_CHARACTER_SLOT);
    StatementBindParameterInt32(Statement, 0, Data->CharacterSlotID);
    StatementBindParameterUInt64(Statement, 1, Data->CharacterSlotOrder);
    StatementBindParameterUInt32(Statement, 2, Data->CharacterSlotFlags);
    StatementBindParameterInt64(Statement, 3, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateAccountCharacterPassword(
    DatabaseRef Database,
    MASTERDB_DATA_ACCOUNT* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_ACCOUNT_CHARACTER_PASSWORD);
    StatementBindParameterString(Statement, 0, Data->CharacterPassword);
    StatementBindParameterUInt32(Statement, 1, Data->CharacterQuestion);
    StatementBindParameterString(Statement, 2, Data->CharacterAnswer);
    StatementBindParameterInt64(Statement, 3, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBInsertCharacter(
    DatabaseRef Database,
    Int64 AccountID,
    CString CharacterName,
    UInt8 CharacterSlotIndex
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_INSERT_CHARACTER);
    StatementBindParameterInt64(Statement, 0, AccountID);
    StatementBindParameterString(Statement, 1, CharacterName);
    StatementBindParameterUInt8(Statement, 2, CharacterSlotIndex);

    return StatementExecute(Statement);
}

Bool MasterDBSelectCharacterByID(
    DatabaseRef Database,
    MASTERDB_DATA_CHARACTER* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CHARACTER_BY_ID);
    StatementBindParameterInt32(Statement, 0, Data->CharacterID);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt64(Statement, 0, &Data->AccountID);
    StatementReadResultInt32(Statement, 1, &Data->CharacterID);
    StatementReadResultString(Statement, 2, MAX_CHARACTER_NAME_LENGTH, Data->Name);
    StatementReadResultUInt8(Statement, 3, &Data->Index);
    StatementReadResultTimestamp(Statement, 4, &Data->CreatedAt);
    StatementReadResultTimestamp(Statement, 5, &Data->UpdatedAt);

    StatementFlushResults(Statement);

    return true;
}

StatementRef MasterDBSelectCharacterByAccount(
    DatabaseRef Database,
    Int64 AccountID
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CHARACTER_BY_ACCOUNT);
    StatementBindParameterInt64(Statement, 0, AccountID);

    if (StatementExecute(Statement)) return Statement;

    return NULL;
}

Bool MasterDBSelectCharacterFetchNext(
    DatabaseRef Database,
    StatementRef Statement,
    MASTERDB_DATA_CHARACTER* Result
) {
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt64(Statement, 0, &Result->AccountID);
    StatementReadResultInt32(Statement, 1, &Result->CharacterID);
    StatementReadResultString(Statement, 2, MAX_CHARACTER_NAME_LENGTH, Result->Name);
    StatementReadResultUInt8(Statement, 3, &Result->Index);
    StatementReadResultTimestamp(Statement, 4, &Result->CreatedAt);
    StatementReadResultTimestamp(Statement, 5, &Result->UpdatedAt);

    return true;
}

StatementRef MasterDBSelectCharacterIndexByAccount(
    DatabaseRef Database,
    Int64 AccountID
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CHARACTER_INDEX_BY_ACCOUNT);
    StatementBindParameterInt64(Statement, 0, AccountID);

    if (StatementExecute(Statement)) return Statement;

    return NULL;
}

Bool MasterDBSelectCharacterIndexFetchNext(
    DatabaseRef Database,
    StatementRef Statement,
    MASTERDB_DATA_CHARACTER_INDEX* Result
) {
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Result->AccountID);
    StatementReadResultInt32(Statement, 1, &Result->CharacterID);
    StatementReadResultString(Statement, 2, MAX_CHARACTER_NAME_LENGTH, Result->Name);
    StatementReadResultUInt8(Statement, 3, &Result->Index);
    StatementReadResultBinary(Statement, 4, sizeof(struct _RTCharacterInfo), &Result->CharacterData, NULL);
    StatementReadResultBinary(Statement, 5, sizeof(struct _RTCharacterEquipmentInfo), &Result->EquipmentData, NULL);
    StatementReadResultTimestamp(Statement, 6, &Result->CreatedAt);
    StatementReadResultTimestamp(Statement, 7, &Result->UpdatedAt);

    return true;
}

Bool MasterDBSelectCharacterNameCount(
    DatabaseRef Database,
    CString Name,
    Int32* Result
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CHARACTER_NAME_COUNT);
    StatementBindParameterString(Statement, 0, Name);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, Result);
    StatementFlushResults(Statement);

    return true;
}

Bool MasterDBDeleteCharacter(
    DatabaseRef Database,
    Int64 AccountID,
    Int32 CharacterID
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_DELETE_CHARACTER);
    StatementBindParameterInt64(Statement, 0, AccountID);
    StatementBindParameterInt32(Statement, 1, CharacterID);

    return StatementExecute(Statement);
}

Bool MasterDBInsertSubpassword(
    DatabaseRef Database,
    Int64 AccountID
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERBD_INSERT_SUBPASSWORD);
    StatementBindParameterInt64(Statement, 0, AccountID);

    if (!StatementExecute(Statement)) return false;

    return true;
}

Bool MasterDBSelectSubpassword(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERBD_SELECT_SUBPASSWORD);
    StatementBindParameterInt32(Statement, 0, Data->AccountID);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Data->AccountID);
    StatementReadResultString(Statement, 1, MAX_SUBPASSWORD_LENGTH, Data->EquipmentPassword);
    StatementReadResultUInt32(Statement, 2, &Data->EquipmentQuestion);
    StatementReadResultString(Statement, 3, MAX_SUBPASSWORD_ANSWER_LENGTH, Data->EquipmentAnswer);
    StatementReadResultBool(Statement, 4, &Data->EquipmentLocked);
    StatementReadResultString(Statement, 5, MAX_SUBPASSWORD_LENGTH, Data->WarehousePassword);
    StatementReadResultUInt32(Statement, 6, &Data->WarehouseQuestion);
    StatementReadResultString(Statement, 7, MAX_SUBPASSWORD_ANSWER_LENGTH, Data->WarehouseAnswer);
    StatementReadResultBool(Statement, 8, &Data->WarehouseLocked);
    StatementReadResultTimestamp(Statement, 9, &Data->CreatedAt);
    StatementReadResultTimestamp(Statement, 10, &Data->UpdatedAt);

    StatementFlushResults(Statement);

    return true;
}

Bool MasterDBUpdateSubpasswordEquipment(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_SUBPASSWORD_EQUIPMENT);
    StatementBindParameterString(Statement, 0, Data->EquipmentPassword);
    StatementBindParameterUInt32(Statement, 1, Data->EquipmentQuestion);
    StatementBindParameterString(Statement, 2, Data->EquipmentAnswer);
    StatementBindParameterBool(Statement, 3, Data->EquipmentLocked);
    StatementBindParameterInt32(Statement, 4, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBUpdateSubpasswordWarehouse(
    DatabaseRef Database,
    MASTERDB_DATA_SUBPASSWORD* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_UPDATE_SUBPASSWORD_WAREHOUSE);
    StatementBindParameterString(Statement, 0, Data->WarehousePassword);
    StatementBindParameterUInt32(Statement, 1, Data->WarehouseQuestion);
    StatementBindParameterString(Statement, 2, Data->WarehouseAnswer);
    StatementBindParameterBool(Statement, 3, Data->WarehouseLocked);
    StatementBindParameterInt32(Statement, 4, Data->AccountID);

    return StatementExecute(Statement);
}

Bool MasterDBInsertService(
    DatabaseRef Database,
    Int32 AccountID,
    UInt32 ServiceType,
    Timestamp Duration
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_INSERT_SERVICE);
    StatementBindParameterInt32(Statement, 0, AccountID);
    StatementBindParameterUInt32(Statement, 1, ServiceType);
    StatementBindParameterUInt64(Statement, 2, Duration);

    if (!StatementExecute(Statement)) return false;

    return true;
}

Bool MasterDBSelectCurrentActiveServiceByAccount(
    DatabaseRef Database,
    MASTERDB_DATA_SERVICE* Data
) {
    StatementRef Statement = MasterDBGetStatement(Database, MASTERDB_SELECT_CURRENT_ACTIVE_SERVICE_BY_ACCOUNT);
    StatementBindParameterInt64(Statement, 0, Data->AccountID);

    if (!StatementExecute(Statement)) return false;
    if (!StatementFetchResult(Statement)) return false;

    StatementReadResultInt32(Statement, 0, &Data->ID);
    StatementReadResultInt64(Statement, 1, &Data->AccountID);
    StatementReadResultUInt32(Statement, 2, &Data->ServiceType);
    StatementReadResultTimestamp(Statement, 3, &Data->StartedAt);
    StatementReadResultTimestamp(Statement, 4, &Data->ExpiredAt);
    StatementReadResultTimestamp(Statement, 5, &Data->CreatedAt);
    StatementReadResultTimestamp(Statement, 6, &Data->UpdatedAt);

    StatementFlushResults(Statement);

    return true;
}
