#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ATTACK_TO_CHARACTER) {
    // TODO: Implementation missing!
    return;

error:
	SocketDisconnect(Socket, Connection);
}
