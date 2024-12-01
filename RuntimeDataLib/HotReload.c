#include "RuntimeDataLoader.h"

ArchiveRef RTRuntimeDataLoadArchive(
    CString RuntimeDataPath,
    CString ServerDataPath,
    CString FileName
) {
    AllocatorRef Allocator = AllocatorGetSystemDefault();                           
    ArchiveRef Archive = ArchiveCreateEmpty(Allocator);                             
    Bool IgnoreErrors = true;                                                       
    ArchiveIteratorRef Iterator = NULL;                                             
    Bool Success = false;                                                           

    if (CStringIsEqual(PathGetFileNameExtension(FileName), "xml")) {
        Success = ArchiveLoadFromFile(
            Archive, 
            PathCombineNoAlloc(ServerDataPath, FileName),
            IgnoreErrors
        );
    }
    else {
        Success = ArchiveLoadFromFileEncryptedNoAlloc(
            Archive, 
            PathCombineNoAlloc(RuntimeDataPath, FileName), 
            IgnoreErrors 
        ); 
    } 

    if (!Success) {
        Error("Error loading runtime data: %s", FileName);
        ArchiveDestroy(Archive);
        return NULL;
    }

    return Archive;
}

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__)			                        \
Void CONCAT(RTRuntimeData, __NAME__ ## Deallocate)(                                     \
	RTRuntimeDataContextRef Context 									                \
) {                                                                                     \
    assert(Context);                                                                    \
    CString Query = __QUERY__;                                                          \
    CString PropertyQuery = NULL;                                                       \
    for (Int Index = 0; Index < Context->CONCAT(__NAME__, Count); Index += 1) {       \
        CONCAT(RTData, __NAME__ ## Ref) Data = &Context->CONCAT(__NAME__, List)[Index]; \

#define RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__)

#define RUNTIME_DATA_PROPERTY_PRECONDITION(__TYPE__, __NAME__, __QUERY__, __VALUE__) 

#define RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__)

#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__)                                                      \
        {                                                                                                       \
            for (Int ChildIndex = 0; ChildIndex < Data->CONCAT(__NAME__, Count); ChildIndex += 1) {           \
                CONCAT(RTData, __NAME__ ## Ref) ChildData = &Data->CONCAT(__NAME__, List)[ChildIndex];          \
                {                                                                                               \
                    CONCAT(RTData, __NAME__ ## Ref) Data = ChildData;

#define RUNTIME_DATA_TYPE_END_CHILD(__NAME__)                                       \
                }                                                                   \
            }                                                                       \
            AllocatorDeallocate(Context->Allocator, Data->CONCAT(__NAME__, List));  \
        }

#define RUNTIME_DATA_TYPE_END(__NAME__)                                             \
        AllocatorDeallocate(Context->Allocator, Context->CONCAT(__NAME__, List));   \
    }                                                                               \
}
#include "Macro.h"

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__)			                                            \
Bool CONCAT(RTRuntimeData, __NAME__ ## HotReload)(                                                          \
	RTRuntimeDataContextRef Context,									                                    \
	CString FileName													                                    \
) {                                                                                                         \
    assert(Context);                                                                                        \
    ArchiveRef Archive = RTRuntimeDataLoadArchive(Context->RuntimeDataPath, Context->ServerDataPath, FileName); \
    Info("Hot reloading runtime data: %s", # __NAME__);                                                     \
    if (!Archive) return false;                                                                             \
    CONCAT(RTRuntimeData, __NAME__ ## Deallocate)(Context);                                                 \
    CString Query = __QUERY__;                                                                              \
    CString PropertyQuery = NULL;                                                                           \
    Context->CONCAT(__NAME__, Count) = ArchiveQueryNodeCount(Archive, -1, Query);                           \
    Int MemorySize = MAX(1, sizeof(struct CONCAT(_RTData, __NAME__)) * Context->CONCAT(__NAME__, Count));   \
    Trace("Data %s memory size: %llu total: %.3f KB", #__NAME__, (UInt64)MemorySize, (Float32)MemorySize / 1000);   \
    Context->CONCAT(__NAME__, List) = (CONCAT(RTData, __NAME__ ## Ref))(AllocatorAllocate(Context->Allocator, MemorySize)); \
    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorFirst(Archive, -1, Query);                        \
    if (!Iterator) Error("Element not found for query: %s\n", Query);                                       \
    Int IteratorIndex = 0;                                                                                  \
    while (Iterator) {                                                                                      \
        assert(IteratorIndex < Context->CONCAT(__NAME__, Count));                                           \
        CONCAT(RTData, __NAME__ ## Ref) Data = &Context->CONCAT(__NAME__, List)[IteratorIndex];             \
        IteratorIndex += 1;

#define RUNTIME_DATA_PROPERTY(__TYPE__, __NAME__, __QUERY__) \
        PropertyQuery = __QUERY__; \
        if (!CONCAT(ParseAttribute, __TYPE__)(Archive, Iterator->Index, PropertyQuery, &Data->__NAME__)) { \
            Error("Couldn't parse attribute %s in %s\n", PropertyQuery, Query); \
        }

#define RUNTIME_DATA_PROPERTY_PRECONDITION(__TYPE__, __NAME__, __QUERY__, __VALUE__) \
        PropertyQuery = __QUERY__; \
        if (!CONCAT(ParseAttribute, __TYPE__ ## Equal)(Archive, Iterator->Index, PropertyQuery, __VALUE__)) { \
            Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator); \
            continue; \
        }

#define RUNTIME_DATA_PROPERTY_ARRAY(__TYPE__, __NAME__, __QUERY__, __COUNT__, __SEPARATOR__) \
        PropertyQuery = __QUERY__; \
        Data->CONCAT(__NAME__, Count) = CONCAT(ParseAttribute, __TYPE__ ## ArrayCounted)(Archive, Iterator->Index, PropertyQuery, Data->__NAME__, __COUNT__, __SEPARATOR__);

#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__)                                                                          \
        {                                                                                                                           \
            CString Query = __QUERY__;                                                                                              \
            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, Query);                      \
            Data->CONCAT(__NAME__, Count) = ArchiveQueryNodeCount(Archive, Iterator->Index, Query);                                 \
            Int MemorySize = MAX(1, sizeof(struct CONCAT(_RTData, __NAME__)) * Data->CONCAT(__NAME__, Count));                      \
            Data->CONCAT(__NAME__, List) = (CONCAT(RTData, __NAME__ ## Ref))(AllocatorAllocate(Context->Allocator, MemorySize));    \
            Int ChildIteratorIndex = 0;                                                                                             \
            while (ChildIterator) {                                                                                                 \
                ArchiveIteratorRef Iterator = ChildIterator;                                                                        \
                assert(ChildIteratorIndex < Data->CONCAT(__NAME__, Count));                                                         \
                CONCAT(RTData, __NAME__ ## Ref) ChildData = &Data->CONCAT(__NAME__, List)[ChildIteratorIndex];                      \
                ChildIteratorIndex += 1;                                                                                            \
                {                                                                                                                   \
                    CONCAT(RTData, __NAME__ ## Ref) Data = ChildData;

#define RUNTIME_DATA_TYPE_END_CHILD(__NAME__)                                           \
                }                                                                       \
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator);   \
            }                                                                           \
        }

#define RUNTIME_DATA_TYPE_END(__NAME__)                             \
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator); \
    }                                                               \
    ArchiveDestroy(Archive);                                        \
    return true;                                                    \
}
#include "Macro.h"
