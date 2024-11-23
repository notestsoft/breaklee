#include "Enumerations.h"
#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, CREATE_CHARACTER) {
	IPC_D2W_DATA_CREATE_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, CREATE_CHARACTER);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

	UInt8* Memory = (UInt8*)&Packet->Data[0];

	ReadMemory(struct _RTEquipmentInfo, EquipmentInfo, 1);
	ReadMemory(struct _RTItemSlot, EquipmentSlots, EquipmentInfo->EquipmentSlotCount);
	ReadMemory(struct _RTItemSlot, EquipmentInventorySlots, EquipmentInfo->InventorySlotCount);
	ReadMemory(struct _RTEquipmentLinkSlot, EquipmentLinkSlots, EquipmentInfo->LinkSlotCount);
	ReadMemory(struct _RTEquipmentLockSlot, EquipmentLockSlots, EquipmentInfo->LockSlotCount);
	ReadMemory(struct _RTInventoryInfo, InventoryInfo, 1);
	ReadMemory(struct _RTItemSlot, InventorySlots, InventoryInfo->SlotCount);
	ReadMemory(struct _RTSkillSlotInfo, SkillSlotInfo, 1);
	ReadMemory(struct _RTSkillSlot, SkillSlots, SkillSlotInfo->SlotCount);
	ReadMemory(struct _RTQuickSlotInfo, QuickSlotInfo, 1);
	ReadMemory(struct _RTQuickSlot, QuickSlots, QuickSlotInfo->SlotCount);

	Bool Success = DatabaseCallProcedure(
		Context->Database,
		"InsertCharacter",
		DB_INPUT_INT32(Packet->AccountID),
		DB_INPUT_STRING(Packet->CharacterName, sizeof(Packet->CharacterName)),
		DB_INPUT_UINT8(Packet->CharacterSlotIndex),
		DB_INPUT_UINT32(Packet->CharacterStyle.RawValue),
        DB_INPUT_UINT16(Packet->StatSTR),
        DB_INPUT_UINT16(Packet->StatDEX),
        DB_INPUT_UINT16(Packet->StatINT),
        DB_INPUT_UINT8(Packet->WorldIndex),
        DB_INPUT_UINT16(Packet->PositionX),
        DB_INPUT_UINT16(Packet->PositionY),
		DB_INPUT_UINT8(EquipmentInfo->EquipmentSlotCount),
		DB_INPUT_UINT8(EquipmentInfo->InventorySlotCount),
		DB_INPUT_UINT8(EquipmentInfo->LinkSlotCount),
		DB_INPUT_UINT8(EquipmentInfo->LockSlotCount),
		DB_INPUT_DATA(EquipmentSlots, EquipmentSlotsLength),
		DB_INPUT_DATA(EquipmentInventorySlots, EquipmentInventorySlotsLength),
		DB_INPUT_DATA(EquipmentLinkSlots, EquipmentLinkSlotsLength),
		DB_INPUT_DATA(EquipmentLockSlots, EquipmentLockSlotsLength),
		DB_INPUT_UINT16(InventoryInfo->SlotCount),
		DB_INPUT_DATA(InventorySlots, InventorySlotsLength),
		DB_INPUT_UINT16(SkillSlotInfo->SlotCount),
		DB_INPUT_DATA(SkillSlots, SkillSlotsLength),
		DB_INPUT_UINT16(QuickSlotInfo->SlotCount),
		DB_INPUT_DATA(QuickSlots, QuickSlotsLength),
        DB_OUTPUT_INT8(Response->Status),
        DB_OUTPUT_INT32(Response->Character.CharacterIndex),
		DB_PARAM_END
	);

	if (Success && Response->Status == CREATE_CHARACTER_STATUS_SUCCESS) {
		DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
			Context->Database,
			"GetCharacterInfo",
            DB_INPUT_INT32(Response->Character.CharacterIndex),
			DB_PARAM_END
		);

		IPC_DATA_CHARACTER_INFO CharacterInfo = { 0 };
		Bool Success = false;
		if (DatabaseHandleReadNext(
			Context->Database,
			Handle,
			DB_TYPE_UINT8, &Response->CharacterSlotIndex,
			DB_TYPE_INT32, &CharacterInfo.CharacterIndex,
			DB_TYPE_UINT64, &CharacterInfo.CreationDate,
			DB_TYPE_INT32, &CharacterInfo.Style,
			DB_TYPE_INT32, &CharacterInfo.Level,
			DB_TYPE_UINT16, &CharacterInfo.OverlordLevel,
			DB_TYPE_INT32, &CharacterInfo.MythRebirth,
			DB_TYPE_INT32, &CharacterInfo.MythHolyPower,
			DB_TYPE_INT32, &CharacterInfo.MythLevel,
			DB_TYPE_INT32, &CharacterInfo.SkillRank,
			DB_TYPE_UINT8, &CharacterInfo.NationMask,
			DB_TYPE_STRING, &CharacterInfo.Name[0], sizeof(CharacterInfo.Name),
			DB_TYPE_UINT64, &CharacterInfo.HonorPoint,
			DB_TYPE_INT32, &CharacterInfo.CostumeActivePageIndex,
			DB_TYPE_DATA, &CharacterInfo.CostumeAppliedSlots[0], sizeof(CharacterInfo.CostumeAppliedSlots),
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
			Success = true;
		}
		if (Success) DatabaseHandleFlush(Context->Database, Handle);
	}
	else if (!Response->Status) {
		Response->Status = CREATE_CHARACTER_STATUS_DBERROR;
	}

	// TODO: Sync all the character data from packet after creation!

	IPCSocketUnicast(Socket, Response);
}
