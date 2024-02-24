#include "String.h"

static Char Buffer[32] = { 0 };

CString UInt8ToStringNoAlloc(
    UInt8 Value
) {
    sprintf(Buffer, "%hhu", Value);
    return Buffer;
}

CString UInt16ToStringNoAlloc(
    UInt16 Value
) {
    sprintf(Buffer, "%hu", Value);
    return Buffer;
}

CString UInt32ToStringNoAlloc(
    UInt32 Value
) {
    sprintf(Buffer, "%u", Value);
    return Buffer;
}

CString UInt64ToStringNoAlloc(
    UInt64 Value
) {
    sprintf(Buffer, "%llu", Value);
    return Buffer;
}

CString Int8ToStringNoAlloc(
    Int8 Value
) {
    sprintf(Buffer, "%hhd", Value);
    return Buffer;
}

CString Int16ToStringNoAlloc(
    Int16 Value
) {
    sprintf(Buffer, "%hd", Value);
    return Buffer;
}

CString Int32ToStringNoAlloc(
    Int32 Value
) {
    sprintf(Buffer, "%d", Value);
    return Buffer;
}

CString Int64ToStringNoAlloc(
    Int64 Value
) {
    sprintf(Buffer, "%lld", Value);
    return Buffer;
}

Void CStringCopySafe(
    CString Destination,
    Int64 DestinationSize,
    CString Source
) {
#ifdef _WIN32
    strcpy_s(Destination, DestinationSize, Source);
#else
    if (Destination == NULL || Source == NULL || DestinationSize < 1) {
        return;
    }

    for (Int64 Index = 0; Index < DestinationSize - 1 && Source[Index] != '\0'; Index += 1) {
        Destination[Index] = Source[Index];
    }

    Destination[DestinationSize - 1] = '\0';
#endif
}

Bool CStringIsEqual(
    CString Lhs,
    CString Rhs
) {
    Int64 LhsLength = strlen(Lhs);
    Int64 RhsLength = strlen(Rhs);

    if (LhsLength != RhsLength) return false;
    if (LhsLength < 1) return true;

    return memcmp(Lhs, Rhs, sizeof(Char) * LhsLength) == 0;
}

static Char kCStringFormatBuffer[MAX_PATH] = { 0 };

CString CStringFormat(
    CString Format,
    ...
) {
    va_list ArgumentPointer;
    va_start(ArgumentPointer, Format);
    vsprintf(&kCStringFormatBuffer[0], Format, ArgumentPointer);
    va_end(ArgumentPointer);
    return kCStringFormatBuffer;
}
