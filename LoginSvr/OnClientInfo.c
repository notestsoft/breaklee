#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(M2N, CLIENT_CONNECT) {
    DatabaseCallProcedure(
        Context->Database,
        "SetSessionOnline",
        DB_INPUT_INT32(Packet->AccountID),
        DB_INPUT_BOOL(Packet->IsOnline),
        DB_PARAM_END
    );
}

IPC_PROCEDURE_BINDING(M2N, CLIENT_DISCONNECT) {
    DatabaseCallProcedure(
        Context->Database,
        "SetSessionOnline",
        DB_INPUT_INT32(Packet->AccountID),
        DB_INPUT_BOOL(Packet->IsOnline),
        DB_PARAM_END
    );
}
