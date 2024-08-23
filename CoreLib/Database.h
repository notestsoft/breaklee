#pragma once

#include "Base.h"

#define DATABASE_MAX_QUERY_LENGTH               1024
#define DATABASE_MAX_BUFFER_LENGTH              1024
#define DATABASE_MAX_PROCEDURE_PARAMETER_COUNT  64

#define DB_PARAM_INPUT  (0)
#define DB_PARAM_OUTPUT (1)
#define DB_PARAM_END    (-1)

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

#define DB_INPUT_BOOL(__PARAMETER__)                DB_PARAM_INPUT, DB_TYPE_INT8, &__PARAMETER__
#define DB_INPUT_CHAR(__PARAMETER__)                DB_PARAM_INPUT, DB_TYPE_CHAR, &__PARAMETER__
#define DB_INPUT_STRING(__PARAMETER__, __SIZE__)    DB_PARAM_INPUT, DB_TYPE_STRING, __PARAMETER__, __SIZE__
#define DB_INPUT_INT8(__PARAMETER__)                DB_PARAM_INPUT, DB_TYPE_INT8, &__PARAMETER__
#define DB_INPUT_INT16(__PARAMETER__)               DB_PARAM_INPUT, DB_TYPE_INT16, &__PARAMETER__
#define DB_INPUT_INT32(__PARAMETER__)               DB_PARAM_INPUT, DB_TYPE_INT32, &__PARAMETER__
#define DB_INPUT_INT64(__PARAMETER__)               DB_PARAM_INPUT, DB_TYPE_INT64, &__PARAMETER__
#define DB_INPUT_UINT8(__PARAMETER__)               DB_PARAM_INPUT, DB_TYPE_UINT8, &__PARAMETER__
#define DB_INPUT_UINT16(__PARAMETER__)              DB_PARAM_INPUT, DB_TYPE_UINT16, &__PARAMETER__
#define DB_INPUT_UINT32(__PARAMETER__)              DB_PARAM_INPUT, DB_TYPE_UINT32, &__PARAMETER__
#define DB_INPUT_UINT64(__PARAMETER__)              DB_PARAM_INPUT, DB_TYPE_UINT64, &__PARAMETER__
#define DB_INPUT_DATA(__PARAMETER__, __SIZE__)      DB_PARAM_INPUT, DB_TYPE_DATA, __PARAMETER__, __SIZE__

#define DB_OUTPUT_BOOL(__PARAMETER__)               DB_PARAM_OUTPUT, DB_TYPE_INT8, &__PARAMETER__
#define DB_OUTPUT_CHAR(__PARAMETER__)               DB_PARAM_OUTPUT, DB_TYPE_CHAR, &__PARAMETER__
#define DB_OUTPUT_STRING(__PARAMETER__, __SIZE__)   DB_PARAM_OUTPUT, DB_TYPE_STRING, __PARAMETER__, __SIZE__
#define DB_OUTPUT_INT8(__PARAMETER__)               DB_PARAM_OUTPUT, DB_TYPE_INT8, &__PARAMETER__
#define DB_OUTPUT_INT16(__PARAMETER__)              DB_PARAM_OUTPUT, DB_TYPE_INT16, &__PARAMETER__
#define DB_OUTPUT_INT32(__PARAMETER__)              DB_PARAM_OUTPUT, DB_TYPE_INT32, &__PARAMETER__
#define DB_OUTPUT_INT64(__PARAMETER__)              DB_PARAM_OUTPUT, DB_TYPE_INT64, &__PARAMETER__
#define DB_OUTPUT_UINT8(__PARAMETER__)              DB_PARAM_OUTPUT, DB_TYPE_UINT8, &__PARAMETER__
#define DB_OUTPUT_UINT16(__PARAMETER__)             DB_PARAM_OUTPUT, DB_TYPE_UINT16, &__PARAMETER__
#define DB_OUTPUT_UINT32(__PARAMETER__)             DB_PARAM_OUTPUT, DB_TYPE_UINT32, &__PARAMETER__
#define DB_OUTPUT_UINT64(__PARAMETER__)             DB_PARAM_OUTPUT, DB_TYPE_UINT64, &__PARAMETER__
#define DB_OUTPUT_DATA(__PARAMETER__, __SIZE__)     DB_PARAM_OUTPUT, DB_TYPE_DATA, __PARAMETER__, __SIZE__

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
