#include "Base.h"

EXTERN_C_BEGIN

Bool EncryptionLoadLibrary();

Bool EncryptionUnloadLibrary();

Bool EncryptionDecryptFile(
    CString Filepath,
    UInt8** Destination,
    Int32* DestinationLength
);

EXTERN_C_END
