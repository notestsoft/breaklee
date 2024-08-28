#include "Diagnostic.h"
#include "MemoryBuffer.h"
#include "Util.h"

struct _MemoryBuffer {
    AllocatorRef Allocator;
    Index Alignment;
    Index Size;
    Index SizeAligned;
    Index WriteOffset;
    Index ReadOffset;
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

Index MemoryBufferGetWriteOffset(
    MemoryBufferRef MemoryBuffer
) {
    return MemoryBuffer->WriteOffset;
}

Index MemoryBufferGetFreeSize(
    MemoryBufferRef MemoryBuffer
) {
    return MemoryBuffer->Size - MemoryBuffer->WriteOffset;
}

UInt8* MemoryBufferGetMemory(
    MemoryBufferRef MemoryBuffer,
    Index Offset
) {
    assert(Offset <= MemoryBuffer->WriteOffset);
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
    assert(Offset + Length <= MemoryBuffer->WriteOffset);

    if (Offset + Length < MemoryBuffer->WriteOffset) {
        UInt8* Destination = MemoryBufferGetMemory(MemoryBuffer, Offset);
        UInt8* Source = MemoryBufferGetMemory(MemoryBuffer, Offset + Length);
        Index TailLength = MemoryBuffer->WriteOffset - (Offset + Length);
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
    Index Length
) {
    Index AppendedOffset = MemoryBuffer->WriteOffset + Length;
    assert(AppendedOffset <= MemoryBuffer->Size);
    
    UInt8* Memory = MemoryBufferGetMemory(MemoryBuffer, MemoryBuffer->WriteOffset);
    memset(Memory, 0, Length);
    
    MemoryBuffer->WriteOffset = AppendedOffset;
    return Memory;
}

UInt8* MemoryBufferAppendCopy(
    MemoryBufferRef MemoryBuffer,
    Void* Source,
    Index Length
) {
    Index AppendedOffset = MemoryBuffer->WriteOffset + Length;
    assert(AppendedOffset <= MemoryBuffer->Size);

    UInt8* Memory = MemoryBufferGetMemory(MemoryBuffer, MemoryBuffer->WriteOffset);
    memcpy(Memory, Source, Length);

    MemoryBuffer->WriteOffset = AppendedOffset;
    return Memory;
}

Index MemoryBufferGetReadOffset(
    MemoryBufferRef MemoryBuffer
) {
    return MemoryBuffer->ReadOffset;
}

Void MemoryBufferSetReadOffset(
    MemoryBufferRef MemoryBuffer,
    Index Offset
) {
    assert(Offset <= MemoryBuffer->ReadOffset);
    MemoryBuffer->ReadOffset = Offset;
}

UInt8* MemoryBufferReadBytes(
    MemoryBufferRef MemoryBuffer,
    Index Length
) {
    if (Length < 1) return NULL;

    assert(MemoryBuffer->ReadOffset + Length <= MemoryBuffer->ReadOffset);

    UInt8* Memory = MemoryBufferGetMemory(MemoryBuffer, MemoryBuffer->ReadOffset);
    MemoryBuffer->ReadOffset += Length;
    return Memory;
}

Void MemoryBufferReadBytesCopy(
    MemoryBufferRef MemoryBuffer,
    UInt8* Destination,
    Index Length
) {
    if (Length < 1) return;
    UInt8* Memory = MemoryBufferReadBytes(MemoryBuffer, Length);
    memcpy(Destination, Memory, Length);
}
