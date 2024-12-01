#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef enum {
    AllocatorModeAllocate,
    AllocatorModeReallocate,
    AllocatorModeDeallocate,
    AllocatorModeDestroy,
} AllocatorMode;

typedef void *(*AllocatorCallback)(
    AllocatorMode Mode, 
    Int Capacity, 
    MemoryRef Memory, 
    MemoryRef Context
);

typedef struct _Allocator *AllocatorRef;

AllocatorRef AllocatorGetDefault();
AllocatorRef AllocatorGetSystemDefault();
AllocatorRef AllocatorGetMalloc();
AllocatorRef AllocatorGetNull();

Void AllocatorSetCurrentDefault(
    AllocatorRef Allocator
);

AllocatorRef AllocatorGetCurrentDefault();

AllocatorRef AllocatorCreate(
    AllocatorRef Allocator, 
    AllocatorCallback Callback, 
    MemoryRef Context
);

Void AllocatorDestroy(
    AllocatorRef Allocator
);

MemoryRef AllocatorAllocate(
    AllocatorRef Allocator, 
    Int Capacity
);

MemoryRef AllocatorReallocate(
    AllocatorRef Allocator, 
    MemoryRef Memory,
    Int Capacity
);

MemoryRef AllocatorDeallocate(
    AllocatorRef Allocator, 
    MemoryRef Memory
);

EXTERN_C_END
