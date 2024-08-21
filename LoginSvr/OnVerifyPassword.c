#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2L, VERIFY_PASSWORD) {
    IPC_L2W_DATA_VERIFY_PASSWORD* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, L2W, VERIFY_PASSWORD);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->Success = false;
    
    UInt8 Success = 0;
    UInt8 PasswordHash[SALTED_HASH_LENGTH] = { 0 };

    Response->Success = (
        DatabaseCallProcedure(
            Context->Database,
            "GetPasswordHash",
            SQL_PARAM_INPUT, SQL_BIGINT, &Packet->AccountID,
            SQL_PARAM_OUTPUT, SQL_TINYINT, &Success,
            SQL_PARAM_OUTPUT, SQL_VARBINARY, PasswordHash, SALTED_HASH_LENGTH,
            SQL_END
        ) &&
        Success &&
        ValidatePasswordHash(Packet->Credentials, PasswordHash)
    );

    IPCSocketUnicast(Server->IPCSocket, Response);
}
