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
    Index BlockSize,
    Index BlockCount
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

Index IndexMemoryPoolGetBlockSize(
    IndexMemoryPoolRef IndexMemoryPool
) {
    return MemoryPoolGetBlockSize(IndexMemoryPool->MemoryPool);
}

Index IndexMemoryPoolGetBlockCount(
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
    Index KeyIndex
) {
    return DictionaryContains(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex);
}

Void *IndexMemoryPoolReserve(
    IndexMemoryPoolRef IndexMemoryPool,
    Index KeyIndex
) {
    assert(!IndexMemoryPoolIsFull(IndexMemoryPool));
    assert(!IndexMemoryPoolIsReserved(IndexMemoryPool, KeyIndex));
    

    Index MemoryPoolIndex = 0;
    Void *Memory = MemoryPoolReserveNext(IndexMemoryPool->MemoryPool, &MemoryPoolIndex);
    DictionaryInsert(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex, &MemoryPoolIndex, sizeof(Index));
    return Memory;
}

Void *IndexMemoryPoolFetch(
    IndexMemoryPoolRef IndexMemoryPool,
    Index KeyIndex
) {
    assert(IndexMemoryPoolIsReserved(IndexMemoryPool, KeyIndex));
    Index MemoryPoolIndex = *(Index *)DictionaryLookup(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex);
    return MemoryPoolFetch(IndexMemoryPool->MemoryPool, MemoryPoolIndex);
}

Void IndexMemoryPoolRelease(
    IndexMemoryPoolRef IndexMemoryPool,
    Index KeyIndex
) {
    assert(IndexMemoryPoolIsReserved(IndexMemoryPool, KeyIndex));
    Index MemoryPoolIndex = *(Index *)DictionaryLookup(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex);
    MemoryPoolRelease(IndexMemoryPool->MemoryPool, MemoryPoolIndex);
    DictionaryRemove(IndexMemoryPool->IndexToMemoryPoolIndex, &KeyIndex);
}
