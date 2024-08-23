#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, UPDATE_ACCOUNT_SESSION) {
    Bool Success = false;
    DatabaseCallProcedure(
        Context->Database,
        "UpdateAccountSession",
        DB_INPUT_INT32(Packet->AccountID),
        DB_INPUT_STRING(Packet->SessionIP, sizeof(Packet->SessionIP)),
        DB_INPUT_UINT64(Packet->SessionTimeout),
        DB_OUTPUT_BOOL(Success),
        DB_PARAM_END
    );
}