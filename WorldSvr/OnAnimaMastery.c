#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ANIMA_MASTERY_TRAIN_SLOT) {
	S2C_DATA_ANIMA_MASTERY_TRAIN_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ANIMA_MASTERY_TRAIN_SLOT);
	if (!Character) goto error;

	Int32 TailLength = sizeof(UInt16) * Packet->MaterialSlotCount;
	if (sizeof(C2S_DATA_ANIMA_MASTERY_TRAIN_SLOT) + TailLength != Packet->Length) goto error;

	Response->ForceEffectOrder = RTCharacterAnimaMasteryTrainSlot(
		Runtime,
		Character,
		Packet->CategoryIndex,
		Packet->MasterySlotIndex,
		Packet->ConversionKitSlotIndex,
		Packet->MaterialSlotCount,
		Packet->MaterialSlotIndex
	);

	Response->Success = (Response->ForceEffectOrder > 0) ? 1 : 0;

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(ANIMA_MASTERY_RESET_SLOT) {
	S2C_DATA_ANIMA_MASTERY_RESET_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ANIMA_MASTERY_RESET_SLOT);
	if (!Character) goto error;

	Response->Success = RTCharacterAnimaMasteryResetSlot(Runtime, Character, Packet->CategoryIndex, Packet->StorageIndex, Packet->InventorySlotIndex);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(ANIMA_MASTERY_UNLOCK_CATEGORY) {
	S2C_DATA_ANIMA_MASTERY_UNLOCK_CATEGORY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ANIMA_MASTERY_UNLOCK_CATEGORY);
	if (!Character) goto error;

	Response->Success = RTCharacterAnimaMasteryUnlockCategory(Runtime, Character, Packet->CategoryIndex);

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketSend(Socket, Connection, Response);
}
 
CLIENT_PROCEDURE_BINDING(ANIMA_MASTERY_SET_ACTIVE_STORAGE_INDEX) {
	S2C_DATA_ANIMA_MASTERY_SET_ACTIVE_STORAGE_INDEX* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ANIMA_MASTERY_SET_ACTIVE_STORAGE_INDEX);
	if (!Character) goto error;

	Response->Success = RTCharacterAnimaMasterySetActiveStorageIndex(Runtime, Character, Packet->CategoryIndex, Packet->StorageSlotIndex);

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(ANIMA_MASTERY_SET_ACTIVE_PRESET_INDEX) {
	S2C_DATA_ANIMA_MASTERY_SET_ACTIVE_PRESET_INDEX* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ANIMA_MASTERY_SET_ACTIVE_PRESET_INDEX);
	if (!Character) goto error;

	Response->Success = RTCharacterAnimaMasterySetActivePresetIndex(Runtime, Character, Packet->PresetIndex);

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketSend(Socket, Connection, Response);
}

CLIENT_PROCEDURE_BINDING(ANIMA_MASTERY_UNSEALING) {
}

CLIENT_PROCEDURE_BINDING(ANIMA_MASTERY_SEALING) {
}
