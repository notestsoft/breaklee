#pragma once

#include "Base.h"

class CArchive {
    friend class CArchiveIterator;
    friend class CFileManager;

    ArchiveRef Handle;
    Bool HasError;

    CArchive(
        CFile* File
    );

    ~CArchive();

public:
    using Iterator = CArchiveIterator;

    Iterator QueryBegin(String Query);
    Iterator QueryEnd();
};

class CArchiveIterator {
    friend class CArchive;

public:
    using ValueType = int;
    using Pointer = ValueType*;
    using Reference = ValueType&;
    using DifferenceType = std::ptrdiff_t;
    using IteratorCategory = std::forward_iterator_tag;

private:
    CArchive* Archive;
    ArchiveIteratorRef Current;

    CArchiveIterator(
        CArchive* Archive,
        ArchiveIteratorRef Iterator
    );

    ValueType operator*() const;

public:
    CArchiveIterator& operator++();

    bool operator==(const CArchiveIterator& Other) const;

    bool operator!=(const CArchiveIterator& Other) const;

    String GetAttribute(
        String Name
    );
};