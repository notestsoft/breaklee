#include "FileIO.h"
#include "Diagnostic.h"
#include "String.h"

#if defined(__linux__) || defined(__APPLE__)

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
    Int32 Handle = open(FilePath, O_RDONLY);
    if (Handle == -1) {
        return NULL;
    }

    return (FileRef)Handle;
}

FileRef FileCreate(
    CString FilePath
) {
    Int32 Handle = open(FilePath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (Handle == -1) {
        return NULL;
    }

    return (FileRef)Handle;
}

Void FileClose(
    FileRef File
) {
    assert((Int32)File != -1);
    close((Int32)File);
}

Bool FileReadNoAlloc(
    FileRef File,
    UInt8* Destination,
    Int32 MaxLength,
    Int32* Length
) {
    assert(File != NULL);

    *Length = 0;

    Int32 FileDescriptor = (Int32)File;
    struct stat FileStat;
    if (fstat(FileDescriptor, &FileStat) == -1) {
        perror("Error reading file size");
        return false;
    }

    if (FileStat.st_size + 1 > MaxLength) {
        fprintf(stderr, "Error reading file, not enough memory!\n");
        return false;
    }

    ssize_t BytesRead = read(FileDescriptor, Destination, FileStat.st_size);
    if (BytesRead == -1) {
        perror("Error reading file");
        return false;
    }

    *Length = (Int32)BytesRead;
    Destination[*Length] = '\0';

    return true;
}

Bool FileRead(
    FileRef File,
    UInt8** Destination,
    Int32* Length
) {
    assert(File != NULL);

    *Destination = NULL;
    *Length = 0;

    Int32 FileDescriptor = (Int32)File;
    struct stat FileStat;
    if (fstat(FileDescriptor, &FileStat) == -1) {
        perror("Error reading file size");
        return false;
    }

    *Length = (Int32)FileStat.st_size;
    Int32 MaxLength = *Length + 1;

    *Destination = (UInt8*)malloc(MaxLength);
    if (*Destination == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
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
    assert((Int32)File != -1);

    if (Append) {
        if (lseek((Int32)File, 0, SEEK_END) == -1) {
            Error("Error seeking to end of file!\n");
            return false;
        }
    }

    ssize_t BytesWritten = write((Int32)File, Source, Length);
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
    Int32 ProcessedFileCount = 0;
    if (!(Dir = opendir(Directory))) {
        return ProcessedFileCount;
    }

    Char Buffer[PATH_MAX] = { 0 };
    struct dirent* Entry;
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
    size_t Length = strlen(Path);
    if (Path[Length - 1] != '/') {
        strncat(Path, "/", PATH_MAX - Length - 1);
    }
}

#endif
