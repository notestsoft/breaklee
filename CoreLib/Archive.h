#pragma once

#include "Base.h"
#include "Array.h"

EXTERN_C_BEGIN

#define ARCHIVE_MAX_NODE_DEPTH 8

struct _ArchiveString {
    Int32 Length;
    Char Data[0];
};

struct _ArchiveIterator {
    Int Index;
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
    Int32 Indentation
);

Bool ArchiveWriteToFile(
    ArchiveRef Archive,
    CString FilePath,
    Bool Prettify
);

Bool ArchiveParseFromSource(
    ArchiveRef Archive,
    CString Source,
    Int32 Length,
    Bool IgnoreErrors
);

Void ArchiveClear(
    ArchiveRef Archive,
    Bool KeepCapacity
);

Int32 ArchiveAddNode(
    ArchiveRef Archive,
    Int32 ParentIndex,
    CString Name,
    Int32 NameLength
);

ArchiveStringRef ArchiveNodeGetName(
    ArchiveRef Archive,
    Int32 NodeIndex
);

Int32 ArchiveNodeGetParent(
    ArchiveRef Archive,
    Int32 NodeIndex
);

Int32 ArchiveNodeGetChildByPath(
    ArchiveRef Archive,
    Int32 NodeIndex,
    CString Path
);

Int32 ArchiveNodeAddAttribute(
    ArchiveRef Archive,
    Int32 NodeIndex,
    CString Name,
    Int32 NameLength,
    CString Data,
    Int32 DataLength
);

Int32 ArchiveNodeGetAttributeByName(
    ArchiveRef Archive,
    Int32 NodeIndex,
    CString Name
);

ArchiveStringRef ArchiveAttributeGetName(
    ArchiveRef Archive,
    Int32 AttributeIndex
);

ArchiveStringRef ArchiveAttributeGetData(
    ArchiveRef Archive,
    Int32 AttributeIndex
);

Int32 ArchiveQueryNodeWithAttribute(
    ArchiveRef Archive,
    Int32 ParentIndex,
    CString Query,
    CString AttributeName,
    CString AttributeValue
);

ArchiveIteratorRef ArchiveQueryNodeIteratorFirst(
    ArchiveRef Archive,
    Int32 ParentIndex,
    CString Query
);

ArchiveIteratorRef ArchiveQueryNodeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
);

Int32 ArchiveQueryNodeCount(
    ArchiveRef Archive,
    Int32 ParentIndex,
    CString Query
);

ArchiveIteratorRef ArchiveNodeIteratorFirst(
    ArchiveRef Archive,
    Int32 ParentIndex
);

ArchiveIteratorRef ArchiveNodeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
);

ArchiveIteratorRef ArchiveAttributeIteratorFirst(
    ArchiveRef Archive,
    Int32 NodeIndex
);

ArchiveIteratorRef ArchiveAttributeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
);

CString ArchiveQueryGetChildName(
    CString Query
);

EXTERN_C_END