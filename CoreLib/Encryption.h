#include "Base.h"

EXTERN_C_BEGIN

Bool EncryptionLoadLibrary();

Bool EncryptionUnloadLibrary();

Bool InflateDecryptBuffer(
    UInt8* Source,
    Int32 SourceLength,
    UInt8** Destination,
    Int32* DestinationLength
);

Bool EncryptionDecryptFile(
    CString Filepath,
    UInt8** Destination,
    Int32* DestinationLength
);

EXTERN_C_END
