#include "Allocator.h"
#include "String.h"
#include "FileIO.h"
#include "Diagnostic.h"

#include <uv.h>

struct _FileEvent {
    AllocatorRef Allocator;
    uv_loop_t* Loop;
    uv_fs_event_t Event;
    FileChangeCallback Callback;
    Void* UserData;
};

Void OnFileSystemEvent(
    uv_fs_event_t* Event,
    const char* FileName,
    int EventFlags,
    int Status
) {
    if (Status < 0) {
        Error("Error creating file event: %s\n", uv_strerror(Status));
        return;
    }

    if (EventFlags & UV_CHANGE) {
        Trace("File modified: %s\n", FileName ? FileName : "---");

        FileEventRef Context = (FileEventRef)Event->data;
        Context->Callback((CString)FileName, Context->UserData);
    }
}

FileEventRef FileEventCreate(
    CString FilePath,
    FileChangeCallback Callback,
    Void* UserData
) {
    AllocatorRef Allocator = AllocatorGetSystemDefault();
    FileEventRef Event = AllocatorAllocate(Allocator, sizeof(struct _FileEvent));
    if (!Event) Fatal("Memory allocation failed!");

    Event->Allocator = Allocator;
    Event->Loop = uv_default_loop();
    Int32 Result = uv_fs_event_init(Event->Loop, &Event->Event);
    if (Result < 0) {
        Error("Error creating file event: %s\n", uv_strerror(Result));
        AllocatorDeallocate(Allocator, Event);
        return NULL;
    }
    Event->Callback = Callback;
    Event->UserData = UserData;
    Event->Event.data = Event;
    
    Result = uv_fs_event_start(&Event->Event, OnFileSystemEvent, FilePath, UV_FS_EVENT_WATCH_ENTRY | UV_FS_EVENT_STAT);
    if (Result < 0) {
        Error("Error start file event: %s\n", uv_strerror(Result));
        AllocatorDeallocate(Allocator, Event);
        return NULL;
    }

    return Event;
}

Void FileEventDestroy(
    FileEventRef Event
) {
    uv_fs_event_stop(&Event->Event);
    AllocatorDeallocate(Event->Allocator, Event);
}

CString PathCombineAll(
    CString Path,
    ...
) {
    static Char FilePathBuffer[MAX_PATH] = { 0 };
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
