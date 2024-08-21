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
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_END
	);

	UInt8 CharacterSlotIndex = 0;
	IPC_DATA_CHARACTER_INFO CharacterInfo = { 0 };
	while (DatabaseHandleReadNext(
		Handle,
		SQL_TINYINT, &CharacterSlotIndex,
		SQL_INTEGER, &CharacterInfo.CharacterID,
		SQL_BIGINT, &CharacterInfo.CreationDate,
		SQL_INTEGER, &CharacterInfo.Style,
		SQL_INTEGER, &CharacterInfo.Level,
		SQL_SMALLINT, &CharacterInfo.OverlordLevel,
		SQL_INTEGER, &CharacterInfo.MythRebirth,
		SQL_INTEGER, &CharacterInfo.MythHolyPower,
		SQL_INTEGER, &CharacterInfo.MythLevel,
		SQL_INTEGER, &CharacterInfo.SkillRank,
		SQL_TINYINT, &CharacterInfo.NationMask,
		SQL_VARCHAR, CharacterInfo.Name, sizeof(CharacterInfo.Name),
		SQL_BIGINT, &CharacterInfo.HonorPoint,
		SQL_INTEGER, &CharacterInfo.CostumeActivePageIndex,
		SQL_VARBINARY, &CharacterInfo.CostumeAppliedSlots, sizeof(CharacterInfo.CostumeAppliedSlots),
		SQL_BIGINT, &CharacterInfo.Currency,
		SQL_TINYINT, &CharacterInfo.WorldIndex,
		SQL_SMALLINT, &CharacterInfo.PositionX,
		SQL_SMALLINT, &CharacterInfo.PositionY,
		SQL_SMALLINT, &CharacterInfo.EquipmentCount,
		SQL_VARBINARY, &CharacterInfo.Equipment, sizeof(CharacterInfo.Equipment),
		SQL_VARBINARY, &CharacterInfo.EquipmentAppearance, sizeof(CharacterInfo.EquipmentAppearance),
		SQL_END
	)) {
		assert(CharacterSlotIndex < MAX_CHARACTER_COUNT);
		Response->Characters[CharacterSlotIndex] = CharacterInfo;
	}

    IPCSocketUnicast(Socket, Response);
    return;

error:
	return;
}