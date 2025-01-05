#pragma once

#include "Base.h"
#include "Allocator.h"

EXTERN_C_BEGIN

#define TABLESET_MAX_NAME_LENGTH 64

typedef struct _CLTableSet* CLTableSetRef;

CLTableSetRef CLTableSetCreateEmpty(
    AllocatorRef Allocator
);

Void CLTableSetDestroy(
    CLTableSetRef TableSet
);

Bool CLTableSetLoadFromFile(
    CLTableSetRef TableSet,
    CString FilePath
);

Bool CLTableSetLoadFromSource(
    CLTableSetRef TableSet,
    CString Source
);

Bool CLTableSetWriteToFile(
    CLTableSetRef TableSet,
    CString FilePath
);

EXTERN_C_END