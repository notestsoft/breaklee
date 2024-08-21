#include "BumpAllocator.h"
#include "Diagnostic.h"
#include "Util.h"

const Int32 kBumpAllocatorDefaultPageCapacity = 0xFFFF;

#pragma pack(push, 1)

struct _BumpAllocatorPage {
    struct _BumpAllocatorPage *Next;
    Index Size;
    Index Capacity;
    Index Index;
    UInt8 *Memory;
};

struct _BumpAllocatorContext {
    AllocatorRef Allocator;
    Index Alignment;
    Index PageHeaderSize;
    struct _BumpAllocatorPage *FirstPage;
    struct _BumpAllocatorPage *CurrentPage;
};

#pragma pack(pop)

Void *_AllocatorBump(
    AllocatorMode Mode,
    Index Capacity,
    Void *Memory,
    Void *Context
);

AllocatorRef BumpAllocatorCreate(
    AllocatorRef Allocator
) {
    struct _BumpAllocatorContext *Context = AllocatorAllocate(Allocator, sizeof(struct _BumpAllocatorContext));
    Context->Allocator = Allocator;
    Context->Alignment = 2 * sizeof(Void*);
    Context->PageHeaderSize = Align(sizeof(struct _BumpAllocatorPage), Context->Alignment);
    Context->FirstPage = NULL;
    Context->CurrentPage = NULL;
    return AllocatorCreate(Allocator, &_AllocatorBump, Context);
}

Void *_AllocatorBump(
    AllocatorMode Mode,
    Index Capacity,
    Void *Memory,
    Void *Context
) {
    struct _BumpAllocatorContext *BumpContext = Context;
    assert(BumpContext);

    switch (Mode) {
    case AllocatorModeAllocate: {
        if (BumpContext->FirstPage == NULL) {
            Index MemoryCapacity = MAX(kBumpAllocatorDefaultPageCapacity, (Int32)Capacity + BumpContext->PageHeaderSize);
            MemoryCapacity = Align(MemoryCapacity, BumpContext->Alignment);
            struct _BumpAllocatorPage* Page = AllocatorAllocate(BumpContext->Allocator, MemoryCapacity);
            Page->Size = MemoryCapacity;
            Page->Next = NULL;
            Page->Capacity = MemoryCapacity - BumpContext->PageHeaderSize;
            Page->Index = 0;
            Page->Memory = (UInt8*)Page + BumpContext->PageHeaderSize;
            BumpContext->FirstPage = Page;
            BumpContext->CurrentPage = Page;
        }
        else if (BumpContext->CurrentPage->Index + Capacity > BumpContext->CurrentPage->Capacity) {
            Index MemoryCapacity = MAX(kBumpAllocatorDefaultPageCapacity, Capacity + BumpContext->PageHeaderSize);
            MemoryCapacity = Align(MemoryCapacity, BumpContext->Alignment);
            struct _BumpAllocatorPage* Page = AllocatorAllocate(BumpContext->Allocator, MemoryCapacity);
            Page->Size = MemoryCapacity;
            Page->Next = NULL;
            Page->Capacity = MemoryCapacity - BumpContext->PageHeaderSize;
            Page->Index = 0;
            Page->Memory = (UInt8*)Page + BumpContext->PageHeaderSize;
            BumpContext->CurrentPage->Next = Page;
            BumpContext->CurrentPage = Page;
        }

        Void *Memory = BumpContext->CurrentPage->Memory + sizeof(UInt8) * BumpContext->CurrentPage->Index;
        BumpContext->CurrentPage->Index += Capacity;
        return Memory;
    }

    case AllocatorModeReallocate:
        Fatal("BumpAllocator doesn't support reallocation!");
        return NULL;

    case AllocatorModeDeallocate:
        return NULL;

    case AllocatorModeDestroy: {
        struct _BumpAllocatorPage *Page = BumpContext->FirstPage;
        while (Page) {
            struct _BumpAllocatorPage *Next = Page->Next;
            AllocatorDeallocate(BumpContext->Allocator, Page);
            Page = Next;
        }

        AllocatorDeallocate(BumpContext->Allocator, BumpContext);
        return NULL;
    }

    default:
        Fatal("Invalid value for Mode!");
    }
}
