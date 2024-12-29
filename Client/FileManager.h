#pragma once

#include "Base.h"

struct CFile {
    UInt8* Data;
    Int32 Length;

    CFile();

    CFile(
        UInt8* Data,
        Int32 Length
    );

    CFile(
        String FilePath
    );

    ~CFile();

    Bool operator!() const;
};

class CFileManager {
    friend class CApplication;

    CApplication* Application;
    CMap<Int32, String> IndexToFolder;
    CMap<String, Int32> FilesToFolder;
    CMap<String, String> FilesToHash;
    CMap<String, CFile*> FilesToData;
    CMap<String, CArchive*> FilesToArchive;

    Bool LoadFileTable();

    CFileManager(
        CApplication* Application
    );

    ~CFileManager();

public:
    CFile* LoadFile(
        String Directory,
        String FileName
    );

    CFile* LoadFile(
        String FileName
    );

    CArchive* LoadArchive(
        String Directory,
        String FileName
    );

    CArchive* LoadArchive(
        String FileName
    );
};
