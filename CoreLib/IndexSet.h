#pragma once

#include "Array.h"

EXTERN_C_BEGIN

struct _IndexSetIterator {
    Index Value;
};

struct _IndexSet {
    AllocatorRef Allocator;
    ArrayRef Indices;
};

typedef struct _IndexSet* IndexSetRef;
typedef struct _IndexSetIterator* IndexSetIteratorRef;

IndexSetRef IndexSetCreate(
    AllocatorRef Allocator,
    Index Capacity
);

Void IndexSetDestroy(
    IndexSetRef Set
);

Index IndexSetGetElementCount(
    IndexSetRef Set
);

Void IndexSetClear(
    IndexSetRef Set
);

Void IndexSetInsert(
    IndexSetRef Set,
    Index Value
);

Void IndexSetRemove(
    IndexSetRef Set,
    Index Value
);

Bool IndexSetContains(
    IndexSetRef Set,
    Index Value
);

IndexSetRef IndexSetUnion(
    IndexSetRef Lhs,
    IndexSetRef Rhs
);

IndexSetRef IndexSetIntersection(
    IndexSetRef Lhs,
    IndexSetRef Rhs
);

IndexSetRef IndexSetDifference(
    IndexSetRef Lhs,
    IndexSetRef Rhs
);

IndexSetRef IndexSetSymmetricDifference(
    IndexSetRef Lhs,
    IndexSetRef Rhs
);

Bool IndexSetIsSubsetOf(
    IndexSetRef Set,
    IndexSetRef Other
);

Bool IndexSetIsSupersetOf(
    IndexSetRef Set,
    IndexSetRef Other
);

IndexSetIteratorRef IndexSetGetIterator(
    IndexSetRef Set
);

IndexSetIteratorRef IndexSetIteratorNext(
    IndexSetRef Set,
    IndexSetIteratorRef Iterator
);

IndexSetIteratorRef IndexSetGetInverseIterator(
    IndexSetRef Set
);

IndexSetIteratorRef IndexSetInverseIteratorNext(
    IndexSetRef Set,
    IndexSetIteratorRef Iterator
);

EXTERN_C_END