#pragma once

#include "Base.h"
#include "Allocator.h"

EXTERN_C_BEGIN

#define TABLESET_MAX_NAME_LENGTH 64

typedef struct _CLTable* CLTableRef;
typedef struct _CLTableSet* CLTableSetRef;

CLTableSetRef CLTableSetCreateEmpty(
    AllocatorRef Allocator
);

Void CLTableSetDestroy(
    CLTableSetRef TableSet
);

Bool CLTableSetLoadFromFile(
    CLTableSetRef TableSet,
    CString FilePath
);

Bool CLTableSetLoadFromSource(
    CLTableSetRef TableSet,
    CString Source
);

Bool CLTableSetWriteToFile(
    CLTableSetRef TableSet,
    CString FilePath
);

CLTableRef CLTableSetGetTable(
    CLTableSetRef TableSet,
    CString TableName
);

Void CLTableAddColumn(
    CLTableRef Table,
    CString ColumnName
);

Int CLTableGetColumnCount(
    CLTableRef Table
);

Int CLTableGetRowCount(
    CLTableRef Table
);

Void CLTableSetValue(
    CLTableRef Table,
    Int ColumnIndex,
    Int RowIndex,
    CString Value
);

Bool CLTableGetValue(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    CString* Result
);

Bool CLTableGetValueInt8(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int8* Result
);

Bool CLTableGetValueInt16(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int16* Result
);

Bool CLTableGetValueInt32(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int32* Result
);

Bool CLTableGetValueInt64(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int64* Result
);

Bool CLTableGetValueInt(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int* Result
);

Bool CLTableGetValueUInt8(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    UInt8* Result
);

Bool CLTableGetValueUInt16(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    UInt16* Result
);

Bool CLTableGetValueUInt32(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    UInt32* Result
);

Bool CLTableGetValueUInt64(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    UInt64* Result
);

Bool CLTableGetValueFloat32(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Float32* Result
);

Bool CLTableGetValueFloat64(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Float64* Result
);

EXTERN_C_END