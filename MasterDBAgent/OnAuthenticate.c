#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, AUTHENTICATE) {
	IPC_D2W_DATA_AUTHENTICATE* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, AUTHENTICATE);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->GroupIndex = Packet->GroupIndex;
	Response->NodeIndex = Packet->NodeIndex;
	Response->LinkConnectionID = Packet->LinkConnectionID;
	Response->Success &= DatabaseCallProcedure(
		Context->Database,
		"Authenticate",
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->Success,
		SQL_PARAM_OUTPUT, SQL_INTEGER, &Response->AccountInfo.CharacterSlotID,
		SQL_PARAM_OUTPUT, SQL_BIGINT, &Response->AccountInfo.CharacterSlotOrder,
		SQL_PARAM_OUTPUT, SQL_INTEGER, &Response->AccountInfo.CharacterSlotOpenMask,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->IsSubpasswordSet,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->AccountInfo.IsPremium,
		SQL_PARAM_OUTPUT, SQL_INTEGER, &Response->AccountInfo.ForceGem,
		SQL_END
	);

	IPCSocketUnicast(Socket, Response);
}
