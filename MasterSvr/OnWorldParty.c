#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"

IPC_PROCEDURE_BINDING(OnWorldRequestPartyInvite, IPC_WORLD_REQPARTYINVITE, IPC_DATA_WORLD_REQPARTYINVITE) {
	ClientContextRef WorldClient = ServerGetWorldClient(Server, Context, Packet->Target.WorldServerID);
	if (WorldClient) {
		SocketSend(Context->WorldSocket, WorldClient->World.Connection, Packet);
	}
}

IPC_PROCEDURE_BINDING(OnWorldRespondPartyInvite, IPC_WORLD_ACKPARTYINVITE, IPC_DATA_WORLD_ACKPARTYINVITE) {
	ClientContextRef WorldClient = ServerGetWorldClient(Server, Context, Packet->Source.WorldServerID);
	if (WorldClient) {
		SocketSend(Context->WorldSocket, WorldClient->World.Connection, Packet);
	}
}
