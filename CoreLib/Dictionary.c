#include "Diagnostic.h"
#include "Dictionary.h"
#include "TempAllocator.h"

struct _DictionaryBucket {
    UInt64 Hash;
    Int KeyOffset;
    Int ElementOffset;
    Bool IsFilled;
    struct _DictionaryBucket* Next;
};

struct _DictionaryBuffer {
    Int Offset;
    Int Capacity;
    MemoryRef Memory;
};
typedef struct _DictionaryBuffer DictionaryBuffer;

struct _Dictionary {
    AllocatorRef Allocator;
    AllocatorRef BucketAllocator;
    DictionaryKeyComparator Comparator;
    DictionaryKeyHasher Hasher;
    DictionaryKeySizeCallback KeySizeCallback;
    DictionaryBuffer KeyBuffer;
    DictionaryBuffer ElementBuffer;
    Int Capacity;
    Int ElementCount;
    DictionaryBucketRef* Buckets;
};

const Int _kDictionaryBufferDefaultCapacity = 65535;
const Float32 _kDictionaryBufferGrowthFactor = 1.5;

static inline Void _DictionaryBufferInit(
    DictionaryRef Dictionary,
    DictionaryBuffer* Buffer
) {
    Buffer->Offset = 0;
    Buffer->Capacity = _kDictionaryBufferDefaultCapacity;
    Buffer->Memory = AllocatorAllocate(Dictionary->Allocator, Buffer->Capacity);
}

static inline Void _DictionaryBufferReserveCapacity(
    DictionaryRef Dictionary,
    DictionaryBuffer* Buffer,
    Int Capacity
) {
    Int NewCapacity = Buffer->Capacity;
    while (NewCapacity < Capacity) {
        NewCapacity *= _kDictionaryBufferGrowthFactor;
    }

    if (NewCapacity > Buffer->Capacity) {
        Buffer->Capacity = NewCapacity;
        Buffer->Memory = AllocatorReallocate(Dictionary->Allocator, Buffer->Memory, Buffer->Capacity);
    }
}

static inline Void* _DictionaryBufferGetElement(
    DictionaryRef Dictionary,
    DictionaryBuffer* Buffer,
    Int32 Offset
) {
    return (Void*)(((UInt8*)Buffer->Memory) + Offset);
}

static inline Int32 _DictionaryBufferInsertElement(
    DictionaryRef Dictionary,
    DictionaryBuffer* Buffer,
    Void* Element,
    Int32 ElementSize
) {
    Int32 RequiredCapacity = Buffer->Offset + ElementSize;
    _DictionaryBufferReserveCapacity(Dictionary, Buffer, RequiredCapacity);
    Int32 Offset = Buffer->Offset;
    UInt8* Destination = ((UInt8*)Buffer->Memory) + Buffer->Offset;
    memcpy(Destination, Element, ElementSize);
    Buffer->Offset += ElementSize;
    return Offset;
}

static inline Void _DictionaryBufferDeinit(
    DictionaryRef Dictionary,
    DictionaryBuffer* Buffer
) {
    AllocatorDeallocate(Dictionary->Allocator, Buffer->Memory);
}

Bool _CStringDictionaryKeyComparator(
    Void* Lhs,
    Void* Rhs
) {
    return strcmp((const char*)Lhs, (const char*)Rhs) == 0;
}

UInt64 _CStringDictionaryKeyHasher(
    Void* Key
) {
    UInt64 Hash = 5381;
    Char* Current = (Char*)Key;

    while (*Current != '\0') {
        Hash = Hash * 33 + (*Current);
        Current += 1;
    }

    return Hash;
}

Int32 _CStringDictionaryKeySizeCallback(
    Void* Key
) {
    return (Int32)strlen((const char*)Key) + 1;
}

Bool _IndexDictionaryKeyComparator(
    Void* Lhs,
    Void* Rhs
) {
    return (*(Int*)Lhs) == (*(Int*)Rhs);
}

UInt64 _IndexDictionaryKeyHasher(
    Void* Key
) {
    return (UInt64)(*(Int*)Key);
}

Int32 _IndexDictionaryKeySizeCallback(
    Void* Key
) {
    return sizeof(Int);
}

DictionaryRef DictionaryCreate(
    AllocatorRef Allocator,
    DictionaryKeyComparator Comparator,
    DictionaryKeyHasher Hasher,
    DictionaryKeySizeCallback KeySizeCallback,
    Int Capacity
) {
    DictionaryRef Dictionary = (DictionaryRef)AllocatorAllocate(
        Allocator,
        sizeof(struct _Dictionary) + sizeof(struct _DictionaryBucket) * Capacity
    );
    if (!Dictionary) Fatal("Memory allocation failed!");
    
    Dictionary->Allocator = Allocator;
    Dictionary->BucketAllocator = TempAllocatorCreate(Allocator);
    Dictionary->Comparator = Comparator;
    Dictionary->Hasher = Hasher;
    Dictionary->KeySizeCallback = KeySizeCallback;
    Dictionary->Capacity = Capacity;
    Dictionary->ElementCount = 0;
    Dictionary->Buckets = (DictionaryBucketRef*)(((UInt8*)Dictionary) + sizeof(struct _Dictionary));
    memset(Dictionary->Buckets, 0, sizeof(struct _DictionaryBucket) * Capacity);
    _DictionaryBufferInit(Dictionary, &Dictionary->KeyBuffer);
    _DictionaryBufferInit(Dictionary, &Dictionary->ElementBuffer);
    return Dictionary;
}

DictionaryRef CStringDictionaryCreate(AllocatorRef Allocator, Int Capacity) {
    return DictionaryCreate(
        Allocator, 
        &_CStringDictionaryKeyComparator, 
        &_CStringDictionaryKeyHasher, 
        &_CStringDictionaryKeySizeCallback,
        Capacity
    );
}

DictionaryRef IndexDictionaryCreate(
    AllocatorRef Allocator, 
    Int Capacity
) {
    return DictionaryCreate(
        Allocator, 
        &_IndexDictionaryKeyComparator, 
        &_IndexDictionaryKeyHasher, 
        &_IndexDictionaryKeySizeCallback,
        Capacity
    );
}

Void DictionaryDestroy(
    DictionaryRef Dictionary
) {
    _DictionaryBufferDeinit(Dictionary, &Dictionary->ElementBuffer);
    _DictionaryBufferDeinit(Dictionary, &Dictionary->KeyBuffer);
    AllocatorDestroy(Dictionary->BucketAllocator);
    AllocatorDeallocate(Dictionary->Allocator, Dictionary);
}

Void DictionaryInsert(
    DictionaryRef Dictionary,
    Void* Key,
    Void* Element,
    Int32 ElementSize
) {
    if (Element == NULL || ElementSize < 1) {
        DictionaryRemove(Dictionary, Key);
        return;
    }

    UInt64 Hash = Dictionary->Hasher(Key);
    Int Index = Hash % Dictionary->Capacity;
    DictionaryBucketRef Bucket = (DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * Index);
    while (Bucket && Bucket->IsFilled) {
        Void* BucketKey = _DictionaryBufferGetElement(Dictionary, &Dictionary->KeyBuffer, Bucket->KeyOffset);
        if (Bucket->Hash == Hash && Dictionary->Comparator(BucketKey, Key)) {
            // TODO: Remove old Element from Buffer and update all indices in Buckets
            Bucket->ElementOffset = _DictionaryBufferInsertElement(Dictionary, &Dictionary->ElementBuffer, Element, ElementSize);
            return;
        }

        if (Bucket->Next) {
            Bucket = Bucket->Next;
        }
        else {
            break;
        }
    }

    assert(Bucket);
    if (Bucket->IsFilled) {
        Bucket->Next = (DictionaryBucketRef)AllocatorAllocate(Dictionary->BucketAllocator, sizeof(struct _DictionaryBucket));
        Bucket = Bucket->Next;
    }

    Bucket->Hash = Hash;
    Bucket->KeyOffset = _DictionaryBufferInsertElement(Dictionary, &Dictionary->KeyBuffer, Key, Dictionary->KeySizeCallback(Key));
    Bucket->ElementOffset = _DictionaryBufferInsertElement(Dictionary, &Dictionary->ElementBuffer, Element, ElementSize);
    Bucket->IsFilled = true;
    Bucket->Next = NULL;

    Dictionary->ElementCount += 1;
}

Bool DictionaryContains(
    DictionaryRef Dictionary, 
    Void *Key
) {
    return DictionaryLookup(Dictionary, Key) != NULL;
}

Void* DictionaryLookup(
    DictionaryRef Dictionary,
    Void* Key
) {
    UInt64 Hash = Dictionary->Hasher(Key);
    Int Index = Hash % Dictionary->Capacity;
    DictionaryBucketRef Bucket = (DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * Index);
    while (Bucket && Bucket->IsFilled) {
        Void* BucketKey = _DictionaryBufferGetElement(Dictionary, &Dictionary->KeyBuffer, Bucket->KeyOffset);
        if (Bucket->Hash == Hash && Dictionary->Comparator(BucketKey, Key)) {
            return _DictionaryBufferGetElement(Dictionary, &Dictionary->ElementBuffer, Bucket->ElementOffset);
        }

        Bucket = Bucket->Next;
    }

    return NULL;
}

Void DictionaryRemove(
    DictionaryRef Dictionary,
    Void* Key
) {
    UInt64 Hash = Dictionary->Hasher(Key);
    Int Index = Hash % Dictionary->Capacity;
    DictionaryBucketRef Bucket = (DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * Index);
    DictionaryBucketRef PreviousBucket = NULL;

    while (Bucket && Bucket->IsFilled) {
        Void* BucketKey = _DictionaryBufferGetElement(Dictionary, &Dictionary->KeyBuffer, Bucket->KeyOffset);
        if (Bucket->Hash == Hash && Dictionary->Comparator(BucketKey, Key)) {
            if (!PreviousBucket) {
                if (Bucket->Next) {
                    memcpy(
                        (DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * Index),
                        Bucket->Next,
                        sizeof(struct _DictionaryBucket)
                    );
                }
                else {
                    Bucket->IsFilled = false;
                }
            }
            else {
                PreviousBucket->Next = Bucket->Next;
            }

            // TODO: Remove Key from Buffer and update all indices in Buckets

            Dictionary->ElementCount -= 1;
            return;
        }

        PreviousBucket = Bucket;
        Bucket = Bucket->Next;
    }
}

Void DictionaryRemoveAll(
    DictionaryRef Dictionary
) {
    // TODO: This is a fallback solution for now...
    _DictionaryBufferDeinit(Dictionary, &Dictionary->ElementBuffer);
    _DictionaryBufferDeinit(Dictionary, &Dictionary->KeyBuffer);
    AllocatorDestroy(Dictionary->BucketAllocator);
    Dictionary->BucketAllocator = TempAllocatorCreate(Dictionary->Allocator);
    Dictionary->ElementCount = 0;
    memset(Dictionary->Buckets, 0, sizeof(struct _DictionaryBucket) * Dictionary->Capacity);
    _DictionaryBufferInit(Dictionary, &Dictionary->KeyBuffer);
    _DictionaryBufferInit(Dictionary, &Dictionary->ElementBuffer);
}

Void DictionaryGetKeyBuffer(
    DictionaryRef Dictionary,
    Void** Memory,
    Int32* Length
) {
    *Memory = Dictionary->KeyBuffer.Memory;
    *Length = Dictionary->KeyBuffer.Offset;
}

Void DictionaryGetValueBuffer(
    DictionaryRef Dictionary,
    Void** Memory,
    Int32* Length
) {
    *Memory = Dictionary->ElementBuffer.Memory;
    *Length = Dictionary->ElementBuffer.Offset;
}

DictionaryKeyIterator DictionaryGetKeyIterator(
    DictionaryRef Dictionary
) {
    DictionaryKeyIterator Iterator = { 0 };
    Iterator.Dictionary = Dictionary;
    Iterator.Bucket = NULL;
    Iterator.BucketIndex = 0;
    Iterator.Key = NULL;
    Iterator.Value = NULL;

    for (Int Index = 0; Index < Dictionary->Capacity; Index += 1) {
        DictionaryBucketRef Bucket = (DictionaryBucketRef)(((UInt8*)Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * Index);
        while (Bucket && !Bucket->IsFilled) {
            Bucket = Bucket->Next;
        }

        if (Bucket && Bucket->IsFilled) {
            Iterator.Bucket = Bucket;
            Iterator.BucketIndex = Index;
            Iterator.Key = _DictionaryBufferGetElement(Dictionary, &Dictionary->KeyBuffer, Bucket->KeyOffset);
            Iterator.Value = _DictionaryBufferGetElement(Dictionary, &Dictionary->ElementBuffer, Bucket->ElementOffset);
            return Iterator;
        }
    }

    return Iterator;
}

DictionaryKeyIterator DictionaryKeyIteratorNext(
    DictionaryKeyIterator Iterator
) {
    if (Iterator.Bucket) {
        Iterator.Bucket = Iterator.Bucket->Next;
        while (Iterator.Bucket && !Iterator.Bucket->IsFilled) {
            Iterator.Bucket = Iterator.Bucket->Next;
        }
    }

    if (Iterator.Bucket) {
        Iterator.Key = _DictionaryBufferGetElement(
            Iterator.Dictionary,
            &Iterator.Dictionary->KeyBuffer,
            Iterator.Bucket->KeyOffset
        );

        return Iterator;
    }

    for (Int Index = Iterator.BucketIndex + 1; Index < Iterator.Dictionary->Capacity; Index += 1) {
        DictionaryBucketRef Bucket = (DictionaryBucketRef)(((UInt8*)Iterator.Dictionary->Buckets) + sizeof(struct _DictionaryBucket) * Index);
        while (Bucket && !Bucket->IsFilled) {
            Bucket = Bucket->Next;
        }

        if (Bucket && Bucket->IsFilled) {
            Iterator.Bucket = Bucket;
            Iterator.BucketIndex = Index;
            Iterator.Key = _DictionaryBufferGetElement(
                Iterator.Dictionary,
                &Iterator.Dictionary->KeyBuffer,
                Iterator.Bucket->KeyOffset
            );
            return Iterator;
        }
    }

    Iterator.Key = NULL;
    return Iterator;
}
