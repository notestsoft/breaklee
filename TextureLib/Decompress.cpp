#include "Decompress.h"

#include <squish.h>

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |       \
                ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif

#pragma pack(push,1)

#define DDS_FOURCC          0x00000004  // DDPF_FOURCC
#define DDS_RGB             0x00000040  // DDPF_RGB
#define DDS_LUMINANCE       0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA           0x00000002  // DDPF_ALPHA
#define DDS_BUMPDUDV        0x00080000  // DDPF_BUMPDUDV
#define DDS_BUMPLUMINANCE   0x00040000  // DDPF_BUMPLUMINANCE

#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
                               DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
                               DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ )

#define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP

static UInt32 DDS_MAGIC = 0x20534444; // "DDS "

struct DDS_PIXELFORMAT {
    UInt32 size;
    UInt32 flags;
    UInt32 fourCC;
    UInt32 RGBBitCount;
    UInt32 RBitMask;
    UInt32 GBitMask;
    UInt32 BBitMask;
    UInt32 ABitMask;
};

struct DDS_HEADER
{
    UInt32 size;
    UInt32 flags;
    UInt32 height;
    UInt32 width;
    UInt32 pitchOrLinearSize;
    UInt32 depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
    UInt32 mipMapCount;
    UInt32 reserved1[11];
    struct DDS_PIXELFORMAT ddspf;
    UInt32 caps;
    UInt32 caps2;
    UInt32 caps3;
    UInt32 caps4;
    UInt32 reserved2;
};

#pragma pack(pop)

Bool DecompressDDSTextureFromMemory(
    AllocatorRef Allocator,
    UInt8* Data,
    Int32 DataLength,
    Int32* OutWidth,
    Int32* OutHeight,
    UInt8** RawData,
    Int32* RawDataLength
) {
    if (!Data || DataLength <= sizeof(DDS_HEADER)) return false;

    UInt32* Magic = reinterpret_cast<UInt32*>(Data);
    if (*Magic != DDS_MAGIC) return false;

    DDS_HEADER* Header = reinterpret_cast<DDS_HEADER*>(Data + sizeof(uint32_t));
    UInt32 Width = Header->width;
    UInt32 Height = Header->height;
    UInt32 FourCC = Header->ddspf.fourCC;

    *OutWidth = Width;
    *OutHeight = Height;

    Int32 SquishFormat = 0;
    if (FourCC == MAKEFOURCC('D', 'X', 'T', '1')) {
        SquishFormat = squish::kDxt1;
    }
    else if (FourCC == MAKEFOURCC('D', 'X', 'T', '3')) {
        SquishFormat = squish::kDxt3;
    }
    else if (FourCC == MAKEFOURCC('D', 'X', 'T', '5')) {
        SquishFormat = squish::kDxt5;
    }
    else if (FourCC == MAKEFOURCC('A', 'T', 'I', '1') || FourCC == MAKEFOURCC('B', 'C', '4', 'U')) {
        SquishFormat = squish::kBc4;
    }
    else if (FourCC == MAKEFOURCC('A', 'T', 'I', '2') || FourCC == MAKEFOURCC('B', 'C', '5', 'U')) {
        SquishFormat = squish::kBc5;
    }
    else {
        return false;
    }

    *RawDataLength = Width * Height * sizeof(UInt32);
    *RawData = (UInt8*)AllocatorAllocate(Allocator, *RawDataLength);
    if (!*RawData) Fatal("Memory allocation failed!");

    UInt8* CompressedData = Data + sizeof(UInt32) + sizeof(DDS_HEADER);
    Int32 CompressedDataLength = DataLength - sizeof(UInt32) - sizeof(DDS_HEADER);
    squish::DecompressImage((squish::u8*)*RawData, Width, Height, CompressedData, SquishFormat);

    return true;
}