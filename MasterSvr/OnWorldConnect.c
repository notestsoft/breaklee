#include "Server.h"
#include "IPCProcs.h"

IPC_PROCEDURE_BINDING(OnWorldConnect, IPC_WORLD_REQCONNECT, IPC_DATA_WORLD_REQCONNECT) {
	ClientContextRef WorldClient = ServerGetWorldClient(Server, Context, Packet->WorldID);
	if (WorldClient != NULL) {
		SocketDisconnect(Socket, WorldClient->World.Connection);
	}

	Client->World.IsWorldInitialized = true;
	Client->World.WorldID = Packet->WorldID;
	memcpy(Client->World.WorldHost, Packet->WorldHost, sizeof(Client->World.WorldHost));
	Client->World.WorldPort = Packet->WorldPort;
	Client->World.WorldType = Packet->WorldType;
	Client->World.PlayerCount = 0;
	Client->World.MaxPlayerCount = Packet->MaxPlayerCount;

	IPC_DATA_WORLD_ACKCONNECT* Response = PacketInit(IPC_DATA_WORLD_ACKCONNECT);
	Response->Command = IPC_WORLD_ACKCONNECT;
	Response->ServerID = Context->Config.MasterSvr.ServerID;
	Response->Success = true;
	SocketSend(Socket, Connection, Response);

	Context->WorldListUpdateTimestamp = ServerGetTimestamp(Server);
}
