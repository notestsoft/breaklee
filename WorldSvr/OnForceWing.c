#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(FORCE_WING_GRADE_UP) {
	if (!Character) goto error;

	Index PacketLength = sizeof(C2S_DATA_FORCE_WING_GRADE_UP) + sizeof(UInt16) * (Packet->InventorySlotCount);
	if (Packet->Length < PacketLength) goto error;

	S2C_DATA_FORCE_WING_GRADE_UP* Response = PacketBufferInit(Connection->PacketBuffer, S2C, FORCE_WING_GRADE_UP);
	Response->Success = false;

	if (Character->ForceWingInfo.Grade < 1) goto error;

	RTDataForceWingGradeInfoRef GradeInfoData = RTRuntimeDataForceWingGradeInfoGet(Runtime->Context, Character->ForceWingInfo.Grade);
	if (!GradeInfoData) goto error;

	if (Character->ForceWingInfo.Level < GradeInfoData->MaxLevel) goto error;

	RTDataForceWingGradeInfoRef NextGradeInfoData = RTRuntimeDataForceWingGradeInfoGet(Runtime->Context, Character->ForceWingInfo.Grade + 1);
	if (!NextGradeInfoData) goto error;

	RTDataForceWingUpgradeRef UpgradeData = RTRuntimeDataForceWingUpgradeGet(Runtime->Context, Character->ForceWingInfo.Grade);
	if (!UpgradeData) goto error;

	Int64 RequiredItemCount[RUNTIME_DATA_MAX_FORCE_WING_UPGRADE_MATERIAL_COUNT] = { 0 };
	for (Index Index = 0; Index < UpgradeData->ForceWingUpgradeMaterialCount; Index += 1) {
		RequiredItemCount[Index] = UpgradeData->ForceWingUpgradeMaterialList[Index].RequiredMaterialItemCount;
	}

	Int64 ConsumableItemCount[RUNTIME_DATA_MAX_FORCE_WING_UPGRADE_MATERIAL_COUNT] = { 0 };
	for (Index InventorySlotIndex = 0; InventorySlotIndex < Packet->InventorySlotCount; InventorySlotIndex += 1) {
		for (Index MaterialIndex = 0; MaterialIndex < UpgradeData->ForceWingUpgradeMaterialCount; MaterialIndex += 1) {
			RequiredItemCount[MaterialIndex] = UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].RequiredMaterialItemCount;

			if (!RTInventoryCanConsumeStackableItems(
				Runtime,
				&Character->InventoryInfo,
				UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].MaterialItemID,
				Packet->InventorySlots[InventorySlotIndex].ItemStackSize,
				1,
				&Packet->InventorySlots[InventorySlotIndex].InventorySlotIndex
			)) continue;

			ConsumableItemCount[MaterialIndex] += Packet->InventorySlots[InventorySlotIndex].ItemStackSize;
			break;
		}
	}

	for (Index Index = 0; Index < UpgradeData->ForceWingUpgradeMaterialCount; Index += 1) {
		if (RequiredItemCount[Index] > ConsumableItemCount[Index]) goto error;
	}

	for (Index InventorySlotIndex = 0; InventorySlotIndex < Packet->InventorySlotCount; InventorySlotIndex += 1) {
		for (Index MaterialIndex = 0; MaterialIndex < UpgradeData->ForceWingUpgradeMaterialCount; MaterialIndex += 1) {
			RequiredItemCount[MaterialIndex] = UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].RequiredMaterialItemCount;

			if (!RTInventoryCanConsumeStackableItems(
				Runtime,
				&Character->InventoryInfo,
				UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].MaterialItemID,
				Packet->InventorySlots[InventorySlotIndex].ItemStackSize,
				1,
				&Packet->InventorySlots[InventorySlotIndex].InventorySlotIndex
			)) continue;

			RTInventoryConsumeStackableItems(
				Runtime,
				&Character->InventoryInfo,
				UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].MaterialItemID,
				Packet->InventorySlots[InventorySlotIndex].ItemStackSize,
				1,
				&Packet->InventorySlots[InventorySlotIndex].InventorySlotIndex
			);

			break;
		}
	}

	Character->ForceWingInfo.Grade = NextGradeInfoData->Grade;
	Character->ForceWingInfo.Level = NextGradeInfoData->MinLevel;

	for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT; Index += 1) {
		Character->ForceWingInfo.PresetTrainingPointCount[Index] += RUNTIME_CHARACTER_FORCE_WING_GRADE_TRAINING_POINT_COUNT;
	}

	RTDataForceWingSkillRef SkillData = RTRuntimeDataForceWingSkillGet(Runtime->Context, Character->ForceWingInfo.Grade);
	if (SkillData) {
		assert(0 <= SkillData->SlotIndex && SkillData->SlotIndex < RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_SKILL_COUNT);

		RTForceWingArrivalSkillSlotRef SkillSlot = &Character->ForceWingInfo.ArrivalSkillSlots[SkillData->SlotIndex];
		memset(SkillSlot, 0, sizeof(struct _RTForceWingArrivalSkillSlot));
		SkillSlot->SlotIndex = SkillData->SlotIndex;
	}

	Character->SyncMask.InventoryInfo = true;
	Character->SyncMask.ForceWingInfo = true;
	Character->SyncPriority.High = true;

	Response->Success = 1;
	Response->ForceWingGrade = Character->ForceWingInfo.Grade;
	Response->ForceWingLevel = Character->ForceWingInfo.Level;
	Response->ForceWingExp = Character->ForceWingInfo.Exp;
	Response->Unknown1 = 0;
	Response->AddedTrainingPointCount = RUNTIME_CHARACTER_FORCE_WING_GRADE_TRAINING_POINT_COUNT;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(FORCE_WING_LEVEL_UP) {
	if (!Character) goto error;

	Index PacketLength = sizeof(C2S_DATA_FORCE_WING_LEVEL_UP) + sizeof(UInt16) * (Packet->InventorySlotCount1 + Packet->InventorySlotCount2);
	if (Packet->Length < PacketLength) goto error;

	S2C_DATA_FORCE_WING_LEVEL_UP* Response = PacketBufferInit(Connection->PacketBuffer, S2C, FORCE_WING_LEVEL_UP);
	Response->Success = RTCharacterForceWingLevelUp(
		Runtime,
		Character,
		Packet->ItemStackCount1,
		Packet->InventorySlotCount1,
		&Packet->InventorySlotIndex[0],
		Packet->ItemStackCount2,
		Packet->InventorySlotCount2,
		&Packet->InventorySlotIndex[Packet->InventorySlotCount1]
	);
	Response->ForceWingGrade = Character->ForceWingInfo.Grade;
	Response->ForceWingLevel = Character->ForceWingInfo.Level;
	Response->ForceWingExp = Character->ForceWingInfo.Exp;
	Response->Unknown1 = 0;
	Response->AddedTrainingPointCount = (Response->Success) ? RUNTIME_CHARACTER_FORCE_WING_LEVEL_TRAINING_POINT_COUNT : 0;
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_FORCEWING_PRESET_SLOT) {
	if (!Character) goto error;

	S2C_DATA_SET_FORCEWING_PRESET_SLOT* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SET_FORCEWING_PRESET_SLOT);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ADD_FORCEWING_TRAINING_LEVEL) {
	if (!Character) goto error;

	S2C_DATA_ADD_FORCEWING_TRAINING_LEVEL* Response = PacketBufferInit(Connection->PacketBuffer, S2C, ADD_FORCEWING_TRAINING_LEVEL);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_ACTIVE_FORCEWING_PRESET) {
	if (!Character) goto error;

	S2C_DATA_SET_ACTIVE_FORCEWING_PRESET* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SET_ACTIVE_FORCEWING_PRESET);
	if (!RTCharacterForceWingSetActivePreset(Runtime, Character, Packet->PresetPageIndex)) {
		Response->Result = 1;
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ROLL_FORCEWING_ARRIVAL_SKILL) {
	if (!Character) goto error;

	S2C_DATA_ROLL_FORCEWING_ARRIVAL_SKILL* Response = PacketBufferInit(Connection->PacketBuffer, S2C, ROLL_FORCEWING_ARRIVAL_SKILL);
	if (!RTCharacterForceWingRollArrivalSkill(Runtime, Character, Packet->SkillSlotIndex, Packet->InventorySlotCount, &Packet->InventorySlotIndex[0])) {
		Response->Result = 1;
	}
	else {
		memcpy(
			&Response->SkillSlot,
			&Character->ForceWingInfo.ArrivalSkillSlots[Packet->SkillSlotIndex],
			sizeof(struct _RTForceWingArrivalSkillSlot)
		);
	}

	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CHANGE_FORCEWING_ARRIVAL_SKILL) {
	if (!Character) goto error;

	S2C_DATA_CHANGE_FORCEWING_ARRIVAL_SKILL* Response = PacketBufferInit(Connection->PacketBuffer, S2C, CHANGE_FORCEWING_ARRIVAL_SKILL);
	Response->Success = RTCharacterForceWingChangeArrivalSkill(Runtime, Character);
	return SocketSend(Socket, Connection, Response);

error:
	return SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_FORCEWING_TRAINING_SLOT_FLAGS) {
	if (!Character) goto error;

	for (Index Index = 0; Index < RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_SIZE; Index += 1) {
		Character->ForceWingInfo.TrainingUnlockFlags[Index] = Packet->Flags[Index];
	}

	Character->SyncMask.ForceWingInfo = true;
	Character->SyncPriority.Low = true;

	return;

error:
	return SocketDisconnect(Socket, Connection);
}
