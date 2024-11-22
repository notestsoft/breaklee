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

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_MESSAGE_NEARBY* Notification = PacketBufferInit(PacketBuffer, S2C, MESSAGE_NEARBY);
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->Nation = Character->Data.StyleInfo.Nation;
	Notification->PayloadLength = Packet->PayloadLength;
	PacketBufferAppendCopy(PacketBuffer, &Packet->Payload[0], Packet->PayloadLength);

	BroadcastToWorld(
		Context,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		kEntityIDNull,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
}
