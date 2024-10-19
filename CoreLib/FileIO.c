#include "String.h"
#include "FileIO.h"
#include "Diagnostic.h"

CString PathCombineAll(
    CString Path,
    ...
) {
    static char FilePathBuffer[MAX_PATH] = { 0 };
    memset(FilePathBuffer, 0, MAX_PATH);
    snprintf(FilePathBuffer, MAX_PATH, "%s", Path);

    va_list Arguments;
    va_start(Arguments, Path);

    while (true) {
        CString NextArgument = va_arg(Arguments, CString);
        if (!NextArgument || !NextArgument[0]) break;

        if (FilePathBuffer[strlen(FilePathBuffer) - 1] != '/') {
            strncat(FilePathBuffer, PLATFORM_PATH_SEPARATOR_STRING, MAX_PATH - strlen(FilePathBuffer) - 1);
        }

        strncat(FilePathBuffer, NextArgument, MAX_PATH - strlen(FilePathBuffer) - 1);
    }
    va_end(Arguments);

    return FilePathBuffer;
}

CString PathGetFileName(
    CString Path
) {
    CString FileName = strrchr(Path, PLATFORM_PATH_SEPARATOR);
    if (FileName) return FileName + 1;
    return Path;
}

CString PathGetFileNameExtension(
	CString Path
) {
	Char* Cursor = strrchr(Path, '.');
	if (!Cursor || Cursor == Path) return "";
	return Cursor + 1;
}

static Char PathRemoveExtensionBuffer[MAX_PATH] = { 0 };

CString PathRemoveExtensionNoAlloc(
    CString Path
) {
    CStringCopySafe(PathRemoveExtensionBuffer, MAX_PATH, Path);
    CString Cursor = strrchr(PathRemoveExtensionBuffer, '.');
    if (Cursor != NULL) *Cursor = '\0';
    return Path;
}

Bool DirectoryCreate(
    CString Path
) {
#ifdef _WIN32
    if (!CreateDirectoryA(Path, NULL)) {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            return false;
        }
    }
#else
    if (mkdir(Path, 0777) == -1) {
        if (errno != EEXIST) {
            return false;
        }
    }
#endif

    return true;
}
