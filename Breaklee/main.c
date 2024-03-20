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
        LogMessageFormat(LOG_LEVEL_ERROR, "Couldn't decrypt file `%s`", FileName);
        goto cleanup;
    }

    Bool IsBinary = (*SkipWhitespaceAndNewlines((Char*)Buffer) != '<');
    CString OutputFilePath = PathRemoveExtensionNoAlloc(FileName);
    PathAppend(OutputFilePath, IsBinary ? ".dat" : ".xml");

    DecryptedFile = FileCreate(OutputFilePath);
    if (!DecryptedFile) {
        LogMessageFormat(LOG_LEVEL_ERROR, "Error creating file `%s`", OutputFilePath);
        goto cleanup;
    }

    LogMessageFormat(LOG_LEVEL_INFO, "Writing file `%s`", OutputFilePath);
    if (!FileWrite(DecryptedFile, Buffer, BufferLength, FALSE)) {
        LogMessageFormat(LOG_LEVEL_ERROR, "Error writing file `%s`", OutputFilePath);
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
    if (ArgumentCount < 2) {
        LogMessage(LOG_LEVEL_ERROR, "Missing argument `pattern`");
        goto error;
    }

    if (!EncryptionLoadLibrary()) {
        LogMessage(LOG_LEVEL_ERROR, "Error loading zlib library");
        goto error;
    }

    CString Pattern = Arguments[1];
    Int32 ProcessedFileCount = FilesProcess(
        Pattern,
        &EncryptDecryptFile,
        NULL
    );

    if (ProcessedFileCount < 1) {
        LogMessageFormat(LOG_LEVEL_WARNING, "No files found for pattern `%s`", Pattern);
        goto error;
    }

    EncryptionUnloadLibrary();
    DiagnosticTeardown();
    
    return EXIT_SUCCESS;

error:

    EncryptionUnloadLibrary();
    DiagnosticTeardown();
    PrintUsage();

    return EXIT_FAILURE;
}
