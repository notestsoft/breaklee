#pragma once

#include "Base.h"

#include "Allocator.h"

EXTERN_C_BEGIN

typedef struct _MemoryBuffer *MemoryBufferRef;

MemoryBufferRef MemoryBufferCreate(
    AllocatorRef Allocator,
    Int Alignment,
    Int Length
);

Void MemoryBufferDestroy(
    MemoryBufferRef MemoryBuffer
);

Int MemoryBufferGetWriteOffset(
    MemoryBufferRef MemoryBuffer
);

Int MemoryBufferGetFreeSize(
    MemoryBufferRef MemoryBuffer
);

UInt8* MemoryBufferGetMemory(
    MemoryBufferRef MemoryBuffer,
    Int Offset
);

Void MemoryBufferPopFront(
    MemoryBufferRef MemoryBuffer,
    Int Length
);

Void MemoryBufferRemove(
    MemoryBufferRef MemoryBuffer,
    Int Offset,
    Int Length
);

Void MemoryBufferClear(
    MemoryBufferRef MemoryBuffer
);

UInt8* MemoryBufferAppend(
    MemoryBufferRef MemoryBuffer,
    Int Length
);

UInt8* MemoryBufferAppendCopy(
    MemoryBufferRef MemoryBuffer,
    Void* Source,
    Int Length
);

#define MemoryBufferAppendCString(MemoryBuffer, __STRING__) \
((CString)MemoryBufferAppendCopy(MemoryBuffer, (Void*)__STRING__, (Int)strlen(__STRING__) + 1))

#define MemoryBufferAppendValue(MemoryBuffer, __TYPE__, __VALUE__) \
*((__TYPE__*)MemoryBufferAppend(MemoryBuffer, sizeof(__TYPE__))) = __VALUE__

#define MemoryBufferAppendStruct(MemoryBuffer, __TYPE__) \
(__TYPE__*)MemoryBufferAppend(MemoryBuffer, sizeof(__TYPE__))

Int MemoryBufferGetReadOffset(
    MemoryBufferRef MemoryBuffer
);

Void MemoryBufferSetReadOffset(
    MemoryBufferRef MemoryBuffer,
    Int Offset
);

UInt8* MemoryBufferReadBytes(
    MemoryBufferRef MemoryBuffer,
    Int Length
);

Void MemoryBufferReadBytesCopy(
    MemoryBufferRef MemoryBuffer,
    Void* Destination,
    Int Length
);

#define MemoryBufferReadValue(MemoryBuffer, __TYPE__, __VALUE__) \
*((__TYPE__*)MemoryBufferReadBytes(MemoryBuffer, sizeof(__TYPE__)))

EXTERN_C_END
