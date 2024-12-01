#include "Diagnostic.h"
#include "MemoryPool.h"
#include "Util.h"

struct _MemoryPool {
    AllocatorRef Allocator;
    Int BlockSize;
    Int BlockSizeAligned;
    Int BlockCount;
    Int ReservedBlockCount;
    Bool *BlockFlags;
    Void *BlockMemory;
};

MemoryPoolRef MemoryPoolCreate(
    AllocatorRef Allocator,
    Int BlockSize,
    Int BlockCount
) {
    Int MemoryPoolSize = NextPowerOfTwo(sizeof(struct _MemoryPool));
    Int BlockFlagsSize = NextPowerOfTwo(BlockCount * sizeof(Bool));
    Int BlockSizeAligned = NextPowerOfTwo(BlockSize);
    Int BlockMemorySize = BlockCount * BlockSizeAligned;
    Int TotalSize = MemoryPoolSize + BlockFlagsSize + BlockMemorySize;
    MemoryPoolRef MemoryPool = (MemoryPoolRef)AllocatorAllocate(Allocator, TotalSize);
    if (!MemoryPool) Fatal("MemoryPool allocation failed!");
    
    MemoryPool->Allocator = Allocator;
    MemoryPool->BlockSize = BlockSize;
    MemoryPool->BlockSizeAligned = BlockSizeAligned;
    MemoryPool->BlockCount = BlockCount;
    MemoryPool->ReservedBlockCount = 0;
    MemoryPool->BlockFlags = (Bool *)((UInt8 *)MemoryPool + MemoryPoolSize);
    MemoryPool->BlockMemory = (Void *)((UInt8 *)MemoryPool + MemoryPoolSize + BlockFlagsSize);
    memset(MemoryPool->BlockFlags, 0, BlockFlagsSize);
    return MemoryPool;
}

Void MemoryPoolDestroy(
    MemoryPoolRef MemoryPool
) {
    AllocatorDeallocate(MemoryPool->Allocator, MemoryPool);
}

Int MemoryPoolGetBlockSize(
    MemoryPoolRef MemoryPool
) {
    assert(MemoryPool);
    return MemoryPool->BlockSize;
}

Int MemoryPoolGetBlockCount(
    MemoryPoolRef MemoryPool
) {
    assert(MemoryPool);
    return MemoryPool->BlockCount;
}

Int MemoryPoolGetReservedBlockCount(
    MemoryPoolRef MemoryPool
) {
    return MemoryPool->ReservedBlockCount;
}

Bool MemoryPoolIsFull(
    MemoryPoolRef MemoryPool
) {
    return MemoryPool->ReservedBlockCount >= MemoryPool->BlockCount;
}

Bool MemoryPoolIsReserved(
    MemoryPoolRef MemoryPool,
    Int BlockIndex
) {
    assert(MemoryPool);
    assert(BlockIndex < MemoryPool->BlockCount);
    return MemoryPool->BlockFlags[BlockIndex];
}

Void MemoryPoolClear(
    MemoryPoolRef MemoryPool
) {
    MemoryPool->ReservedBlockCount = 0;
    Int BlockFlagsSize = NextPowerOfTwo(MemoryPool->BlockCount * sizeof(Bool));
    memset(MemoryPool->BlockFlags, 0, BlockFlagsSize);
}

Void *MemoryPoolReserve(
    MemoryPoolRef MemoryPool,
    Int BlockIndex
) {
    assert(MemoryPool);
    assert(BlockIndex < MemoryPool->BlockCount);
    assert(!MemoryPool->BlockFlags[BlockIndex]);
    MemoryPool->BlockFlags[BlockIndex] = true;
    MemoryPool->ReservedBlockCount += 1;
    Void *MemoryBlock = (Void *)((UInt8 *)MemoryPool->BlockMemory + (BlockIndex * MemoryPool->BlockSizeAligned));
    memset(MemoryBlock, 0, MemoryPool->BlockSizeAligned);
    return MemoryBlock;
}

Void *MemoryPoolReserveNext(
    MemoryPoolRef MemoryPool,
    Int *OutBlockIndex
) {
    assert(MemoryPool);
    
    for (Int BlockIndex = 0; BlockIndex < MemoryPool->BlockCount; BlockIndex += 1) {
        if (!MemoryPool->BlockFlags[BlockIndex]) {
            *OutBlockIndex = BlockIndex;
            return MemoryPoolReserve(MemoryPool, BlockIndex);
        }
    }

    *OutBlockIndex = 0;
    return NULL;
}

Void *MemoryPoolFetch(
    MemoryPoolRef MemoryPool,
    Int BlockIndex
) {
    assert(MemoryPool);
    assert(BlockIndex < MemoryPool->BlockCount);
    if (!MemoryPool->BlockFlags[BlockIndex]) return NULL;
    return (Void *)((UInt8 *)MemoryPool->BlockMemory + (BlockIndex * MemoryPool->BlockSizeAligned));
}

Void MemoryPoolRelease(
    MemoryPoolRef MemoryPool,
    Int BlockIndex
) {
    assert(MemoryPool);
    assert(BlockIndex < MemoryPool->BlockCount);
    assert(MemoryPool->BlockFlags[BlockIndex]);
    MemoryPool->BlockFlags[BlockIndex] = false;
    MemoryPool->ReservedBlockCount -= 1;
}
