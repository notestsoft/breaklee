#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, UPDATE_ACCOUNT_SESSION) {
    Bool Success = false;
    Success &= DatabaseCallProcedure(
        Context->Database,
        "UpdateAccountSession",
        SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
        SQL_PARAM_INPUT, SQL_VARCHAR, Packet->SessionIP, sizeof(Packet->SessionIP),
        SQL_PARAM_INPUT, SQL_BIGINT, &Packet->SessionTimeout,
        SQL_PARAM_OUTPUT, SQL_TINYINT, &Success,
        SQL_END
    );
}