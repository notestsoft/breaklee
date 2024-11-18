#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ADD_ABILITY) {
	if (!Character) goto error;

	Int32 MaterialSlotCount = (Packet->Length - sizeof(C2S_DATA_ADD_ABILITY)) / sizeof(UInt16);
	
	S2C_DATA_ADD_ABILITY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ADD_ABILITY);
	Response->AbilityType = Packet->AbilityType;

	switch (Packet->AbilityType) {
	case CSC_DATA_ABILITY_TYPE_ESSENCE:
		Response->Result = RTCharacterAddEssenceAbility(Runtime, Character, Packet->InventorySlotIndex, &Packet->MaterialSlotIndex[0], MaterialSlotCount);
		break;

	case CSC_DATA_ABILITY_TYPE_BLENDED:
		Response->Result = RTCharacterAddBlendedAbility(Runtime, Character, Packet->InventorySlotIndex, &Packet->MaterialSlotIndex[0], MaterialSlotCount);
		break;

	case CSC_DATA_ABILITY_TYPE_KARMA:
		Response->Result = RTCharacterAddKarmaAbility(Runtime, Character, Packet->InventorySlotIndex, &Packet->MaterialSlotIndex[0], MaterialSlotCount);
		break;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(UPGRADE_ABILITY) {
	if (!Character) goto error;

	Int32 InventorySlotCount = (Packet->Length - sizeof(C2S_DATA_UPGRADE_ABILITY)) / sizeof(UInt16);

	S2C_DATA_UPGRADE_ABILITY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, UPGRADE_ABILITY);
	switch (Packet->AbilityType) {
	case CSC_DATA_ABILITY_TYPE_ESSENCE:
		Response->Result = RTCharacterUpgradeEssenceAbility(
			Runtime,
			Character,
			Packet->AbilityID,
			InventorySlotCount,
			Packet->InventorySlotIndex
		);
		break;

	case CSC_DATA_ABILITY_TYPE_BLENDED:
		Response->Result = RTCharacterUpgradeBlendedAbility(
			Runtime,
			Character,
			Packet->AbilityID,
			InventorySlotCount,
			Packet->InventorySlotIndex
		);
		break;

	case CSC_DATA_ABILITY_TYPE_KARMA:
		Response->Result = RTCharacterUpgradeKarmaAbility(
			Runtime,
			Character,
			Packet->AbilityID,
			InventorySlotCount,
			Packet->InventorySlotIndex
		);
		break;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(REMOVE_ABILITY) {
	if (!Character) goto error;

	S2C_DATA_REMOVE_ABILITY* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REMOVE_ABILITY);
	switch (Packet->AbilityType) {
		case CSC_DATA_ABILITY_TYPE_ESSENCE:
		Response->Result = RTCharacterRemoveEssenceAbility(Runtime, Character, Packet->AbilityID);
		break;

	case CSC_DATA_ABILITY_TYPE_BLENDED:
		Response->Result = RTCharacterRemoveBlendedAbility(Runtime, Character, Packet->AbilityID);
		break;

	case CSC_DATA_ABILITY_TYPE_KARMA:
		Response->Result = RTCharacterRemoveKarmaAbility(Runtime, Character, Packet->AbilityID);
		break;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
