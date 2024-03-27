#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef struct _PacketBuffer *PacketBufferRef;

PacketBufferRef PacketBufferCreate(
    AllocatorRef Allocator,
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Index Alignment,
    Index Length
);

Void PacketBufferDestroy(
    PacketBufferRef PacketBuffer
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

Void* PacketBufferAppend(
    PacketBufferRef PacketBuffer,
    Index Length
);

Void* PacketBufferAppendCopy(
    PacketBufferRef PacketBuffer,
    Void* Source,
    Index Length
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

UInt32 PacketGetLength(
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
