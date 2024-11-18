#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GUILD_INVENTORY_GET_ITEM_LIST) {
    S2C_DATA_GUILD_INVENTORY_GET_ITEM_LIST* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GUILD_INVENTORY_GET_ITEM_LIST);
    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(GUILD_INVENTORY_MERGE_ITEM) {
    S2C_DATA_GUILD_INVENTORY_MERGE_ITEM* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GUILD_INVENTORY_MERGE_ITEM);
    SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(GUILD_INVENTORY_GET_HISTORY) {
    S2C_DATA_GUILD_INVENTORY_GET_HISTORY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, GUILD_INVENTORY_GET_HISTORY);
    SocketSend(Socket, Connection, Response);
}
