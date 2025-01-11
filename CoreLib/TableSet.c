#include "Array.h"
#include "Diagnostic.h"
#include "Dictionary.h"
#include "FileIO.h"
#include "ParsePrimitives.h"
#include "String.h"
#include "TableSet.h"

#include <ctype.h>

struct _CLValueIndex {
    Int RowIndex;
    Int ColumnIndex;
};
typedef struct _CLValueIndex CLValueIndex;

Bool _CLValueIndexDictionaryKeyComparator(
    Void* Lhs,
    Void* Rhs
) {
    return memcmp(Lhs, Rhs, sizeof(struct _CLValueIndex)) == 0;
}

UInt64 _CLValueIndexDictionaryKeyHasher(
    Void* Key
) {
    UInt64 Hash = 5381;
    CLValueIndex* Current = (CLValueIndex*)Key;
    Hash = Hash * 33 + Current->RowIndex;
    Hash = Hash * 33 + Current->ColumnIndex;
    return Hash;
}

Int32 _CLValueIndexDictionaryKeySizeCallback(
    Void* Key
) {
    return sizeof(struct _CLValueIndex);
}

struct _CLTable {
    Char Name[TABLESET_MAX_NAME_LENGTH];
    Int Index;
    Int ColumnCount;
    Int RowCount;
    DictionaryRef NameToColumnIndex;
    DictionaryRef IndexToValue;
};

struct _CLTableSet {
    AllocatorRef Allocator;
    ArrayRef Tables;
    DictionaryRef NameToTable;
};

CLTableRef CLTableSetAddTable(
    CLTableSetRef TableSet,
    CString Name
) {
    CLTableRef Table = (CLTableRef)ArrayAppendUninitializedElement(TableSet->Tables);
    CStringCopySafe(Table->Name, TABLESET_MAX_NAME_LENGTH, Name);
    Table->Index = ArrayGetElementCount(TableSet->Tables) - 1;
    Table->ColumnCount = 0;
    Table->RowCount = 0;
    Table->NameToColumnIndex = CStringDictionaryCreate(TableSet->Allocator, 8);
    Table->IndexToValue = DictionaryCreate(
        TableSet->Allocator,
        &_CLValueIndexDictionaryKeyComparator,
        &_CLValueIndexDictionaryKeyHasher,
        &_CLValueIndexDictionaryKeySizeCallback,
        32768
    );
    
    DictionaryInsert(TableSet->NameToTable, Name, &Table->Index, sizeof(Int));
    CLTableAddColumn(Table, Table->Name);
    return Table;
}

static FORCE_INLINE Bool CharacterIsWhitespace(
    Char Character
) {
    return (Character == 0x09 || Character == 0x20);
}

static FORCE_INLINE Bool CharacterIsLinebreak(
    Char Character
) {
    return (
        Character == 0x0A ||
        Character == 0x0B ||
        Character == 0x0C ||
        Character == '\r'
    );
}

static FORCE_INLINE Void SkipWhitespaceAndNewline(
    Char** Cursor
) {
    while (**Cursor != '\0' && (CharacterIsWhitespace(**Cursor) || CharacterIsLinebreak(**Cursor))) {
        *Cursor += 1;
    }
}

static FORCE_INLINE Void SkipUntilToken(
    Char** Cursor,
    Char Character
) {
    while (**Cursor != '\0' && **Cursor != Character) {
        *Cursor += 1;
    }
}

static FORCE_INLINE Bool ExpectToken(
    Char** Cursor,
    Char Character
) {
    if (**Cursor != Character) {
        if (isprint(Character) && isprint(**Cursor)) {
            Error("Expected '%c' found '%c'", Character, **Cursor);
        }
        else {
            Error("Expected '%02X' found '%02X'", Character, **Cursor);
        }

        return false;
    }

    return true;
}

static FORCE_INLINE Bool ConsumeToken(
    Char** Cursor,
    Char Character
) {
    if (**Cursor != Character) {
        if (isprint(Character) && isprint(**Cursor)) {
            Error("Expected '%c' found '%c'", Character, **Cursor);
        }
        else {
            Error("Expected 0x%02X found 0x%02X", Character, **Cursor);
        }

        return false;
    }

    *Cursor += 1;
    return true;
}

static FORCE_INLINE Bool TryConsumeToken(
    Char** Cursor,
    Char Character
) {
    if (**Cursor != Character) {
        return false;
    }

    *Cursor += 1;
    return true;
}

CLTableSetRef CLTableSetCreateEmpty(
    AllocatorRef Allocator
) {
    CLTableSetRef TableSet = (CLTableSetRef)AllocatorAllocate(Allocator, sizeof(struct _CLTableSet));
    if (!TableSet) Fatal("Memory allocation failed!");
    
    TableSet->Allocator = Allocator;
    TableSet->Tables = ArrayCreateEmpty(Allocator, sizeof(struct _CLTable), 8);
    TableSet->NameToTable = CStringDictionaryCreate(Allocator, 8);

    return TableSet;
}

Void CLTableSetDestroy(
    CLTableSetRef TableSet
) {
    for (Int Index = 0; Index < ArrayGetElementCount(TableSet->Tables); Index += 1) {
        CLTableRef Table = (CLTableRef)ArrayGetElementAtIndex(TableSet->Tables, Index);
        DictionaryDestroy(Table->NameToColumnIndex);
        DictionaryDestroy(Table->IndexToValue);
    }

    ArrayDestroy(TableSet->Tables);
    DictionaryDestroy(TableSet->NameToTable);
    AllocatorDeallocate(TableSet->Allocator, TableSet);
}

Bool CLTableSetLoadFromFile(
    CLTableSetRef TableSet,
    CString FilePath
) {
    UInt8* Source = NULL;
    Int32 SourceLength = 0;
    FileRef File = FileOpen(FilePath);
    if (!File) goto error;
    if (!FileRead(File, &Source, &SourceLength)) goto error;
    FileClose(File);
    
    Bool Success = CLTableSetLoadFromSource(TableSet, (CString)Source);
    free(Source);

    return Success;

error:
    if (File) FileClose(File);
    if (Source) free(Source);

    return false;
}

Bool CLTableSetLoadFromSource(
    CLTableSetRef TableSet,
    CString Source
) {
    Char* Cursor = Source;
    while (*Cursor != '\0') {
        SkipWhitespaceAndNewline(&Cursor);
        if (*Cursor == '\0') break;

        if (!ConsumeToken(&Cursor, '[')) return false;
        CString TableName = Cursor;
        SkipUntilToken(&Cursor, ']');
        if (!ExpectToken(&Cursor, ']')) return false;
        *Cursor = '\0';

        CLTableRef Table = CLTableSetAddTable(TableSet, TableName);
        Cursor += 1;

        while (*Cursor != '\0' && !CharacterIsLinebreak(*Cursor)) {
            if (!ConsumeToken(&Cursor, '\t')) return false;

            CString ColumnName = Cursor;
            while (*Cursor != '\0' && *Cursor != '\t' && !CharacterIsLinebreak(*Cursor)) {
                Cursor += 1;
            }

            Char ColumnEnd = *Cursor;
            *Cursor = '\0';
            CLTableAddColumn(Table, ColumnName);
            *Cursor = ColumnEnd;

            if (CharacterIsLinebreak(*Cursor)) break;
        }

        Int ColumnIndex = 0;
        while (*Cursor != '\0') {
            SkipWhitespaceAndNewline(&Cursor);
            if (*Cursor == '\0') break;
            if (*Cursor == '[') break;

            CString Value = Cursor;
            while (*Cursor != '\0' && *Cursor != '\t' && !CharacterIsLinebreak(*Cursor)) {
                Cursor += 1;
            }
            Char ValueEnd = *Cursor;
            *Cursor = '\0';
            CLTableSetValue(Table, ColumnIndex, Table->RowCount, Value);
            *Cursor = ValueEnd;
            ColumnIndex += 1;

            if (*Cursor == '\0') {
                if (ColumnIndex != Table->ColumnCount) {
                    Error("Expected %d values but found %d values!", Table->ColumnCount, ColumnIndex);
                    return false;
                }
                
                break;
            }

            Bool IsSeparated = TryConsumeToken(&Cursor, '\t');
            if (CharacterIsLinebreak(*Cursor)) {
                if (ColumnIndex != Table->ColumnCount) {
                    Error("Expected %d values but found %d values!", Table->ColumnCount, ColumnIndex);
                    return false;
                }

                Table->RowCount += 1;
                ColumnIndex = 0;
                continue;
            }

            if (!IsSeparated) return false;
        }
    }

    return true;
}

Bool CLTableSetWriteToFile(
    CLTableSetRef TableSet,
    CString FilePath
) {
    for (Int TableIndex = 0; TableIndex < ArrayGetElementCount(TableSet->Tables); TableIndex += 1) {
        CLTableRef Table = (CLTableRef)ArrayGetElementAtIndex(TableSet->Tables, TableIndex);
        for (Int ColumnIndex = 0; ColumnIndex < Table->ColumnCount; ColumnIndex += 1) {
            DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(Table->NameToColumnIndex);
            while (Iterator.Key) {
                if (ColumnIndex == *(Int*)Iterator.Value) {
                    printf(ColumnIndex > 0 ? "\t%s" : "[%s]", (CString)Iterator.Key);
                    break;
                }

                Iterator = DictionaryKeyIteratorNext(Iterator);
            }
        }

        printf("\n");

        for (Int RowIndex = 0; RowIndex < Table->RowCount; RowIndex += 1) {
            for (Int ColumnIndex = 0; ColumnIndex < Table->ColumnCount; ColumnIndex += 1) {
                CLValueIndex Index = { .RowIndex = RowIndex, .ColumnIndex = ColumnIndex };
                CString Value = DictionaryLookup(Table->IndexToValue, &Index);
                if (Value) {
                    printf("%s", Value);
                }

                if (ColumnIndex + 1 < Table->ColumnCount) {
                    printf("\t");
                }
            }

            printf("\n");
        }
    }

    return true;
}

CLTableRef CLTableSetGetTable(
    CLTableSetRef TableSet,
    CString TableName
) {
    Int* TableIndexPtr = (Int*)DictionaryLookup(TableSet->NameToTable, TableName);
    if (!TableIndexPtr) return NULL;

    return (CLTableRef)ArrayGetElementAtIndex(TableSet->Tables, *TableIndexPtr);
}

Void CLTableAddColumn(
    CLTableRef Table,
    CString ColumnName
) {
    DictionaryInsert(Table->NameToColumnIndex, ColumnName, &Table->ColumnCount, sizeof(Int));
    Table->ColumnCount += 1;
}

Int CLTableGetColumnCount(
    CLTableRef Table
) {
    return Table->ColumnCount;
}

Int CLTableGetRowCount(
    CLTableRef Table
) {
    return Table->RowCount;
}

Void CLTableSetValue(
    CLTableRef Table,
    Int ColumnIndex,
    Int RowIndex,
    CString Value
) {
    CLValueIndex Index = {
        .ColumnIndex = ColumnIndex,
        .RowIndex = RowIndex
    };

    DictionaryInsert(Table->IndexToValue, &Index, Value, strlen(Value) + 1);
}

Bool CLTableGetValue(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    CString* Result
) {
    Int* ColumnIndexPtr = (Int*)DictionaryLookup(Table->NameToColumnIndex, ColumnName);
    if (!ColumnIndexPtr) return false;

    CLValueIndex Index = { .ColumnIndex = *ColumnIndexPtr, .RowIndex = RowIndex };
    *Result = (CString)DictionaryLookup(Table->IndexToValue, &Index);
    return *Result != NULL;
}

Bool CLTableGetValueInt8(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int8* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt8(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueInt16(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int16* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt16(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueInt32(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int32* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt32(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueInt64(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int64* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt64(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueInt(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Int* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueUInt8(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    UInt8* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = UINT8_MAX;
        return true;
    }

    ParseUInt8(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueUInt16(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    UInt16* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = UINT16_MAX;
        return true;
    }

    ParseUInt16(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueUInt32(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    UInt32* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = UINT32_MAX;
        return true;
    }

    ParseUInt32(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueUInt64(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    UInt64* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = UINT64_MAX;
        return true;
    }

    ParseUInt64(Value, Result);
    return true;

error:
    return false;
}

Bool CLTableGetValueFloat32(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Float32* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = -1;
        return true;
    }

    *Result = strtof(Value, NULL);
    return true;

error:
    return false;
}

Bool CLTableGetValueFloat64(
    CLTableRef Table,
    CString ColumnName,
    Int RowIndex,
    Float64* Result
) {
    CString Value = NULL;
    if (!CLTableGetValue(Table, ColumnName, RowIndex, &Value)) goto error;

    if (CStringIsEqual(Value, "<null>") || strlen(Value) < 1) {
        *Result = -1;
        return true;
    }

    *Result = strtod(Value, NULL);
    return true;

error:
    return false;
}
