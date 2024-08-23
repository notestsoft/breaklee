#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2L, VERIFY_PASSWORD) {
    IPC_L2W_DATA_VERIFY_PASSWORD* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, L2W, VERIFY_PASSWORD);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->Success = false;
    
    Bool Success = false;
    UInt8 PasswordHash[SALTED_HASH_LENGTH] = { 0 };

    if (!DatabaseCallProcedure(
        Context->Database,
        "GetPasswordHash",
        DB_INPUT_INT32(Packet->AccountID),
        DB_OUTPUT_BOOL(Success),
        DB_OUTPUT_DATA(PasswordHash, SALTED_HASH_LENGTH),
        DB_PARAM_END
    )) {
        Success = false;
    }

    if (Success && !ValidatePasswordHash(Packet->Credentials, PasswordHash)) {
        Success = false;
    }

    IPCSocketUnicast(Server->IPCSocket, Response);
}
