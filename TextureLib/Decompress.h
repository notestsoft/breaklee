#pragma once

#include "Base.h"

EXTERN_C_BEGIN

Bool DecompressDDSTextureFromMemory(
    AllocatorRef Allocator,
    UInt8* Data,
    Int32 DataLength,
    Int32* OutWidth,
    Int32* OutHeight,
    UInt8** RawData,
    Int32* RawDataLength
);

EXTERN_C_END