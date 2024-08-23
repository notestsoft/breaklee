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

	if (!DatabaseCallProcedure(
		Context->Database,
		"Authenticate", 
		DB_INPUT_INT32(Packet->AccountID),
		DB_OUTPUT_INT8(Response->Success),
		DB_OUTPUT_INT32(Response->AccountInfo.CharacterSlotID),
		DB_OUTPUT_UINT64(Response->AccountInfo.CharacterSlotOrder),
		DB_OUTPUT_INT32(Response->AccountInfo.CharacterSlotOpenMask),
		DB_OUTPUT_INT8(Response->IsSubpasswordSet),
		DB_OUTPUT_INT32(Response->AccountInfo.IsPremium),
		DB_OUTPUT_INT32(Response->AccountInfo.ForceGem),
		DB_PARAM_END
	)) {
		Response->Success = false;
	}

	IPCSocketUnicast(Socket, Response);
}
