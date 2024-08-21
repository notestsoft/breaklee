#include "Base.h"

Void PrintUsage() {
    fprintf(
        stdout,
        "\n"
        "USAGE:\n"
        "breaklee <pattern>\n"
        "\n"
        "ARGUMENTS:\n"
        "   pattern         File path pattern\n"
        "\n"
    ); 
}

CString SkipWhitespaceAndNewlines(
    CString String
) {
    CString Cursor = String;
    while (*Cursor != '0') {
        switch (*Cursor) {
        case 0x09:
        case 0x20:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case '\r':
            Cursor += 1;
            break;

        default:
            return Cursor;
        }
    }

    UNREACHABLE("Unreachable");
}

Void EncryptDecryptFile(
    CString FileName,
    FileRef File,
    Void* UserData
) {
    UInt8* Source = NULL;
    Int32 SourceLength = 0;
    UInt8* Buffer = NULL;
    Int32 BufferLength = 0;
    ArchiveRef Archive = NULL;
    FileRef DecryptedFile = NULL;

    if (!FileRead(File, &Source, &SourceLength)) goto cleanup;

    if (!InflateDecryptBuffer(Source, SourceLength, &Buffer, &BufferLength)) {
        Error("Couldn't decrypt file `%s`", FileName);
        goto cleanup;
    }

    Bool IsBinary = (*SkipWhitespaceAndNewlines((Char*)Buffer) != '<');
    CString OutputFilePath = PathRemoveExtensionNoAlloc(FileName);
    PathAppend(OutputFilePath, IsBinary ? ".dat" : ".xml");

    DecryptedFile = FileCreate(OutputFilePath);
    if (!DecryptedFile) {
        Error("Error creating file `%s`", OutputFilePath);
        goto cleanup;
    }

    Info("Writing file `%s`", OutputFilePath);
    if (!FileWrite(DecryptedFile, Buffer, BufferLength, FALSE)) {
        Error("Error writing file `%s`", OutputFilePath);
        goto cleanup;
    }

cleanup:

    if (Source) free(Source);
    if (Buffer) free(Buffer);
    if (Archive) ArchiveDestroy(Archive);
    if (DecryptedFile) FileClose(DecryptedFile);
}

Int32 main(
    Int32 ArgumentCount,
    CString* Arguments
) {
    RenderLogo();
    DiagnosticSetup(
        stdout,
        LOG_LEVEL_TRACE,
        NULL,
        NULL
    );

    if (ArgumentCount < 2) {
        Error("Missing argument `pattern`");
        goto error;
    }

    CString Pattern = Arguments[1];
    Char DirectoryBuffer[MAX_PATH] = { 0 };
    CString Directory = PathGetCurrentDirectory(DirectoryBuffer, MAX_PATH);
    Int32 ProcessedFileCount = FilesProcess(
        Directory,
        Pattern,
        &EncryptDecryptFile,
        NULL
    );

    if (ProcessedFileCount < 1) {
        Warn("No files found for pattern `%s`", Pattern);
        goto error;
    }

    DiagnosticTeardown();
    
    return EXIT_SUCCESS;

error:
    DiagnosticTeardown();
    PrintUsage();

    return EXIT_FAILURE;
}
