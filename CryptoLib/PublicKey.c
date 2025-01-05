#include "PublicKey.h"

#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bn.h>

struct _CLPublicKeyCipher {
    AllocatorRef Allocator;
    RSA* Handle;
    Int KeyLength;
    UInt8 Key[0];
};

CLPublicKeyCipherRef CLPublicKeyCipherCreate(
    AllocatorRef Allocator
) {
    RSA* RSA = RSA_new();
    BIGNUM* Exponent = BN_new();
    BIO* BIO = BIO_new(BIO_s_mem());

    if (!RSA || !Exponent || !BIO) goto error;
    if (!BN_set_word(Exponent, RSA_F4)) goto error;
    if (RSA_generate_key_ex(RSA, 2048, Exponent, NULL) != 1) goto error;

    i2d_RSAPublicKey_bio(BIO, RSA);

    UInt8* Key = NULL;
    Int KeyLength = BIO_get_mem_data(BIO, &Key);
    if (!Key) goto error;

    CLPublicKeyCipherRef Cipher = (CLPublicKeyCipherRef)AllocatorAllocate(
        Allocator, 
        sizeof(struct _CLPublicKeyCipher) + KeyLength
    );
    if (!Cipher) Fatal("Memory allocation failed!");
    
    Cipher->Allocator = Allocator;
    Cipher->Handle = RSA;
    Cipher->KeyLength = KeyLength;
    memcpy(&Cipher->Key[0], Key, KeyLength);

    BN_free(Exponent);
    BIO_free(BIO);

    return Cipher;

error:
    if (RSA) RSA_free(RSA);
    if (Exponent) BN_free(Exponent);
    if (BIO) BIO_free(BIO);

    return NULL;
}

Void CLPublicKeyCipherDestroy(
    CLPublicKeyCipherRef Cipher
) {
    RSA_free(Cipher->Handle);
    AllocatorDeallocate(Cipher->Allocator, Cipher);
}

Int CLPublicKeyCipherGetKeyLength(
    CLPublicKeyCipherRef Cipher
) {
    return Cipher->KeyLength;
}

UInt8* CLPublicKeyCipherGetKey(
    CLPublicKeyCipherRef Cipher
) {
    return &Cipher->Key[0];
}

Int CLPublicKeyCipherDecrypt(
    CLPublicKeyCipherRef Cipher,
    UInt8* EncryptedPayload,
    UInt8* DecryptedPayload
) {
    Int32 Length = RSA_size(Cipher->Handle);
    Int32 DecryptedPayloadLength = RSA_private_decrypt(
        Length,
        EncryptedPayload,
        DecryptedPayload,
        Cipher->Handle,
        RSA_PKCS1_OAEP_PADDING
    );

    return DecryptedPayloadLength;
}
