#include "FileIO.h"
#include "Diagnostic.h"

CString PathGetFileName(
    CString Path
) {
    CString FileName = strrchr(Path, '\\');
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
    strcpy_s(PathRemoveExtensionBuffer, MAX_PATH, Path);
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
