#pragma once

#include "Base.h"
#include "Array.h"

EXTERN_C_BEGIN

struct _ArchiveString {
    Int64 Length;
    Char Data[0];
};

struct _ArchiveIterator {
    Int64 Index;
};

typedef struct _Archive* ArchiveRef;
typedef struct _ArchiveString* ArchiveStringRef;
typedef struct _ArchiveIterator* ArchiveIteratorRef;

ArchiveRef ArchiveCreateEmpty(
    AllocatorRef Allocator
);

Bool ArchiveLoadFromData(
    ArchiveRef Archive,
    UInt8* Data,
    Int32 DataLength,
    Bool IgnoreErrors
);

Bool ArchiveLoadFromFileEncrypted(
    ArchiveRef Archive,
    CString FilePath,
    Bool IgnoreErrors
);

Bool ArchiveLoadFromFileEncryptedNoAlloc(
    ArchiveRef Archive,
    CString FilePath,
    Bool IgnoreErrors
);

Bool ArchiveLoadFromFile(
    ArchiveRef Archive,
    CString FilePath,
    Bool IgnoreErrors
);

Void ArchiveDestroy(
    ArchiveRef Archive
);

Bool ArchiveWriteToFileHandle(
    ArchiveRef Archive,
    FILE* File,
    Bool Prettify,
    Int64 Indentation
);

Bool ArchiveWriteToFile(
    ArchiveRef Archive,
    CString FilePath,
    Bool Prettify
);

Bool ArchiveParseFromSource(
    ArchiveRef Archive,
    CString Source,
    Int64 Length,
    Bool IgnoreErrors
);

Void ArchiveClear(
    ArchiveRef Archive,
    Bool KeepCapacity
);

Int64 ArchiveAddNode(
    ArchiveRef Archive,
    Int64 ParentIndex,
    CString Name,
    Int64 NameLength
);

ArchiveStringRef ArchiveNodeGetName(
    ArchiveRef Archive,
    Int64 NodeIndex
);

Int64 ArchiveNodeGetParent(
    ArchiveRef Archive,
    Int64 NodeIndex
);

Int64 ArchiveNodeGetChildByPath(
    ArchiveRef Archive,
    Int64 NodeIndex,
    CString Path
);

Int64 ArchiveNodeAddAttribute(
    ArchiveRef Archive,
    Int64 NodeIndex,
    CString Name,
    Int64 NameLength,
    CString Data,
    Int64 DataLength
);

Int64 ArchiveNodeGetAttributeByName(
    ArchiveRef Archive,
    Int64 NodeIndex,
    CString Name
);

ArchiveStringRef ArchiveAttributeGetName(
    ArchiveRef Archive,
    Int64 AttributeIndex
);

ArchiveStringRef ArchiveAttributeGetData(
    ArchiveRef Archive,
    Int64 AttributeIndex
);

Int64 ArchiveQueryNodeWithAttribute(
    ArchiveRef Archive,
    Int64 ParentIndex,
    CString Query,
    CString AttributeName,
    CString AttributeValue
);

ArchiveIteratorRef ArchiveQueryNodeIteratorFirst(
    ArchiveRef Archive,
    Int64 ParentIndex,
    CString Query
);

ArchiveIteratorRef ArchiveQueryNodeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
);

Int32 ArchiveQueryNodeCount(
    ArchiveRef Archive,
    Int64 ParentIndex,
    CString Query
);

ArchiveIteratorRef ArchiveNodeIteratorFirst(
    ArchiveRef Archive,
    Int64 ParentIndex
);

ArchiveIteratorRef ArchiveNodeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
);

ArchiveIteratorRef ArchiveAttributeIteratorFirst(
    ArchiveRef Archive,
    Int64 NodeIndex
);

ArchiveIteratorRef ArchiveAttributeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
);

CString ArchiveQueryGetChildName(
    CString Query
);

EXTERN_C_END