#include "Diagnostic.h"
#include "IndexSet.h"

// TODO: Add hash based implementation instead of just using an array!

IndexSetRef IndexSetCreate(
    AllocatorRef Allocator,
    Index Capacity
) {
    IndexSetRef Set = (IndexSetRef)AllocatorAllocate(Allocator, sizeof(struct _IndexSet));
    if (!Set) FatalError("Memory allocation failed!");
    
    Set->Allocator = Allocator;
    Set->Indices = ArrayCreateEmpty(Allocator, sizeof(Index), Capacity);
    return Set;
}

Void IndexSetDestroy(
    IndexSetRef Set
) {
    ArrayDestroy(Set->Indices);
    AllocatorDeallocate(Set->Allocator, Set);
}

Index IndexSetGetElementCount(
    IndexSetRef Set
) {
    return ArrayGetElementCount(Set->Indices);
}

Void IndexSetClear(
    IndexSetRef Set
) {
    ArrayRemoveAllElements(Set->Indices, true);
}

Void IndexSetInsert(
    IndexSetRef Set,
    Index Value
) {
    if (IndexSetContains(Set, Value)) return;
    
    ArrayAppendElement(Set->Indices, &Value);
}

Void IndexSetRemove(
    IndexSetRef Set,
    Index Value
) {
    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Set->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Set->Indices, ElementIndex));
        if (Element == Value) {
            ArrayRemoveElementAtIndex(Set->Indices, Value);
            break;
        }
    }
}

Bool IndexSetContains(
    IndexSetRef Set,
    Index Value
) {
    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Set->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Set->Indices, ElementIndex));
        if (Element == Value) {
            return true;
        }
    }

    return false;
}

IndexSetRef IndexSetUnion(
    IndexSetRef Lhs,
    IndexSetRef Rhs
) {
    IndexSetRef Result = IndexSetCreate(Lhs->Allocator, ArrayGetElementCount(Lhs->Indices) + ArrayGetElementCount(Rhs->Indices));
    
    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Lhs->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Lhs->Indices, ElementIndex));
        IndexSetInsert(Result, Element);
    }

    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Rhs->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Rhs->Indices, ElementIndex));
        IndexSetInsert(Result, Element);
    }
}

IndexSetRef IndexSetIntersection(
    IndexSetRef Lhs,
    IndexSetRef Rhs
) {
    IndexSetRef Result = IndexSetCreate(Lhs->Allocator, ArrayGetElementCount(Lhs->Indices) + ArrayGetElementCount(Rhs->Indices));
    
    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Lhs->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Lhs->Indices, ElementIndex));
        if (IndexSetContains(Rhs, Element)) {
            IndexSetInsert(Result, Element);
        }
    }
}

IndexSetRef IndexSetDifference(
    IndexSetRef Lhs,
    IndexSetRef Rhs
) {
    IndexSetRef Result = IndexSetCreate(Lhs->Allocator, ArrayGetElementCount(Lhs->Indices) + ArrayGetElementCount(Rhs->Indices));
    
    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Lhs->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Lhs->Indices, ElementIndex));
        IndexSetInsert(Result, Element);
    }

    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Rhs->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Rhs->Indices, ElementIndex));
        IndexSetRemove(Result, Element);
    }
}

IndexSetRef IndexSetSymmetricDifference(
    IndexSetRef Lhs,
    IndexSetRef Rhs
) {
    IndexSetRef Result = IndexSetCreate(Lhs->Allocator, ArrayGetElementCount(Lhs->Indices) + ArrayGetElementCount(Rhs->Indices));
    
    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Lhs->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Lhs->Indices, ElementIndex));
        IndexSetInsert(Result, Element);
    }

    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Rhs->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Rhs->Indices, ElementIndex));
        if (IndexSetContains(Lhs, Element)) {
            IndexSetRemove(Result, Element);
        }
        else {
            IndexSetInsert(Result, Element);
        }
    }
}

Bool IndexSetIsSubsetOf(
    IndexSetRef Set,
    IndexSetRef Other
) {
    for (Index ElementIndex = 0; ElementIndex < ArrayGetElementCount(Set->Indices); ElementIndex += 1) {
        Index Element = *((Index*)ArrayGetElementAtIndex(Set->Indices, ElementIndex));
        if (!IndexSetContains(Other, Element)) {
            return false;
        }
    }

    return true;
}

Bool IndexSetIsSupersetOf(
    IndexSetRef Set,
    IndexSetRef Other
) {
    return IndexSetIsSubsetOf(Other, Set);
}

IndexSetIteratorRef IndexSetGetIterator(
    IndexSetRef Set
) {
    if (ArrayGetElementCount(Set->Indices) < 1) return NULL;

    return (IndexSetIteratorRef)ArrayGetElementAtIndex(Set->Indices, 0);
}

IndexSetIteratorRef IndexSetIteratorNext(
    IndexSetRef Set,
    IndexSetIteratorRef Iterator
) {
    IndexSetIteratorRef Start = IndexSetGetIterator(Set);
    Index ElementSize = ArrayGetElementSize(Set->Indices);
    Index Offset = ((UInt8*)Iterator - (UInt8*)Start) / ElementSize + 1;

    if (0 <= Offset && Offset < ArrayGetElementCount(Set->Indices)) {
        return (IndexSetIteratorRef)ArrayGetElementAtIndex(Set->Indices, Offset);
    }

    return NULL;
}

IndexSetIteratorRef IndexSetGetInverseIterator(
    IndexSetRef Set
) {
    if (ArrayGetElementCount(Set->Indices) < 1) return NULL;

    return (IndexSetIteratorRef)ArrayGetElementAtIndex(Set->Indices, IndexSetGetElementCount(Set) - 1);
}

IndexSetIteratorRef IndexSetInverseIteratorNext(
    IndexSetRef Set,
    IndexSetIteratorRef Iterator
) {
    IndexSetIteratorRef Start = IndexSetGetIterator(Set);
    Index ElementSize = ArrayGetElementSize(Set->Indices);
    Index Offset = ((UInt8*)Iterator - (UInt8*)Start) / ElementSize;
    if (Offset < 1) return NULL;
    
    Offset -= 1;
    if (0 <= Offset && Offset < ArrayGetElementCount(Set->Indices)) {
        return (IndexSetIteratorRef)ArrayGetElementAtIndex(Set->Indices, Offset);
    }

    return NULL;
}
