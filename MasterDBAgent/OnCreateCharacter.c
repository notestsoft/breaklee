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
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->AccountID,
		SQL_PARAM_INPUT, SQL_VARCHAR, Packet->CharacterName, sizeof(Packet->CharacterName),
		SQL_PARAM_INPUT, SQL_INTEGER, &Packet->CharacterData.StyleInfo.Style,
		SQL_PARAM_INPUT, SQL_TINYINT, &Packet->CharacterSlotIndex,
		SQL_PARAM_INPUT, SQL_SMALLINT, &Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_STR],
		SQL_PARAM_INPUT, SQL_SMALLINT, &Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_DEX],
		SQL_PARAM_INPUT, SQL_SMALLINT, &Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_INT],
		SQL_PARAM_INPUT, SQL_TINYINT, &Packet->CharacterData.Info.WorldIndex,
		SQL_PARAM_INPUT, SQL_SMALLINT, &Packet->CharacterData.Info.PositionX,
		SQL_PARAM_INPUT, SQL_SMALLINT, &Packet->CharacterData.Info.PositionY,
		SQL_PARAM_OUTPUT, SQL_TINYINT, &Response->Status,
		SQL_PARAM_OUTPUT, SQL_INTEGER, &Response->Character.CharacterID,
		SQL_END
	);

	if (Success && Response->Status == CREATE_CHARACTER_STATUS_SUCCESS) {
		DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
			Context->Database,
			"GetCharacterInfo",
			SQL_PARAM_INPUT, SQL_INTEGER, &Response->Character.CharacterID,
			SQL_END
		);

		IPC_DATA_CHARACTER_INFO CharacterInfo = { 0 };
		if (DatabaseHandleReadNext(
			Handle,
			SQL_TINYINT, &Response->CharacterSlotIndex,
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
			Response->Character = CharacterInfo;
		}

		DatabaseHandleFlush(Handle);
	}

	// TODO: Sync all the character data from packet after creation!

	IPCSocketUnicast(Socket, Response);
}
