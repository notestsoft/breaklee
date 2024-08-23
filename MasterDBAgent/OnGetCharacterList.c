#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_CHARACTER_LIST) {
	IPC_D2W_DATA_GET_CHARACTER_LIST* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_CHARACTER_LIST);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
		Context->Database,
		"GetCharacterList",
		DB_INPUT_INT32(Packet->AccountID),
		DB_PARAM_END
	);

	UInt8 CharacterSlotIndex = 0;
	IPC_DATA_CHARACTER_INFO CharacterInfo = { 0 };
	while (DatabaseHandleReadNext(
		Handle,
		DB_TYPE_UINT8, &CharacterSlotIndex,
		DB_TYPE_INT32, &CharacterInfo.CharacterID,
		DB_TYPE_UINT64, &CharacterInfo.CreationDate,
		DB_TYPE_INT32, &CharacterInfo.Style,
		DB_TYPE_INT32, &CharacterInfo.Level,
		DB_TYPE_INT16, &CharacterInfo.OverlordLevel,
		DB_TYPE_INT32, &CharacterInfo.MythRebirth,
		DB_TYPE_INT32, &CharacterInfo.MythHolyPower,
		DB_TYPE_INT32, &CharacterInfo.MythLevel,
		DB_TYPE_INT32, &CharacterInfo.SkillRank,
		DB_TYPE_UINT8, &CharacterInfo.NationMask,
		DB_TYPE_STRING, CharacterInfo.Name, sizeof(CharacterInfo.Name),
		DB_TYPE_INT64, &CharacterInfo.HonorPoint,
		DB_TYPE_INT32, &CharacterInfo.CostumeActivePageIndex,
		DB_TYPE_DATA, &CharacterInfo.CostumeAppliedSlots, sizeof(CharacterInfo.CostumeAppliedSlots),
		DB_TYPE_INT64, &CharacterInfo.Currency,
		DB_TYPE_UINT8, &CharacterInfo.WorldIndex,
		DB_TYPE_INT16, &CharacterInfo.PositionX,
		DB_TYPE_INT16, &CharacterInfo.PositionY,
		DB_TYPE_INT16, &CharacterInfo.EquipmentCount,
		DB_TYPE_DATA, &CharacterInfo.Equipment, sizeof(CharacterInfo.Equipment),
		DB_TYPE_DATA, &CharacterInfo.EquipmentAppearance, sizeof(CharacterInfo.EquipmentAppearance),
		DB_PARAM_END
	)) {
		assert(CharacterSlotIndex < MAX_CHARACTER_COUNT);
		Response->Characters[CharacterSlotIndex] = CharacterInfo;
	}

    IPCSocketUnicast(Socket, Response);
    return;

error:
	return;
}