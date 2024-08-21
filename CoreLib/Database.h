#pragma once

#include "Base.h"

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#define SQL_END -1

#define DATABASE_MAX_QUERY_LENGTH               1024
#define DATABASE_MAX_BUFFER_LENGTH              1024
#define DATABASE_MAX_PROCEDURE_PARAMETER_COUNT  8

EXTERN_C_BEGIN

typedef struct _DataTable* DataTableRef;
typedef struct _Database* DatabaseRef;
typedef struct _Statement* StatementRef;
typedef Void* DatabaseHandleRef;

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
);

Void DatabaseDisconnect(
    DatabaseRef Database
);

CString DatabaseGetName(
    DatabaseRef Database
);

Bool DatabaseBeginTransaction(
    DatabaseRef Database
);

Bool DatabaseCommitTransaction(
    DatabaseRef Database
);

Bool DatabaseRollbackTransaction(
    DatabaseRef Database
);

Bool DatabaseExecuteQuery(
    DatabaseRef Database,
    CString Query
);

DatabaseHandleRef DatabaseCallProcedureFetch(
    DatabaseRef Database,
    const Char* Procedure,
    ...
);

Bool DatabaseHandleReadNext(
    DatabaseHandleRef Handle,
    ...
);

Void DatabaseHandleFlush(
    DatabaseHandleRef Handle
);

Bool DatabaseCallProcedure(
    DatabaseRef Database,
    const Char* Procedure,
    ...
);

EXTERN_C_END
