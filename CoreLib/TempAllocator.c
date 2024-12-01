#include "Array.h"
#include "Diagnostic.h"
#include "TempAllocator.h"

struct _TempAllocatorContext {
    AllocatorRef Allocator;
    ArrayRef Allocations;
};

Bool _IsPointerEqual(
    MemoryRef Lhs,
    MemoryRef Rhs
) {
    return *((MemoryRef*)Lhs) == Rhs;
}

MemoryRef _AllocatorTemp(
    AllocatorMode Mode,
    Int Capacity,
    MemoryRef Memory,
    MemoryRef Context
) {
    struct _TempAllocatorContext *TempContext = Context;
    assert(TempContext);

    switch (Mode) {
    case AllocatorModeAllocate: {
        Void *Memory = AllocatorAllocate(TempContext->Allocator, Capacity);
        if (!Memory) Fatal("Memory allocation failed!");
        ArrayAppendElement(TempContext->Allocations, &Memory);
        return Memory;
    }

    case AllocatorModeReallocate: {
        void *NewMemory = AllocatorReallocate(TempContext->Allocator, Memory, Capacity);
        if (NewMemory) {
            Int ElementIndex = 0;
            if (ArrayGetIndexOfElement(TempContext->Allocations, &_IsPointerEqual, Memory, &ElementIndex)) {
                ArraySetElementAtIndex(TempContext->Allocations, ElementIndex, &NewMemory);
            }
        }
        return NewMemory;
    }

    case AllocatorModeDeallocate:
        return NULL;

    case AllocatorModeDestroy: {
        for (Int Index = 0; Index < ArrayGetElementCount(TempContext->Allocations); Index += 1) {
            MemoryRef Memory = *((MemoryRef*)ArrayGetElementAtIndex(TempContext->Allocations, Index));
            AllocatorDeallocate(TempContext->Allocator, Memory);
        }

        ArrayDestroy(TempContext->Allocations);
        AllocatorDeallocate(TempContext->Allocator, TempContext);
        return NULL;
    }

    default:
        Fatal("Invalid value for Mode!");
    }
}

AllocatorRef TempAllocatorCreate(AllocatorRef Allocator) {
    struct _TempAllocatorContext *Context = (struct _TempAllocatorContext *)AllocatorAllocate(
        Allocator,
        sizeof(struct _TempAllocatorContext)
    );
    Context->Allocator = Allocator;
    Context->Allocations = ArrayCreateEmpty(Allocator, sizeof(MemoryRef), 8);
    return AllocatorCreate(Allocator, &_AllocatorTemp, Context);
}
