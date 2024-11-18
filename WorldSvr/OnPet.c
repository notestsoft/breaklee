#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(PET_COMMAND) {
	if (!Character) goto error;

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);

	S2C_DATA_PET_COMMAND* Response = PacketBufferInit(PacketBuffer, S2C, PET_COMMAND);
	Response->CommandType = Packet->CommandType;

	if (Packet->CommandType == CSC_DATA_PET_COMMAND_TYPE_INFO) {
		if (Packet->Length != sizeof(C2S_DATA_PET_COMMAND) + sizeof(C2S_DATA_PET_COMMAND_INFO)) goto error;

		C2S_DATA_PET_COMMAND_INFO* PacketData = (C2S_DATA_PET_COMMAND_INFO*)Packet->Data;
		S2C_DATA_PET_COMMAND_INFO* ResponseData = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_PET_COMMAND_INFO);

		SocketSend(Socket, Connection, Response);
		return;
	}

	if (Packet->CommandType == CSC_DATA_PET_COMMAND_TYPE_UNSEAL) {
		if (Packet->Length != sizeof(C2S_DATA_PET_COMMAND) + sizeof(C2S_DATA_PET_COMMAND_UNSEAL)) goto error;

		C2S_DATA_PET_COMMAND_UNSEAL* PacketData = (C2S_DATA_PET_COMMAND_UNSEAL*)Packet->Data;
		S2C_DATA_PET_COMMAND_UNSEAL* ResponseData = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_PET_COMMAND_UNSEAL);

		SocketSend(Socket, Connection, Response);
		return;
	}

	if (Packet->CommandType == CSC_DATA_PET_COMMAND_TYPE_TRAIN) {
		if (Packet->Length != sizeof(C2S_DATA_PET_COMMAND) + sizeof(C2S_DATA_PET_COMMAND_TRAIN)) goto error;

		C2S_DATA_PET_COMMAND_TRAIN* PacketData = (C2S_DATA_PET_COMMAND_TRAIN*)Packet->Data;
		S2C_DATA_PET_COMMAND_TRAIN* ResponseData = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_PET_COMMAND_TRAIN);

		SocketSend(Socket, Connection, Response);
		return;
	}

error:
	SocketDisconnect(Socket, Connection);
	return;
}
