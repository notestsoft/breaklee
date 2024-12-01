#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "Notification.h"
#include "Server.h"

// TODO: Move ForceWingGradeUp to RuntimeLib!
CLIENT_PROCEDURE_BINDING(FORCE_WING_GRADE_UP) {
	if (!Character) goto error;

	Int PacketLength = sizeof(C2S_DATA_FORCE_WING_GRADE_UP) + sizeof(UInt16) * (Packet->InventorySlotCount);
	if (Packet->Length < PacketLength) goto error;

	S2C_DATA_FORCE_WING_GRADE_UP* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, FORCE_WING_GRADE_UP);
	Response->Success = false;

	if (Character->Data.ForceWingInfo.Info.Grade < 1) goto error;

	RTDataForceWingGradeInfoRef GradeInfoData = RTRuntimeDataForceWingGradeInfoGet(Runtime->Context, Character->Data.ForceWingInfo.Info.Grade);
	if (!GradeInfoData) goto error;

	if (Character->Data.ForceWingInfo.Info.Level < GradeInfoData->MaxLevel) goto error;

	RTDataForceWingGradeInfoRef NextGradeInfoData = RTRuntimeDataForceWingGradeInfoGet(Runtime->Context, Character->Data.ForceWingInfo.Info.Grade + 1);
	if (!NextGradeInfoData) goto error;

	RTDataForceWingUpgradeRef UpgradeData = RTRuntimeDataForceWingUpgradeGet(Runtime->Context, Character->Data.ForceWingInfo.Info.Grade);
	if (!UpgradeData) goto error;

	Int64 RequiredItemCount[RUNTIME_DATA_MAX_FORCE_WING_UPGRADE_MATERIAL_COUNT] = { 0 };
	for (Int Index = 0; Index < UpgradeData->ForceWingUpgradeMaterialCount; Index += 1) {
		RequiredItemCount[Index] = UpgradeData->ForceWingUpgradeMaterialList[Index].RequiredMaterialItemCount;
	}

	Int64 ConsumableItemCount[RUNTIME_DATA_MAX_FORCE_WING_UPGRADE_MATERIAL_COUNT] = { 0 };
	for (Int InventorySlotIndex = 0; InventorySlotIndex < Packet->InventorySlotCount; InventorySlotIndex += 1) {
		for (Int MaterialIndex = 0; MaterialIndex < UpgradeData->ForceWingUpgradeMaterialCount; MaterialIndex += 1) {
			RequiredItemCount[MaterialIndex] = UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].RequiredMaterialItemCount;

			if (!RTInventoryCanConsumeStackableItems(
				Runtime,
				&Character->Data.InventoryInfo,
				UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].MaterialItemID,
				Packet->InventorySlots[InventorySlotIndex].ItemStackSize,
				1,
				&Packet->InventorySlots[InventorySlotIndex].InventorySlotIndex
			)) continue;

			ConsumableItemCount[MaterialIndex] += Packet->InventorySlots[InventorySlotIndex].ItemStackSize;
			break;
		}
	}

	for (Int Index = 0; Index < UpgradeData->ForceWingUpgradeMaterialCount; Index += 1) {
		if (RequiredItemCount[Index] > ConsumableItemCount[Index]) goto error;
	}

	for (Int InventorySlotIndex = 0; InventorySlotIndex < Packet->InventorySlotCount; InventorySlotIndex += 1) {
		for (Int MaterialIndex = 0; MaterialIndex < UpgradeData->ForceWingUpgradeMaterialCount; MaterialIndex += 1) {
			RequiredItemCount[MaterialIndex] = UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].RequiredMaterialItemCount;

			if (!RTInventoryCanConsumeStackableItems(
				Runtime,
				&Character->Data.InventoryInfo,
				UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].MaterialItemID,
				Packet->InventorySlots[InventorySlotIndex].ItemStackSize,
				1,
				&Packet->InventorySlots[InventorySlotIndex].InventorySlotIndex
			)) continue;

			RTInventoryConsumeStackableItems(
				Runtime,
				&Character->Data.InventoryInfo,
				UpgradeData->ForceWingUpgradeMaterialList[MaterialIndex].MaterialItemID,
				Packet->InventorySlots[InventorySlotIndex].ItemStackSize,
				1,
				&Packet->InventorySlots[InventorySlotIndex].InventorySlotIndex
			);

			break;
		}
	}

	Character->Data.ForceWingInfo.Info.Grade = NextGradeInfoData->Grade;
	Character->Data.ForceWingInfo.Info.Level = NextGradeInfoData->MinLevel;

	Int32 AddedTrainingPointCount = NextGradeInfoData->Grade * RUNTIME_CHARACTER_FORCE_WING_GRADE_TRAINING_POINT_COUNT;

	for (Int Index = 0; Index < RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_COUNT; Index += 1) {
		Character->Data.ForceWingInfo.Info.PresetTrainingPointCount[Index] += AddedTrainingPointCount;
	}

	RTDataForceWingSkillRef SkillData = RTRuntimeDataForceWingSkillGet(Runtime->Context, Character->Data.ForceWingInfo.Info.Grade);
	if (SkillData) {
		assert(0 <= SkillData->SlotIndex && SkillData->SlotIndex < RUNTIME_CHARACTER_MAX_FORCE_WING_ARRIVAL_SKILL_COUNT);

		RTForceWingArrivalSkillSlotRef SkillSlot = &Character->Data.ForceWingInfo.Info.ArrivalSkillSlots[SkillData->SlotIndex];
		memset(SkillSlot, 0, sizeof(struct _RTForceWingArrivalSkillSlot));
		SkillSlot->SlotIndex = SkillData->SlotIndex;
	}

	Int32 ForceWingSkillSlotIndices[] = {
		RUNTIME_SPECIAL_SKILL_SLOT_FORCE_WING_1,
		RUNTIME_SPECIAL_SKILL_SLOT_FORCE_WING_2,
		RUNTIME_SPECIAL_SKILL_SLOT_FORCE_WING_3,
	};

	for (Int Index = 0; Index < sizeof(ForceWingSkillSlotIndices) / sizeof(Int32); Index += 1) {
		Int32 SlotIndex = ForceWingSkillSlotIndices[Index];
		RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySlotIndex(Runtime, Character, SlotIndex);
		if (!SkillSlot) continue;

		RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
		assert(SkillData);
		if (SkillData->SkillGroup != RUNTIME_SKILL_GROUP_WING) continue;
		
		RTCharacterChangeSkillLevel(Runtime, Character, SkillSlot->ID, SkillSlot->Index, SkillSlot->Level, Character->Data.ForceWingInfo.Info.Grade);
	}

	Character->SyncMask.InventoryInfo = true;
	Character->SyncMask.ForceWingInfo = true;

	Response->Success = 1;
	Response->ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
	Response->ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
	Response->ForceWingExp = Character->Data.ForceWingInfo.Info.Exp;
	Response->Unknown1 = 0;
	Response->AddedTrainingPointCount = AddedTrainingPointCount;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(FORCE_WING_LEVEL_UP) {
	if (!Character) goto error;

	Int PacketLength = sizeof(C2S_DATA_FORCE_WING_LEVEL_UP) + sizeof(UInt16) * ((Int)Packet->InventorySlotCount1 + Packet->InventorySlotCount2);
	if (Packet->Length < PacketLength) goto error;

	S2C_DATA_FORCE_WING_LEVEL_UP* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, FORCE_WING_LEVEL_UP);
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
	Response->ForceWingGrade = Character->Data.ForceWingInfo.Info.Grade;
	Response->ForceWingLevel = Character->Data.ForceWingInfo.Info.Level;
	Response->ForceWingExp = Character->Data.ForceWingInfo.Info.Exp;
	Response->Unknown1 = 0;
	Response->AddedTrainingPointCount = (Response->Success) ? RUNTIME_CHARACTER_FORCE_WING_LEVEL_TRAINING_POINT_COUNT : 0;
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_FORCEWING_PRESET_SLOT) {
	if (!Character) goto error;

	S2C_DATA_SET_FORCEWING_PRESET_SLOT* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, SET_FORCEWING_PRESET_SLOT);
	if (!RTCharacterForceWingSetPresetTraining(Runtime, Character, Packet->PresetPageIndex, Packet->PresetSlotIndex, Packet->TrainingSlotIndex)) {
		Response->Result = 1;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ADD_FORCEWING_TRAINING_LEVEL) {
	if (!Character) goto error;

	S2C_DATA_ADD_FORCEWING_TRAINING_LEVEL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ADD_FORCEWING_TRAINING_LEVEL);
	if (!RTCharacterForceWingAddTrainingLevel(Runtime, Character, Packet->PresetPageIndex, Packet->PresetSlotIndex, Packet->TrainingSlotIndex, Packet->AddedTrainingLevel, &Response->TrainingLevel)) {
		Response->Result = 1;
	}
	else {
		Response->RemainingTrainingPointCount = Character->Data.ForceWingInfo.Info.PresetTrainingPointCount[Packet->PresetPageIndex];
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_ACTIVE_FORCEWING_PRESET) {
	if (!Character) goto error;

	S2C_DATA_SET_ACTIVE_FORCEWING_PRESET* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, SET_ACTIVE_FORCEWING_PRESET);
	if (!RTCharacterForceWingSetActivePreset(Runtime, Character, Packet->PresetPageIndex)) {
		Response->Result = 1;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(ROLL_FORCEWING_ARRIVAL_SKILL) {
	if (!Character) goto error;

	S2C_DATA_ROLL_FORCEWING_ARRIVAL_SKILL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ROLL_FORCEWING_ARRIVAL_SKILL);
	if (!RTCharacterForceWingRollArrivalSkill(Runtime, Character, Packet->SkillSlotIndex, Packet->InventorySlotCount, &Packet->InventorySlotIndex[0])) {
		Response->Result = 1;
	}
	else {
		memcpy(
			&Response->SkillSlot,
			&Character->Data.ForceWingInfo.Info.ArrivalSkillSlots[Packet->SkillSlotIndex],
			sizeof(struct _RTForceWingArrivalSkillSlot)
		);
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CHANGE_FORCEWING_ARRIVAL_SKILL) {
	if (!Character) goto error;

	S2C_DATA_CHANGE_FORCEWING_ARRIVAL_SKILL* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, CHANGE_FORCEWING_ARRIVAL_SKILL);
	Response->Success = RTCharacterForceWingChangeArrivalSkill(Runtime, Character);
	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(SET_FORCEWING_TRAINING_SLOT_FLAGS) {
	if (!Character) goto error;

	for (Int Index = 0; Index < RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_PAGE_SIZE; Index += 1) {
		Character->Data.ForceWingInfo.Info.TrainingUnlockFlags[Index] = Packet->Flags[Index];
	}

	Character->SyncMask.ForceWingInfo = true;
	return;

error:
	SocketDisconnect(Socket, Connection);
}
