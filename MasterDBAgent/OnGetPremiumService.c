#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_PREMIUM_SERVICE) {
	IPC_D2W_DATA_GET_PREMIUM_SERVICE* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_PREMIUM_SERVICE);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->AccountID = Packet->AccountID;
		
	if (!DatabaseCallProcedure(
		Context->Database,
		"GetPremiumService",
		DB_INPUT_INT32(Packet->AccountID),
		DB_OUTPUT_INT32(Response->ServiceType),
		DB_OUTPUT_UINT64(Response->StartedAt),
		DB_OUTPUT_UINT64(Response->ExpiredAt),
		DB_OUTPUT_BOOL(Response->HasService),
		DB_PARAM_END
	)) {
		Response->HasService = false;
	}

    IPCSocketUnicast(Socket, Response);
}