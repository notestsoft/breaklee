#pragma once

#include "Base.h"

EXTERN_C_BEGIN

CString UInt8ToStringNoAlloc(
    UInt8 Value
);

CString UInt16ToStringNoAlloc(
    UInt16 Value
);

CString UInt32ToStringNoAlloc(
    UInt32 Value
);

CString UInt64ToStringNoAlloc(
    UInt64 Value
);

CString Int8ToStringNoAlloc(
    Int8 Value
);

CString Int16ToStringNoAlloc(
    Int16 Value
);

CString Int32ToStringNoAlloc(
    Int32 Value
);

CString Int64ToStringNoAlloc(
    Int64 Value
);

Void CStringCopySafe(
    CString Destination,
    Int64 DestinationSize,
    CString Source
);

Bool CStringIsEqual(
    CString Lhs, 
    CString Rhs
);

Bool CStringHasFileExtension(
    CString String,
    CString Extension
);

CString CStringFormat(
    PRINTFLIKE_PARAMETER(CString Format),
    ...
) PRINTFLIKE_ATTRIBUTE(2, 3);

EXTERN_C_END
