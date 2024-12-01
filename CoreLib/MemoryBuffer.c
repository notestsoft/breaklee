#include "Diagnostic.h"
#include "MemoryBuffer.h"
#include "Util.h"

struct _MemoryBuffer {
    AllocatorRef Allocator;
    Int Alignment;
    Int Size;
    Int SizeAligned;
    Int WriteOffset;
    Int ReadOffset;
    UInt8 *Memory;
};

MemoryBufferRef MemoryBufferCreate(
    AllocatorRef Allocator,
    Int Alignment,
    Int Length
) {
    Int MemorySize = sizeof(UInt8) * Length;
    Int MemorySizeAligned = Align(MemorySize, Alignment);
    Int HeaderSizeAligned = Align(sizeof(struct _MemoryBuffer), Alignment);
    Int TotalSize = HeaderSizeAligned + MemorySizeAligned;
    
    MemoryBufferRef MemoryBuffer = (MemoryBufferRef)AllocatorAllocate(Allocator, TotalSize);
    if (!MemoryBuffer) Fatal("MemoryBuffer allocation failed!");
    
    MemoryBuffer->Allocator = Allocator;
    MemoryBuffer->Alignment = Alignment;
    MemoryBuffer->Size = MemorySize;
    MemoryBuffer->SizeAligned = MemorySizeAligned;
    MemoryBuffer->WriteOffset = 0;
    MemoryBuffer->ReadOffset = 0;
    MemoryBuffer->Memory = (UInt8 *)((UInt8 *)MemoryBuffer + HeaderSizeAligned);
    memset(MemoryBuffer->Memory, 0, MemorySizeAligned);
    return MemoryBuffer;
}

Void MemoryBufferDestroy(
    MemoryBufferRef MemoryBuffer
) {
    AllocatorDeallocate(MemoryBuffer->Allocator, MemoryBuffer);
}

Int MemoryBufferGetWriteOffset(
    MemoryBufferRef MemoryBuffer
) {
    return MemoryBuffer->WriteOffset;
}

Int MemoryBufferGetFreeSize(
    MemoryBufferRef MemoryBuffer
) {
    return MemoryBuffer->Size - MemoryBuffer->WriteOffset;
}

UInt8* MemoryBufferGetMemory(
    MemoryBufferRef MemoryBuffer,
    Int Offset
) {
    assert(Offset <= MemoryBuffer->WriteOffset);
    return MemoryBuffer->Memory + sizeof(UInt8) * Offset;
}

Void MemoryBufferPopFront(
    MemoryBufferRef MemoryBuffer,
    Int Length
) {
    MemoryBufferRemove(MemoryBuffer, 0, Length);
}

Void MemoryBufferRemove(
    MemoryBufferRef MemoryBuffer,
    Int Offset,
    Int Length
) {
    assert(Offset + Length <= MemoryBuffer->WriteOffset);

    if (Offset + Length < MemoryBuffer->WriteOffset) {
        UInt8* Destination = MemoryBufferGetMemory(MemoryBuffer, Offset);
        UInt8* Source = MemoryBufferGetMemory(MemoryBuffer, Offset + Length);
        Int TailLength = MemoryBuffer->WriteOffset - (Offset + Length);
        memmove(Destination, Source, TailLength);
    }

    MemoryBuffer->WriteOffset -= Length;
}

Void MemoryBufferClear(
    MemoryBufferRef MemoryBuffer
) {
    MemoryBuffer->WriteOffset = 0;
}

UInt8* MemoryBufferAppend(
    MemoryBufferRef MemoryBuffer,
    Int Length
) {
    Int AppendedOffset = MemoryBuffer->WriteOffset + Length;
    assert(AppendedOffset <= MemoryBuffer->Size);
    
    UInt8* Memory = MemoryBufferGetMemory(MemoryBuffer, MemoryBuffer->WriteOffset);
    memset(Memory, 0, Length);
    
    MemoryBuffer->WriteOffset = AppendedOffset;
    return Memory;
}

UInt8* MemoryBufferAppendCopy(
    MemoryBufferRef MemoryBuffer,
    Void* Source,
    Int Length
) {
    Int AppendedOffset = MemoryBuffer->WriteOffset + Length;
    assert(AppendedOffset <= MemoryBuffer->Size);

    UInt8* Memory = MemoryBufferGetMemory(MemoryBuffer, MemoryBuffer->WriteOffset);
    memcpy(Memory, Source, Length);

    MemoryBuffer->WriteOffset = AppendedOffset;
    return Memory;
}

Int MemoryBufferGetReadOffset(
    MemoryBufferRef MemoryBuffer
) {
    return MemoryBuffer->ReadOffset;
}

Void MemoryBufferSetReadOffset(
    MemoryBufferRef MemoryBuffer,
    Int Offset
) {
    assert(Offset <= MemoryBuffer->ReadOffset);
    MemoryBuffer->ReadOffset = Offset;
}

UInt8* MemoryBufferReadBytes(
    MemoryBufferRef MemoryBuffer,
    Int Length
) {
    if (Length < 1) return NULL;

    assert(MemoryBuffer->ReadOffset + Length <= MemoryBuffer->ReadOffset);

    UInt8* Memory = MemoryBufferGetMemory(MemoryBuffer, MemoryBuffer->ReadOffset);
    MemoryBuffer->ReadOffset += Length;
    return Memory;
}

Void MemoryBufferReadBytesCopy(
    MemoryBufferRef MemoryBuffer,
    Void* Destination,
    Int Length
) {
    if (Length < 1) return;
    UInt8* Memory = MemoryBufferReadBytes(MemoryBuffer, Length);
    memcpy(Destination, Memory, Length);
}
