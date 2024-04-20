#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(LOGIN) {
    S2C_DATA_LOGIN* Response = PacketBufferInit(Connection->PacketBuffer, S2C, LOGIN);
    Response->Result = 0;
    SocketSend(Socket, Connection, Response);
}
