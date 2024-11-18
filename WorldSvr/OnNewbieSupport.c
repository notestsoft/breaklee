#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(TAKE_NEWBIE_REWARD) {
	S2C_DATA_TAKE_NEWBIE_REWARD* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, TAKE_NEWBIE_REWARD);

	if (!Character) goto error;

	Response->Success = RTCharacterTakeNewbieSupportReward(
		Runtime,
		Character,
		Packet->CategoryType,
		Packet->RewardIndex,
		Packet->ConditionValue1,
		Packet->ConditionValue2,
		Packet->InventorySlotCount,
		Packet->InventorySlotIndex
	);

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketSend(Socket, Connection, Response);
}
