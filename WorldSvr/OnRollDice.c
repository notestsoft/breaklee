#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ROLL_DICE) {
	if (!Character) goto error;

	S2C_DATA_NFY_USER_MESSAGE* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_USER_MESSAGE);
	Notification->MessageType = 10;
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->NameLength = strlen(Character->Name) + 1;
	CStringCopySafe(Notification->Name, MAX_CHARACTER_NAME_LENGTH + 1, Character->Name);

	S2C_DATA_NFY_USER_MESSAGE_ROLL_DICE* RollDice = PacketBufferAppendStruct(Context->ClientSocket->PacketBuffer, S2C_DATA_NFY_USER_MESSAGE_ROLL_DICE);
	RollDice->Value = RandomRange(
		&Client->DiceSeed,
		Context->Config.WorldSvr.MinRollDiceValue,
		Context->Config.WorldSvr.MaxRollDiceValue
	);

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
