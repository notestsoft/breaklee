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

#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__)			            \
Bool CONCAT(RTRuntimeData, __NAME__ ## HotReload)(                                  \
	RTRuntimeDataContextRef Context,									            \
    CString RuntimeDataPath,												        \
	CString ServerDataPath,													        \
	CString FileName													            \
) {                                                                                 \
    assert(Context);                                                                \
    ArchiveRef Archive = RTRuntimeDataLoadArchive(                                  \
        RuntimeDataPath,                                                            \
        ServerDataPath,                                                             \
        FileName                                                                    \
    );                                                                              \
    Info("Hot reloading runtime data: %s", # __NAME__); \
    if (!Archive) return false;                                                     \
                                                                                    \
    CString Query = __QUERY__;                                                      \
    CString PropertyQuery = NULL;                                                   \
    ArchiveIteratorRef Iterator = ArchiveQueryNodeIteratorByPathFirst(Archive, -1, Query); \
    Context->CONCAT(__NAME__, Count) = 0; \
    memset(&Context->CONCAT(__NAME__, List)[0], 0, sizeof(struct CONCAT(_RTData, __NAME__)) * __COUNT__); \
    while (Iterator) { \
        assert(Context->CONCAT(__NAME__, Count) < __COUNT__); \
        CONCAT(RTData, __NAME__ ## Ref) Data = &Context->CONCAT(__NAME__, List)[Context->CONCAT(__NAME__, Count)];

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

#define RUNTIME_DATA_TYPE_BEGIN_CHILD(__NAME__, __QUERY__, __COUNT__) \
        { \
            CString Query = __QUERY__; \
            ArchiveIteratorRef ChildIterator = ArchiveQueryNodeIteratorFirst(Archive, Iterator->Index, Query); \
            while (ChildIterator) { \
                ArchiveIteratorRef Iterator = ChildIterator; \
                assert(Data->CONCAT(__NAME__, Count) < __COUNT__); \
                CONCAT(RTData, __NAME__ ## Ref) ChildData = &Data->CONCAT(__NAME__, List)[Data->CONCAT(__NAME__, Count)]; \
                { \
                    CONCAT(RTData, __NAME__ ## Ref) Data = ChildData;

#define RUNTIME_DATA_TYPE_END_CHILD(__NAME__, __COUNT__) \
                } \
                Data->CONCAT(__NAME__, Count) += 1; \
                ChildIterator = ArchiveQueryNodeIteratorNext(Archive, ChildIterator); \
            } \
        }

#define RUNTIME_DATA_TYPE_END(__NAME__) \
        Context->CONCAT(__NAME__, Count) += 1; \
        Iterator = ArchiveQueryNodeIteratorNext(Archive, Iterator); \
    }\
\
    ArchiveDestroy(Archive);\
    return true;\
\
error:\
    Error("Error reloading runtime data: %s", # __NAME__); \
    ArchiveDestroy(Archive);\
    return false;\
}
#include "Macro.h"
