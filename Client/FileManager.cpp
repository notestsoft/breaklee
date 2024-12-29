#include "Archive.h"
#include "Application.h"
#include "FileManager.h"

CFile::CFile() :
    Data(nullptr),
    Length(0) {
}

CFile::CFile(
    UInt8* Data, 
    Int32 Length
) : Data(Data),
    Length(Length) {
}

CFile::CFile(
    String FilePath
) : Data(nullptr),
    Length(0) {
    String EncryptedSuffix = ".enc";
    Bool IsEncrypted = FilePath.rfind(EncryptedSuffix) == (FilePath.size() - EncryptedSuffix.size());

    if (IsEncrypted) {
        EncryptionDecryptFile(
            (CString)FilePath.c_str(),
            &this->Data,
            &this->Length
        );
    }
    else {
        FileRef File = FileOpen((CString)FilePath.c_str());
        if (!File) return;
        if (!FileRead(File, &this->Data, &this->Length)) return;

        FileClose(File);
    }
}

CFile::~CFile() {
    free(Data);
    Data = nullptr;
}

Bool CFile::operator!() const {
    return Data != nullptr;
}

CFileManager::CFileManager(
    CApplication* Application
) : Application(Application) {
    if (!LoadFileTable()) Fatal("Loading resource file table failed!");
}

CFileManager::~CFileManager() {
}

CFile* CFileManager::LoadFile(
    String Directory,
    String FileName
) {
    auto Iterator = this->FilesToData.find(FileName);
    if (Iterator == this->FilesToData.end()) {
        String FilePath = String(Directory);
        FilePath = FilePath.append("\\");
        FilePath = FilePath.append(FileName);
        Trace("LoadFile(%s)", FilePath.c_str());
        this->FilesToData[FileName] = new CFile(FilePath); // TODO: The file manager it self shouldn't retain the file
    }

    return this->FilesToData[FileName];
}

CFile* CFileManager::LoadFile(
    String FileName
) {
    auto Iterator = this->FilesToData.find(FileName);
    if (Iterator == this->FilesToData.end()) {
        String Directory = String(Application->ApplicationDirectory);
        auto FolderIndexIterator = this->FilesToFolder.find(FileName);
        if (FolderIndexIterator != this->FilesToFolder.end()) {
            auto FolderIterator = this->IndexToFolder.find(FolderIndexIterator->second);
            if (FolderIterator != this->IndexToFolder.end() && FolderIterator->second.size() > 0) {
                Directory = Directory.append("\\");
                Directory = Directory.append(FolderIterator->second);
            }
        }

        Directory = Directory.append("\\");
        return LoadFile(Directory, FileName);
    }

    return this->FilesToData[FileName];
}

CArchive* CFileManager::LoadArchive(
    String Directory,
    String FileName
) {
    auto Iterator = this->FilesToArchive.find(FileName);
    if (Iterator == this->FilesToArchive.end()) {
        auto File = this->LoadFile(Directory, FileName);

        this->FilesToArchive[FileName] = new CArchive(File); // TODO: The file manager it self shouldn't retain the archive
    }

    return this->FilesToArchive[FileName];
}

CArchive* CFileManager::LoadArchive(
    String FileName
) {
    auto Iterator = this->FilesToArchive.find(FileName);
    if (Iterator == this->FilesToArchive.end()) {
        auto File = this->LoadFile(FileName);

        this->FilesToArchive[FileName] = new CArchive(File); // TODO: The file manager it self shouldn't retain the archive
    }

    return this->FilesToArchive[FileName];
}

Bool CFileManager::LoadFileTable() {
    auto File = LoadFile("xdata.enc");
    if (!File) return false;

    UInt8* Cursor = (UInt8*)File->Data;
    UInt8* End = Cursor + File->Length;

    if (Cursor + sizeof(UInt32) > End) return false;
    UInt32 FolderCount = *(UInt32*)Cursor;
    Cursor += sizeof(UInt32);

    Char FolderPath[MAX_PATH] = { 0 };
    for (Int FolderIndex = 0; FolderIndex < FolderCount; FolderIndex += 1) {
        if (Cursor + MAX_PATH > End) return false;

        memcpy(FolderPath, Cursor, MAX_PATH);
        this->IndexToFolder[FolderIndex] = String(FolderPath);
        Cursor += MAX_PATH;
    }

    if (Cursor + sizeof(UInt32) > End) return false;
    UInt32 FileCount = *(UInt32*)Cursor;
    Cursor += sizeof(UInt32);

    UInt8 FolderIndex = 0;
    Char FileName[61] = { 0 };
    Char FileHash[33] = { 0 };
    for (Int Index = 0; Index < FileCount; Index += 1) {
        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 FolderIndex = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(FileName) > End) return false;
        memcpy(FileName, Cursor, sizeof(FileName));
        Cursor += sizeof(FileName);

        if (Cursor + sizeof(FileHash) > End) return false;
        memcpy(FileHash, Cursor, sizeof(FileHash));
        Cursor += sizeof(FileHash);

        this->FilesToFolder[String(FileName)] = FolderIndex - 1;
        this->FilesToHash[String(FileName)] = String(FileHash);
    }

    return true;
}
