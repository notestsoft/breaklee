#include "Diagnostic.h"
#include "MemoryBuffer.h"
#include "Util.h"

struct _MemoryBuffer {
    AllocatorRef Allocator;
    Index Alignment;
    Index Size;
    Index SizeAligned;
    Index Offset;
    UInt8 *Memory;
};

MemoryBufferRef MemoryBufferCreate(
    AllocatorRef Allocator,
    Index Alignment,
    Index Length
) {
    Index MemorySize = sizeof(UInt8) * Length;
    Index MemorySizeAligned = Align(MemorySize, Alignment);
    Index HeaderSizeAligned = Align(sizeof(struct _MemoryBuffer), Alignment);
    Index TotalSize = HeaderSizeAligned + MemorySizeAligned;
    
    MemoryBufferRef MemoryBuffer = (MemoryBufferRef)AllocatorAllocate(Allocator, TotalSize);
    if (!MemoryBuffer) Fatal("MemoryBuffer allocation failed!");
    
    MemoryBuffer->Allocator = Allocator;
    MemoryBuffer->Alignment = Alignment;
    MemoryBuffer->Size = MemorySize;
    MemoryBuffer->SizeAligned = MemorySizeAligned;
    MemoryBuffer->Offset = 0;
    MemoryBuffer->Memory = (UInt8 *)((UInt8 *)MemoryBuffer + HeaderSizeAligned);
    memset(MemoryBuffer->Memory, 0, MemorySizeAligned);
    return MemoryBuffer;
}

Void MemoryBufferDestroy(
    MemoryBufferRef MemoryBuffer
) {
    AllocatorDeallocate(MemoryBuffer->Allocator, MemoryBuffer);
}

Index MemoryBufferGetOffset(
    MemoryBufferRef MemoryBuffer
) {
    return MemoryBuffer->Offset;
}

Index MemoryBufferGetFreeSize(
    MemoryBufferRef MemoryBuffer
) {
    return MemoryBuffer->Size - MemoryBuffer->Offset;
}

UInt8* MemoryBufferGetMemory(
    MemoryBufferRef MemoryBuffer,
    Index Offset
) {
    assert(Offset <= MemoryBuffer->Offset);
    return MemoryBuffer->Memory + sizeof(UInt8) * Offset;
}

Void MemoryBufferPopFront(
    MemoryBufferRef MemoryBuffer,
    Index Length
) {
    MemoryBufferRemove(MemoryBuffer, 0, Length);
}

Void MemoryBufferRemove(
    MemoryBufferRef MemoryBuffer,
    Index Offset,
    Index Length
) {
    assert(Offset + Length <= MemoryBuffer->Offset);

    if (Offset + Length < MemoryBuffer->Offset) {
        UInt8* Destination = MemoryBufferGetMemory(MemoryBuffer, Offset);
        UInt8* Source = MemoryBufferGetMemory(MemoryBuffer, Offset + Length);
        Index TailLength = MemoryBuffer->Offset - (Offset + Length);
        memmove(Destination, Source, TailLength);
    }

    MemoryBuffer->Offset -= Length;
}

Void MemoryBufferClear(
    MemoryBufferRef MemoryBuffer
) {
    MemoryBuffer->Offset = 0;
}

UInt8* MemoryBufferAppend(
    MemoryBufferRef MemoryBuffer,
    Index Length
) {
    Index AppendedOffset = MemoryBuffer->Offset + Length;
    assert(AppendedOffset <= MemoryBuffer->Size);
    
    UInt8* Memory = MemoryBufferGetMemory(MemoryBuffer, MemoryBuffer->Offset);
    memset(Memory, 0, Length);
    
    MemoryBuffer->Offset = AppendedOffset;
    return Memory;
}

UInt8* MemoryBufferAppendCopy(
    MemoryBufferRef MemoryBuffer,
    Void* Source,
    Index Length
) {
    Index AppendedOffset = MemoryBuffer->Offset + Length;
    assert(AppendedOffset <= MemoryBuffer->Size);

    UInt8* Memory = MemoryBufferGetMemory(MemoryBuffer, MemoryBuffer->Offset);
    memcpy(Memory, Source, Length);

    MemoryBuffer->Offset = AppendedOffset;
    return Memory;
}
