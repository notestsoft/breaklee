#include <openssl/evp.h>
#include <openssl/rand.h>

#include "Diagnostic.h"
#include "ParsePrimitives.h"
#include "Util.h"
#include "String.h"

static Char kRandomKeyTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

Index Align(
    Index Value,
    Index Alignment
) {
    assert(IsPowerOfTwo(Alignment));

    return (Value + Alignment - 1) & ~(Alignment - 1);
}

Bool IsPowerOfTwo(
    Index Value
) {
    return (Value & (Value - 1)) == 0;
}

Index NextPowerOfTwo(Index Value) {
    Value--;
    Value |= Value >> 1;
    Value |= Value >> 2;
    Value |= Value >> 4;
    Value |= Value >> 8;
    Value |= Value >> 16;
    Value++;
    return Value;
}

// TODO: This is returning the timestamp in seconds but we mostly depend on milliseconds!
Timestamp GetTimestamp() {
	return (Timestamp)time(NULL);
}

Timestamp GetTimestampMs() {
    return (Timestamp)time(NULL) * 1000;
}

Bool GetPlatformErrorMessage(
	CString Buffer,
	Int32 Length
) {
	assert(Buffer);
#if _WIN32
	DWORD MessageID = GetLastError();
	if (!MessageID) return false;
	
	LPSTR MessageBuffer = NULL;
	DWORD MessageLength = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		MessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&MessageBuffer,
		0,
		NULL
	);

	memcpy_s(Buffer, Length, MessageBuffer, MessageLength);
	LocalFree(MessageBuffer);
#elif defined(__linux__)
    strerror_r(errno, Buffer, Length);
#elif defined(__APPLE__)
    mach_error_t MessageID = errno;
    if (!MessageID) return false;

    Char *MessageBuffer = mach_error_string(MessageID);
    Int64 MessageLength = strlen(MessageBuffer);
    memcpy(Buffer, MessageBuffer, MIN(Length, MessageLength));
#else
    #error "Unsupported Platform"
#endif
    return true;
}

EVP_MD_CTX *CreateHashAlgorithmSha512() {
    EVP_MD_CTX *Context = EVP_MD_CTX_new();
    if (!Context) return NULL;
    
    const EVP_MD *MD = EVP_sha512();
    
    if (EVP_DigestInit_ex(Context, MD, NULL) != 1) {
        EVP_MD_CTX_free(Context);
        return NULL;
    }
    
    return Context;
}

Bool CreatePasswordHash(
    CString Password,
    UInt8** Salt,
    Int32* SaltLength,
    UInt8* Hash,
    Int32* HashLength
) {
    EVP_MD_CTX *Context = NULL;
    UInt32 PasswordLength = (UInt32)strlen(Password);
    UInt32 SaltBufferLength = 64;
    UInt32 BufferLength = SaltBufferLength + PasswordLength + 1;
    UInt8* Buffer = NULL;
    UInt32 HashDataLength = 0;
    
    Buffer = (UInt8*)malloc(sizeof(UInt8) * BufferLength);
    if (!Buffer) goto error;

    UInt8* SaltBuffer = &Buffer[0];
    UInt8* PasswordBuffer = &Buffer[SaltBufferLength];
    memcpy(PasswordBuffer, Password, PasswordLength);
    memset(&Buffer[SaltBufferLength + PasswordLength], 0, sizeof(UInt8));

    if (!(Context = CreateHashAlgorithmSha512())) goto error;
    if (RAND_bytes(SaltBuffer, SaltBufferLength) != 1) goto error;
    if (EVP_DigestUpdate(Context, Buffer, BufferLength) != 1) goto error;
    if (EVP_DigestFinal_ex(Context, Hash, &HashDataLength) != 1) goto error;

    // We are safe to just return the SaltBuffer because we know it is pointing to the start of the Buffer memory.
    *Salt = (UInt8*)SaltBuffer;
    *SaltLength = (Int32)SaltBufferLength;
    *HashLength = (Int32)HashDataLength;
    EVP_MD_CTX_free(Context);
    return true;

error:
    if (Context) EVP_MD_CTX_free(Context);
    if (Buffer) free(Buffer);
    return false;
}

Bool ValidatePasswordHash(
    CString Password,
    UInt8* Salt,
    Int32 SaltLength,
    UInt8* Hash,
    Int32 HashLength
) {
    EVP_MD_CTX *Context = NULL;
    UInt32 PasswordLength = (UInt32)strlen(Password);
    UInt32 SaltBufferLength = (UInt32)SaltLength;
    UInt32 BufferLength = SaltBufferLength + PasswordLength + 1;
    UInt8* Buffer = NULL;
    UInt32 HashDataLength = 0;
    UInt8 HashData[EVP_MAX_MD_SIZE] = { 0 };

    Buffer = (UInt8*)malloc(sizeof(UInt8) * BufferLength);
    if (!Buffer) goto error;

    UInt8* SaltBuffer = &Buffer[0];
    UInt8* PasswordBuffer = &Buffer[SaltBufferLength];
    memcpy(SaltBuffer, Salt, SaltBufferLength);
    memcpy(PasswordBuffer, Password, PasswordLength);
    memset(&Buffer[SaltBufferLength + PasswordLength], 0, sizeof(UInt8));

    if (!(Context = CreateHashAlgorithmSha512())) goto error;
    if (EVP_DigestUpdate(Context, Buffer, BufferLength) != 1) goto error;
    if (EVP_DigestFinal_ex(Context, HashData, &HashDataLength) != 1) goto error;
    
    if (HashLength != HashDataLength) goto error;
    if (memcmp(Hash, HashData, HashDataLength) != 0) goto error;
    
    EVP_MD_CTX_free(Context);
    free(Buffer);
    return true;

error:
    if (Context) EVP_MD_CTX_free(Context);
    if (Buffer) free(Buffer);
    return false;
}

Void GenerateRandomKey(
    CString Buffer,
    Int32 Length
) {
    srand((UInt32)time(NULL));

    for (Int32 Index = 0; Index < Length; Index++) {
        Buffer[Index] = kRandomKeyTable[rand() % (sizeof(kRandomKeyTable) - 1)];
    }
}

UInt32 SwapUInt32(
    UInt32 Value
) {
    Value = ((Value << 8) & 0xFF00FF00) | ((Value >> 8) & 0xFF00FF);
    return (Value << 16) | (Value >> 16);
}

Int32 Random(Int32* Seed) {
    *Seed = ((*Seed * 1103515245) + 12345) & 0x7fffffff;
    return *Seed;
}

Int32 RandomRange(Int32* Seed, Int32 Min, Int32 Max) {
    Int32 Value = Random(Seed);
    return Min + Value % (Max - Min + 1);
}

Void ReadConfigString(
    CString FilePath,
    CString KeyPath,
    CString Default,
    CString Result,
    Int32 ResultLength
) {
#if _WIN32
    Char AppName[MAX_PATH] = { 0 };

    Int32 AppNameLength = 0;
    CString Cursor = strchr(KeyPath, '.');
    if (Cursor) {
        AppNameLength = Cursor - KeyPath;
        memcpy(AppName, KeyPath, AppNameLength);
        AppName[AppNameLength] = '\0';
    }

    Int32 KeyNameOffset = AppNameLength + 1;
    Int32 KeyNameLength = strlen(KeyPath) - KeyNameOffset;
    Char KeyName[MAX_PATH] = { 0 };
    memcpy(KeyName, KeyPath + KeyNameOffset, KeyNameLength);
    KeyName[KeyNameLength] = '\0';

    GetPrivateProfileStringA(
        AppName,
        KeyName,
        Default,
        Result,
        ResultLength,
        FilePath
    );
#else
#warning Add ini parsing for non Win32 system
    CStringCopySafe(Result, ResultLength, Default);
#endif
}

Void ReadConfigCharArray(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Char* Result,
    Int32 ResultLength
) {
    ReadConfigString(FilePath, KeyPath, Default, Result, ResultLength);
}

Void ReadConfigBool(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Bool* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseBool(Buffer, Result);
}

Void ReadConfigInt8(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int8* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseInt8(Buffer, Result);
}

Void ReadConfigInt16(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int16* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseInt16(Buffer, Result);
}

Void ReadConfigInt32(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int32* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseInt32(Buffer, Result);
}

Void ReadConfigInt64(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Int64* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseInt64(Buffer, Result);
}

Void ReadConfigUInt8(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt8* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseUInt8(Buffer, Result);
}

Void ReadConfigUInt16(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt16* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseUInt16(Buffer, Result);
}

Void ReadConfigUInt32(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt32* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseUInt32(Buffer, Result);
}

Void ReadConfigUInt64(
    CString FilePath,
    CString KeyPath,
    CString Default,
    UInt64* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseUInt64(Buffer, Result);
}

Void ReadConfigIndex(
    CString FilePath,
    CString KeyPath,
    CString Default,
    Index* Result
) {
    Char Buffer[MAX_PATH] = { 0 };
    ReadConfigString(FilePath, KeyPath, Default, Buffer, MAX_PATH);
    ParseIndex(Buffer, Result);
}

Timestamp PlatformGetTickCount() {
#ifdef _WIN32
    return GetTickCount64();
#else
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (Timestamp)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
}

void __unreachable(const char* message, const char* file, size_t line) {
    exit(EXIT_FAILURE);
}
