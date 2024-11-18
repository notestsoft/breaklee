#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

// TODO: Skill Point Count going out of sync with client!
CLIENT_PROCEDURE_BINDING(CHANGE_CHARACTER_SKILL_LEVEL) {
	if (!Character) goto error;

	Bool Success = RTCharacterChangeSkillLevel(
		Runtime,
		Character,
		Packet->SkillIndex,
		Packet->SlotIndex,
		Packet->CurrentSkillLevel,
		Packet->TargetSkillLevel
	);
	if (!Success) goto error;

	S2C_DATA_CHANGE_CHARACTER_SKILL_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHANGE_CHARACTER_SKILL_LEVEL);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
