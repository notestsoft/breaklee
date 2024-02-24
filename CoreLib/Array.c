#include "Array.h"
#include "Diagnostic.h"

#define ARRAY_MIN_CAPACITY 8

struct _Array {
    AllocatorRef Allocator;
    Index Size;
    Index Count;
    Index Capacity;
    UInt8* Memory;
};

Void _ArrayReserveCapacity(
    ArrayRef Array,
    Index NewCapacity
) {
    Index Capacity = MAX(Array->Capacity, ARRAY_MIN_CAPACITY);
    while (Capacity < NewCapacity) {
        Capacity <<= 1;
    }

    if (Array->Capacity < Capacity) {
        MemoryRef Memory = AllocatorReallocate(Array->Allocator, Array->Memory, Array->Size * Capacity);
        if (!Memory) FatalError("Memory allocation failed!");

        Array->Capacity = Capacity;
        Array->Memory = Memory;
    }
}

ArrayRef ArrayCreate(
    AllocatorRef Allocator,
    Index Size,
    MemoryRef Memory, 
    Index Count
) {
    ArrayRef Array = ArrayCreateEmpty(Allocator, Size, Count);
    memcpy(Array->Memory, Memory, Size * Count);
    return Array;
}

ArrayRef ArrayCreateEmpty(
    AllocatorRef Allocator,
    Index Size,
    Index Capacity
) {
    ArrayRef Array = AllocatorAllocate(Allocator, sizeof(struct _Array));
    if (!Array) FatalError("Memory allocation failed!");
    
    Array->Allocator = Allocator;
    Array->Size = Size;
    Array->Count = 0;
    Array->Capacity = MAX(ARRAY_MIN_CAPACITY, Capacity);
    Array->Memory = (UInt8 *)AllocatorAllocate(Allocator, Array->Capacity * Array->Size);
    if (!Array->Memory) FatalError("Memory allocation failed!");
    
    return Array;
}

Void ArrayDestroy(
    ArrayRef Array
) {
    AllocatorDeallocate(Array->Allocator, Array->Memory);
    AllocatorDeallocate(Array->Allocator, Array);
}

Index ArrayGetElementSize(
    ArrayRef Array
) {
    return Array->Size;
}

Index ArrayGetElementCount(
    ArrayRef Array
) {
    return Array->Count;
}

MemoryRef ArrayGetElementAtIndex(
    ArrayRef Array, 
    Index Index
) {
    assert(Index < Array->Count);
    return Array->Memory + Array->Size * Index;
}

Void ArrayCopyElementAtIndex(
    ArrayRef Array, 
    Index Index, 
    MemoryRef Memory
) {
    assert(Index < Array->Count);
    memcpy(Memory, ArrayGetElementAtIndex(Array, Index), Array->Size);
}

Void ArrayAppendElement(
    ArrayRef Array, 
    MemoryRef Memory
) {
    _ArrayReserveCapacity(Array, Array->Count + 1);
    memcpy(Array->Memory + Array->Size * Array->Count, Memory, Array->Size);
    Array->Count += 1;
}

MemoryRef ArrayAppendUninitializedElement(
    ArrayRef Array
) {
    _ArrayReserveCapacity(Array, Array->Count + 1);
    Array->Count += 1;
    return ArrayGetElementAtIndex(Array, Array->Count - 1);
}

Void ArrayAppendArray(
    ArrayRef Array, 
    ArrayRef Other
) {
    assert(Array->Size == Other->Size);

    if (Other->Count < 1) return;

    _ArrayReserveCapacity(Array, Array->Count + Other->Count);
    memcpy(Array->Memory + Array->Size * Array->Count, Other->Memory, Array->Size * Other->Count);
    Array->Count += Other->Count;
}

Void ArrayAppendMemory(
    ArrayRef Array, 
    MemoryRef Memory, 
    Index Count
) {
    assert(Count % Array->Size == 0);

    _ArrayReserveCapacity(Array, Array->Count + Count);

    memcpy(Array->Memory + Array->Size * Array->Count, Memory, Count);
    Array->Count += Count / Array->Size;
}

MemoryRef ArrayAppendUninitializedMemory(
    ArrayRef Array, 
    Index Count
) {
    assert(Count % Array->Size == 0);

    _ArrayReserveCapacity(Array, Array->Count + Count);

    Int64 Index = Array->Count;
    Array->Count += Count / Array->Size;
    return ArrayGetElementAtIndex(Array, Index);
}

Void ArrayInsertElementAtIndex(
    ArrayRef Array, 
    Index ElementIndex,
    MemoryRef Memory
) {
    if (ElementIndex == Array->Count) {
        ArrayAppendElement(Array, Memory);
        return;
    }

    assert(ElementIndex < Array->Count);
    _ArrayReserveCapacity(Array, Array->Count + 1);

    Index TailLength = Array->Count - ElementIndex;
    if (TailLength > 0) {
        UInt8* Source = Array->Memory + Array->Size * ElementIndex;
        UInt8* Destination = Array->Memory + Array->Size * (ElementIndex + 1);
        memmove(Destination, Source, Array->Size * TailLength);
    }

    memcpy(Array->Memory + Array->Size * ElementIndex, Memory, Array->Size);
    Array->Count += 1;
}

Void ArraySetElementAtIndex(
    ArrayRef Array, 
    Index Index, 
    MemoryRef Memory
) {
    assert(Index < Array->Count);
    memcpy(Array->Memory + Array->Size * Index, Memory, Array->Size);
}

Void ArrayRemoveElement(
    ArrayRef Array,
    Void* Element
) {
    for (Index Index = 0; Index < Array->Count; Index += 1) {
        Void* Other = ArrayGetElementAtIndex(Array, Index);
        if (!memcmp(Element, Other, Array->Size)) {
            ArrayRemoveElementAtIndex(Array, Index);
            break;
        }
    }
}

Void ArrayRemoveElementAtIndex(
    ArrayRef Array, 
    Index ElementIndex
) {
    assert(ElementIndex < Array->Count);

    Index TailLength = Array->Count - ElementIndex - 1;
    if (TailLength > 0) {
        UInt8* Source = ArrayGetElementAtIndex(Array, ElementIndex + 1);
        UInt8* Destination = ArrayGetElementAtIndex(Array, ElementIndex);
        memmove(Destination, Source, Array->Size * TailLength);
    }

    Array->Count -= 1;
}

Bool ArrayContainsElement(
    ArrayRef Array,
    Void* Element
) {
    for (Index Index = 0; Index < Array->Count; Index += 1) {
        Void* Other = ArrayGetElementAtIndex(Array, Index);
        if (!memcmp(Element, Other, Array->Size)) {
            return true;
        }
    }

    return false;
}

Index ArrayGetElementIndexInMemoryBounds(
    ArrayRef Array,
    MemoryRef Offset
) {
    assert(Array->Memory <= (UInt8*)Offset);
    assert((UInt8*)Offset < Array->Memory + Array->Size * Array->Count);
    return (Int64)((UInt8*)Offset - (UInt8*)Array->Memory) / Array->Size;
}

Void ArrayRemoveElementInMemoryBounds(
    ArrayRef Array, 
    MemoryRef Offset
) {
    Int64 Index = ArrayGetElementIndexInMemoryBounds(Array, Offset);
    ArrayRemoveElementAtIndex(Array, Index);
}

Void ArrayRemoveAllElements(
    ArrayRef Array, 
    Bool KeepCapacity
) {
    Array->Count = 0;

    if (!KeepCapacity) {
        Index Capacity = MIN(ARRAY_MIN_CAPACITY, Array->Capacity);
        UInt8* Memory = realloc(Array->Memory, Capacity);
        if (!Memory) FatalError("Memory allocation failed!");
        Array->Capacity = Capacity;
        Array->Memory = Memory;
    }
}

Bool ArrayGetIndexOfElement(
    ArrayRef Array,
    ArrayPredicate Predicate, 
    MemoryRef Element,
    Index *OutIndex
) {
    for (Int64 Index = 0; Index < Array->Count; Index += 1) {
        MemoryRef Lhs = ArrayGetElementAtIndex(Array, Index);
        if (Predicate(Lhs, Element)) {
            *OutIndex = Index;
            return true;
        }
    }

    return false;
}
