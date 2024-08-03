#include "Enumerations.h"
#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, CREATE_CHARACTER) {
	IPC_D2W_DATA_CREATE_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, CREATE_CHARACTER);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	Int32 NameExists = 0;
	if (!MasterDBSelectCharacterNameCount(Context->Database, Packet->CharacterName, &NameExists)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	if (NameExists) {
		Response->Status = CREATE_CHARACTER_STATUS_NAME_OCCUPIED;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	DatabaseBeginTransaction(Context->Database);

	MASTERDB_DATA_ACCOUNT Account = { 0 };
	Account.AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Packet->AccountID, &Account)) {
		DatabaseRollbackTransaction(Context->Database);
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	MASTERDB_DATA_CHARACTER Character = { 0 };
	Character.AccountID = Account.AccountID;
	Character.Index = Packet->CharacterSlotIndex;
	CStringCopySafe(Character.Name, MAX_CHARACTER_NAME_LENGTH + 1, Packet->CharacterName);

	if (!MasterDBInsertCharacter(Context->Database, Packet->AccountID, Packet->CharacterName, Packet->CharacterSlotIndex)) {
		DatabaseRollbackTransaction(Context->Database);
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	Character.CharacterID = (Int32)DatabaseGetLastInsertID(Context->Database);
	if (!MasterDBSelectCharacterByID(Context->Database, &Character)) {
		DatabaseRollbackTransaction(Context->Database);
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

#define CHARACTER_DATA_PROTOCOL(__TYPE__, __NAME__) \
	{ \
        DataTableRef Table = DatabaseGetDataTable(Context->Database, TABLE_SCOPE_CHARACTER, #__NAME__); \
        if (!Table || !DataTableInsert(Table, Character.CharacterID, (UInt8*)&Packet->CharacterData.__NAME__, sizeof(__TYPE__))) { \
			DatabaseRollbackTransaction(Context->Database); \
			Response->Status = CREATE_CHARACTER_STATUS_DBERROR; \
			IPCSocketUnicast(Socket, Response); \
			return; \
        } \
	}

#include "RuntimeLib/CharacterDataDefinition.h"

	if (!DatabaseCommitTransaction(Context->Database)) {
		DatabaseRollbackTransaction(Context->Database);
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	Response->Status = CREATE_CHARACTER_STATUS_SUCCESS;
	Response->CharacterSlotIndex = Packet->CharacterSlotIndex;
	Response->Character.ID = Character.CharacterID;
	Response->Character.CreationDate = Character.CreatedAt;
	CStringCopySafe(Response->Character.Name, MAX_CHARACTER_NAME_LENGTH + 1, Character.Name);

	Response->Character.Style = Packet->CharacterData.Info.Style.RawValue;
	Response->Character.Level = Packet->CharacterData.Info.Basic.Level;
	Response->Character.OverlordLevel = Packet->CharacterData.OverlordMasteryInfo.Info.Level;
	Response->Character.MythRebirth = Packet->CharacterData.MythMasteryInfo.Info.Rebirth;
	Response->Character.MythHolyPower = Packet->CharacterData.MythMasteryInfo.Info.HolyPower;
	Response->Character.MythLevel = Packet->CharacterData.MythMasteryInfo.Info.Level;
	Response->Character.SkillRank = Packet->CharacterData.Info.Skill.Rank;
	Response->Character.NationMask = Packet->CharacterData.Info.Profile.Nation;
	Response->Character.HonorPoint = Packet->CharacterData.Info.Honor.Point;
	Response->Character.CostumeActivePageIndex = Packet->CharacterData.CostumeInfo.Info.ActivePageIndex;
	memcpy(Response->Character.CostumeAppliedSlots, Packet->CharacterData.CostumeInfo.AppliedSlots, sizeof(struct _RTAppliedCostumeSlot) * RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT);
	Response->Character.Alz = Packet->CharacterData.Info.Alz;
	Response->Character.MapID = Packet->CharacterData.Info.Position.WorldID;
	Response->Character.PositionX = Packet->CharacterData.Info.Position.X;
	Response->Character.PositionY = Packet->CharacterData.Info.Position.Y;
	Response->Character.EquipmentCount = Packet->CharacterData.EquipmentInfo.Info.EquipmentSlotCount;
	memcpy(Response->Character.Equipment, Packet->CharacterData.EquipmentInfo.EquipmentSlots, sizeof(struct _RTItemSlot) * RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT);
	memcpy(Response->Character.EquipmentAppearance, Packet->CharacterData.AppearanceInfo.EquipmentSlots, sizeof(struct _RTItemSlotAppearance) * RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT);

	IPCSocketUnicast(Socket, Response);
}
