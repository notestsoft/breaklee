#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"

CLIENT_PROCEDURE_BINDING(WAR_ENTRY_STATUS) {
	S2C_DATA_WAR_ENTRY_STATUS* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, WAR_ENTRY_STATUS);
	Response->Unknown1[0] = 0x00;
	Response->Unknown1[1] = 0x00;
	Response->Unknown1[2] = 0x00;
	Response->Unknown1[3] = 0x00;
	Response->RecievedWarReward = 0x00;
	SocketSend(Socket, Connection, Response);
}
