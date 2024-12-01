#include "Archive.h"
#include "Diagnostic.h"
#include "Dictionary.h"
#include "Encryption.h"
#include "FileIO.h"
#include "ParsePrimitives.h"

struct _ArchiveMemory {
    UInt8* Memory;
    Int32 Length;
};

struct _ArchiveNodeKey {
    Int32 NameIndex;
    Int32 ParentIndex;
};

struct _ArchiveNode {
    Int Index;
    struct _ArchiveNodeKey Key;
    ArrayRef AttributeIndices;
};

struct _ArchiveAttribute {
    Int Index;
    Int32 NodeIndex;
    Int32 NameIndex;
    Int32 DataIndex;
};

struct _Archive {
    AllocatorRef Allocator;
    ArrayRef NameTable;
    ArrayRef DataTable;
    ArrayRef Nodes;
    ArrayRef Attributes;
    DictionaryRef NodeBuckets;
};

typedef struct _ArchiveMemory* ArchiveMemoryRef;
typedef struct _ArchiveNode* ArchiveNodeRef;
typedef struct _ArchiveAttribute* ArchiveAttributeRef;

Bool _ArchiveNodeDictionaryKeyComparator(
    Void* Lhs,
    Void* Rhs
) {
    struct _ArchiveNodeKey* LhsKey = (struct _ArchiveNodeKey*)Lhs;
    struct _ArchiveNodeKey* RhsKey = (struct _ArchiveNodeKey*)Rhs;
    
    return LhsKey->NameIndex == RhsKey->NameIndex && LhsKey->ParentIndex == RhsKey->ParentIndex;
}

UInt64 _ArchiveNodeDictionaryKeyHasher(
    Void* Key
) {
    UInt64 Hash = 5381;
    Hash = Hash * 33 + ((struct _ArchiveNodeKey*)Key)->NameIndex;
    Hash = Hash * 33 + ((struct _ArchiveNodeKey*)Key)->ParentIndex;
    return Hash;
}

Int32 _ArchiveNodeDictionaryKeySizeCallback(
    Void* Key
) {
    return sizeof(struct _ArchiveNodeKey);
}

DictionaryRef ArchiveNodeDictionaryCreate(
    AllocatorRef Allocator,
    Int Capacity
) {
    return DictionaryCreate(
        Allocator,
        &_ArchiveNodeDictionaryKeyComparator,
        &_ArchiveNodeDictionaryKeyHasher,
        &_ArchiveNodeDictionaryKeySizeCallback,
        Capacity
    );
}

ArchiveRef ArchiveCreateEmpty(
    AllocatorRef Allocator
) {
    ArchiveRef Archive = (ArchiveRef)AllocatorAllocate(Allocator, sizeof(struct _Archive));
    if (!Archive) Fatal("Memory allocation failed!");

    Archive->Allocator = Allocator;
    Archive->NameTable = ArrayCreateEmpty(Archive->Allocator, sizeof(UInt8), 0x1000);
    Archive->DataTable = ArrayCreateEmpty(Archive->Allocator, sizeof(UInt8), 0x1000);
    Archive->Nodes = ArrayCreateEmpty(Archive->Allocator, sizeof(struct _ArchiveNode), 0x10);
    Archive->Attributes = ArrayCreateEmpty(Archive->Allocator, sizeof(struct _ArchiveAttribute), 0x10);
    Archive->NodeBuckets = ArchiveNodeDictionaryCreate(Archive->Allocator, 0x1000);
    return Archive;
}

Bool ArchiveLoadFromData(
    ArchiveRef Archive,
    UInt8* Data,
    Int32 DataLength,
    Bool IgnoreErrors
) {
    assert(Archive);

    if (!ArchiveParseFromSource(Archive, (CString)Data, DataLength, IgnoreErrors)) {
        goto error;
    }

    return true;

error:
    Error("Error loading archive from data");
    return false;
}

Bool ArchiveLoadFromFileEncrypted(
    ArchiveRef Archive,
    CString FilePath,
    Bool IgnoreErrors
) {
    assert(Archive);
    return ArchiveLoadFromFileEncryptedNoAlloc(Archive, FilePath, IgnoreErrors);
}

Bool ArchiveLoadFromFileEncryptedNoAlloc(
    ArchiveRef Archive,
    CString FilePath,
    Bool IgnoreErrors
) {
    assert(Archive);

    UInt8* Buffer = NULL;
    Int32 BufferLength = 0;

    if (!EncryptionDecryptFile(FilePath, &Buffer, &BufferLength)) {
        goto error;
    }

    if (!ArchiveParseFromSource(Archive, (CString)Buffer, BufferLength, IgnoreErrors)) {
        goto error;
    }

    free(Buffer);
    return true;

error:
    Error("Error loading archive: %s", FilePath);
    if (Buffer) free(Buffer);
    return false;
}

Bool ArchiveLoadFromFile(
    ArchiveRef Archive,
    CString FilePath,
    Bool IgnoreErrors
) {
    assert(Archive);

    UInt8 *Source = NULL;
    Int32 SourceLength = 0;
    FileRef File = FileOpen(FilePath);
    if (!File) goto error;
    if (!FileRead(File, &Source, &SourceLength)) goto error;
    
    FileClose(File);
    File = NULL;

    ArchiveClear(Archive, true);
    if (!ArchiveParseFromSource(Archive, (CString)Source, SourceLength, IgnoreErrors)) {
        goto error;
    }

    free(Source);
    return true;

error:
    if (File) FileClose(File);
    if (Source) free(Source);

    return false;
}

Void ArchiveDestroy(
    ArchiveRef Archive
) {
    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(Archive->NodeBuckets);
    while (Iterator.Key) {
        ArrayRef Bucket = (ArrayRef)DictionaryLookup(Archive->NodeBuckets, Iterator.Key);
        ArrayDealloc(Bucket);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    for (Int Index = 0; Index < ArrayGetElementCount(Archive->Nodes); Index += 1) {
        ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, Index);
        ArrayDestroy(Node->AttributeIndices);
    }

    ArrayDestroy(Archive->NameTable);
    ArrayDestroy(Archive->DataTable);
    ArrayDestroy(Archive->Nodes);
    ArrayDestroy(Archive->Attributes);
    DictionaryDestroy(Archive->NodeBuckets);
    AllocatorDeallocate(Archive->Allocator, Archive);
}

static inline Bool ArchiveNodeWriteToFile(
    ArchiveRef Archive,
    Int32 NodeIndex,
    FILE* File,
    Bool Prettify,
    Int32 Indentation
) {
    ArchiveStringRef NodeName = ArchiveNodeGetName(Archive, NodeIndex);
   
    if (Prettify)
        for (Int Index = 0; Index < Indentation * 4; Index++)
            fprintf(File, " ");

    fprintf(File, "<%.*s", (Int32)NodeName->Length, NodeName->Data);

    ArchiveIteratorRef Iterator = ArchiveAttributeIteratorFirst(Archive, NodeIndex);
    while (Iterator) {
        ArchiveStringRef AttributeName = ArchiveAttributeGetName(Archive, Iterator->Index);
        ArchiveStringRef AttributeData = ArchiveAttributeGetData(Archive, Iterator->Index);

        fprintf(File, " %.*s=\"%.*s\"", (Int32)AttributeName->Length, AttributeName->Data, (Int32)AttributeData->Length, AttributeData->Data);

        Iterator = ArchiveAttributeIteratorNext(Archive, Iterator);
    }

    Iterator = ArchiveNodeIteratorFirst(Archive, NodeIndex);
    if (!Iterator) {
        fprintf(File, " />");

        if (Prettify)
            fprintf(File, "\n");

        return true;
    }
    else {
        fprintf(File, ">");

        if (Prettify)
            fprintf(File, "\n");
    }

    while (Iterator) {
        if (!ArchiveNodeWriteToFile(Archive, Iterator->Index, File, Prettify, Indentation + 1)) return false;

        Iterator = ArchiveNodeIteratorNext(Archive, Iterator);
    }

    if (Prettify)
        for (Int Index = 0; Index < Indentation * 4; Index++)
            fprintf(File, " ");

    fprintf(File, "</%.*s>", (Int32)NodeName->Length, NodeName->Data);

    if (Prettify)
        fprintf(File, "\n");

    return true;
}

Bool ArchiveWriteToFileHandle(
    ArchiveRef Archive,
    FILE* File,
    Bool Prettify,
    Int32 Indentation
) {
    if (!File) goto error;

    ArchiveIteratorRef Iterator = ArchiveNodeIteratorFirst(Archive, -1);
    while (Iterator) {
        if (!ArchiveNodeWriteToFile(Archive, Iterator->Index, File, Prettify, Indentation)) return false;

        Iterator = ArchiveNodeIteratorNext(Archive, Iterator);
    }

    return true;

error:
    return false;
}

Bool ArchiveWriteToFile(
    ArchiveRef Archive,
    CString FilePath,
    Bool Prettify
) {
    FILE* File = fopen(FilePath, "w");
    if (!File) goto error;

    ArchiveIteratorRef Iterator = ArchiveNodeIteratorFirst(Archive, -1);
    while (Iterator) {
        if (!ArchiveNodeWriteToFile(Archive, Iterator->Index, File, Prettify, 0)) return false;

        Iterator = ArchiveNodeIteratorNext(Archive, Iterator);
    }

    fclose(File);
    return true;

error:
    if (File) fclose(File);

    return false;
}

Void ArchiveClear(
    ArchiveRef Archive,
    Bool KeepCapacity
) {
    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(Archive->NodeBuckets);
    while (Iterator.Key) {
        ArrayRef Bucket = (ArrayRef)DictionaryLookup(Archive->NodeBuckets, Iterator.Key);
        ArrayDealloc(Bucket);
        Iterator = DictionaryKeyIteratorNext(Iterator);
    }
    DictionaryRemoveAll(Archive->NodeBuckets);

    for (Int Index = 0; Index < ArrayGetElementCount(Archive->Nodes); Index += 1) {
        ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, Index);
        ArrayDestroy(Node->AttributeIndices);
    }

    ArrayRemoveAllElements(Archive->NameTable, KeepCapacity);
    ArrayRemoveAllElements(Archive->DataTable, KeepCapacity);
    ArrayRemoveAllElements(Archive->Nodes, KeepCapacity);
    ArrayRemoveAllElements(Archive->Attributes, KeepCapacity);
}

static inline Bool ArchiveContainsNode(
    ArchiveRef Archive,
    Int32 NodeIndex
) {
    return 0 <= NodeIndex && NodeIndex < ArrayGetElementCount(Archive->Nodes);
}

static inline Int32 ArchiveAddName(
    ArchiveRef Archive,
    CString Name,
    Int32 Length
) {
    Int Index = (Int32)ArrayGetElementCount(Archive->NameTable);
    Bool IsZeroTerminated = Length > 0 && Name[Length - 1] == 0;
    Int32 StringLength = IsZeroTerminated ? Length : Length + 1;

    ArrayAppendMemory(Archive->NameTable, (UInt8*)&StringLength, sizeof(Int32));

    if (Length > 0) {
        ArrayAppendMemory(Archive->NameTable, Name, Length);
    }

    if (!IsZeroTerminated) {
        UInt8 Zero = 0;
        ArrayAppendMemory(Archive->NameTable, &Zero, 1);
    }

    return Index;
}

static inline Int32 ArchiveLookupName(
    ArchiveRef Archive,
    CString Name,
    Int32 Length
) {
    if (ArrayGetElementCount(Archive->NameTable) < 1) return -1;
    
    UInt8* Start = ArrayGetElementAtIndex(Archive->NameTable, 0);
    UInt8* Cursor = Start;
    UInt8* End = ArrayGetElementAtIndex(Archive->NameTable, ArrayGetElementCount(Archive->NameTable) - 1);
    Bool IsZeroTerminated = (Length < 1) ? true : Name[Length - 1] == 0;

    while (Cursor <= End) {
        ArchiveStringRef String = (ArchiveStringRef)Cursor;
        if ((IsZeroTerminated && String->Length == Length) ||
            (!IsZeroTerminated && String->Length - 1 == Length)) {
            if (memcmp(Name, String->Data, MIN(String->Length, Length)) == 0) {
                return (Int32)(Cursor - Start);
            }
        }

        Cursor += sizeof(Int32) + String->Length;
    }

    return -1;
}

static inline Int32 ArchiveAddData(
    ArchiveRef Archive,
    CString Data,
    Int32 Length
) {
    Int Index = (Int32)ArrayGetElementCount(Archive->DataTable);
    Bool IsZeroTerminated = Length > 0 && Data[Length - 1] == 0;
    Int32 StringLength = IsZeroTerminated ? Length : Length + 1;

    ArrayAppendMemory(Archive->DataTable, (UInt8*)&StringLength, sizeof(Int32));

    if (Length > 0) {
        ArrayAppendMemory(Archive->DataTable, Data, Length);
    }

    if (!IsZeroTerminated) {
        UInt8 Zero = 0;
        ArrayAppendMemory(Archive->DataTable, &Zero, 1);
    }

    return Index;
}

Int32 ArchiveAddNode(
    ArchiveRef Archive,
    Int32 ParentIndex,
    CString Name,
    Int32 NameLength
) {
    assert(ParentIndex == -1 || ArchiveContainsNode(Archive, ParentIndex));
   
    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayAppendUninitializedElement(Archive->Nodes);
    Node->Index = (Int32)ArrayGetElementCount(Archive->Nodes) - 1;
    Node->Key.ParentIndex = ParentIndex;
    Node->Key.NameIndex = ArchiveLookupName(Archive, Name, NameLength);
    if (Node->Key.NameIndex < 0) {
        Node->Key.NameIndex = ArchiveAddName(Archive, Name, NameLength);
    }
    assert(0 <= Node->Key.NameIndex);
 
    Node->AttributeIndices = ArrayCreateEmpty(Archive->Allocator, sizeof(Int32), 64);

    ArrayRef Bucket = (ArrayRef)DictionaryLookup(Archive->NodeBuckets, &Node->Key);
    if (!Bucket) {
        struct _Array NewBucket = { 0 };
        ArrayInitializeEmpty(&NewBucket, Archive->Allocator, sizeof(Int32), 8);
        DictionaryInsert(Archive->NodeBuckets, &Node->Key, &NewBucket, sizeof(struct _Array));
        Bucket = (ArrayRef)DictionaryLookup(Archive->NodeBuckets, &Node->Key);
        assert(Bucket);
    }

    ArrayAppendElement(Bucket, &Node->Index);

    return Node->Index;
}

ArchiveStringRef ArchiveNodeGetName(
    ArchiveRef Archive,
    Int32 NodeIndex
) {
    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, NodeIndex);
    return (ArchiveStringRef)ArrayGetElementAtIndex(Archive->NameTable, Node->Key.NameIndex);
}

Int32 ArchiveNodeGetParent(
    ArchiveRef Archive,
    Int32 NodeIndex
) {
    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, NodeIndex);
    return Node->Key.ParentIndex;
}

Int32 ArchiveNodeGetChildByPath(
    ArchiveRef Archive,
    Int32 NodeIndex,
    CString Path
) {
    CString Cursor = strchr(Path, '.');
    if (!Cursor) {
        ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(
            Archive,
            NodeIndex,
            Path
        );

        return Iterator ? Iterator->Index : -1;
    }

    Int32 Length = (Int32)(Cursor - Path);
    if (Length < 1) return -1;

    Char Name[64];
    assert(Length < 64);
    memcpy(Name, Path, Length);
    Name[Length] = '\0';

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(
        Archive, 
        NodeIndex, 
        Name
    );

    if (!Iterator) return -1;

    Int32 Offset = Length + 1;
    Length = (Int32)strlen(Path) - Offset;
    memcpy(Name, Path + Offset, Length);
    Name[Length] = '\0';

    return ArchiveNodeGetChildByPath(Archive, Iterator->Index, Name);
}

Int32 _ArchiveNodeGetAttributeByNameWithLength(
    ArchiveRef Archive,
    Int32 NodeIndex,
    CString Name,
    Int32 NameLength
) {
    Int32 NameIndex = ArchiveLookupName(Archive, Name, NameLength);
    if (NameIndex < 0) return -1;

    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, NodeIndex);

    for (Int Index = 0; Index < ArrayGetElementCount(Node->AttributeIndices); Index++) {
        Int32 AttributeIndex = *(Int32*)ArrayGetElementAtIndex(Node->AttributeIndices, Index);

        ArchiveAttributeRef Attribute = (ArchiveAttributeRef)ArrayGetElementAtIndex(
            Archive->Attributes,
            AttributeIndex
        );

        if (Attribute->NameIndex == NameIndex) {
            return Attribute->Index;
        }
    }

    return -1;
}

Int32 ArchiveNodeAddAttribute(
    ArchiveRef Archive,
    Int32 NodeIndex,
    CString Name,
    Int32 NameLength,
    CString Data,
    Int32 DataLength
) {
    assert(ArchiveContainsNode(Archive, NodeIndex));

    ArchiveAttributeRef Attribute = (ArchiveAttributeRef)ArrayAppendUninitializedElement(Archive->Attributes);
    Attribute->Index = (Int32)ArrayGetElementCount(Archive->Attributes) - 1;
    Attribute->NodeIndex = NodeIndex;
    Attribute->NameIndex = ArchiveLookupName(Archive, Name, NameLength);
    if (Attribute->NameIndex < 0) {
        Attribute->NameIndex = ArchiveAddName(Archive, Name, NameLength);
    }
    assert(0 <= Attribute->NameIndex);

    Attribute->DataIndex = ArchiveAddData(Archive, Data, DataLength);
    assert(0 <= Attribute->DataIndex);

    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, NodeIndex);
    ArrayAppendElement(Node->AttributeIndices, &Attribute->Index);

    return Attribute->Index;
}

Int32 ArchiveNodeGetAttributeByName(
    ArchiveRef Archive,
    Int32 NodeIndex,
    CString Name
) {
    return _ArchiveNodeGetAttributeByNameWithLength(Archive, NodeIndex, Name, (Int32)strlen(Name));
}

ArchiveStringRef ArchiveAttributeGetName(
    ArchiveRef Archive,
    Int32 AttributeIndex
) {
    ArchiveAttributeRef Attribute = (ArchiveAttributeRef)ArrayGetElementAtIndex(
        Archive->Attributes, 
        AttributeIndex
    );
    return (ArchiveStringRef)ArrayGetElementAtIndex(Archive->NameTable, Attribute->NameIndex);
}

ArchiveStringRef ArchiveAttributeGetData(
    ArchiveRef Archive,
    Int32 AttributeIndex
) {
    ArchiveAttributeRef Attribute = (ArchiveAttributeRef)ArrayGetElementAtIndex(
        Archive->Attributes,
        AttributeIndex
    );
    return (ArchiveStringRef)ArrayGetElementAtIndex(Archive->DataTable, Attribute->DataIndex);
}

Int32 ArchiveQueryNodeWithAttribute(
    ArchiveRef Archive,
    Int32 ParentIndex,
    CString Query,
    CString AttributeName,
    CString AttributeValue
) {
    Int32 NodeIndex = ArchiveNodeGetChildByPath(Archive, ParentIndex, Query);
    if (NodeIndex < 0) return -1;

    Int32 QueryIndex = ArchiveNodeGetParent(Archive, NodeIndex);
    CString SubQuery = strrchr(Query, '.');
    if (SubQuery) {
        SubQuery += 1;
    } else {
        SubQuery = Query;
    }

    Char Value[MAX_PATH] = { 0 };

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, QueryIndex, SubQuery);
    while (Iterator) {
        if (ParseAttributeString(Archive, Iterator->Index, AttributeName, Value, MAX_PATH) &&
            strcmp(AttributeValue, Value) == 0) {
            return Iterator->Index;
        }

        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return -1;
}

ArchiveIteratorRef ArchiveQueryNodeIteratorSingleFirst(
    ArchiveRef Archive,
    Int32 ParentIndex,
    CString Query
) {
    Int32 NameIndex = ArchiveLookupName(Archive, Query, (Int32)strlen(Query));
    if (NameIndex < 0) return NULL;

    struct _ArchiveNodeKey Key = { .NameIndex = NameIndex, .ParentIndex = ParentIndex };
    ArrayRef Bucket = DictionaryLookup(Archive->NodeBuckets, &Key);
    if (Bucket && ArrayGetElementCount(Bucket) > 0) {
        Int32 NodeIndex = *(Int32*)ArrayGetElementAtIndex(Bucket, 0);
        return (ArchiveIteratorRef)ArrayGetElementAtIndex(Archive->Nodes, NodeIndex);
    }

    return NULL;
}

ArchiveIteratorRef ArchiveQueryNodeIteratorFirst(
    ArchiveRef Archive,
    Int32 NodeIndex,
    CString Path
) {
    CString Cursor = strchr(Path, '.');
    if (!Cursor) {
        return ArchiveQueryNodeIteratorSingleFirst(
            Archive,
            NodeIndex,
            Path
        );
    }

    Int32 Length = (Int32)(Cursor - Path);
    if (Length < 1) return NULL;

    Char Name[MAX_PATH];
    assert(Length < 64);
    memcpy(Name, Path, Length);
    Name[Length] = '\0';

    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorSingleFirst(
        Archive,
        NodeIndex,
        Name
    );

    if (!Iterator) return NULL;

    Int32 Offset = Length + 1;
    Length = (Int32)strlen(Path) - Offset;
    memcpy(Name, Path + Offset, Length);
    Name[Length] = '\0';
    
    return ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, Name);
}

ArchiveIteratorRef ArchiveQueryNodeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
) {
    assert(Iterator);

    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, Iterator->Index);
    ArrayRef Bucket = DictionaryLookup(Archive->NodeBuckets, &Node->Key);
    assert(Bucket);

    for (Int Index = 0; Index < ArrayGetElementCount(Bucket); Index += 1) {
        Int32 NodeIndex = *(Int32*)ArrayGetElementAtIndex(Bucket, Index);
        if (NodeIndex != Iterator->Index) continue;

        if (Index + 1 < ArrayGetElementCount(Bucket)) {
            NodeIndex = *(Int32*)ArrayGetElementAtIndex(Bucket, Index + 1);
            return (ArchiveIteratorRef)ArrayGetElementAtIndex(Archive->Nodes, NodeIndex);
        }

        return NULL;
    }

    return NULL;
}

Int32 ArchiveQueryNodeCount(
    ArchiveRef Archive,
    Int32 ParentIndex,
    CString Query
) {
    Int32 Result = 0;
    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, ParentIndex, Query);
    while (Iterator) {
        Result += 1;
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator);
    }

    return Result;
}

ArchiveIteratorRef ArchiveNodeIteratorFirst(
    ArchiveRef Archive,
    Int32 ParentIndex
) {
    for (Int Index = 0; Index < ArrayGetElementCount(Archive->Nodes); Index++) {
        ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, Index);
        if (Node->Key.ParentIndex == ParentIndex) {
            return (ArchiveIteratorRef)Node;
        }
    }

    return NULL;
}

ArchiveIteratorRef ArchiveNodeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
) {
    assert(Iterator);

    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, Iterator->Index);
    for (Int Index = Iterator->Index + 1; Index < ArrayGetElementCount(Archive->Nodes); Index += 1) {
        ArchiveNodeRef Next = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, Index);
        if (Next->Key.ParentIndex == Node->Key.ParentIndex) {
            return (ArchiveIteratorRef)Next;
        }
    }

    return NULL;
}

ArchiveIteratorRef ArchiveAttributeIteratorFirst(
    ArchiveRef Archive,
    Int32 NodeIndex
) {
    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(Archive->Nodes, NodeIndex);

    if (ArrayGetElementCount(Node->AttributeIndices) < 1) return NULL;

    return (ArchiveIteratorRef)ArrayGetElementAtIndex(Node->AttributeIndices, 0);
}

ArchiveIteratorRef ArchiveAttributeIteratorNext(
    ArchiveRef Archive,
    ArchiveIteratorRef Iterator
) {
    assert(Iterator);

    ArchiveAttributeRef Attribute = (ArchiveAttributeRef)ArrayGetElementAtIndex(
        Archive->Attributes,
        Iterator->Index
    );
    ArchiveNodeRef Node = (ArchiveNodeRef)ArrayGetElementAtIndex(
        Archive->Nodes, 
        Attribute->NodeIndex
    );

    Int32 FirstAttributeIndex = *(Int32*)ArrayGetElementAtIndex(Node->AttributeIndices, 0);
    ArchiveAttributeRef FirstAttribute = (ArchiveAttributeRef)ArrayGetElementAtIndex(
        Archive->Attributes,
        FirstAttributeIndex
    );

    Int32 NextIndex = (Int32)(Attribute - FirstAttribute) / sizeof(struct _ArchiveAttribute) + 1;

    if (NextIndex >= ArrayGetElementCount(Node->AttributeIndices)) return NULL;

    return (ArchiveIteratorRef)ArrayGetElementAtIndex(Node->AttributeIndices, NextIndex);
}

CString ArchiveQueryGetChildName(
    CString Query
) {
    CString Cursor = strrchr(Query, '.');
    return Cursor ? Cursor + 1 : Query;
}
