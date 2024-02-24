#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(OnWorldConnect, IPC_WORLD_ACKCONNECT, IPC_DATA_WORLD_ACKCONNECT) { 
	if (!Packet->Success) {
		return FatalError("Connection to master server failed!");
	}

	Master->ServerID = Packet->ServerID;
	Context->UserListUpdateTimestamp = ServerGetTimestamp(Server);
}
