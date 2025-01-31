#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GUILD_DUNGEON_ENTRANCE_COUNT) {
	S2C_DATA_GUILD_DUNGEON_ENTRANCE_COUNT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GUILD_DUNGEON_ENTRANCE_COUNT);
	Response->DungeonIndex = 4349;
	SocketSend(Socket, Connection, Response);
}
