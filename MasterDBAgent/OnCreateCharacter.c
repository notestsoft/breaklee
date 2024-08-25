#include "Enumerations.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, CREATE_CHARACTER) {
	IPC_D2W_DATA_CREATE_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, CREATE_CHARACTER);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	Bool Success = DatabaseCallProcedure(
		Context->Database,
		"InsertCharacter",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_STRING(Packet->CharacterName, sizeof(Packet->CharacterName)),
		DB_INPUT_UINT8(Packet->CharacterSlotIndex),
		DB_INPUT_INT32(Packet->CharacterData.StyleInfo.Style),
        DB_INPUT_INT16(Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_STR]),
        DB_INPUT_INT16(Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_DEX]),
        DB_INPUT_INT16(Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_INT]),
        DB_INPUT_INT8(Packet->CharacterData.Info.WorldIndex),
        DB_INPUT_INT16(Packet->CharacterData.Info.PositionX),
        DB_INPUT_INT16(Packet->CharacterData.Info.PositionY),
        DB_OUTPUT_INT8(Response->Status),
        DB_OUTPUT_INT32(Response->Character.CharacterID),
		DB_PARAM_END
	);

	if (Success && Response->Status == CREATE_CHARACTER_STATUS_SUCCESS) {
		DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
			Context->Database,
			"GetCharacterInfo",
            DB_INPUT_INT32(Response->Character.CharacterID),
			DB_PARAM_END
		);

		IPC_DATA_CHARACTER_INFO CharacterInfo = { 0 };
		if (DatabaseHandleReadNext(
			Handle,
			DB_TYPE_UINT8, &Response->CharacterSlotIndex,
			DB_TYPE_INT32, &CharacterInfo.CharacterID,
			DB_TYPE_UINT64, &CharacterInfo.CreationDate,
			DB_TYPE_INT32, &CharacterInfo.Style,
			DB_TYPE_INT32, &CharacterInfo.Level,
			DB_TYPE_UINT16, &CharacterInfo.OverlordLevel,
			DB_TYPE_INT32, &CharacterInfo.MythRebirth,
			DB_TYPE_INT32, &CharacterInfo.MythHolyPower,
			DB_TYPE_INT32, &CharacterInfo.MythLevel,
			DB_TYPE_INT32, &CharacterInfo.SkillRank,
			DB_TYPE_UINT8, &CharacterInfo.NationMask,
			DB_TYPE_STRING, CharacterInfo.Name, sizeof(CharacterInfo.Name),
			DB_TYPE_UINT64, &CharacterInfo.HonorPoint,
			DB_TYPE_INT32, &CharacterInfo.CostumeActivePageIndex,
			DB_TYPE_DATA, &CharacterInfo.CostumeAppliedSlots, sizeof(CharacterInfo.CostumeAppliedSlots),
			DB_TYPE_UINT64, &CharacterInfo.Currency,
			DB_TYPE_UINT8, &CharacterInfo.WorldIndex,
			DB_TYPE_UINT16, &CharacterInfo.PositionX,
			DB_TYPE_UINT16, &CharacterInfo.PositionY,
			DB_TYPE_UINT16, &CharacterInfo.EquipmentCount,
			DB_TYPE_DATA, &CharacterInfo.Equipment, sizeof(CharacterInfo.Equipment),
			DB_TYPE_DATA, &CharacterInfo.EquipmentAppearance, sizeof(CharacterInfo.EquipmentAppearance),
			DB_PARAM_END
		)) {
			Response->Character = CharacterInfo;
		}

		DatabaseHandleFlush(Handle);
	}
	else if (!Response->Status) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
	}

	// TODO: Sync all the character data from packet after creation!

	IPCSocketUnicast(Socket, Response);
}
