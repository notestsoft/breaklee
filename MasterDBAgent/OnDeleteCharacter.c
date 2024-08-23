#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, DELETE_CHARACTER) {
	IPC_D2W_DATA_DELETE_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, DELETE_CHARACTER);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->CharacterID = Packet->CharacterID;

	if (!DatabaseCallProcedure(
		Context->Database,
		"DeleteCharacter",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_INT32(Packet->CharacterID),
		DB_OUTPUT_BOOL(Response->Success),
		DB_PARAM_END
	)) {
		Response->Success = false;
	}

	IPCSocketUnicast(Socket, Response);
}