#include "Server.h"
#include "IPCProcs.h"

IPC_PROCEDURE_BINDING(OnAuthConnect, IPC_AUTH_ACKCONNECT, IPC_DATA_AUTH_ACKCONNECT) {
	Context->WorldListUpdateTimestamp = ServerGetTimestamp(Server);
}
