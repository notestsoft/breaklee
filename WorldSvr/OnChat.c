#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(MESSAGE_NEARBY) {
	if (!Character) return;

	Int32 PacketLength = sizeof(C2S_DATA_MESSAGE_NEARBY) + Packet->PayloadLength;
	if (Packet->Length != PacketLength) return;

	S2C_DATA_MESSAGE_NEARBY* Notification = PacketBufferInit(Socket->PacketBuffer, S2C, MESSAGE_NEARBY);
	Notification->CharacterIndex = Character->CharacterIndex;
	Notification->Unknown1 = 0;
	Notification->PayloadLength = Packet->PayloadLength;
	PacketBufferAppendCopy(Socket->PacketBuffer, &Packet->Payload[0], Packet->PayloadLength);

	BroadcastToWorld(
		Context,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		kEntityIDNull,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
}
