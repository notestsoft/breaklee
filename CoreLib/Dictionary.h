#pragma once

#include "Allocator.h"

EXTERN_C_BEGIN

typedef Bool (*DictionaryKeyComparator)(
    Void* Lhs,
    Void* Rhs
);

typedef UInt64 (*DictionaryKeyHasher)(
    Void* Key
);

typedef Int32 (*DictionaryKeySizeCallback)(
    Void* Key
);

typedef struct _Dictionary *DictionaryRef;
typedef struct _DictionaryBucket* DictionaryBucketRef;

struct _DictionaryKeyIterator {
    DictionaryRef Dictionary;
    DictionaryBucketRef Bucket;
    Int BucketIndex;
    Void* Key;
    Void* Value;
};
typedef struct _DictionaryKeyIterator DictionaryKeyIterator;

DictionaryRef DictionaryCreate(
    AllocatorRef Allocator, 
    DictionaryKeyComparator Comparator, 
    DictionaryKeyHasher Hasher,
    DictionaryKeySizeCallback KeySizeCallback, 
    Int Capacity
);

DictionaryRef CStringDictionaryCreate(
    AllocatorRef Allocator, 
    Int Capacity
);

DictionaryRef IndexDictionaryCreate(
    AllocatorRef Allocator, 
    Int Capacity
);

Void DictionaryDestroy(
    DictionaryRef Dictionary
);

Void DictionaryInsert(
    DictionaryRef Dictionary, 
    Void* Key, 
    Void* Element, 
    Int32 ElementSize
);

Bool DictionaryContains(
    DictionaryRef Dictionary,
    Void *Key
);

Void* DictionaryLookup(
    DictionaryRef Dictionary, 
    Void* Key
);

Void DictionaryRemove(
    DictionaryRef Dictionary,
    Void* Key
);

Void DictionaryRemoveAll(
    DictionaryRef Dictionary
);

Void DictionaryGetKeyBuffer(
    DictionaryRef Dictionary, 
    Void **Memory, 
    Int32 *Length
);

Void DictionaryGetValueBuffer(
    DictionaryRef Dictionary, 
    Void **Memory, 
    Int32 *Length
);

DictionaryKeyIterator DictionaryGetKeyIterator(
    DictionaryRef Dictionary
);

DictionaryKeyIterator DictionaryKeyIteratorNext(
    DictionaryKeyIterator Iterator
);

EXTERN_C_END
