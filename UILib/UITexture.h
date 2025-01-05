#pragma once

#include "Base.h"

EXTERN_C_BEGIN

struct _UITexture {
    Char FileName[PLATFORM_PATH_MAX];
    Int32 Width;
    Int32 Height;
    UIHandleRef Handle;
};

EXTERN_C_END