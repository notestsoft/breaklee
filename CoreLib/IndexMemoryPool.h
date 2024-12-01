#pragma once

#include "Base.h"

#include "Allocator.h"

EXTERN_C_BEGIN

typedef struct _IndexMemoryPool *IndexMemoryPoolRef;

IndexMemoryPoolRef IndexMemoryPoolCreate(
    AllocatorRef Allocator,
    Int BlockSize,
    Int BlockCount
);

Void IndexMemoryPoolDestroy(
    IndexMemoryPoolRef IndexMemoryPool
);

Int IndexMemoryPoolGetBlockSize(
    IndexMemoryPoolRef IndexMemoryPool
);

Int IndexMemoryPoolGetBlockCount(
    IndexMemoryPoolRef IndexMemoryPool
);

Bool IndexMemoryPoolIsFull(
    IndexMemoryPoolRef IndexMemoryPool
);

Bool IndexMemoryPoolIsReserved(
    IndexMemoryPoolRef IndexMemoryPool,
    Int KeyIndex
);

Void *IndexMemoryPoolReserve(
    IndexMemoryPoolRef IndexMemoryPool,
    Int KeyIndex
);

Void *IndexMemoryPoolFetch(
    IndexMemoryPoolRef IndexMemoryPool,
    Int KeyIndex
);

Void IndexMemoryPoolRelease(
    IndexMemoryPoolRef IndexMemoryPool,
    Int KeyIndex
);

EXTERN_C_END
