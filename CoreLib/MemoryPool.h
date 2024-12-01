#pragma once

#include "Base.h"

#include "Allocator.h"

EXTERN_C_BEGIN

typedef struct _MemoryPool *MemoryPoolRef;

MemoryPoolRef MemoryPoolCreate(
    AllocatorRef Allocator,
    Int BlockSize,
    Int BlockCount
);

Void MemoryPoolDestroy(
    MemoryPoolRef MemoryPool
);

Int MemoryPoolGetBlockSize(
    MemoryPoolRef MemoryPool
);

Int MemoryPoolGetBlockCount(
    MemoryPoolRef MemoryPool
);

Int MemoryPoolGetReservedBlockCount(
    MemoryPoolRef MemoryPool
);

Bool MemoryPoolIsFull(
    MemoryPoolRef MemoryPool
);

Bool MemoryPoolIsReserved(
    MemoryPoolRef MemoryPool,
    Int BlockIndex
);

Void MemoryPoolClear(
    MemoryPoolRef MemoryPool
);

Void *MemoryPoolReserve(
    MemoryPoolRef MemoryPool,
    Int BlockIndex
);

Void *MemoryPoolReserveNext(
    MemoryPoolRef MemoryPool,
    Int *OutBlockIndex
);

Void *MemoryPoolFetch(
    MemoryPoolRef MemoryPool,
    Int BlockIndex
);

Void MemoryPoolRelease(
    MemoryPoolRef MemoryPool,
    Int BlockIndex
);

EXTERN_C_END
