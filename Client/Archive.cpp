#include "Archive.h"
#include "FileManager.h"

CArchive::CArchive(
    CFile* File
) : Handle(ArchiveCreateEmpty(AllocatorGetSystemDefault())) {
    this->HasError = ArchiveLoadFromData(
        this->Handle,
        File->Data,
        File->Length,
        true
    );
}

CArchive::~CArchive() {
    ArchiveDestroy(this->Handle);
}

CArchive::Iterator CArchive::QueryBegin(
    String Query
) {
    return Iterator(this, ArchiveQueryNodeIteratorByPathFirst(this->Handle, -1, (CString)Query.c_str()));
}

CArchive::Iterator CArchive::QueryEnd() {
    return Iterator(this, nullptr);
}

/*
Bool RTRuntimeDataContextLoad(
    RTRuntimeDataContextRef Context,
    CString RuntimeDataPath,
    CString ServerDataPath
) {
#define RUNTIME_DATA_TYPE_BEGIN(__NAME__, __QUERY__, __COUNT__) \
    { \
        CString Query = __QUERY__; \
        CString PropertyQuery = NULL; \
        Iterator = ArchiveQueryNodeIteratorByPathFirst(Archive, -1, Query); \
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
        } \
    }
}*/

CArchiveIterator::CArchiveIterator(
    CArchive* Archive,
    ArchiveIteratorRef Iterator
) : Archive(Archive),
    Current(Iterator) {
}

CArchiveIterator::ValueType CArchiveIterator::operator*() const {
    return this->Current->Index;
}

CArchiveIterator& CArchiveIterator::operator++() {
    this->Current = ArchiveQueryNodeIteratorNext(this->Archive->Handle, this->Current);
    return *this;
}

bool CArchiveIterator::operator==(const CArchiveIterator& Other) const {
    return this->Current == Other.Current;
}

bool CArchiveIterator::operator!=(const CArchiveIterator& Other) const {
    return this->Current != Other.Current;
}

String CArchiveIterator::GetAttribute(
    String Name
) {
    auto AttributeIndex = ArchiveNodeGetAttributeByName(
        this->Archive->Handle, 
        this->Current->Index, 
        (CString)Name.c_str()
    );
    if (AttributeIndex < 0) return String("");

    auto ArchiveString = ArchiveAttributeGetData(
        this->Archive->Handle, 
        AttributeIndex
    );

    return String(ArchiveString->Data);
}