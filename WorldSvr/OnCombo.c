#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(COMBO_SKILL_EVENT) {
	if (!Character) goto error;

	if (!RTCharacterIsAlive(Runtime, Character)) {
		S2C_DATA_NFY_ERROR* Error = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_ERROR);
		Error->ErrorCommand = Packet->Command;
		Error->ErrorSubCommand = 0;
		Error->ErrorCode = 15;
		SocketSend(Socket, Connection, Error);
		return;
	}

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, Packet->SkillSlotIndex);
	if (!SkillSlot) goto error;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
	assert(SkillData);

	//if (pSkillListSlot->_stType != ST_SPECSKIL)
	//	return	P_ERROR;

	Int32 CaseCombo = 2;
	RTDataDummySkillRef DummySkill = RTRuntimeDataDummySkillGet(Runtime->Context, RUNTIME_DATA_DUMMY_SKILL_CASE_COMBO_MODE, SkillData->Element);
	assert(DummySkill);

	if (DummySkill->SkillIndex != SkillData->SkillID) goto error;

	if (!Character->Data.StyleInfo.ExtendedStyle.IsComboActive) {
		if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] < SkillData->Sp) {
			S2C_DATA_NFY_COMBO_SKILL_SET* Response = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_COMBO_SKILL_SET);
			Response->Result = S2C_DATA_COMBO_SKILL_SET_RESULT_FAILURE;
			Response->CurrentMP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
			SocketSend(Socket, Connection, Response);
			return;
		}

		Int32 RequiredMP = RTCharacterCalculateRequiredMP(
			Runtime,
			Character,
			SkillData->Mp[0],
			SkillData->Mp[1],
			SkillSlot->Level
		);

		if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) goto error;

		Character->Data.StyleInfo.ExtendedStyle.IsComboActive = true;
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] -= SkillData->Sp;
		Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] -= RequiredMP;
		Character->Data.Info.CurrentSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
		Character->Data.Info.CurrentMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		Character->SyncMask.Info = true;
	}
	else {
		Character->SkillComboLevel = 1;
		Character->Data.StyleInfo.ExtendedStyle.IsComboActive = false;
		Character->SyncMask.Info = true;
	}

	S2C_DATA_NFY_COMBO_SKILL_SET* Response = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_COMBO_SKILL_SET);
	Response->Result = S2C_DATA_COMBO_SKILL_SET_RESULT_SUCCESS;
	Response->CurrentMP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
	SocketSend(Socket, Connection, Response);

	S2C_DATA_NFY_COMBO_SKILL_EVENT* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_COMBO_SKILL_EVENT);
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
	BroadcastToWorld(
		Context,
		RTRuntimeGetWorldByCharacter(Runtime, Character),
		kEntityIDNull,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);

	if (Character->Data.StyleInfo.ExtendedStyle.IsComboActive) {
		S2C_DATA_NFY_CHARACTER_DATA* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_CHARACTER_DATA);
		Notification->Type = S2C_DATA_CHARACTER_UPDATE_TYPE_SP_DECREASE;
		Notification->SP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
		SocketSend(Socket, Connection, Notification);
	}

	return;

error:
	SocketDisconnect(Socket, Connection);
}
