#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"

IPC_PROCEDURE_BINDING(OnWorldGetPremiumService, IPC_WORLD_REQPREMIUMSERVICE, IPC_DATA_WORLD_REQPREMIUMSERVICE) {
	IPC_DATA_WORLD_ACKPREMIUMSERVICE* Response = PacketBufferInitExtended(Connection->PacketBuffer, IPC, WORLD_ACKPREMIUMSERVICE);
	Response->ConnectionID = Packet->ConnectionID;

	MASTERDB_DATA_SERVICE Service = { 0 };
	Service.AccountID = Packet->AccountID;
	if (MasterDBSelectCurrentActiveServiceByAccount(Context->Database, &Service)) {
		Response->HasService = true;
		Response->ServiceType = Service.ServiceType;
		Response->StartedAt = Service.StartedAt;
		Response->ExpiredAt = Service.ExpiredAt;
	}

	SocketSend(Socket, Connection, Response);
}
