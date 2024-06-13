#ifdef USE_MYSQL_INCLUDE
#include <mysql/mysql.h>
#else
#include <mariadb/mysql.h>
#endif

#include "Diagnostic.h"
#include "Dictionary.h"
#include "Database.h"
#include "String.h"

#define DATABASE_STATEMENT_BUCKET_SIZE	64
#define DATABASE_RESULT_BUFFER_SIZE		8192

#pragma pack(push, 8)

struct _Database {
    MYSQL* Connection;
    struct _StatementBucket* FirstBucket;
    struct _StatementBucket* LastBucket;
    Int64 LastInsertID;
	DictionaryRef DataTables;
} ALIGNED(8);

struct _Buffer {
    Bool Buffered;
    Index Length;

    union {
        UInt8*  Buffer;
        UInt8*  Binary;
        Bool    Bool;
        CString String;
        UInt8   UInt8;
        UInt16  UInt16;
        UInt32  UInt32;
        UInt64  UInt64;
        Int8    Int8;
        Int16   Int16;
        Int32   Int32;
        Int64   Int64;
        Float32 Float32;
        Float64 Float64;
    } Value;
} ALIGNED(8);

typedef struct _Buffer* BufferRef;

struct _DataTable {
	DatabaseRef Database;
	StatementRef Insert;
	StatementRef Select;
	StatementRef Update;
	StatementRef Delete;
} ALIGNED(8);

struct _Statement {
	DatabaseRef Database;
	MYSQL_STMT* Statement;
	UInt64 ParameterCount;
	MYSQL_BIND* Parameters;
	struct _Buffer* ParameterValues;
	Int32 ResultCount;
	MYSQL_BIND* Results;
	struct _Buffer* ResultValues;
} ALIGNED(8);

#pragma pack(pop)

struct _StatementBucket {
	Int32 Count;
	struct _Statement Statements[DATABASE_STATEMENT_BUCKET_SIZE];
	struct _StatementBucket* NextBucket;
} ALIGNED(8);

DatabaseRef DatabaseConnect(
	CString Host,
	CString Username,
	CString Password,
	CString Database,
	UInt16 Port,
	Bool AutoReconnect
) {
	Info(
		"Connecting to database...\n\n\thost = %s\n\tusername = %s\n\tpassword = ****\n\tdatabase = %s\n\tport = %d\n",
		Host,
		Username,
		Database,
		Port
	);

	MYSQL* Connection = mysql_init(NULL);
	if (Connection == NULL) return NULL;

	my_bool AutoReconnectArgument = AutoReconnect;
	mysql_options(Connection, MYSQL_OPT_RECONNECT, &AutoReconnectArgument);

	Bool Success = mysql_real_connect(
		Connection,
		Host,
		Username,
		Password,
		Database,
		Port,
		NULL,
		0
	) != NULL;
	
	if (!Success) {
		mysql_close(Connection);
		return NULL;
	}

	DatabaseRef Result = (DatabaseRef)malloc(sizeof(struct _Database));
	assert(Result);
	Result->Connection = Connection;
	Result->FirstBucket = malloc(sizeof(struct _StatementBucket));
	assert(Result->FirstBucket);
	memset(Result->FirstBucket, 0, sizeof(struct _StatementBucket));
	Result->LastBucket = Result->FirstBucket;
	Result->LastInsertID = 0;
	Result->DataTables = CStringDictionaryCreate(AllocatorGetDefault(), 8);
	return Result;
}

Void DatabaseDisconnect(
	DatabaseRef Database
) {
	struct _StatementBucket* Bucket = Database->FirstBucket;
	while (Bucket) {
		for (Int32 StatementIndex = 0; StatementIndex < Bucket->Count; StatementIndex++) {
			StatementRef Statement = &Bucket->Statements[StatementIndex];

			if (Statement->Parameters) {
				free(Statement->Parameters);

				for (Int32 ParameterIndex = 0; ParameterIndex < Statement->ParameterCount; ParameterIndex++) {
					if (Statement->ParameterValues[ParameterIndex].Buffered) {
						free(Statement->ParameterValues[ParameterIndex].Value.Buffer);
					}
				}
			}

			if (Statement->Results) {
				for (Int32 ResultIndex = 0; ResultIndex < Statement->ResultCount; ResultIndex++) {
					if (Statement->ResultValues[ResultIndex].Buffered) {
						free(Statement->ResultValues[ResultIndex].Value.Buffer);
					}
				}

				free(Statement->Results);
				free(Statement->ResultValues);
			}

			mysql_stmt_close(Statement->Statement);
		}

		struct _StatementBucket* NextBucket = Bucket->NextBucket;
		free(Bucket);
		Bucket = NextBucket;
	}

	mysql_close(Database->Connection);
	DictionaryDestroy(Database->DataTables);
	free(Database);
}

Bool DatabaseBeginTransaction(
	DatabaseRef Database
) {	
	Trace("START TRANSACTION");
	if (mysql_query(Database->Connection, "START TRANSACTION;") != 0) {
		Error(
			"Database start transaction failed: %s", 
			mysql_error(Database->Connection)
		);
		return false;
	}

	return true;
}

Bool DatabaseCommitTransaction(
	DatabaseRef Database
) {
	Trace("COMMIT TRANSACTION");
	if (mysql_query(Database->Connection, "COMMIT;") != 0) {
		Error(
			"Database commit transaction failed: %s",
			mysql_error(Database->Connection)
		);
		return false;
	}

	return true;
}

Bool DatabaseRollbackTransaction(
	DatabaseRef Database
) {
	Trace("ROLLBACK TRANSACTION");
	if (mysql_query(Database->Connection, "ROLLBACK;") != 0) {
		Error(
			"Database rollback transaction failed: %s",
			mysql_error(Database->Connection)
		);
		return false;
	}

	return true;
}

StatementRef DatabaseCreateStatement(
	DatabaseRef Database, 
	CString Query
) {
	if (Database->LastBucket->Count + 1 >= DATABASE_STATEMENT_BUCKET_SIZE) {
		Database->LastBucket->NextBucket = malloc(sizeof(struct _StatementBucket));
		assert(Database->LastBucket->NextBucket);
		memset(Database->LastBucket->NextBucket, 0, sizeof(struct _StatementBucket));
		Database->LastBucket = Database->LastBucket->NextBucket;
	}

	StatementRef Statement = &Database->LastBucket->Statements[Database->LastBucket->Count];
	memset(Statement, 0, sizeof(struct _Statement));

	Statement->Database = Database;
	Statement->Statement = mysql_stmt_init(Database->Connection);
	if (Statement->Statement == NULL) goto error;
	
	Int32 Status = mysql_stmt_prepare(Statement->Statement, Query, strlen(Query));
	if (Status != 0) goto error;

	Statement->ParameterCount = mysql_stmt_param_count(Statement->Statement);
	if (Statement->ParameterCount > 0) {
		Statement->Parameters = malloc(sizeof(MYSQL_BIND) * Statement->ParameterCount);
		assert(Statement->Parameters);
		memset(Statement->Parameters, 0, sizeof(MYSQL_BIND) * Statement->ParameterCount);

		Statement->ParameterValues = malloc(sizeof(struct _Buffer) * Statement->ParameterCount);
		assert(Statement->ParameterValues);
		memset(Statement->ParameterValues, 0, sizeof(struct _Buffer) * Statement->ParameterCount);

		for (Int32 Index = 0; Index < Statement->ParameterCount; Index++) {
			Statement->Parameters[Index].is_null_value = true;
		}

		if (mysql_stmt_bind_param(Statement->Statement, Statement->Parameters)) goto error;
	}
	
	Statement->ResultCount = mysql_stmt_field_count(Statement->Statement);
	if (Statement->ResultCount > 0) {
		Statement->Results = malloc(sizeof(MYSQL_BIND) * Statement->ResultCount);
		assert(Statement->Results);
		memset(Statement->Results, 0, sizeof(MYSQL_BIND) * Statement->ResultCount);

		Statement->ResultValues = malloc(sizeof(struct _Buffer) * Statement->ResultCount);
		assert(Statement->ResultValues);
		memset(Statement->ResultValues, 0, sizeof(struct _Buffer) * Statement->ResultCount);

		MYSQL_RES* Metadata = mysql_stmt_result_metadata(Statement->Statement);
		if (!Metadata) goto error;

		MYSQL_FIELD* Fields = mysql_fetch_fields(Metadata);
		for (Int32 Index = 0; Index < Statement->ResultCount; Index++) {
			MYSQL_BIND* Result = &Statement->Results[Index];
			Result->buffer_type = Fields[Index].type;
			Result->is_null = NULL;

			BufferRef Buffer = &Statement->ResultValues[Index];
			Buffer->Buffered = (
				Result->buffer_type == FIELD_TYPE_TINY_BLOB ||
				Result->buffer_type == FIELD_TYPE_MEDIUM_BLOB ||
				Result->buffer_type == FIELD_TYPE_LONG_BLOB ||
				Result->buffer_type == FIELD_TYPE_BLOB ||
				Result->buffer_type == FIELD_TYPE_VAR_STRING ||
				Result->buffer_type == FIELD_TYPE_STRING
			);

			if (Buffer->Buffered) {
				Buffer->Value.Buffer = malloc(DATABASE_RESULT_BUFFER_SIZE);
				assert(Buffer->Value.Buffer);
				Result->buffer = Buffer->Value.Buffer;
				Result->buffer_length = MIN(Fields[Index].length, DATABASE_RESULT_BUFFER_SIZE);
				Result->length = (unsigned long*)&Buffer->Length;
			}
			else {
				Result->buffer = &Buffer->Value;
				Result->buffer_length = sizeof(Buffer->Value);
				Result->length = (unsigned long*)&Buffer->Length;
			}
		}

		mysql_free_result(Metadata);

		if (mysql_stmt_bind_result(Statement->Statement, Statement->Results)) goto error;
	}

	Database->LastBucket->Count += 1;
	return Statement;

error:
	Error(
		"Database create Statement failed: %s", 
		mysql_error(Database->Connection)
	);

	if (Statement->Statement != NULL) {
		mysql_stmt_close(Statement->Statement);
	}

	return NULL;
}

Int64 DatabaseGetLastInsertID(
	DatabaseRef Database
) {
	return Database->LastInsertID;
}

DataTableRef DatabaseCreateDataTable(
	DatabaseRef Database,
	CString Scope,
	CString Name
) {
	assert(!DictionaryLookup(Database->DataTables, Name));

	Info("Migrating data table: `%s%s`", Scope, Name);

	struct _DataTable Memory = { 0 };
	DictionaryInsert(Database->DataTables, Name, &Memory, sizeof(struct _DataTable));
	DataTableRef Table = DictionaryLookup(Database->DataTables, Name);
	assert(Table);

	if (!StatementExecute(DatabaseCreateStatement(Database, CStringFormat(
		"CREATE TABLE IF NOT EXISTS `%s%s` ("
		"   `%sID` INT NOT NULL PRIMARY KEY,"
		"   `Data` BLOB NOT NULL"
		") ENGINE = INNODB DEFAULT CHARSET = utf8;",
		Scope,
		Name,
		Scope
	)))) {
		Fatal("Database migration failed for data table: `%s%s`", Scope, Name);
	}

	Table->Database = Database;
	Table->Insert = DatabaseCreateStatement(Database, CStringFormat(
		"INSERT IGNORE INTO `%s%s` (`%sID`, `Data`) VALUES (?, ?);",
		Scope,
		Name,
		Scope
	));
	Table->Select = DatabaseCreateStatement(Database, CStringFormat(
		"SELECT `Data` FROM `%s%s` WHERE `%sID` = ?;",
		Scope,
		Name,
		Scope
	));
	Table->Update = DatabaseCreateStatement(Database, CStringFormat(
		"UPDATE `%s%s` SET `Data` = ? WHERE `%sID` = ?;",
		Scope,
		Name,
		Scope
	));
	Table->Delete = DatabaseCreateStatement(Database, CStringFormat(
		"DELETE FROM `%s%s` WHERE `%sID` = ?;",
		Scope,
		Name,
		Scope
	));
	assert(Table->Insert);
	assert(Table->Select);
	assert(Table->Update);
	assert(Table->Delete);
	return Table;
}

DataTableRef DatabaseGetDataTable(
	DatabaseRef Database,
	CString Scope,
	CString Name
) {
    return (DataTableRef)DictionaryLookup(Database->DataTables, Name);
}

Bool DataTableInsert(
	DataTableRef Table,
	Int32 ID,
	UInt8* Data,
	Int32 DataLength
) {
	StatementBindParameterInt32(Table->Insert, 0, ID);
	StatementBindParameterBinary(Table->Insert, 1, Data, DataLength);
	return StatementExecute(Table->Insert);
}

Bool DataTableSelect(
	DataTableRef Table,
	Int32 ID,
	UInt8* Data,
	Int32 DataLength
) {
	StatementBindParameterInt64(Table->Select, 0, ID);

	if (!StatementExecute(Table->Select)) return false;
	if (!StatementFetchResult(Table->Select)) return false;

	StatementReadResultBinary(Table->Select, 0, DataLength, Data, NULL);
	StatementFlushResults(Table->Select);

	return true;
}

Bool DataTableUpdate(
	DataTableRef Table,
	Int32 ID,
	UInt8* Data,
	Int32 DataLength
) {
	StatementBindParameterBinary(Table->Update, 0, Data, DataLength);
	StatementBindParameterInt32(Table->Update, 1, ID);
	return StatementExecute(Table->Update);
}

Bool DataTableDelete(
	DataTableRef Table,
	Int32 ID
) {
	StatementBindParameterInt64(Table->Delete, 0, ID);
	return StatementExecute(Table->Delete);
}

Void StatementBindParameter(
	StatementRef Statement,
	Int32 Index,
	Int32 Type,
	Bool Unsigned,
	UInt8* Value,
	Int32 Length
) {
	assert(0 <= Index && Index < Statement->ParameterCount);
	assert(Value);
	
	MYSQL_BIND* Binding = &Statement->Parameters[Index];
	memset(Binding, 0, sizeof(MYSQL_BIND));

	Binding->buffer_type = Type;
	Binding->buffer_length = Length;
	Binding->is_null_value = Value == NULL;
	Binding->is_unsigned = Unsigned;

	BufferRef Buffer = &Statement->ParameterValues[Index];
	if (Length <= sizeof(struct _Buffer)) {
		memset(&Buffer->Value, 0, sizeof(Buffer->Value));
		memcpy(&Buffer->Value, Value, Length);
		Binding->buffer = &Buffer->Value;
	}
	else {
		if (Buffer->Buffered) {
			Buffer->Value.Buffer = realloc(Buffer->Value.Buffer, Length);
		}
		else {
			Buffer->Buffered = true;
			Buffer->Value.Buffer = malloc(Length);
		}

		if (!Buffer->Value.Buffer) Fatal("Memory allocation failed!");

		memcpy(Buffer->Value.Buffer, Value, Length);	
		Binding->buffer = Buffer->Value.Buffer;
	}
}

Void StatementBindParameterBinary(
	StatementRef Statement, 
	Int32 Index, 
	Void* Value,
	Int32 Length
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_BLOB, false, Value, Length);
}

Void StatementBindParameterBool(
	StatementRef Statement,
	Int32 Index, 
	Bool Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_TINY, false, (UInt8*)&Value, sizeof(Bool));
}

Void StatementBindParameterString(
	StatementRef Statement, 
	Int32 Index, 
	CString Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_STRING, false, (UInt8 *)Value, (Int32)strlen(Value));
}

Void StatementBindParameterUInt8(
	StatementRef Statement, 
	Int32 Index, 
	UInt8 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_TINY, true, &Value, sizeof(UInt8));
}

Void StatementBindParameterUInt16(
	StatementRef Statement, 
	Int32 Index, 
	UInt16 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_SHORT, true, (UInt8*)&Value, sizeof(UInt16));
}

Void StatementBindParameterUInt32(
	StatementRef Statement, 
	Int32 Index, 
	UInt32 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_LONG, true, (UInt8*)&Value, sizeof(UInt32));
}

Void StatementBindParameterUInt64(
	StatementRef Statement,
	Int32 Index, 
	UInt64 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_LONGLONG, true, (UInt8*)&Value, sizeof(UInt64));
}

Void StatementBindParameterInt8(
	StatementRef Statement, 
	Int32 Index, 
	Int8 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_TINY, false, (UInt8*)&Value, sizeof(Int8));
}

Void StatementBindParameterInt16(
	StatementRef Statement, 
	Int32 Index, 
	Int16 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_SHORT, false, (UInt8*)&Value, sizeof(Int16));
}

Void StatementBindParameterInt32(
	StatementRef Statement, 
	Int32 Index, 
	Int32 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_LONG, false, (UInt8*)&Value, sizeof(Int32));
}

Void StatementBindParameterInt64(
	StatementRef Statement,
	Int32 Index,
	Int64 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_LONGLONG, false, (UInt8*)&Value, sizeof(Int64));
}

Void StatementBindParameterFloat32(
	StatementRef Statement,
	Int32 Index, 
	Float32 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_FLOAT, false, (UInt8*)&Value, sizeof(Float32));
}

Void StatementBindParameterFloat64(
	StatementRef Statement,
	Int32 Index, 
	Float64 Value
) {
	StatementBindParameter(Statement, Index, MYSQL_TYPE_DOUBLE, false, (UInt8*)&Value, sizeof(Float64));
}

Bool StatementExecute(
	StatementRef Statement
) {
	if (Statement->Parameters) {
		if (mysql_stmt_bind_param(Statement->Statement, Statement->Parameters)) {
			Error(
				"Database Statement parameter binding failed: %s", 
				mysql_error(Statement->Database->Connection)
			);
			return false;
		}
	}

	if (Statement->Results) {
		if (mysql_stmt_bind_result(Statement->Statement, Statement->Results)) {
			Error(
				"Database Statement result binding failed: %s",
				mysql_error(Statement->Database->Connection)
			);
			return false;
		}
	}

	if (mysql_stmt_execute(Statement->Statement) != 0) {
		Error(
			"Database Statement execution failed: %s",
			mysql_error(Statement->Database->Connection)
		);
		return false;
	}

	Statement->Database->LastInsertID = StatementLastInsertID(Statement);

	return true;
}

Bool StatementFetchResult(
	StatementRef Statement
) {
	Int32 Result = mysql_stmt_fetch(Statement->Statement);
	if (Result == 1) {
		Error(
			"Database error fetching data: %s",
			mysql_error(Statement->Database->Connection)
		);
		return false;
	}

	return Result != MYSQL_NO_DATA;
}

Void StatementFlushResults(
	StatementRef Statement
) {
	while (StatementFetchResult(Statement)) { }
}

Int64 StatementLastInsertID(
	StatementRef Statement
) {
	return mysql_stmt_insert_id(Statement->Statement);
}

Int64 StatementGetResultLength(
	StatementRef Statement,
	Int64 Index
) {
	Int64 Length = Statement->ResultValues[Index].Length;
	return Length;

	switch (Statement->Results[Index].buffer_type) {
	case MYSQL_TYPE_VAR_STRING:
		return (Length <= 256) ? Length - 1 : Length - 2;

	case MYSQL_TYPE_TINY_BLOB:
		return Length - 1;

	case MYSQL_TYPE_BLOB:
		return Length - 2;

	case MYSQL_TYPE_MEDIUM_BLOB:
		return Length - 3;

	case MYSQL_TYPE_LONG_BLOB:
		return Length - 4;

	default:
		return Length;
	}
}

Void StatementReadResultBinary(
	StatementRef Statement,
	Int64 Index,
	Int64 MaxLength,
	Void* Result,
	Int64* Length
) {
	Int64 ResultLength = StatementGetResultLength(Statement, Index);
	assert(0 <= Index && Index < Statement->ResultCount);
	assert(ResultLength <= MaxLength);
	memcpy(Result, Statement->ResultValues[Index].Value.Buffer, ResultLength);

	if (Length != NULL) {
		*Length = ResultLength;
    }
}

Void StatementReadResultBool(
	StatementRef Statement,
	Int64 Index,
	Bool* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.Bool;
}

Void StatementReadResultString(
	StatementRef Statement,
	Int64 Index,
	Int64 MaxLength,
	CString Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);

    MYSQL_BIND *Value = &Statement->Results[Index];
	UInt64 Length = *Value->length;
    if (Length < 1 || Value->is_null_value) {
        CStringCopySafe(Result, MaxLength, "");
	} else {
		assert(Length <= MaxLength);
		memcpy(Result, Statement->ResultValues[Index].Value.Buffer, sizeof(Char) * Length);
	}
}

Void StatementReadResultUInt8(
	StatementRef Statement,
	Int64 Index,
	UInt8* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.UInt8;
}

Void StatementReadResultUInt16(
	StatementRef Statement,
	Int64 Index,
	UInt16* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.UInt16;
}

Void StatementReadResultUInt32(
	StatementRef Statement,
	Int64 Index,
	UInt32* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.UInt32;
}

Void StatementReadResultUInt64(
	StatementRef Statement,
	Int64 Index,
	UInt64* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.UInt64;
}

Void StatementReadResultInt8(
	StatementRef Statement,
	Int64 Index,
	Int8* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.Int8;
}

Void StatementReadResultInt16(
	StatementRef Statement,
	Int64 Index,
	Int16* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.Int16;
}

Void StatementReadResultInt32(
	StatementRef Statement,
	Int64 Index,
	Int32* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
    memcpy(Result, &Statement->ResultValues[Index].Value.Int32, sizeof(Int32));
}

Void StatementReadResultInt64(
	StatementRef Statement,
	Int64 Index,
	Int64* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.Int64;
}

Void StatementReadResultFloat32(
	StatementRef Statement,
	Int64 Index,
	Float32* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.Float32;
}

Void StatementReadResultFloat64(
	StatementRef Statement,
	Int64 Index,
	Float64* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = Statement->ResultValues[Index].Value.Float64;
}

Void StatementReadResultTimestamp(
	StatementRef Statement,
	Int64 Index,
	Timestamp* Result
) {
	assert(0 <= Index && Index < Statement->ResultCount);
	*Result = (Timestamp)Statement->ResultValues[Index].Value.UInt64;
}
