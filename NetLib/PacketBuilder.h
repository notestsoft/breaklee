#pragma once

#include "Socket.h"

EXTERN_C_BEGIN

#define PACKET_BUILDER_BUFFER_SIZE  0x1FFFF
#define PACKET_BUILDER_BUFFER_COUNT 8

PacketRef _PacketInit(
    Int32 Length,
    Bool Extended
);

#define PacketInit(__TYPE__) \
(__TYPE__*)_PacketInit(sizeof(__TYPE__), false)

#define PacketInitExtended(__TYPE__) \
(__TYPE__*)_PacketInit(sizeof(__TYPE__), true)

UInt16 ClientPacketGetCommand(
    PacketRef Packet
);

UInt16 ServerPacketGetCommand(
    PacketRef Packet
);

UInt16 IPCPacketGetCommand(
    PacketRef Packet
);

UInt8* PacketAppendMemory(
    Int64 Length
);

UInt8* PacketAppendMemoryCopy(
    Void* Source,
    Int64 Length
);

#define PacketAppendCString(__STRING__) \
(CString)PacketAppendMemoryCopy((UInt8*)__STRING__, (Int32)strlen(__STRING__) + 1)

#define PacketAppendValue(__TYPE__, __VALUE__) \
*((__TYPE__*)PacketAppendMemory(sizeof(__TYPE__))) = __VALUE__

#define PacketAppendStruct(__TYPE__) \
(__TYPE__*)PacketAppendMemory(sizeof(__TYPE__))

Void PacketLogBytes(
    Void *Packet
);

EXTERN_C_END
