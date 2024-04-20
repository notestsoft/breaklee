#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define C2S_DATA_SIGNATURE_DEFAULT  \
UInt16 Magic;                       \
UInt16 Length;                      \
UInt32 Checksum;                    \
UInt16 Command

#define S2C_DATA_SIGNATURE_DEFAULT	\
UInt16 Magic;                       \
UInt16 Length;                      \
UInt16 Command						\

#define S2C_DATA_SIGNATURE_EXTENDED	\
UInt16 Magic;                       \
UInt32 Length;                      \
UInt16 Command

#pragma pack(pop)

EXTERN_C_END
