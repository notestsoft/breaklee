#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2L, VERIFY_PASSWORD) {
    IPC_L2W_DATA_VERIFY_PASSWORD* Response = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, L2W, VERIFY_PASSWORD);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->Success = false;
    
    if (!DatabaseCallProcedure(
        Context->Database,
        "AccountVerifyPassword",
        DB_INPUT_INT32(Packet->AccountID),
        DB_INPUT_STRING(Packet->Credentials, sizeof(Packet->Credentials)),
        DB_INPUT_INT32(Context->Config.Login.HashIterations),
        DB_OUTPUT_BOOL(Response->Success),
        DB_PARAM_END
    )) {
        Response->Success = false;
    }

    IPCSocketUnicast(Server->IPCSocket, Response);
}
