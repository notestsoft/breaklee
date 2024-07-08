#include "FileIO.h"
#include "Diagnostic.h"

#ifdef __APPLE__

FileRef FileOpen(
    CString FilePath
) {
    return NULL;
}

FileRef FileCreate(
    CString FilePath
) {
    return NULL;
}

Void FileClose(
    FileRef File
) {
}

Bool FileRead(
    FileRef File,
    UInt8** Destination,
    Int32* Length
) {
    return 0;
}

Bool FileWrite(
    FileRef File,
    UInt8* Source,
    Int32 Length,
    Bool Append
) {
    return 0;
}

Bool FileExists(
    CString FilePath
) {
    return 0;
}

Int32 FilesProcess(
	CString Directory,
	CString Pattern,
	FilesProcessCallback Callback,
	Void* UserData
) {
    return 0;
}

CString PathCombineNoAlloc(
    CString Directory,
    CString File
) {
    return NULL;
}

void PathCombine(
    CString Directory,
    CString File,
    CString Result
) {
}

void PathAppend(
    CString Path,
    CString Extension
) {
}

#endif
