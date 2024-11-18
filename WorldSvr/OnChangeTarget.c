#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SELECT_TARGET_CHARACTER) {
	if (!Character) goto error;

	RTCharacterRef Target = RTWorldManagerGetCharacter(Runtime->WorldManager, Packet->TargetID);
	if (!Target) goto error;

	Character->TargetCharacterID = Target->ID;

	S2C_DATA_SELECT_TARGET_CHARACTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, SELECT_TARGET_CHARACTER);
	Response->Unknown1 = 0;
	Response->CurrentHP = Target->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Response->MaxHP = Target->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
	Response->AbsorbHP = 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	{
		S2C_DATA_SELECT_TARGET_CHARACTER* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, SELECT_TARGET_CHARACTER);
		SocketSend(Socket, Connection, Response);
	}
}

CLIENT_PROCEDURE_BINDING(DESELECT_TARGET_CHARACTER) {
	if (!Character) return;

	Character->TargetCharacterID = kEntityIDNull;
}
