#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(AUTH_ACCOUNT) {
    S2C_DATA_AUTH_ACCOUNT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, AUTH_ACCOUNT);
    Response->CharacterIndex = Packet->CharacterIndex;
    Response->GuildIndex = 0;
    Response->Result = 0;
    SocketSend(Socket, Connection, Response);
}
