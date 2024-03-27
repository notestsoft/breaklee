#include "IPCProtocol.h"
#include "IPCProcs.h"
#include "Server.h"

IPC_PROCEDURE_BINDING(OnMasterConnect, IPC_AUTH_REQCONNECT, IPC_DATA_AUTH_REQCONNECT) {
	IPC_DATA_AUTH_ACKCONNECT* Response = PacketBufferInitExtended(Connection->PacketBuffer, IPC, AUTH_ACKCONNECT);
	Response->ConnectionID = 0;
	Response->Success = ServerGetMaster(Server, Context, Packet->ServerID) == NULL;
	Master->ServerID = Packet->ServerID;
	SocketSend(Socket, Connection, Response);

	if (!Response->Success) {
		SocketDisconnect(Socket, Connection);
	}
}
