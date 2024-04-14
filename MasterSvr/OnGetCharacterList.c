#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, GET_CHARACTER_LIST) {
	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, &Account)) goto error;

	IPC_M2W_DATA_GET_CHARACTER_LIST* Response = IPCPacketBufferInit(Connection->PacketBuffer, M2W, GET_CHARACTER_LIST);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	
	StatementRef Statement = MasterDBSelectCharacterIndexByAccount(
		Context->Database,
		Packet->AccountID
	);
	if (!Statement) goto error;

	Int32 Index = 0;
	MASTERDB_DATA_CHARACTER_INDEX Character = { 0 };
	while (MasterDBSelectCharacterIndexFetchNext(Context->Database, Statement, &Character)) {
		assert(Index < MAX_CHARACTER_COUNT);

		// TODO: Add EquipmentAppearance

		Response->Characters[Character.Index].ID = Character.CharacterID;
		Response->Characters[Character.Index].CreationDate = Character.CreatedAt;
		Response->Characters[Character.Index].Style = Character.CharacterData.Style.RawValue;
		Response->Characters[Character.Index].Level = Character.CharacterData.Basic.Level;
		Response->Characters[Character.Index].OverlordLevel = Character.CharacterData.Overlord.Level;
		Response->Characters[Character.Index].SkillRank = Character.CharacterData.Skill.Rank;
		Response->Characters[Character.Index].NationMask = Character.CharacterData.Profile.Nation;
		memcpy(Response->Characters[Character.Index].Name, Character.Name, MAX_CHARACTER_NAME_LENGTH);
		Response->Characters[Character.Index].HonorPoint = Character.CharacterData.Honor.Point;
		Response->Characters[Character.Index].Alz = Character.CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
		Response->Characters[Character.Index].MapID = Character.CharacterData.Position.WorldID;
		Response->Characters[Character.Index].PositionX = Character.CharacterData.Position.X;
		Response->Characters[Character.Index].PositionY = Character.CharacterData.Position.Y;
		Response->Characters[Character.Index].EquipmentCount = Character.EquipmentData.Count;
		memcpy(Response->Characters[Character.Index].Equipment, Character.EquipmentData.Slots, sizeof(struct _RTItemSlot) * Character.EquipmentData.Count);

		Index += 1;
	}

    IPCSocketUnicast(Socket, Response);
    return;

error:
	return;
}