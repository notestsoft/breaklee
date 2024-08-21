#pragma once

#include "Base.h"

#define SALT_LENGTH 16
#define HASH_LENGTH EVP_MAX_MD_SIZE
#define SALTED_HASH_LENGTH (SALT_LENGTH + HASH_LENGTH)

EXTERN_C_BEGIN

#define CONFIG_DATA_BUFFER_SIZE 4096

#define BENCHMARK(__NAME__, __CODE__) \
do { \
    LARGE_INTEGER _Start, _End, _Freq; \
    QueryPerformanceFrequency(&_Freq); \
    QueryPerformanceCounter(&_Start); \
    { __CODE__ } \
    QueryPerformanceCounter(&_End); \
    LONGLONG _Elapsed = _End.QuadPart - _Start.QuadPart; \
    LONGLONG _ElapsedMilliseconds = (_Elapsed * 1000LL) / _Freq.QuadPart; \
    Trace("[%s] Time elapsed: %lldms", __NAME__, _ElapsedMilliseconds); \
} while (0)

Index Align(
    Index Value,
    Index Alignment
);

Bool IsPowerOfTwo(
    Index Value
);

Index NextPowerOfTwo(
    Index Value
);

Timestamp GetTimestamp();

Timestamp GetTimestampMs();

Bool GetPlatformErrorMessage(
	CString Buffer,
	Int32 Length
);

Bool CreatePasswordHash(
    CString Password,
    UInt8* PasswordHash
);

Bool ValidatePasswordHash(
    CString Password,
    UInt8* PasswordHash
);

Void GenerateRandomKey(
    CString Buffer,
    Int32 Length
);

UInt32 SwapUInt32(
    UInt32 Value
);

Int32 Random(
    Int32* Seed
);

Int32 RandomRange(
    Int32* Seed,
    Int32 Min,
    Int32 Max
);

Void ReadConfigString(
    CString FilePath,
    CString KeyPath,
    CString Default,
    CString Result,
    Int32 ResultLength
);

Void ReadConfigCharArray(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Char* Result,
    Int32 ResultLength
);

Void ReadConfigBool(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Bool* Result
);

Void ReadConfigInt8(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int8* Result
);

Void ReadConfigInt16(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int16* Result
);

Void ReadConfigInt32(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int32* Result
);

Void ReadConfigInt64(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int64* Result
);

Void ReadConfigUInt8(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt8* Result
);

Void ReadConfigUInt16(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt16* Result
);

Void ReadConfigUInt32(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt32* Result
);

Void ReadConfigUInt64(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt64* Result
);

Void ReadConfigIndex(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Index* Result
);

Timestamp PlatformGetTickCount();

Void PlatformSleep(
    UInt64 Milliseconds
); 

Void RenderLogo();

EXTERN_C_END
