#include "IPCPacketBuffer.h"
#include "IPCSocket.h"

struct _IPCPacketBuffer {
    AllocatorRef Allocator;
    MemoryBufferRef MemoryBuffer;
};

IPCPacketBufferRef IPCPacketBufferCreate(
    AllocatorRef Allocator,
    Index Alignment,
    Index Length
) {
    IPCPacketBufferRef PacketBuffer = (IPCPacketBufferRef)AllocatorAllocate(Allocator, sizeof(struct _IPCPacketBuffer));
    if (!PacketBuffer) Fatal("PacketBuffer allocation failed!");
    
    PacketBuffer->Allocator = Allocator;
    PacketBuffer->MemoryBuffer = MemoryBufferCreate(Allocator, Alignment, Length);
    return PacketBuffer;
}

Void IPCPacketBufferDestroy(
    IPCPacketBufferRef PacketBuffer
) {
    MemoryBufferDestroy(PacketBuffer->MemoryBuffer);
    AllocatorDeallocate(PacketBuffer->Allocator, PacketBuffer);
}

Void* _IPCPacketBufferInit(
    IPCPacketBufferRef PacketBuffer,
    UInt32 Length,
    UInt16 Command
) {
    MemoryBufferClear(PacketBuffer->MemoryBuffer);

    IPCPacketRef Packet = (IPCPacketRef)MemoryBufferAppend(PacketBuffer->MemoryBuffer, Length);
    Packet->Length = Length;
    Packet->Command = IPC_COMMAND_ROUTE;
    Packet->SubCommand = Command;
    return Packet;
}

Void* IPCPacketBufferAppend(
    IPCPacketBufferRef PacketBuffer,
    Index Length
) {
    if (Length < 1) return NULL;
    IPCPacketRef Packet = (IPCPacketRef)MemoryBufferGetMemory(PacketBuffer->MemoryBuffer, 0);
    Void* Memory = MemoryBufferAppend(PacketBuffer->MemoryBuffer, Length);
    Packet->Length += Length;
    return Memory;
}

Void* IPCPacketBufferAppendCopy(
    IPCPacketBufferRef PacketBuffer,
    Void* Source,
    Index Length
) {
    if (Length < 1) return NULL;
    Void* Memory = IPCPacketBufferAppend(PacketBuffer, Length);
    memcpy(Memory, Source, Length);
    return Memory;
}

CString IPCPacketBufferAppendCString(
    IPCPacketBufferRef PacketBuffer,
    CString Value
) {
    return (CString)IPCPacketBufferAppendCopy(PacketBuffer, (Void*)Value, (Index)strlen(Value) + 1);
}

Void IPCPacketLogBytes(
    Void* Packet
) {
    UInt32 Length = ((IPCPacketRef)Packet)->Length;
    UInt8* Buffer = (UInt8*)Packet;

    fprintf(stdout, "Packet Size: %d\n", Length);

    for (Int32 Index = 0; Index < Length; Index += 1) {
        fprintf(stdout, "%02hhX ", Buffer[Index]);
    }

    fprintf(stdout, "\n");
}

MemoryBufferRef IPCPacketBufferGetMemoryBuffer(
    IPCPacketBufferRef PacketBuffer
) {
    return PacketBuffer->MemoryBuffer;
}
