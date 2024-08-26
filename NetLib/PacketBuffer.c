#include "PacketBuffer.h"
#include "PacketSignature.h"

struct _PacketBuffer {
    AllocatorRef Allocator;
    MemoryBufferRef MemoryBuffer;
    UInt16 ProtocolIdentifier;
    UInt16 ProtocolVersion;
    UInt16 ProtocolExtension;
    Bool IsClient;
};

PacketBufferRef PacketBufferCreate(
    AllocatorRef Allocator,
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Index Alignment,
    Index Length,
    Bool IsClient
) {
    PacketBufferRef PacketBuffer = (PacketBufferRef)AllocatorAllocate(Allocator, sizeof(struct _PacketBuffer));
    if (!PacketBuffer) Fatal("PacketBuffer allocation failed!");
    
    PacketBuffer->Allocator = Allocator;
    PacketBuffer->MemoryBuffer = MemoryBufferCreate(Allocator, Alignment, Length);
    PacketBuffer->ProtocolIdentifier = ProtocolIdentifier;
    PacketBuffer->ProtocolVersion = ProtocolVersion;
    PacketBuffer->ProtocolExtension = ProtocolExtension;
    PacketBuffer->IsClient = IsClient;
    return PacketBuffer;
}

Void PacketBufferDestroy(
    PacketBufferRef PacketBuffer
) {
    MemoryBufferDestroy(PacketBuffer->MemoryBuffer);
    AllocatorDeallocate(PacketBuffer->Allocator, PacketBuffer);
}

Void* _PacketBufferInit(
    PacketBufferRef PacketBuffer,
    Bool Extended,
    UInt32 Length,
    UInt16 Command
) {
    MemoryBufferClear(PacketBuffer->MemoryBuffer);

    Void* Packet = MemoryBufferAppend(PacketBuffer->MemoryBuffer, Length);

    if (PacketBuffer->IsClient) {
        assert(Length >= 10);

        *(UInt16*)((UInt8*)Packet + 0) = PacketBuffer->ProtocolIdentifier + PacketBuffer->ProtocolVersion;
        *(UInt16*)((UInt8*)Packet + 2) = Length;
        *(UInt16*)((UInt8*)Packet + 8) = Command;
    }
    else {
        if (Extended) {
            assert(Length >= 8);

            *(UInt16*)((UInt8*)Packet + 0) = PacketBuffer->ProtocolIdentifier + PacketBuffer->ProtocolVersion + PacketBuffer->ProtocolExtension;
            *(UInt32*)((UInt8*)Packet + 2) = Length;
            *(UInt16*)((UInt8*)Packet + 6) = Command;
        }
        else {
            assert(Length >= 6);

            *(UInt16*)((UInt8*)Packet + 0) = PacketBuffer->ProtocolIdentifier + PacketBuffer->ProtocolVersion;
            *(UInt16*)((UInt8*)Packet + 2) = Length;
            *(UInt16*)((UInt8*)Packet + 4) = Command;
        }
    }

    return Packet;
}

Void* PacketBufferInitFromFile(
    PacketBufferRef PacketBuffer,
    CString FilePath
) {
    MemoryBufferClear(PacketBuffer->MemoryBuffer);

    FileRef File = FileOpen(FilePath);
    if (!File) Fatal("Error reading file!");

    UInt8* Buffer = NULL;
    Int32 BufferLength = 0;
    if (!FileRead(File, &Buffer, &BufferLength)) {
        FileClose(File);
        Fatal("Error reading file!");
    }

    Void* Packet = MemoryBufferAppendCopy(PacketBuffer->MemoryBuffer, Buffer, BufferLength);
    free(Buffer);
    FileClose(File);
    return Packet;
}

Void* PacketBufferAppend(
    PacketBufferRef PacketBuffer,
    Index Length
) {
    Void* Packet = MemoryBufferGetMemory(PacketBuffer->MemoryBuffer, 0);

    UInt32 PacketLength = PacketGetLength(
        PacketBuffer->ProtocolIdentifier,
        PacketBuffer->ProtocolVersion,
        PacketBuffer->ProtocolExtension,
        Packet
    );

    Void* Memory = MemoryBufferAppend(PacketBuffer->MemoryBuffer, Length);
    PacketLength += Length;
    
    PacketSetLength(
        PacketBuffer->ProtocolIdentifier,
        PacketBuffer->ProtocolVersion,
        PacketBuffer->ProtocolExtension,
        Packet,
        PacketLength
    );

    return Memory;
}

Void* PacketBufferAppendCopy(
    PacketBufferRef PacketBuffer,
    Void* Source,
    Index Length
) {
    if (Length < 1) return;
    Void* Memory = PacketBufferAppend(PacketBuffer, Length);
    memcpy(Memory, Source, Length);
    return Memory;
}

CString PacketBufferAppendCString(
    PacketBufferRef PacketBuffer,
    CString Value
) {
    return (CString)PacketBufferAppendCopy(PacketBuffer, (Void*)Value, (Index)strlen(Value) + 1);
}

Int32 PacketGetLength(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
) {
    UInt16 PacketMagic = *((UInt16*)Packet);
    PacketMagic -= ProtocolIdentifier;
    PacketMagic -= ProtocolVersion;
    
    if (PacketMagic == ProtocolExtension) {
        return *((UInt32*)((UInt8*)Packet + sizeof(UInt16)));
    }
    
    return *((UInt16*)((UInt8*)Packet + sizeof(UInt16)));
}

Void PacketSetLength(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet,
    UInt32 Length
) {
    UInt16 PacketMagic = *((UInt16*)Packet);
    PacketMagic -= ProtocolIdentifier;
    PacketMagic -= ProtocolVersion;
    
    if (PacketMagic == ProtocolExtension) {
        *((UInt32*)((UInt8*)Packet + sizeof(UInt16))) = Length;
    }
    
    *((UInt16*)((UInt8*)Packet + sizeof(UInt16))) = Length;
}

Void PacketLogBytes(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
) {
    UInt32 Length = PacketGetLength(ProtocolIdentifier, ProtocolVersion, ProtocolExtension, Packet);
    UInt8* Buffer = (UInt8*)Packet;

    fprintf(stdout, "Packet Size: %d\n", Length);

    for (Int32 Index = 0; Index < Length; Index += 1) {
        fprintf(stdout, "%02hhX ", Buffer[Index]);
    }

    fprintf(stdout, "\n");
}

UInt16 ClientPacketGetCommand(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void *Packet
) {
    return *(UInt16*)((UInt8*)Packet + 8);
}

UInt16 ServerPacketGetCommand(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void *Packet
) {
    UInt16 PacketMagic = *((UInt16*)Packet);
    PacketMagic -= ProtocolIdentifier;
    PacketMagic -= ProtocolVersion;
    
    if (PacketMagic == ProtocolExtension) {
        return *((UInt16*)((UInt8*)Packet + 6));
    }
    
    return *((UInt16*)((UInt8*)Packet + 4));
}

Int32 ClientPacketGetHeaderLength(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
) {
    UInt16 PacketMagic = *((UInt16*)Packet);
    PacketMagic -= ProtocolIdentifier;
    PacketMagic -= ProtocolVersion;
    
    if (PacketMagic == ProtocolExtension) {
        return 12;
    }

    return 10;
}

Int32 ServerPacketGetHeaderLength(
    UInt16 ProtocolIdentifier,
    UInt16 ProtocolVersion,
    UInt16 ProtocolExtension,
    Void* Packet
) {
    UInt16 PacketMagic = *((UInt16*)Packet);
    PacketMagic -= ProtocolIdentifier;
    PacketMagic -= ProtocolVersion;
    
    if (PacketMagic == ProtocolExtension) {
        return 8;
    }
    
    return 6;
}
