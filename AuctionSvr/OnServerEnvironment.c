#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SERVER_ENVIRONMENT) {
	SendEnvironmentNotification(Context, Connection);
}
