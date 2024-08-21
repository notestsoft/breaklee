#include "FileIO.h"
#include "Diagnostic.h"
#include "String.h"

#ifdef _WIN32

static DWORD FileTransferByteCount = 0;
static BOOL FileTransferCompleted = false;
static Char FilePathBuffer[MAX_PATH] = { 0 };

VOID CALLBACK IOCompletionRoutine(
    __in DWORD ErrorCode,
    __in DWORD NumberOfBytesTransfered,
    __in LPOVERLAPPED Overlapped
) {
    FileTransferByteCount = NumberOfBytesTransfered;
    FileTransferCompleted = true;
}

FileRef FileOpen(
    CString FilePath
) {
    HANDLE Handle = CreateFileA(
        FilePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (Handle == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    return (FileRef)Handle;
}

FileRef FileCreate(
    CString FilePath
) {
    HANDLE Handle = CreateFileA(
        FilePath,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (Handle == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    return (FileRef)Handle;
}

Void FileClose(
    FileRef File
) {
    assert(File);
    CloseHandle(File);
}

Bool FileReadNoAlloc(
    FileRef File,
    UInt8* Destination,
    Int32 MaxLength,
    Int32* Length
) {
    assert(File);

    *Length = 0;

    LARGE_INTEGER FileSize = { 0 };
    if (!GetFileSizeEx(File, &FileSize)) {
        Error("Error reading file size!\n");
        return false;
    }

    if (FileSize.QuadPart + 1 > MaxLength) {
        Error("Error reading file, not enough memory!");
        return false;
    }

    OVERLAPPED Overlapped = { 0 };
    *Length = (Int32)FileSize.QuadPart;

    FileTransferByteCount = 0;
    FileTransferCompleted = false;

    if (!ReadFileEx(File, Destination, *Length, &Overlapped, (LPOVERLAPPED_COMPLETION_ROUTINE)IOCompletionRoutine)) {
        Error("Error reading file!\n");
        *Length = 0;
        return false;
    }

    while (!FileTransferCompleted) {
        SleepEx(1, true);
    }

    assert(*Length == (Int32)FileTransferByteCount);
    Destination[*Length] = '\0';
    return true;
}

Bool FileRead(
    FileRef File,
    UInt8** Destination,
    Int32* Length
) {
    assert(File);

    *Destination = NULL;
    *Length = 0;

    LARGE_INTEGER FileSize = { 0 };
    if (!GetFileSizeEx(File, &FileSize)) {
        Error("Error reading file size!\n");
        return false;
    }

    *Length = (Int32)FileSize.QuadPart;
    Int32 MaxLength = *Length + 1;

    *Destination = (UInt8*)malloc(MaxLength);
    if (*Destination == NULL) {
        Error("Memory allocation failed!\n");
        return false;
    }

    if (!FileReadNoAlloc(File, *Destination, MaxLength, Length)) {
        free(*Destination);
        *Destination = NULL;
        *Length = 0;
        return false;
    }

    return true;
}

Bool FileWrite(
    FileRef File,
    UInt8* Source,
    Int32 Length,
    Bool Append
) {
    OVERLAPPED Overlapped = { 0 };
    if (Append) {
        Overlapped.Offset = 0xFFFFFFFF;
        Overlapped.OffsetHigh = 0xFFFFFFFF;
    }

    FileTransferByteCount = 0;
    FileTransferCompleted = false;

    if (!WriteFileEx(File, Source, (DWORD)Length, &Overlapped, (LPOVERLAPPED_COMPLETION_ROUTINE)IOCompletionRoutine)) {
        Error("Error writing file!\n");
        return false;
    }

    while (!FileTransferCompleted) {
        SleepEx(1, true);
    }

    assert(Length == FileTransferByteCount);

    return true;
}

Bool FileExists(
    CString FilePath
) {
    WIN32_FIND_DATA Data;
    HANDLE Handle = FindFirstFileA(FilePath, &Data);
    if (Handle != INVALID_HANDLE_VALUE) {
        FindClose(Handle);
        return true;
    }

    return false;
}

Int32 FilesProcess(
    CString Directory,
    CString Pattern,
    FilesProcessCallback Callback,
    Void* UserData
) {
    CString PatternDirectory = PathCombineNoAlloc(Directory, Pattern);
    WIN32_FIND_DATA FindData;
    HANDLE FileHandle = FindFirstFile(PatternDirectory, &FindData);
    Int32 ProcessedFileCount = 0;
    CHAR Buffer[MAX_PATH] = { 0 };

    if (FileHandle == INVALID_HANDLE_VALUE) {
        return ProcessedFileCount;
    }

    do {
        if (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            CString FilePath = PathCombineNoAlloc(Directory, FindData.cFileName);
            FileRef File = FileOpen(FilePath);
            
            if (File) Callback(
                FindData.cFileName,
                File,
                UserData
            );
            if (File) FileClose(File);

            ProcessedFileCount += 1;
        }
    } while (FindNextFile(FileHandle, &FindData) != 0);

    FindClose(FileHandle);

    return ProcessedFileCount;
}

CString PathCombineNoAlloc(
    CString Directory,
    CString File
) {
    PathCombine(Directory, File, FilePathBuffer);
    return FilePathBuffer;
}

Void PathCombine(
    CString Directory, 
    CString File, 
    CString Result
) {
    assert(Directory);
    assert(File);
    
    CStringCopySafe(Result, MAX_PATH, Directory);
    PathAppendSeparator(Result);
    strcat_s(Result, MAX_PATH, File);
}

Void PathAppend(
    CString Path,
    CString Extension
) {
    assert(Path);
    assert(Extension);

    strcat_s(Path, MAX_PATH, Extension);
}

Void PathAppendSeparator(
    CString Path
) {
    Int32 Length = strlen(Path);
    if (Path[Length - 1] == PLATFORM_PATH_SEPARATOR) return;

    Char Separator[2] = { PLATFORM_PATH_SEPARATOR, '\0' };
    strcat_s(Path, MAX_PATH, Separator);
}

#endif
