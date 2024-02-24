#include "PacketBuilder.h"
#include "PacketSignature.h"

#define PACKET_HEADER_MAGIC 0xB7E2
#define EXTENDED_PACKET_HEADER_MAGIC 0xC8F3

static Int32 kPacketBuilderBufferIndex = 0;
static UInt8 kPacketBuilderBuffer[PACKET_BUILDER_BUFFER_SIZE * PACKET_BUILDER_BUFFER_COUNT] = { 0 };

PacketRef _PacketInit(
    Int32 Length,
    Bool Extended
) {
    assert(0 < Length && Length <= PACKET_BUILDER_BUFFER_SIZE);
    kPacketBuilderBufferIndex = (kPacketBuilderBufferIndex + 1) % PACKET_BUILDER_BUFFER_COUNT;
    UInt8* Buffer = &kPacketBuilderBuffer[0] + PACKET_BUILDER_BUFFER_SIZE * kPacketBuilderBufferIndex;
    memset(Buffer, 0, Length);
    PacketRef Packet = (PacketRef)Buffer;
    Packet->Magic = (Extended) ? EXTENDED_PACKET_HEADER_MAGIC : PACKET_HEADER_MAGIC;
    Packet->Length = (UInt16)Length;
    return Packet;
}

UInt16 ClientPacketGetCommand(
    PacketRef Packet
) {
    if (Packet->Magic == PACKET_HEADER_MAGIC) {
        struct { C2S_DATA_SIGNATURE; } *Data = (Void *)Packet;
        return Data->Command;
    }
    
    if (Packet->Magic == EXTENDED_PACKET_HEADER_MAGIC) {
        struct { C2S_DATA_SIGNATURE_EXTENDED; } *Data = (Void *)Packet;
        return Data->Command;
    }
    
    return 0;
}

UInt16 ServerPacketGetCommand(
    PacketRef Packet
) {
    if (Packet->Magic == PACKET_HEADER_MAGIC) {
        struct { S2C_DATA_SIGNATURE; } *Data = (Void *)Packet;
        return Data->Command;
    }
    
    if (Packet->Magic == EXTENDED_PACKET_HEADER_MAGIC) {
        struct { S2C_DATA_SIGNATURE_EXTENDED; } *Data = (Void *)Packet;
        return Data->Command;
    }
    
    return 0;
}

UInt16 IPCPacketGetCommand(
    PacketRef Packet
) {
    struct { IPC_DATA_SIGNATURE; } *Data = (Void *)Packet;
    return Data->Command;
}

Int32 PacketGetLength() {
    UInt8* Buffer = &kPacketBuilderBuffer[0] + PACKET_BUILDER_BUFFER_SIZE * kPacketBuilderBufferIndex;
    PacketRef Packet = (PacketRef)Buffer;
    if (Packet->Magic == EXTENDED_PACKET_HEADER_MAGIC) {
        ExtendedPacketRef ExtendedPacket = (ExtendedPacketRef)Buffer;
        return ExtendedPacket->Length;
    }
    
    return Packet->Length;
}

Void PacketAppendLength(
    Int64 Length
) {
    UInt8* Buffer = &kPacketBuilderBuffer[0] + PACKET_BUILDER_BUFFER_SIZE * kPacketBuilderBufferIndex;
    PacketRef Packet = (PacketRef)Buffer;
    if (Packet->Magic == EXTENDED_PACKET_HEADER_MAGIC) {
        ExtendedPacketRef ExtendedPacket = (ExtendedPacketRef)Buffer;
        ExtendedPacket->Length += Length;
    } else {
        Packet->Length += (UInt16)Length;
    }
}

UInt8* PacketAppendMemory(
    Int64 Length
) {
    UInt8* Buffer = &kPacketBuilderBuffer[0] + PACKET_BUILDER_BUFFER_SIZE * kPacketBuilderBufferIndex;
    PacketRef Packet = (PacketRef)Buffer;
    Int32 PacketLength = PacketGetLength();
    assert(0 < Length && PacketLength + Length <= PACKET_BUILDER_BUFFER_SIZE);
    UInt8* Memory = Buffer + PacketLength;
    memset(Memory, 0, Length);
    PacketAppendLength(Length);
    return Memory;
}

UInt8* PacketAppendMemoryCopy(
    Void* Source,
    Int64 Length
) {
    if (Length < 1) return NULL;
    UInt8* Buffer = &kPacketBuilderBuffer[0] + PACKET_BUILDER_BUFFER_SIZE * kPacketBuilderBufferIndex;
    PacketRef Packet = (PacketRef)Buffer;
    Int32 PacketLength = PacketGetLength();
    assert(0 < Length && PacketLength + Length <= PACKET_BUILDER_BUFFER_SIZE);
    UInt8* Memory = Buffer + PacketLength;
    memcpy(Memory, Source, Length);
    PacketAppendLength(Length);
    return Memory;
}

Void PacketLogBytes(
    Void *Packet
) {
    UInt8* Buffer = (UInt8*)Packet;

    fprintf(stdout, "Packet Size: %d\n", ((PacketRef)Packet)->Length);

    for (Int32 Index = 0; Index < ((PacketRef)Packet)->Length; Index += 1) {
        fprintf(stdout, "%02hhX ", Buffer[Index]);
    }

    fprintf(stdout, "\n");
}
