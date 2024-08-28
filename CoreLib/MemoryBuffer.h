#pragma once

#include "Base.h"

#include "Allocator.h"

EXTERN_C_BEGIN

typedef struct _MemoryBuffer *MemoryBufferRef;

MemoryBufferRef MemoryBufferCreate(
    AllocatorRef Allocator,
    Index Alignment,
    Index Length
);

Void MemoryBufferDestroy(
    MemoryBufferRef MemoryBuffer
);

Index MemoryBufferGetWriteOffset(
    MemoryBufferRef MemoryBuffer
);

Index MemoryBufferGetFreeSize(
    MemoryBufferRef MemoryBuffer
);

UInt8* MemoryBufferGetMemory(
    MemoryBufferRef MemoryBuffer,
    Index Offset
);

Void MemoryBufferPopFront(
    MemoryBufferRef MemoryBuffer,
    Index Length
);

Void MemoryBufferRemove(
    MemoryBufferRef MemoryBuffer,
    Index Offset,
    Index Length
);

Void MemoryBufferClear(
    MemoryBufferRef MemoryBuffer
);

UInt8* MemoryBufferAppend(
    MemoryBufferRef MemoryBuffer,
    Index Length
);

UInt8* MemoryBufferAppendCopy(
    MemoryBufferRef MemoryBuffer,
    Void* Source,
    Index Length
);

#define MemoryBufferAppendCString(MemoryBuffer, __STRING__) \
((CString)MemoryBufferAppendCopy(MemoryBuffer, (Void*)__STRING__, (Index)strlen(__STRING__) + 1))

#define MemoryBufferAppendValue(MemoryBuffer, __TYPE__, __VALUE__) \
*((__TYPE__*)MemoryBufferAppend(MemoryBuffer, sizeof(__TYPE__))) = __VALUE__

#define MemoryBufferAppendStruct(MemoryBuffer, __TYPE__) \
(__TYPE__*)MemoryBufferAppend(MemoryBuffer, sizeof(__TYPE__))

Index MemoryBufferGetReadOffset(
    MemoryBufferRef MemoryBuffer
);

Void MemoryBufferSetReadOffset(
    MemoryBufferRef MemoryBuffer,
    Index Offset
);

UInt8* MemoryBufferReadBytes(
    MemoryBufferRef MemoryBuffer,
    Index Length
);

Void MemoryBufferReadBytesCopy(
    MemoryBufferRef MemoryBuffer,
    UInt8* Destination,
    Index Length
);

#define MemoryBufferReadValue(MemoryBuffer, __TYPE__, __VALUE__) \
*((__TYPE__*)MemoryBufferReadBytes(MemoryBuffer, sizeof(__TYPE__)))

EXTERN_C_END
