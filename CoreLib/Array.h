#pragma once

#include "Base.h"

#include "Allocator.h"

EXTERN_C_BEGIN

struct _Array {
    AllocatorRef Allocator;
    Int Size;
    Int Count;
    Int Capacity;
    UInt8* Memory;
};

typedef struct _Array* ArrayRef;

typedef Bool (*ArrayPredicate)(
    MemoryRef Lhs,
    MemoryRef Rhs
);

ArrayRef ArrayCreate(
    AllocatorRef Allocator,
    Int Size,
    MemoryRef Memory, 
    Int Count
);

ArrayRef ArrayCreateEmpty(
    AllocatorRef Allocator,
    Int Size,
    Int Capacity
);

Void ArrayInitializeEmpty(
    ArrayRef Array,
    AllocatorRef Allocator,
    Int Size,
    Int Capacity
);

Void ArrayDealloc(
    ArrayRef Array
);

Void ArrayDestroy(
    ArrayRef Array
);

Int ArrayGetElementSize(
    ArrayRef Array
);

Int ArrayGetElementCount(
    ArrayRef Array
);

MemoryRef ArrayGetElementAtIndex(
    ArrayRef Array, 
    Int Index
);

Void ArrayCopyElementAtIndex(
    ArrayRef Array, 
    Int Index, 
    MemoryRef Memory
);

Void ArrayAppendElement(
    ArrayRef Array, 
    MemoryRef Memory
);

MemoryRef ArrayAppendUninitializedElement(
    ArrayRef Array
);

Void ArrayAppendArray(
    ArrayRef Array, 
    ArrayRef Other
);

Void ArrayAppendMemory(
    ArrayRef Array, 
    MemoryRef Memory, 
    Int Count
);

MemoryRef ArrayAppendUninitializedMemory(
    ArrayRef Array, 
    Int Count
);

Void ArrayInsertElementAtIndex(
    ArrayRef Array, 
    Int Index,
    MemoryRef Memory
);

Void ArraySetElementAtIndex(
    ArrayRef Array, 
    Int Index,
    MemoryRef Memory
);

Void ArrayRemoveElement(
    ArrayRef Array,
    Void* Element
);

Void ArrayRemoveElementAtIndex(
    ArrayRef Array, 
    Int Index
);

Bool ArrayContainsElement(
    ArrayRef Array,
    Void* Element
);

Int32 ArrayGetElementIndexInMemoryBounds(
    ArrayRef Array,
    MemoryRef Offset
);

Void ArrayRemoveElementInMemoryBounds(
    ArrayRef Array, 
    MemoryRef Offset
);

Void ArrayRemoveAllElements(
    ArrayRef Array, 
    Bool KeepCapacity
);

Bool ArrayGetIndexOfElement(
    ArrayRef Array,
    ArrayPredicate Predicate, 
    MemoryRef Element,
    Int *OutIndex
);

EXTERN_C_END
