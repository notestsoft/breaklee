#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(DELETE_CHARACTER) {
    // TODO: Implementation missing!
    
error:
	return SocketDisconnect(Socket, Connection);
}
