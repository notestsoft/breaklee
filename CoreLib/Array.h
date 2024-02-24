#pragma once

#include "Base.h"

#include "Allocator.h"

EXTERN_C_BEGIN

typedef struct _Array* ArrayRef;

typedef Bool (*ArrayPredicate)(
    MemoryRef Lhs,
    MemoryRef Rhs
);

ArrayRef ArrayCreate(
    AllocatorRef Allocator,
    Index Size,
    MemoryRef Memory, 
    Index Count
);

ArrayRef ArrayCreateEmpty(
    AllocatorRef Allocator,
    Index Size,
    Index Capacity
);

Void ArrayDestroy(
    ArrayRef Array
);

Index ArrayGetElementSize(
    ArrayRef Array
);

Index ArrayGetElementCount(
    ArrayRef Array
);

MemoryRef ArrayGetElementAtIndex(
    ArrayRef Array, 
    Index Index
);

Void ArrayCopyElementAtIndex(
    ArrayRef Array, 
    Index Index, 
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
    Index Count
);

MemoryRef ArrayAppendUninitializedMemory(
    ArrayRef Array, 
    Index Count
);

Void ArrayInsertElementAtIndex(
    ArrayRef Array, 
    Index Index, 
    MemoryRef Memory
);

Void ArraySetElementAtIndex(
    ArrayRef Array, 
    Index Index, 
    MemoryRef Memory
);

Void ArrayRemoveElement(
    ArrayRef Array,
    Void* Element
);

Void ArrayRemoveElementAtIndex(
    ArrayRef Array, 
    Index Index
);

Bool ArrayContainsElement(
    ArrayRef Array,
    Void* Element
);

Index ArrayGetElementIndexInMemoryBounds(
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
    Index *OutIndex
);

EXTERN_C_END
