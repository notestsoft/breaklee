#include "Base.h"
#include "Archive.h"

EXTERN_C_BEGIN

Void ParseBool(
    CString Value,
    Bool* Result
);

Void ParseInt8(
    CString Value,
    Int8* Result
);

Void ParseInt16(
    CString Value,
    Int16* Result
);

Void ParseInt32(
    CString Value,
    Int32* Result
);

Void ParseInt64(
    CString Value,
    Int64* Result
);

Void ParseUInt8(
    CString Value,
    UInt8* Result
);

Void ParseUInt16(
    CString Value,
    UInt16* Result
);

Void ParseUInt32(
    CString Value,
    UInt32* Result
);

Void ParseUInt64(
    CString Value,
    UInt64* Result
);

Void ParseInt(
    CString Value,
    Int* Result
);

Bool ParseAttributeInt8(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int8* Result
);

Bool ParseAttributeInt16(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int16* Result
);

Bool ParseAttributeInt32(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result
);

Bool ParseAttributeInt64(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int64* Result
);

Bool ParseAttributeInt(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int* Result
);

Bool ParseAttributeUInt8(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt8* Result
);

Bool ParseAttributeUInt16(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt16* Result
);

Bool ParseAttributeUInt32(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt32* Result
);

Bool ParseAttributeUInt64(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt64* Result
);

Bool ParseAttributeFloat32(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Float32* Result
);

Bool ParseAttributeFloat64(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Float64* Result
);

Bool ParseAttributeInt32Array(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int64 Count,
    Char Separator
);

Int32 ParseAttributeInt32ArrayCounted(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int64 Count,
    Char Separator
);

Bool ParseAttributeUInt32Array(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt32* Result,
    Int64 Count
);

Int32 ParseAttributeUInt64ArrayCounted(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt64* Result,
    Int64 Count,
    Char Separator
);

Int32 ParseAttributeInt32Array2D(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32* Result,
    Int64 Count,
    Int64 GroupCount,
    Char Separator,
    Char GroupSeparator
);

Bool ParseAttributeCharArray(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Char* Result,
    Int64 Count,
    Char Separator
);

Int32 ParseAttributeCharArrayCounted(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Char* Result,
    Int64 Count,
    Char Separator
);

Bool ParseAttributeString(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    CString Result,
    Int64 Length
);

Bool ParseAttributeInt8Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int8 Value
);

Bool ParseAttributeInt16Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int16 Value
);

Bool ParseAttributeInt32Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int32 Value
);

Bool ParseAttributeInt64Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Int64 Value
);

Bool ParseAttributeUInt8Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt8 Value
);

Bool ParseAttributeUInt16Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt16 Value
);

Bool ParseAttributeUInt32Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt32 Value
);

Bool ParseAttributeUInt64Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    UInt64 Value
);

Bool ParseAttributeFloat32Equal(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    Float32 Value
);

Bool ParseAttributeStringEqual(
    ArchiveRef Object,
    Int32 NodeIndex,
    CString Name,
    CString Value,
    Int64 Length
);

EXTERN_C_END
