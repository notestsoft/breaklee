#include "Enumerations.h"
#include "MasterDB.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2M, CREATE_CHARACTER) {
	IPC_M2W_DATA_CREATE_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, M2W, CREATE_CHARACTER);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	Int32 NameExists = 0;
	if (!MasterDBSelectCharacterNameCount(Context->Database, Packet->Name, &NameExists)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	if (NameExists) {
		Response->Status = CREATE_CHARACTER_STATUS_NAME_OCCUPIED;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	Account->AccountID = Packet->AccountID;
	if (!MasterDBGetOrCreateAccount(Context->Database, Account)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	Character->AccountID = Account->AccountID;
	CStringCopySafe(Character->Name, MAX_CHARACTER_NAME_LENGTH + 1, Packet->Name);
	Character->Index = Packet->SlotIndex;
	Character->CharacterData = Packet->CharacterData;
    Character->EquipmentData = Packet->CharacterEquipment;
	Character->InventoryData = Packet->CharacterInventory;
    Character->SkillSlotData = Packet->CharacterSkillSlots;
    Character->QuickSlotData = Packet->CharacterQuickSlots;

	if (!MasterDBInsertCharacter(Context->Database, Character)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	Character->CharacterID = (Int32)DatabaseGetLastInsertID(Context->Database);
	if (!MasterDBSelectCharacterByID(Context->Database, Character)) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
		IPCSocketUnicast(Socket, Response);
		return;
	}

	Response->Status = CREATE_CHARACTER_STATUS_SUCCESS;
	Response->SlotIndex = Packet->SlotIndex;
	Response->Character.ID = Character->CharacterID;
	Response->Character.CreationDate = Character->CreatedAt;
	Response->Character.Style = Character->CharacterData.Style.RawValue;
	Response->Character.Level = Character->CharacterData.Basic.Level;
	Response->Character.OverlordLevel = Character->CharacterData.Overlord.Level;
	Response->Character.SkillRank = Character->CharacterData.Skill.Rank;
	CStringCopySafe(Response->Character.Name, MAX_CHARACTER_NAME_LENGTH, Character->Name);
	Response->Character.HonorPoint = Character->CharacterData.Honor.Point;
	Response->Character.Alz = Character->CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
	Response->Character.MapID = Character->CharacterData.Position.WorldID;
	Response->Character.PositionX = Character->CharacterData.Position.X;
	Response->Character.PositionY = Character->CharacterData.Position.Y;
	Response->Character.EquipmentCount = Character->EquipmentData.Count;
	memcpy(Response->Character.Equipment, Character->EquipmentData.Slots, sizeof(struct _RTItemSlot) * Character->EquipmentData.Count);
	
	IPCSocketUnicast(Socket, Response);
}