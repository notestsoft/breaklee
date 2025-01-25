#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(PVP_REQUEST) {
    if (!Character) goto error;

    return;

error:
    SocketDisconnect(Socket, Connection);
    return;
}

CLIENT_PROCEDURE_BINDING(PVP_RESPONSE) {
    if (!Character) goto error;

    return;

error:
    SocketDisconnect(Socket, Connection);
    return;
}

CLIENT_PROCEDURE_BINDING(PVP_REQUEST_END) {
    if (!Character) goto error;

    return;

error:
    SocketDisconnect(Socket, Connection);
    return;
}
