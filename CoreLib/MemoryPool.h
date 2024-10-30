#pragma once

#include "Base.h"

#include "Allocator.h"

EXTERN_C_BEGIN

typedef struct _MemoryPool *MemoryPoolRef;

MemoryPoolRef MemoryPoolCreate(
    AllocatorRef Allocator,
    Index BlockSize,
    Index BlockCount
);

Void MemoryPoolDestroy(
    MemoryPoolRef MemoryPool
);

Index MemoryPoolGetBlockSize(
    MemoryPoolRef MemoryPool
);

Index MemoryPoolGetBlockCount(
    MemoryPoolRef MemoryPool
);

Index MemoryPoolGetReservedBlockCount(
    MemoryPoolRef MemoryPool
);

Bool MemoryPoolIsFull(
    MemoryPoolRef MemoryPool
);

Bool MemoryPoolIsReserved(
    MemoryPoolRef MemoryPool,
    Index BlockIndex
);

Void MemoryPoolClear(
    MemoryPoolRef MemoryPool
);

Void *MemoryPoolReserve(
    MemoryPoolRef MemoryPool,
    Index BlockIndex
);

Void *MemoryPoolReserveNext(
    MemoryPoolRef MemoryPool,
    Index *OutBlockIndex
);

Void *MemoryPoolFetch(
    MemoryPoolRef MemoryPool,
    Index BlockIndex
);

Void MemoryPoolRelease(
    MemoryPoolRef MemoryPool,
    Index BlockIndex
);

EXTERN_C_END
