#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"

IPC_PROCEDURE_BINDING(OnWorldVerifyResult, IPC_WORLD_VERIFYRESULT, IPC_DATA_WORLD_VERIFYRESULT) {
	IPC_DATA_AUTH_VERIFYRESULT* Response = PacketInitExtended(IPC_DATA_AUTH_VERIFYRESULT);
	Response->Command = IPC_AUTH_VERIFYRESULT;
	Response->ConnectionID = Packet->ConnectionID;
	Response->ServerID = Packet->ServerID;
	Response->WorldID = Packet->WorldID;
	Response->Status = Packet->Status;
	return SocketSendAll(Context->AuthSocket, Response);

error:
	Response->Status = 0;
	return SocketSendAll(Context->AuthSocket, Response);
}
