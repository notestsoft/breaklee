#include "Array.h"
#include "Diagnostic.h"

#define ARRAY_MIN_CAPACITY 8

Void _ArrayReserveCapacity(
    ArrayRef Array,
    Int NewCapacity
) {
    Int Capacity = MAX(Array->Capacity, ARRAY_MIN_CAPACITY);
    while (Capacity < NewCapacity) {
        Capacity <<= 1;
    }

    if (Array->Capacity < Capacity) {
        MemoryRef Memory = AllocatorReallocate(Array->Allocator, Array->Memory, Array->Size * Capacity);
        if (!Memory) Fatal("Memory allocation failed!");

        Array->Capacity = Capacity;
        Array->Memory = Memory;
    }
}

ArrayRef ArrayCreate(
    AllocatorRef Allocator,
    Int Size,
    MemoryRef Memory, 
    Int Count
) {
    ArrayRef Array = ArrayCreateEmpty(Allocator, Size, Count);
    memcpy(Array->Memory, Memory, (size_t)Size * Count);
    return Array;
}

ArrayRef ArrayCreateEmpty(
    AllocatorRef Allocator,
    Int Size,
    Int Capacity
) {
    ArrayRef Array = AllocatorAllocate(Allocator, sizeof(struct _Array));
    if (!Array) Fatal("Memory allocation failed!");
    ArrayInitializeEmpty(Array, Allocator, Size, Capacity);    
    return Array;
}

Void ArrayInitializeEmpty(
    ArrayRef Array,
    AllocatorRef Allocator,
    Int Size,
    Int Capacity
) {
    Array->Allocator = Allocator;
    Array->Size = Size;
    Array->Count = 0;
    Array->Capacity = MAX(ARRAY_MIN_CAPACITY, Capacity);
    Array->Memory = (UInt8*)AllocatorAllocate(Allocator, Array->Capacity * Array->Size);
    if (!Array->Memory) Fatal("Memory allocation failed!");
}

Void ArrayDealloc(
    ArrayRef Array
) {
    AllocatorDeallocate(Array->Allocator, Array->Memory);
}

Void ArrayDestroy(
    ArrayRef Array
) {
    ArrayDealloc(Array);
    AllocatorDeallocate(Array->Allocator, Array);
}

Int ArrayGetElementSize(
    ArrayRef Array
) {
    return Array->Size;
}

Int ArrayGetElementCount(
    ArrayRef Array
) {
    return Array->Count;
}

MemoryRef ArrayGetElementAtIndex(
    ArrayRef Array, 
    Int Index
) {
    assert(Index < Array->Count);
    return Array->Memory + Array->Size * Index;
}

Void ArrayCopyElementAtIndex(
    ArrayRef Array, 
    Int Index,
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
    Int Count
) {
    assert(Count % Array->Size == 0);

    _ArrayReserveCapacity(Array, Array->Count + Count);

    memcpy(Array->Memory + Array->Size * Array->Count, Memory, Count);
    Array->Count += Count / Array->Size;
}

MemoryRef ArrayAppendUninitializedMemory(
    ArrayRef Array, 
    Int Count
) {
    assert(Count % Array->Size == 0);

    _ArrayReserveCapacity(Array, Array->Count + Count);

    Int Index = Array->Count;
    Array->Count += Count / Array->Size;
    return ArrayGetElementAtIndex(Array, Index);
}

Void ArrayInsertElementAtIndex(
    ArrayRef Array, 
    Int ElementIndex,
    MemoryRef Memory
) {
    if (ElementIndex == Array->Count) {
        ArrayAppendElement(Array, Memory);
        return;
    }

    assert(ElementIndex < Array->Count);
    _ArrayReserveCapacity(Array, Array->Count + 1);

    Int32 TailLength = Array->Count - ElementIndex;
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
    Int Index, 
    MemoryRef Memory
) {
    assert(Index < Array->Count);
    memcpy(Array->Memory + Array->Size * Index, Memory, Array->Size);
}

Void ArrayRemoveElement(
    ArrayRef Array,
    Void* Element
) {
    for (Int Index = 0; Index < Array->Count; Index += 1) {
        Void* Other = ArrayGetElementAtIndex(Array, Index);
        if (!memcmp(Element, Other, Array->Size)) {
            ArrayRemoveElementAtIndex(Array, Index);
            break;
        }
    }
}

Void ArrayRemoveElementAtIndex(
    ArrayRef Array, 
    Int ElementIndex
) {
    assert(ElementIndex < Array->Count);

    Int TailLength = Array->Count - ElementIndex - 1;
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
    for (Int Index = 0; Index < Array->Count; Index += 1) {
        Void* Other = ArrayGetElementAtIndex(Array, Index);
        if (!memcmp(Element, Other, Array->Size)) {
            return true;
        }
    }

    return false;
}

Int32 ArrayGetElementIndexInMemoryBounds(
    ArrayRef Array,
    MemoryRef Offset
) {
    assert(Array->Memory <= (UInt8*)Offset);
    assert((UInt8*)Offset < Array->Memory + Array->Size * Array->Count);
    return (Int)((UInt8*)Offset - (UInt8*)Array->Memory) / Array->Size;
}

Void ArrayRemoveElementInMemoryBounds(
    ArrayRef Array, 
    MemoryRef Offset
) {
    Int Index = ArrayGetElementIndexInMemoryBounds(Array, Offset);
    ArrayRemoveElementAtIndex(Array, Index);
}

Void ArrayRemoveAllElements(
    ArrayRef Array, 
    Bool KeepCapacity
) {
    Array->Count = 0;

    if (!KeepCapacity) {
        Int Capacity = MIN(ARRAY_MIN_CAPACITY, Array->Capacity);
        UInt8* Memory = realloc(Array->Memory, Capacity);
        if (!Memory) Fatal("Memory allocation failed!");
        Array->Capacity = Capacity;
        Array->Memory = Memory;
    }
}

Bool ArrayGetIndexOfElement(
    ArrayRef Array,
    ArrayPredicate Predicate, 
    MemoryRef Element,
    Int *OutIndex
) {
    for (Int Index = 0; Index < Array->Count; Index += 1) {
        MemoryRef Lhs = ArrayGetElementAtIndex(Array, Index);
        if (Predicate(Lhs, Element)) {
            *OutIndex = Index;
            return true;
        }
    }

    return false;
}
