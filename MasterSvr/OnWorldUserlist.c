#include "Server.h"
#include "IPCProcs.h"

IPC_PROCEDURE_BINDING(OnWorldUserlist, IPC_WORLD_NFYUSERLIST, IPC_DATA_WORLD_NFYUSERLIST) { 
	Client->World.PlayerCount = Packet->PlayerCount;
	Client->World.MaxPlayerCount = Packet->MaxPlayerCount;
	Context->WorldListUpdateTimestamp = ServerGetTimestamp(Server);
}
