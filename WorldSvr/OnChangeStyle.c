#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(CHANGE_STYLE) {
	if (!Character) goto error;

	// TODO: Verify packet length!

	// TODO: Implementation missing!
	S2C_DATA_CHANGE_STYLE* Response = PacketInit(S2C_DATA_CHANGE_STYLE);
	Response->Command = S2C_CHANGE_STYLE;
	Response->Success = 1;
	SocketSend(Socket, Connection, Response);

	S2C_DATA_NFY_CHANGE_STYLE* Notification = PacketInit(S2C_DATA_NFY_CHANGE_STYLE);
	Notification->Command = S2C_NFY_CHANGE_STYLE;
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->CharacterStyle = Packet->CharacterStyle;
	Notification->CharacterLiveStyle = Packet->CharacterLiveStyle;
	Notification->GuildID = Packet->GuildID;
	Notification->UnknownFlags = Packet->UnknownFlags;
	Notification->GuildNameLength = Packet->GuildNameLength;
	memcpy(Notification->GuildName, Packet->GuildName, Packet->GuildNameLength);

	return BroadcastToWorld(
		Context,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

error:
	return SocketDisconnect(Socket, Connection);
}
