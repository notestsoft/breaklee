#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, NFY_WORLD_INFO) {
	if (IPCNodeIDIsNull(ConnectionContext->NodeID)) return;

	WorldInfoRef WorldInfo = (WorldInfoRef)DictionaryLookup(Context->WorldInfoTable, &ConnectionContext->NodeID);
	if (!WorldInfo) {
		struct _WorldInfo NewWorldInfo = { 0 };
		NewWorldInfo.NodeID = ConnectionContext->NodeID;
		DictionaryInsert(Context->WorldInfoTable, &ConnectionContext->NodeID, &NewWorldInfo, sizeof(struct _WorldInfo));
		WorldInfo = (WorldInfoRef)DictionaryLookup(Context->WorldInfoTable, &ConnectionContext->NodeID);
		assert(WorldInfo);
	}

	WorldInfo->PlayerCount = Packet->PlayerCount;
	WorldInfo->MaxPlayerCount = Packet->MaxPlayerCount;
	CStringCopySafe(WorldInfo->Host, 64 + 1, Packet->Host);
	WorldInfo->Port = Packet->Port;
	WorldInfo->Type = Packet->Type;
	WorldInfo->LobbyPlayerCount = Packet->LobbyPlayerCount;
	WorldInfo->Unknown1 = Packet->Unknown1;
	WorldInfo->CapellaPlayerCount = Packet->CapellaPlayerCount;
	WorldInfo->ProcyonPlayerCount = Packet->ProcyonPlayerCount;
	WorldInfo->Unknown2 = Packet->Unknown2;
	WorldInfo->CapellaPlayerCount2 = Packet->CapellaPlayerCount2;
	WorldInfo->ProcyonPlayerCount2 = Packet->ProcyonPlayerCount2;
	WorldInfo->Unknown3 = Packet->Unknown3;
	WorldInfo->MinLevel = Packet->MinLevel;
	WorldInfo->MaxLevel = Packet->MaxLevel;
	WorldInfo->MinRank = Packet->MinRank;
	WorldInfo->MaxRank = Packet->MaxRank;
}
