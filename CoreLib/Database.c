#include "Diagnostic.h"
#include "Dictionary.h"
#include "Database.h"
#include "String.h"

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#define DATABASE_RESULT_BUFFER_SIZE		8192

struct _Database {
	AllocatorRef Allocator;
	SQLHENV Environment;
	SQLHDBC Connection;
	SQLHSTMT Statement;
	Char Driver[MAX_PATH];
	Char Host[MAX_PATH];
	Char Database[MAX_PATH];
	Char Username[MAX_PATH];
	Char Password[MAX_PATH];
	UInt16 Port;
	Int64 ResultBufferSize;
	Bool AutoReconnect;
	Int64 LastInsertID;
	DictionaryRef DataTables;
};

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
};

typedef struct _Buffer* BufferRef;

struct _DataTable {
	DatabaseRef Database;
	StatementRef Insert;
	StatementRef Select;
	StatementRef Update;
	StatementRef Delete;
};

struct _DatabaseTypeMapping {
	Int32 Type;
	SQLSMALLINT SQLType;
	SQLSMALLINT NativeType;
	SQLULEN NativeSize;
	CString NativeName;
};

#define DB_TYPE_CHAR    (0)
#define DB_TYPE_STRING  (1)
#define DB_TYPE_INT8    (2)
#define DB_TYPE_INT16   (3)
#define DB_TYPE_INT32   (4)
#define DB_TYPE_INT64   (5)
#define DB_TYPE_UINT8   (6)
#define DB_TYPE_UINT16  (7)
#define DB_TYPE_UINT32  (8)
#define DB_TYPE_UINT64  (9)
#define DB_TYPE_DATA    (10)

const struct _DatabaseTypeMapping kDatabaseTypeMapping[] = {
	{	DB_TYPE_CHAR,		SQL_CHAR,		SQL_C_CHAR,		sizeof(Char),	"Char"		},
	{	DB_TYPE_STRING,		SQL_VARCHAR,	SQL_C_CHAR,		0,				"CString"	},
	{	DB_TYPE_INT8,		SQL_TINYINT,	SQL_C_STINYINT, sizeof(Int8),	"Int8"		},
	{	DB_TYPE_INT16,		SQL_SMALLINT,	SQL_C_SSHORT,	sizeof(Int16),	"Int16"		},
	{	DB_TYPE_INT32,		SQL_INTEGER,	SQL_C_SLONG,	sizeof(Int32),	"Int32"		},
	{	DB_TYPE_INT64,		SQL_BIGINT,		SQL_C_SBIGINT,	sizeof(Int64),	"Int64"		},
	{	DB_TYPE_UINT8,		SQL_TINYINT,	SQL_C_UTINYINT, sizeof(UInt8),	"UInt8"		},
	{	DB_TYPE_UINT16,		SQL_SMALLINT,	SQL_C_USHORT,	sizeof(UInt16),	"UInt16"	},
	{	DB_TYPE_UINT32,		SQL_INTEGER,	SQL_C_ULONG,	sizeof(UInt32),	"UInt32"	},
	{	DB_TYPE_UINT64,		SQL_BIGINT,		SQL_C_UBIGINT,	sizeof(UInt64),	"UInt64"	},
	{	DB_TYPE_DATA,		SQL_BINARY,		SQL_C_BINARY,	0,				"UInt8[]"	},
};

static inline struct _DatabaseTypeMapping DatabaseTypeGetMapping(
	Int32 Type
) {
	for (Int32 Index = 0; Index < sizeof(kDatabaseTypeMapping) / sizeof(kDatabaseTypeMapping[0]); ++Index) {
		if (kDatabaseTypeMapping[Index].Type == Type) {
			return kDatabaseTypeMapping[Index];
		}
	}

	struct _DatabaseTypeMapping kDatabaseTypeMappingNull = { 0 };
	return kDatabaseTypeMappingNull;
}

static inline SQLSMALLINT DatabaseTypeGetNativeDirection(
	Int32 Direction
) {
	switch (Direction) {
	case DB_PARAM_INPUT:	return SQL_PARAM_INPUT;
	case DB_PARAM_OUTPUT:	return SQL_PARAM_OUTPUT;
	default:				return SQL_PARAM_ERROR;
	}
}

Void HandleDatabaseError(
	SQLSMALLINT HandleType,
	SQLHANDLE Handle
) {
	SQLCHAR SqlState[1024] = { 0 };
	SQLCHAR Message[1024] = { 0 };
	SQLINTEGER NativeError = 0;
	SQLSMALLINT MessageLength = 0;
	SQLGetDiagRec(HandleType, Handle, 1, SqlState, &NativeError, Message, sizeof(Message), &MessageLength);
	Error("Database error: %s", Message);
}

DatabaseRef DatabaseConnect(
	AllocatorRef Allocator,
	CString Driver,
	CString Host,
	CString Database,
	CString Username,
	CString Password,
	UInt16 Port,
	Int64 ResultBufferSize,
	Bool AutoReconnect
) {
	SQLHENV Environment;
	SQLRETURN ReturnCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &Environment);
	if (ReturnCode != SQL_SUCCESS && ReturnCode != SQL_SUCCESS_WITH_INFO) {
		HandleDatabaseError(SQL_HANDLE_ENV, Environment);
		return NULL;
	}

	ReturnCode = SQLSetEnvAttr(Environment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	if (ReturnCode != SQL_SUCCESS && ReturnCode != SQL_SUCCESS_WITH_INFO) {
		HandleDatabaseError(SQL_HANDLE_ENV, Environment);
		SQLFreeHandle(SQL_HANDLE_ENV, Environment);
		return NULL;
	}

	SQLHDBC Handle;
	ReturnCode = SQLAllocHandle(SQL_HANDLE_DBC, Environment, &Handle);
	if (ReturnCode != SQL_SUCCESS && ReturnCode != SQL_SUCCESS_WITH_INFO) {
		HandleDatabaseError(SQL_HANDLE_DBC, Handle);
		SQLFreeHandle(SQL_HANDLE_ENV, Environment);
		return NULL;
	}

	CString ConnectionString = CStringFormat(
		"DRIVER={%s};SERVER=%s;PORT=%d;DATABASE=%s;UID=%s;PWD=%s;",
		Driver,
		Host,
		Port,
		Database,
		Username,
		Password
	);

	ReturnCode = SQLDriverConnect(Handle, NULL, ConnectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
	if (ReturnCode != SQL_SUCCESS && ReturnCode != SQL_SUCCESS_WITH_INFO) {
		HandleDatabaseError(SQL_HANDLE_DBC, Handle);
		SQLFreeHandle(SQL_HANDLE_DBC, Handle);
		SQLFreeHandle(SQL_HANDLE_ENV, Environment);
		return NULL;
	}

	DatabaseRef Result = (DatabaseRef)AllocatorAllocate(Allocator, sizeof(struct _Database));
	if (!Result) {
		SQLFreeHandle(SQL_HANDLE_DBC, Handle);
		SQLFreeHandle(SQL_HANDLE_ENV, Environment);
		return NULL;
	}

	Result->Allocator = Allocator;
	CStringCopySafe(Result->Driver, MAX_PATH, Driver);
	CStringCopySafe(Result->Host, MAX_PATH, Host);
	CStringCopySafe(Result->Database, MAX_PATH, Database);
	CStringCopySafe(Result->Username, MAX_PATH, Username);
	CStringCopySafe(Result->Password, MAX_PATH, Password);
	Result->Port = Port;
	Result->Connection = Handle;
	Result->Environment = Environment;
	Result->ResultBufferSize = (ResultBufferSize > 0) ? ResultBufferSize : DATABASE_RESULT_BUFFER_SIZE;
	Result->AutoReconnect = AutoReconnect;
	Result->LastInsertID = 0;
	Result->DataTables = CStringDictionaryCreate(AllocatorGetDefault(), 8);
	return Result;
}

Void DatabaseDisconnect(
	DatabaseRef Database
) {
	if (!Database) return;

	SQLFreeHandle(SQL_HANDLE_DBC, Database->Connection);
	SQLFreeHandle(SQL_HANDLE_ENV, Database->Environment);
	DictionaryDestroy(Database->DataTables);
	AllocatorDeallocate(Database->Allocator, Database);
}

CString DatabaseGetName(
	DatabaseRef Database
) {
	return Database->Database;
}

Bool DatabaseBeginTransaction(
	DatabaseRef Database
) {
	SQLRETURN ReturnCode = SQLSetConnectAttr(
		Database->Connection,
		SQL_ATTR_AUTOCOMMIT,
		(SQLPOINTER)SQL_AUTOCOMMIT_OFF,
		SQL_IS_INTEGER
	);

	if (ReturnCode != SQL_SUCCESS && ReturnCode != SQL_SUCCESS_WITH_INFO) {
		HandleDatabaseError(SQL_HANDLE_DBC, Database->Connection);
		return false;
	}

	return true;
}

Bool DatabaseCommitTransaction(
	DatabaseRef Database
) {
	SQLRETURN ReturnCode = SQLEndTran(
		SQL_HANDLE_DBC,
		Database->Connection,
		SQL_COMMIT
	);

	if (ReturnCode != SQL_SUCCESS && ReturnCode != SQL_SUCCESS_WITH_INFO) {
		HandleDatabaseError(SQL_HANDLE_DBC, Database->Connection);
		return false;
	}

	return true;
}

Bool DatabaseRollbackTransaction(
	DatabaseRef Database
) {
	SQLRETURN ReturnCode = SQLEndTran(
		SQL_HANDLE_DBC,
		Database->Connection,
		SQL_ROLLBACK
	);

	if (ReturnCode != SQL_SUCCESS && ReturnCode != SQL_SUCCESS_WITH_INFO) {
		HandleDatabaseError(SQL_HANDLE_DBC, Database->Connection);
		return false;
	}

	return true;
}

Bool DatabaseExecuteQuery(
	DatabaseRef Database,
	CString Query
) {
	SQLHSTMT Statement = 0;
	SQLRETURN ReturnCode = SQLAllocHandle(SQL_HANDLE_STMT, Database->Connection, &Statement);
	if (!SQL_SUCCEEDED(ReturnCode)) {
		Error("Failed to allocate SQL statement handle.");
		return false;
	}

	ReturnCode = SQLExecDirect(Statement, Query, SQL_NTS);
	if (!SQL_SUCCEEDED(ReturnCode)) {
		HandleDatabaseError(SQL_HANDLE_STMT, Statement);
		SQLFreeHandle(SQL_HANDLE_STMT, Statement);
		return false;
	}

	SQLFreeHandle(SQL_HANDLE_STMT, Statement);
	return true;
}

DatabaseHandleRef DatabaseCallProcedureFetchInternal(
	DatabaseRef Database,
	const Char* Procedure,
	va_list Arguments
) {
	SQLHSTMT Statement;
	SQLRETURN ReturnCode = SQLAllocHandle(SQL_HANDLE_STMT, Database->Connection, &Statement);
	if (!SQL_SUCCEEDED(ReturnCode)) {
		Error("Failed to allocate statement handle.\n");
		return NULL;
	}

	SQLCHAR Query[DATABASE_MAX_QUERY_LENGTH] = { 0 };
	Int32 ParameterCount = 0;
	SQLSMALLINT ParameterDirections[DATABASE_MAX_PROCEDURE_PARAMETER_COUNT] = { 0 };
	SQLSMALLINT ParameterTypes[DATABASE_MAX_PROCEDURE_PARAMETER_COUNT] = { 0 };
	SQLSMALLINT ParameterNativeTypes[DATABASE_MAX_PROCEDURE_PARAMETER_COUNT] = { 0 };
	SQLSMALLINT ParameterLengths[DATABASE_MAX_PROCEDURE_PARAMETER_COUNT] = { 0 };
	SQLCHAR* ParameterValues[DATABASE_MAX_PROCEDURE_PARAMETER_COUNT] = { 0 };

	snprintf((Char*)Query, sizeof(Query), "{ CALL %s(", Procedure);
	while (true) {
		Int32 ParameterDirection = va_arg(Arguments, Int32);
		if (ParameterDirection == DB_PARAM_END) break;

		Int32 ParameterType = va_arg(Arguments, Int32);
		if (ParameterType == DB_PARAM_END) break;

		assert(ParameterCount < DATABASE_MAX_PROCEDURE_PARAMETER_COUNT);

		struct _DatabaseTypeMapping ParameterMapping = DatabaseTypeGetMapping(ParameterType);

		ParameterDirections[ParameterCount] = DatabaseTypeGetNativeDirection(ParameterDirection);
		ParameterTypes[ParameterCount] = ParameterMapping.SQLType;
		ParameterNativeTypes[ParameterCount] = ParameterMapping.NativeType;
		ParameterLengths[ParameterCount] = ParameterMapping.NativeSize;
		ParameterValues[ParameterCount] = va_arg(Arguments, SQLPOINTER);

		if (ParameterLengths[ParameterCount] < 1) {
			ParameterLengths[ParameterCount] = va_arg(Arguments, SQLULEN);
		}

		if (!ParameterNativeTypes[ParameterCount]) {
			Error("Not supported parameter type %d.\n", ParameterType);
			SQLFreeHandle(SQL_HANDLE_STMT, Statement);
			return NULL;
		}

		if (ParameterCount > 0) strcat((char*)Query, ", ");
		strcat((char*)Query, "?");
		ParameterCount += 1;
	}
	strcat((char*)Query, ") }");

	ReturnCode = SQLPrepare(Statement, Query, SQL_NTS);
	if (!SQL_SUCCEEDED(ReturnCode)) {
		HandleDatabaseError(SQL_HANDLE_STMT, Statement);
		SQLFreeHandle(SQL_HANDLE_STMT, Statement);
		return NULL;
	}

	for (Int32 ParameterIndex = 0; ParameterIndex < ParameterCount; ++ParameterIndex) {
		ReturnCode = SQLBindParameter(
			Statement,
			ParameterIndex + 1,
			ParameterDirections[ParameterIndex],
			ParameterNativeTypes[ParameterIndex],
			ParameterTypes[ParameterIndex],
			ParameterLengths[ParameterIndex],
			0,
			ParameterValues[ParameterIndex],
			ParameterLengths[ParameterIndex],
			NULL
		);

		if (!SQL_SUCCEEDED(ReturnCode)) {
			HandleDatabaseError(SQL_HANDLE_STMT, Statement);
			SQLFreeHandle(SQL_HANDLE_STMT, Statement);
			return NULL;
		}
	}

	ReturnCode = SQLExecute(Statement);
	if (!SQL_SUCCEEDED(ReturnCode)) {
		HandleDatabaseError(SQL_HANDLE_STMT, Statement);
		SQLFreeHandle(SQL_HANDLE_STMT, Statement);
		return NULL;
	}

	return (DatabaseHandleRef)Statement;
}

DatabaseHandleRef DatabaseCallProcedureFetch(
	DatabaseRef Database,
	const Char* Procedure,
	...
) {
	va_list Arguments;
	va_start(Arguments, Procedure);
	DatabaseHandleRef Handle = DatabaseCallProcedureFetchInternal(Database, Procedure, Arguments);
	va_end(Arguments);
	return Handle;
}

Bool DatabaseHandleReadNext(
	DatabaseHandleRef Handle,
	...
) {
	if (!Handle) return false;

	SQLHSTMT Statement = (SQLHSTMT)Handle;
	SQLRETURN ReturnCode = SQLFetch(Statement);
	if (ReturnCode == SQL_NO_DATA) {
		SQLFreeHandle(SQL_HANDLE_STMT, Statement);
		return false;
	}
	else if (!SQL_SUCCEEDED(ReturnCode)) {
		HandleDatabaseError(SQL_HANDLE_STMT, Statement);
		SQLFreeHandle(SQL_HANDLE_STMT, Statement);
		return false;
	}

	va_list Arguments;
	va_start(Arguments, Handle);

	Int32 ColumnIndex = 1;
	while (true) {
		Int32 DataType = va_arg(Arguments, Int32);
		if (DataType == DB_PARAM_END) break;

		struct _DatabaseTypeMapping DataMapping = DatabaseTypeGetMapping(DataType);

		SQLPOINTER Buffer = va_arg(Arguments, SQLPOINTER);
		SQLLEN BufferLength = DataMapping.NativeSize;

		if (BufferLength < 1) {
			BufferLength = va_arg(Arguments, SQLLEN);
		}

		ReturnCode = SQLGetData(Statement, ColumnIndex++, DataMapping.NativeType, Buffer, BufferLength, NULL);
		if (!SQL_SUCCEEDED(ReturnCode)) {
			HandleDatabaseError(SQL_HANDLE_STMT, Statement);
			va_end(Arguments);
			SQLFreeHandle(SQL_HANDLE_STMT, Statement);
			return false;
		}
	}

	va_end(Arguments);
	SQLFreeHandle(SQL_HANDLE_STMT, Statement);
	return true;
}

Void DatabaseHandleFlush(
	DatabaseHandleRef Handle
) {
	if (!Handle) return;
	SQLFreeHandle(SQL_HANDLE_STMT, (SQLHSTMT)Handle);
}

Bool DatabaseCallProcedure(
	DatabaseRef Database,
	const Char* Procedure,
	...
) {
	Trace("Call Procedure: %s", Procedure);

	va_list Arguments;
	va_start(Arguments, Procedure);
	DatabaseHandleRef Handle = DatabaseCallProcedureFetchInternal(Database, Procedure, Arguments);
	va_end(Arguments);

	if (!Handle) return false;
	SQLFreeHandle(SQL_HANDLE_STMT, (SQLHSTMT)Handle);
	return true;
}
