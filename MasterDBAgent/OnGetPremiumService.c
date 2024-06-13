#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_PREMIUM_SERVICE) {
	IPC_D2W_DATA_GET_PREMIUM_SERVICE* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_PREMIUM_SERVICE);
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