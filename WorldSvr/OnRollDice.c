#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ROLL_DICE) {
	if (!Character) goto error;

	S2C_DATA_NFY_USER_MESSAGE* Notification = PacketInit(S2C_DATA_NFY_USER_MESSAGE);
	Notification->Command = S2C_NFY_USER_MESSAGE;
	Notification->MessageType = 10;
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->NameLength = strlen(Client->CharacterName) + 1;
	CStringCopySafe(Notification->Name, MAX_CHARACTER_NAME_LENGTH, Client->CharacterName);

	S2C_DATA_NFY_USER_MESSAGE_ROLL_DICE* RollDice = PacketAppendStruct(S2C_DATA_NFY_USER_MESSAGE_ROLL_DICE);
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
	return SocketDisconnect(Socket, Connection);
}
