#include "Server.h"
#include "IPCProcs.h"
#include "MasterDB.h"

IPC_PROCEDURE_BINDING(OnWorldDBSync, IPC_WORLD_REQDBSYNC, IPC_DATA_WORLD_REQDBSYNC) {
	IPC_DATA_WORLD_ACKDBSYNC* Response = PacketBufferInitExtended(Connection->PacketBuffer, IPC, WORLD_ACKDBSYNC);
	Response->ConnectionID = Packet->ConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->CharacterID = Packet->CharacterID;
	Response->SyncMaskFailed.RawValue = 0;

	Int32 DataOffset = 0;

	if (Packet->SyncMask.Info) {
		RTCharacterInfoRef CharacterInfo = (RTCharacterInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterInfo);

		Bool Success = MasterDBUpdateCharacterInfo(
			Context->Database,
			Packet->CharacterID,
			CharacterInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.Info = true;
		}
	}

    if (Packet->SyncMask.EquipmentInfo) {
		RTCharacterEquipmentInfoRef EquipmentInfo = (RTCharacterEquipmentInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterEquipmentInfo);

		Bool Success = MasterDBUpdateCharacterEquipmentData(
			Context->Database,
			Packet->CharacterID,
			EquipmentInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.EquipmentInfo = true;
		}
    }

    if (Packet->SyncMask.InventoryInfo) {
		RTCharacterInventoryInfoRef InventoryInfo = (RTCharacterInventoryInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterInventoryInfo);

		Bool Success = MasterDBUpdateCharacterInventoryData(
			Context->Database,
			Packet->CharacterID,
			InventoryInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.InventoryInfo = true;
		}
    }

    if (Packet->SyncMask.SkillSlotInfo) {
		RTCharacterSkillSlotInfoRef SkillSlotInfo = (RTCharacterSkillSlotInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterSkillSlotInfo);

		Bool Success = MasterDBUpdateCharacterSkillSlotData(
			Context->Database,
			Packet->CharacterID,
			SkillSlotInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.SkillSlotInfo = true;
		}
    }

    if (Packet->SyncMask.QuickSlotInfo) {
		RTCharacterQuickSlotInfoRef QuickSlotInfo = (RTCharacterQuickSlotInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterQuickSlotInfo);

		Bool Success = MasterDBUpdateCharacterQuickSlotData(
			Context->Database,
			Packet->CharacterID,
			QuickSlotInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.QuickSlotInfo = true;
		}
    }

    if (Packet->SyncMask.QuestSlotInfo) {
		RTCharacterQuestSlotInfoRef QuestSlotInfo = (RTCharacterQuestSlotInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterQuestSlotInfo);

		Bool Success = MasterDBUpdateCharacterQuestSlotData(
			Context->Database,
			Packet->CharacterID,
			QuestSlotInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.QuestSlotInfo = true;
		}
    }

    if (Packet->SyncMask.QuestFlagInfo) {
		RTCharacterQuestFlagInfoRef QuestFlagInfo = (RTCharacterQuestFlagInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterQuestFlagInfo);

		Bool Success = MasterDBUpdateCharacterQuestFlagData(
			Context->Database,
			Packet->CharacterID,
			QuestFlagInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.QuestFlagInfo = true;
		}
    }

	if (Packet->SyncMask.DungeonQuestFlagInfo) {
		RTCharacterDungeonQuestFlagInfoRef QuestFlagInfo = (RTCharacterDungeonQuestFlagInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterDungeonQuestFlagInfo);

		Bool Success = MasterDBUpdateCharacterDungeonQuestFlagData(
			Context->Database,
			Packet->CharacterID,
			QuestFlagInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.DungeonQuestFlagInfo = true;
		}
	}

	if (Packet->SyncMask.EssenceAbilityInfo) {
		RTCharacterEssenceAbilityInfoRef AbilityInfo = (RTCharacterEssenceAbilityInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterEssenceAbilityInfo);

		Bool Success = MasterDBUpdateCharacterEssenceAbilityData(
			Context->Database,
			Packet->CharacterID,
			AbilityInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.EssenceAbilityInfo = true;
		}
	}

	if (Packet->SyncMask.OverlordMasteryInfo) {
		RTCharacterOverlordMasteryInfoRef OverlordMasteryInfo = (RTCharacterOverlordMasteryInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterOverlordMasteryInfo);

		Bool Success = MasterDBUpdateCharacterOverlordData(
			Context->Database,
			Packet->CharacterID,
			OverlordMasteryInfo
		);

		if (!Success) {
			Response->SyncMaskFailed.OverlordMasteryInfo = true;
		}
	}

	if (Packet->SyncMask.CollectionInfo) {
		RTCharacterCollectionInfoRef CollectionInfo = (RTCharacterCollectionInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterCollectionInfo);

		MASTERDB_DATA_ACCOUNT Account = { 0 };
		Account.AccountID = Packet->AccountID;
		memcpy(&Account.CollectionData, CollectionInfo, sizeof(struct _RTCharacterCollectionInfo));

		Bool Success = MasterDBUpdateAccountCollectionData(
			Context->Database,
			&Account
		);

		if (!Success) {
			Response->SyncMaskFailed.CollectionInfo = true;
		}
	}

	if (Packet->SyncMask.WarehouseInfo) {
		RTCharacterWarehouseInfoRef WarehouseInfo = (RTCharacterWarehouseInfoRef)&Packet->Data[DataOffset];
		DataOffset += sizeof(struct _RTCharacterWarehouseInfo);

		MASTERDB_DATA_ACCOUNT Account = { 0 };
		Account.AccountID = Packet->AccountID;
		memcpy(&Account.WarehouseData, WarehouseInfo, sizeof(struct _RTCharacterWarehouseInfo));

		Bool Success = MasterDBUpdateAccountWarehouseData(
			Context->Database,
			&Account
		);

		if (!Success) {
			Response->SyncMaskFailed.WarehouseInfo = true;
		}
	}
 
    SocketSend(Socket, Connection, Response);
}
