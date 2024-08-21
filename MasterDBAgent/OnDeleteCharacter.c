#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, DELETE_CHARACTER) {
	IPC_D2W_DATA_DELETE_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, DELETE_CHARACTER);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->CharacterID = Packet->CharacterID;
	Response->Success &= DatabaseCallProcedure(
		Context->Database,
		"DeleteCharacter",
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->CharacterID,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->Success,
		SQL_END
	);

	IPCSocketUnicast(Socket, Response);
}