#include "Diagnostic.h"
#include "Dictionary.h"
#include "MemoryPool.h"
#include "IndexMemoryPool.h"

struct _IndexMemoryPool {
    AllocatorRef Allocator;
    MemoryPoolRef MemoryPool;
    DictionaryRef IndexToMemoryPoolIndex;
};

IndexMemoryPoolRef IndexMemoryPoolCreate(
    AllocatorRef Allocator,
    Int BlockSize,
    Int BlockCount
) {
    IndexMemoryPoolRef IndexMemoryPool = (IndexMemoryPoolRef)AllocatorAllocate(Allocator, sizeof(struct _IndexMemoryPool));
    if (!IndexMemoryPool) Fatal("Memory allocation failed!");

    IndexMemoryPool->Allocator = Allocator;
    IndexMemoryPool->MemoryPool = MemoryPoolCreate(Allocator, BlockSize, BlockCount);
    IndexMemoryPool->IndexToMemoryPoolIndex = IndexDictionaryCreate(Allocator, BlockCount);
    return IndexMemoryPool;
}

Void IndexMemoryPoolDestroy(
    IndexMemoryPoolRef IndexMemoryPool
) {
    MemoryPoolDestroy(IndexMemoryPool->MemoryPool);
    DictionaryDestroy(IndexMemoryPool->IndexToMemoryPoolIndex);
    AllocatorDeallocate(IndexMemoryPool->Allocator, IndexMemoryPool);
}

Int IndexMemoryPoolGetBlockSize(
    IndexMemoryPoolRef IndexMemoryPool
) {
    return MemoryPoolGetBlockSize(IndexMemoryPool->MemoryPool);
}

Int IndexMemoryPoolGetBlockCount(
    IndexMemoryPoolRef IndexMemoryPool
) {
    return MemoryPoolGetBlockCount(IndexMemoryPool->MemoryPool);
}

Bool IndexMemoryPoolIsFull(
    IndexMemoryPoolRef IndexMemoryPool
) {
    return MemoryPoolIsFull(IndexMemoryPool->MemoryPool);
}

Bool IndexMemoryPoolIsReserved(
    IndexMemoryPoolRef IndexMemoryPool,
    Int KeyIndex
) {
    return DictionaryContains(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex);
}

Void *IndexMemoryPoolReserve(
    IndexMemoryPoolRef IndexMemoryPool,
    Int KeyIndex
) {
    assert(!IndexMemoryPoolIsFull(IndexMemoryPool));
    assert(!IndexMemoryPoolIsReserved(IndexMemoryPool, KeyIndex));
    
    Int MemoryPoolIndex = 0;
    Void *Memory = MemoryPoolReserveNext(IndexMemoryPool->MemoryPool, &MemoryPoolIndex);
    DictionaryInsert(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex, &MemoryPoolIndex, sizeof(Int));
    return Memory;
}

Void *IndexMemoryPoolFetch(
    IndexMemoryPoolRef IndexMemoryPool,
    Int KeyIndex
) {
    assert(IndexMemoryPoolIsReserved(IndexMemoryPool, KeyIndex));
    Int MemoryPoolIndex = *(Int *)DictionaryLookup(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex);
    return MemoryPoolFetch(IndexMemoryPool->MemoryPool, MemoryPoolIndex);
}

Void IndexMemoryPoolRelease(
    IndexMemoryPoolRef IndexMemoryPool,
    Int KeyIndex
) {
    assert(IndexMemoryPoolIsReserved(IndexMemoryPool, KeyIndex));
    Int MemoryPoolIndex = *(Int *)DictionaryLookup(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex);
    MemoryPoolRelease(IndexMemoryPool->MemoryPool, MemoryPoolIndex);
    DictionaryRemove(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex);
}
