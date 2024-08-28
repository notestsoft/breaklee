#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef struct _IPCPacketBuffer *IPCPacketBufferRef;

IPCPacketBufferRef IPCPacketBufferCreate(
    AllocatorRef Allocator,
    Index Alignment,
    Index Length
);

Void IPCPacketBufferDestroy(
    IPCPacketBufferRef PacketBuffer
);

Void* _IPCPacketBufferInit(
    IPCPacketBufferRef PacketBuffer,
    UInt32 Length,
    UInt16 Command
);

#define IPCPacketBufferInit(PacketBuffer, __NAMESPACE__, __NAME__) \
(IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__*)_IPCPacketBufferInit(PacketBuffer, sizeof(IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__), IPC_ ## __NAMESPACE__ ## _ ## __NAME__)

Void* IPCPacketBufferAppend(
    IPCPacketBufferRef PacketBuffer,
    Index Length
);

Void* IPCPacketBufferAppendCopy(
    IPCPacketBufferRef PacketBuffer,
    Void* Source,
    Index Length
);

CString IPCPacketBufferAppendCString(
    IPCPacketBufferRef PacketBuffer,
    CString Value
);

#define IPCPacketBufferAppendValue(PacketBuffer, __TYPE__, __VALUE__) \
*((__TYPE__*)IPCPacketBufferAppend(PacketBuffer, sizeof(__TYPE__))) = __VALUE__

#define IPCPacketBufferAppendStruct(PacketBuffer, __TYPE__) \
(__TYPE__*)IPCPacketBufferAppend(PacketBuffer, sizeof(__TYPE__))

Void IPCPacketLogBytes(
    Void* Packet
);

MemoryBufferRef IPCPacketBufferGetMemoryBuffer(
    IPCPacketBufferRef PacketBuffer
);

EXTERN_C_END
