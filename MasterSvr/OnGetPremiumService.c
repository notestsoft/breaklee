#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, GET_PREMIUM_SERVICE) {
	IPC_M2W_DATA_GET_PREMIUM_SERVICE* Response = IPCPacketBufferInit(Connection->PacketBuffer, M2W, GET_PREMIUM_SERVICE);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	MASTERDB_DATA_SERVICE Service = { 0 };
	Service.AccountID = Packet->AccountID;
	if (MasterDBSelectCurrentActiveServiceByAccount(Context->Database, &Service)) {
		Response->HasService = true;
		Response->ServiceType = Service.ServiceType;
		Response->StartedAt = Service.StartedAt;
		Response->ExpiredAt = Service.ExpiredAt;
	}

    IPCSocketUnicast(Socket, Response);
}