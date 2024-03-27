#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(GET_EVENT_PASS_SEASON) {
	if (!Character) goto error;

	// TODO: Add season info

	S2C_DATA_GET_EVENT_PASS_SEASON* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_EVENT_PASS_SEASON);
	Response->ExpirationDate = (UInt32)time(NULL);
	Response->Unknown2 = 5;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(GET_EVENT_PASS_REWARD_LIST) {
	if (!Character) goto error;
	
	// TODO: Add reward list data

	S2C_DATA_GET_EVENT_PASS_REWARD_LIST* Response = PacketBufferInit(Connection->PacketBuffer, S2C, GET_EVENT_PASS_REWARD_LIST);
	Response->RewardSlotCount = 0;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}
