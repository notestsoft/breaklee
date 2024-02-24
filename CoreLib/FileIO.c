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
