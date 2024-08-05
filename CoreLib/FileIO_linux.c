#include "FileIO.h"
#include "Diagnostic.h"
#include "String.h"

#ifdef __linux__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>

FileRef FileOpen(
    CString FilePath
) {
    int Handle = open(FilePath, O_RDONLY);
    if (Handle == -1) {
        return NULL;
    }

    Info("FileOpen(%s, %d)", FilePath, Handle);
    return (FileRef)Handle;
}

FileRef FileCreate(
    CString FilePath
) {
    int Handle = open(FilePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (Handle == -1) {
        return NULL;
    }

    Info("FileCreate(%s, %d)", FilePath, Handle);
    return (FileRef)Handle;
}

Void FileClose(
    FileRef File
) {
    Info("FileClose(%d)", File);
    assert(File != -1);
    close(File);
}

Bool FileRead(
    FileRef File,
    UInt8** Destination,
    Int32* Length
) {
    assert(File != -1);

    *Destination = NULL;
    *Length = 0;

    struct stat FileStat;
    if (fstat(File, &FileStat) == -1) {
        Error("Error reading file size!\n");
        return false;
    }

    *Length = (Int32)FileStat.st_size;
    *Destination = (UInt8*)malloc(*Length + 1);
    if (*Destination == NULL) {
        Error("Memory allocation failed!\n");
        return false;
    }

    ssize_t BytesRead = read(File, *Destination, *Length);
    if (BytesRead == -1) {
        Error("Error reading file!\n");
        free(*Destination);
        *Destination = NULL;
        *Length = 0;
        return false;
    }

    (*Destination)[*Length] = '\0';
    assert(BytesRead == *Length);

    return true;
}

Bool FileWrite(
    FileRef File,
    UInt8* Source,
    Int32 Length,
    Bool Append
) {
    assert(File != -1);

    if (Append) {
        if (lseek(File, 0, SEEK_END) == -1) {
            Error("Error seeking to end of file!\n");
            return false;
        }
    }

    ssize_t BytesWritten = write(File, Source, Length);
    if (BytesWritten == -1) {
        Error("Error writing file!\n");
        return false;
    }

    assert(BytesWritten == Length);
    return true;
}

Bool FileExists(
    CString FilePath
) {
    struct stat Buffer;
    return (stat(FilePath, &Buffer) == 0);
}

Int32 FilesProcess(
    CString Directory,
    CString Pattern,
    FilesProcessCallback Callback,
    Void* UserData
) {
    DIR* Dir;
    struct dirent* Entry;
    Int32 ProcessedFileCount = 0;
    Char Buffer[PATH_MAX] = { 0 };

    if (!(Dir = opendir(Directory))) {
        return ProcessedFileCount;
    }

    while ((Entry = readdir(Dir)) != NULL) {
        if (Entry->d_type == DT_REG) {
            snprintf(Buffer, PATH_MAX, "%s/%s", Directory, Entry->d_name);
            FileRef File = FileOpen(Buffer);

            if (File) Callback(Entry->d_name, File, UserData);
            if (File) FileClose(File);

            ProcessedFileCount += 1;
        }
    }

    closedir(Dir);
    return ProcessedFileCount;
}

CString PathCombineNoAlloc(
    CString Directory,
    CString File
) {
    static Char FilePathBuffer[PATH_MAX] = { 0 };
    snprintf(FilePathBuffer, PATH_MAX, "%s/%s", Directory, File);
    return FilePathBuffer;
}

Void PathCombine(
    CString Directory,
    CString File,
    CString Result
) {
    assert(Directory);
    assert(File);

    snprintf(Result, PATH_MAX, "%s/%s", Directory, File);
}

Void PathAppend(
    CString Path,
    CString Extension
) {
    assert(Path);
    assert(Extension);

    strncat(Path, Extension, PATH_MAX - strlen(Path) - 1);
}

Void PathAppendSeparator(
    CString Path
) {
    Int32 Length = strlen(Path);
    if (Path[Length - 1] != '/') {
        strncat(Path, "/", PATH_MAX - Length - 1);
    }
}

#endif
