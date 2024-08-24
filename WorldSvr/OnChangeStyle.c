#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CHANGE_STYLE) {
	if (!Character) goto error;

	// TODO: IMPORTANT VALIDATE THE CHANGE!
	Character->Data.StyleInfo.Style.RawValue = Packet->CharacterStyle;
	Character->Data.StyleInfo.LiveStyle.RawValue = Packet->CharacterLiveStyle;

	// TODO: Implementation missing!
	S2C_DATA_CHANGE_STYLE* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CHANGE_STYLE);
	Response->Success = 1;
	SocketSend(Socket, Connection, Response);

	S2C_DATA_NFY_CHANGE_STYLE* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_CHANGE_STYLE);
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->CharacterStyle = Packet->CharacterStyle;
	Notification->CharacterLiveStyle = Packet->CharacterLiveStyle;
	Notification->GuildID = Packet->GuildID;
	Notification->UnknownFlags = Packet->UnknownFlags;
	Notification->GuildNameLength = Packet->GuildNameLength;
	memcpy(Notification->GuildName, Packet->GuildName, Packet->GuildNameLength);

	BroadcastToWorld(
		Context,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		kEntityIDNull,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
