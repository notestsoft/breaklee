#pragma once

#include "Base.h"

EXTERN_C_BEGIN

struct _PacketBuffer {
    AllocatorRef Allocator;
    MemoryBufferRef MemoryBuffer;
    UInt16 ProtocolIdentifier;
    UInt16 ProtocolVersion;
    UInt16 ProtocolExtension;
    Bool IsClient;
};

typedef struct _PacketBuffer *PacketBufferRef;

PacketBufferRef PacketBufferCreate(
    AllocatorRef Allocator,
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Int Alignment,
    Int Length,
    Bool IsClient
);

Void PacketBufferInitialize(
    PacketBufferRef PacketBuffer,
    AllocatorRef Allocator,
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Int Alignment,
    Int Length,
    Bool IsClient
);

Void PacketBufferFree(
    PacketBufferRef PacketBuffer
);

Void PacketBufferDestroy(
    PacketBufferRef PacketBuffer
);

UInt16 PacketBufferGetMagic(
    PacketBufferRef PacketBuffer,
    Bool Extended
);

Void* _PacketBufferInit(
    PacketBufferRef PacketBuffer,
    Bool Extended,
    UInt32 Length,
    UInt16 Command
);

#define PacketBufferInit(PacketBuffer, __NAMESPACE__, __NAME__) \
(__NAMESPACE__ ## _DATA_ ## __NAME__*)_PacketBufferInit(PacketBuffer, false, sizeof(__NAMESPACE__ ## _DATA_ ## __NAME__), __NAMESPACE__ ## _ ## __NAME__)

#define PacketBufferInitExtended(PacketBuffer, __NAMESPACE__, __NAME__) \
(__NAMESPACE__ ## _DATA_ ## __NAME__*)_PacketBufferInit(PacketBuffer, true, sizeof(__NAMESPACE__ ## _DATA_ ## __NAME__), __NAMESPACE__ ## _ ## __NAME__)

Void* PacketBufferInitFromFile(
    PacketBufferRef PacketBuffer,
    CString FilePath
);

Void* PacketBufferAppend(
    PacketBufferRef PacketBuffer,
    Int Length
);

Void* PacketBufferAppendCopy(
    PacketBufferRef PacketBuffer,
    Void* Source,
    Int Length
);

CString PacketBufferAppendCString(
    PacketBufferRef PacketBuffer,
    CString Value
);

#define PacketBufferAppendValue(PacketBuffer, __TYPE__, __VALUE__) \
*((__TYPE__*)PacketBufferAppend(PacketBuffer, sizeof(__TYPE__))) = __VALUE__

#define PacketBufferAppendStruct(PacketBuffer, __TYPE__) \
(__TYPE__*)PacketBufferAppend(PacketBuffer, sizeof(__TYPE__))

UInt16 ClientPacketGetCommand(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
);

UInt16 ServerPacketGetCommand(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
);

Int32 ClientPacketGetHeaderLength(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
);

Int32 ServerPacketGetHeaderLength(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
);

Int32 PacketGetLength(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
);

Void PacketSetLength(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet,
    UInt32 Length
);

Void PacketLogBytes(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void *Packet
);

EXTERN_C_END
