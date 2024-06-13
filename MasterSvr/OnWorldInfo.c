#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, NFY_WORLD_INFO) {
	if (!ConnectionContext->NodeID.Serial) return;

	Index WorldIndex = ConnectionContext->NodeID.Serial;
	WorldInfoRef WorldInfo = (WorldInfoRef)DictionaryLookup(Context->WorldInfoTable, &WorldIndex);
	if (!WorldInfo) {
		struct _WorldInfo NewWorldInfo = { 0 };
		NewWorldInfo.NodeID = ConnectionContext->NodeID;
		DictionaryInsert(Context->WorldInfoTable, &WorldIndex, &NewWorldInfo, sizeof(struct _WorldInfo));
		WorldInfo = (WorldInfoRef)DictionaryLookup(Context->WorldInfoTable, &WorldIndex);
		assert(WorldInfo);
	}

	WorldInfo->PlayerCount = Packet->PlayerCount;
	WorldInfo->MaxPlayerCount = Packet->MaxPlayerCount;
	CStringCopySafe(WorldInfo->Host, 64 + 1, Packet->Host);
	WorldInfo->Port = Packet->Port;
	WorldInfo->Type = Packet->Type;
}
