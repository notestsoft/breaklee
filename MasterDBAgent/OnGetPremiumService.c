#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_PREMIUM_SERVICE) {
	IPC_D2W_DATA_GET_PREMIUM_SERVICE* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_PREMIUM_SERVICE);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->HasService = DatabaseCallProcedure(
		Context->Database,
		"GetPremiumService",
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_OUTPUT, SQL_INTEGER, &Response->ServiceType,
		SQL_PARAM_OUTPUT, SQL_BIGINT, &Response->StartedAt,
		SQL_PARAM_OUTPUT, SQL_BIGINT, &Response->ExpiredAt,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->HasService,
		SQL_END
	);

    IPCSocketUnicast(Socket, Response);
}