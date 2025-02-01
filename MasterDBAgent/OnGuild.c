#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GUILD_CREATE) {
	IPC_D2W_DATA_GUILD_CREATE* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GUILD_CREATE);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->Result = 0;

	if (!DatabaseCallProcedure(
		Context->Database,
		"InsertGuild",
		DB_INPUT_INT32(Packet->CharacterIndex),
		DB_INPUT_STRING(Packet->GuildName, sizeof(Packet->GuildName)),
		DB_OUTPUT_UINT8(Response->Result),
		DB_OUTPUT_STRING(Response->GuildName, sizeof(Response->GuildName)),
		DB_OUTPUT_INT32(Response->GuildIndex),
		DB_PARAM_END
	)) {
		Response->Result = 1;
	}

	IPCSocketUnicast(Socket, Response);
}
