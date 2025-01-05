#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef struct _CLPublicKeyCipher* CLPublicKeyCipherRef;

CLPublicKeyCipherRef CLPublicKeyCipherCreate(
    AllocatorRef Allocator
);

Void CLPublicKeyCipherDestroy(
    CLPublicKeyCipherRef Cipher
);

Int CLPublicKeyCipherGetKeyLength(
    CLPublicKeyCipherRef Cipher
);

UInt8* CLPublicKeyCipherGetKey(
    CLPublicKeyCipherRef Cipher
);

Int CLPublicKeyCipherDecrypt(
    CLPublicKeyCipherRef Cipher,
    UInt8* EncryptedPayload,
    UInt8* DecryptedPayload
);

EXTERN_C_END