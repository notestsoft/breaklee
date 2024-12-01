#pragma once

#include "Array.h"

EXTERN_C_BEGIN

struct _IndexSetIterator {
    Int Value;
};

struct _IndexSet {
    AllocatorRef Allocator;
    ArrayRef Indices;
};

typedef struct _IndexSet* IndexSetRef;
typedef struct _IndexSetIterator* IndexSetIteratorRef;

IndexSetRef IndexSetCreate(
    AllocatorRef Allocator,
    Int Capacity
);

Void IndexSetDestroy(
    IndexSetRef Set
);

Int IndexSetGetElementCount(
    IndexSetRef Set
);

Void IndexSetClear(
    IndexSetRef Set
);

Void IndexSetInsert(
    IndexSetRef Set,
    Int Value
);

Void IndexSetRemove(
    IndexSetRef Set,
    Int Value
);

Bool IndexSetContains(
    IndexSetRef Set,
    Int Value
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