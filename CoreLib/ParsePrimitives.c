#include "ParsePrimitives.h"

Void ParseBool(
    CString Value,
    Bool* Result
) {
    *Result = (Bool)strtoll(Value, NULL, 0);
}

Void ParseInt8(
    CString Value,
    Int8* Result
) {
    *Result = (Int8)strtoll(Value, NULL, 0);
}

Void ParseInt16(
    CString Value,
    Int16* Result
) {
    *Result = (Int16)strtoll(Value, NULL, 0);
}

Void ParseInt32(
    CString Value,
    Int32* Result
) {
    *Result = (Int32)strtoll(Value, NULL, 0);
}

Void ParseInt64(
    CString Value,
    Int64* Result
) {
    *Result = (Int64)strtoll(Value, NULL, 0);
}

Void ParseUInt8(
    CString Value,
    UInt8* Result
) {
    *Result = (UInt8)strtoull(Value, NULL, 0);
}

Void ParseUInt16(
    CString Value,
    UInt16* Result
) {
    *Result = (UInt16)strtoull(Value, NULL, 0);
}

Void ParseUInt32(
    CString Value,
    UInt32* Result
) {
    *Result = (UInt32)strtoull(Value, NULL, 0);
}

Void ParseUInt64(
    CString Value,
    UInt64* Result
) {
    *Result = (UInt64)strtoull(Value, NULL, 0);
}

Void ParseInt(
    CString Value,
    Int* Result
) {
    *Result = (Int)strtoll(Value, NULL, 0);
}

Bool ParseAttributeInt8(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int8* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt8(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeInt16(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int16* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt16(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeInt32(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt32(Data->Data, Result);
    return true;
    
error:
    return false;
}

Bool ParseAttributeInt64(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int64* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt64(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeInt(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = -1;
        return true;
    }

    ParseInt(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeUInt8(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt8* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = UINT8_MAX;
        return true;
    }

    ParseUInt8(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeUInt16(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt16* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = UINT16_MAX;
        return true;
    }

    ParseUInt16(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeUInt32(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt32* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = UINT32_MAX;
        return true;
    }

    ParseUInt32(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeUInt64(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt64* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = -1;
        return true;
    }

    ParseUInt64(Data->Data, Result);
    return true;

error:
    return false;
}

Bool ParseAttributeFloat32(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Float32* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = -1;
        return true;
    }

    *Result = strtof(Data->Data, NULL);
    return true;

error:
    return false;
}

Bool ParseAttributeFloat64(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Float64* Result
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        *Result = 0.0;
        return true;
    }

    *Result = strtod(Data->Data, NULL);
    return true;

error:
    return false;
}

Bool ParseAttributeInt32Array(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int64 Count,
    Char Separator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(Int32) * Count);

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        return true;
    }

    Int Index = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length && Index < Count) {
        if (*Cursor == '\0') break;

        if (*Cursor == Separator) {
            Cursor += 1;
            continue;
        }

        Char* Next;
        Result[Index] = (Int32)strtoll(Cursor, &Next, 10);
        Index += 1;
        Cursor = Next;
    }

    return true;

error:
    return false;
}

Int32 ParseAttributeInt32ArrayCounted(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int64 Count,
    Char Separator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(Int32) * Count);

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        return 0;
    }

    Int Index = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length) {
        if (*Cursor == '\0') break;

        if (*Cursor == Separator) {
            Cursor += 1;
            continue;
        }

        assert(Index < Count);
        Char* Next;
        Result[Index] = (Int32)strtoll(Cursor, &Next, 10);
        Index += 1;
        Cursor = Next;
    }

    return Index;

error:
    return 0;
}

Bool ParseAttributeUInt32Array(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt32* Result,
    Int64 Count
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(UInt32) * Count);

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        return true;
    }

    Int Index = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length) {
        if (*Cursor == '\0') break;

        if (*Cursor == ':') {
            Cursor += 1;
            continue;
        }

        assert(Index < Count);
        Char* Next = NULL;
        Result[Index] = (UInt32)strtoull(Cursor, &Next, 10);
        Index += 1;
        Cursor = Next;
    }

    return true;

error:
    return false;
}

Int32 ParseAttributeUInt64ArrayCounted(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt64* Result,
    Int64 Count,
    Char Separator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(UInt64) * Count);

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        return 0;
    }

    Int Index = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length) {
        if (*Cursor == '\0') break;

        if (*Cursor == Separator) {
            Cursor += 1;
            continue;
        }

        assert(Index < Count);
        Char* Next;
        Result[Index] = (UInt64)strtoull(Cursor, &Next, 10);
        Index += 1;
        Cursor = Next;
    }

    return Index;

error:
    return 0;
}

Int32 ParseAttributeInt32Array2D(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int64 Count,
    Int64 GroupCount,
    Char Separator,
    Char GroupSeparator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    memset(Result, 0, sizeof(Int32) * Count * GroupCount);

    if (Data->Length < 1 || strlen(Data->Data) < 1) {
        return 0;
    }

    Int64 Index = 0;
    Int32 GroupIndex = 0;
    Char* Cursor = Data->Data;
    while (Cursor < Data->Data + Data->Length) {
        if (*Cursor == '\0') break;

        if (*Cursor == Separator) {
            Cursor += 1;
            Index += 1;
            continue;
        }

        if (*Cursor == GroupSeparator) {
            Cursor += 1;
            GroupIndex += 1;
            Index = 0;
            continue;
        }

        assert(GroupIndex * Count + Index < Count * GroupCount);
        Char* Next;
        Result[GroupIndex * Count + Index] = (Int32)strtoll(Cursor, &Next, 10);
        Cursor = Next;
    }

    return (GroupIndex < 1 && Index > 0) ? 1 : GroupIndex;

error:
    return 0;
}

// TODO: Check if Separator should be used
Bool ParseAttributeCharArray(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Char* Result,
    Int64 Count,
    Char Separator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    Int64 Length = MIN(Count, Data->Length);
    memcpy(Result, Data->Data, Length);
    if (Length > 0) Result[Length - 1] = '\0';
    return true;

error:
    return false;
}

Int32 ParseAttributeCharArrayCounted(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Char* Result,
    Int64 Count,
    Char Separator
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    Int64 Length = Data->Length;
    memcpy(Result, Data->Data, Length);
    if (Length > 0) Result[strlen(Result)] = '\0';
    return (Int32)strlen(Result);

error:
    return 0;
}

Bool ParseAttributeString(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    CString Result,
    Int64 Length
) {
    Int32 AttributeIndex = ArchiveNodeGetAttributeByName(Object, NodeIndex, Name);
    if (AttributeIndex < 0) goto error;

    ArchiveStringRef Data = ArchiveAttributeGetData(Object, AttributeIndex);
    if (!Data) goto error;

    assert(Data->Length + 1 <= Length);
    memcpy(Result, Data->Data, Data->Length);
    Result[Data->Length] = '\0';
    return true;

error:
    return false;
}

Bool ParseAttributeInt8Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int8 Value
) {
    Int8 Result = 0;
    return ParseAttributeInt8(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeInt16Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int16 Value
) {
    Int16 Result = 0;
    return ParseAttributeInt16(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeInt32Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32 Value
) {
    Int32 Result = 0;
    return ParseAttributeInt32(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeInt64Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int64 Value
) {
    Int64 Result = 0;
    return ParseAttributeInt64(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeUInt8Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt8 Value
) {
    UInt8 Result = 0;
    return ParseAttributeUInt8(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeUInt16Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt16 Value
) {
    UInt16 Result = 0;
    return ParseAttributeUInt16(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeUInt32Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt32 Value
) {
    UInt32 Result = 0;
    return ParseAttributeUInt32(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeUInt64Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt64 Value
) {
    UInt64 Result = 0;
    return ParseAttributeUInt64(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeFloat32Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Float32 Value
) {
    Float32 Result = 0;
    return ParseAttributeFloat32(Object, NodeIndex, Name, &Result) && Result == Value;
}

Bool ParseAttributeStringEqual(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    CString Value,
    Int64 Length
) {
    Char Buffer[MAX_PATH];
    return ParseAttributeString(Object, NodeIndex, Name, Buffer, Length) && strcmp(Value, Buffer) == 0;
}
