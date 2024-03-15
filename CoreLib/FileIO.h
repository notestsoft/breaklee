#pragma once

#include "Base.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

EXTERN_C_BEGIN

typedef Void* FileRef;

FileRef FileOpen(
	CString FilePath
);

FileRef FileCreate(
	CString FilePath
);

Void FileClose(
	FileRef File
);

Bool FileRead(
	FileRef File,
	UInt8** Destination,
	Int32* Length
);

Bool FileWrite(
	FileRef File,
	UInt8* Source,
	Int32 Length,
	Bool Append
);

Bool FileExists(
	CString FilePath
);

typedef Void (*FilesProcessCallback)(
	CString FileName,
	FileRef File,
	Void* UserData
);

Int32 FilesProcess(
	CString Pattern,
	FilesProcessCallback Callback,
	Void* UserData
);

#ifdef _WIN32
#define PathGetCurrentDirectory _getcwd
#else
#define PathGetCurrentDirectory getcwd
#endif

CString PathCombineNoAlloc(
	CString Directory,
	CString File
);

Void PathCombine(
	CString Directory, 
	CString File, 
	CString Result
);

Void PathAppend(
	CString Path,
	CString Extension
);

Void PathAppendSeparator(
	CString Path
);

CString PathGetFileName(
	CString Path
);

CString PathGetFileNameExtension(
	CString Path
);

CString PathRemoveExtensionNoAlloc(
	CString Path
);

Bool DirectoryCreate(
    CString Path
);

EXTERN_C_END
