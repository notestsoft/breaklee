#pragma once

#include "Base.h"

#include "Allocator.h"

EXTERN_C_BEGIN

typedef struct _IndexMemoryPool *IndexMemoryPoolRef;

IndexMemoryPoolRef IndexMemoryPoolCreate(
    AllocatorRef Allocator,
    Index BlockSize,
    Index BlockCount
);

Void IndexMemoryPoolDestroy(
    IndexMemoryPoolRef IndexMemoryPool
);

Index IndexMemoryPoolGetBlockSize(
    IndexMemoryPoolRef IndexMemoryPool
);

Index IndexMemoryPoolGetBlockCount(
    IndexMemoryPoolRef IndexMemoryPool
);

Bool IndexMemoryPoolIsFull(
    IndexMemoryPoolRef IndexMemoryPool
);

Bool IndexMemoryPoolIsReserved(
    IndexMemoryPoolRef IndexMemoryPool,
    Index KeyIndex
);

Void *IndexMemoryPoolReserve(
    IndexMemoryPoolRef IndexMemoryPool,
    Index KeyIndex
);

Void *IndexMemoryPoolFetch(
    IndexMemoryPoolRef IndexMemoryPool,
    Index KeyIndex
);

Void IndexMemoryPoolRelease(
    IndexMemoryPoolRef IndexMemoryPool,
    Index KeyIndex
);

EXTERN_C_END
