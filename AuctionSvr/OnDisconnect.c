#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"

CLIENT_PROCEDURE_BINDING(DISCONNECT) {
    SocketDisconnect(Socket, Connection);
}
