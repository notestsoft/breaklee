#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_CHARACTER) {
	if (!Character) goto error;

	// TODO: Move logic to Runtime!

	if (!RTCharacterIsAlive(Runtime, Character)) goto error;

	RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);

	if (Packet->SlotIndex < 0 || Packet->SlotIndex > RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) {
		goto error;
	}

	PacketBufferRef ResponsePacketBuffer = SocketGetNextPacketBuffer(Socket);
	PacketBufferRef NotificationPacketBuffer = SocketGetNextPacketBuffer(Socket);

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, Packet->SkillIndex);
	if (!SkillData) goto error;

	S2C_DATA_SKILL_TO_CHARACTER* Response = PacketBufferInit(ResponsePacketBuffer, S2C, SKILL_TO_CHARACTER);
	Response->SkillIndex = Packet->SkillIndex;

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySlotIndex(Runtime, Character, Packet->SlotIndex);
	if (!SkillSlot) SkillSlot = RTCharacterGetSkillSlotBySkillIndex(Runtime, Character, Packet->SkillIndex);
	if (SkillSlot->ID != Packet->SkillIndex) goto error;

	// TODO: Add SkillData validations
	// TODO: Add skill cast time and cooldown checks
	// TODO: Consume sp

	Int32 RequiredMP = RTCharacterCalculateRequiredMP(
		Runtime,
		Character,
		SkillData->Mp[0],
		SkillData->Mp[1],
		SkillSlot->Level // TODO: This level has to be set to the item grade for the astral skill as it can be level 0
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) {
		// TODO: Send error notification
		// return; TODO: Some skills should always be executed like the astral skill even if there is no mp left!
	}

    RTCharacterAddMP(Runtime, Character, -RequiredMP, false);

	if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_MOVEMENT) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_MOVEMENT);
		if (Packet->Length != PacketLength) goto error;
		
		C2S_DATA_SKILL_GROUP_MOVEMENT* PacketData = (C2S_DATA_SKILL_GROUP_MOVEMENT*)&Packet->Data[0];

		S2C_DATA_SKILL_GROUP_MOVEMENT* ResponseData = PacketBufferAppendStruct(ResponsePacketBuffer, S2C_DATA_SKILL_GROUP_MOVEMENT);
		ResponseData->CharacterMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];

		Int32 CharacterPositionError = RTCalculateDistance(
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y,
			PacketData->PositionBegin.X,
			PacketData->PositionBegin.Y
		);
		//if (CharacterPositionError > 1) 
		//	goto error;
		
		Bool IsValidRange = RTCheckSkillTargetDistance(
			SkillData,
			1,
			1,
			PacketData->PositionBegin.X,
			PacketData->PositionBegin.Y,
			PacketData->PositionEnd.X,
			PacketData->PositionEnd.Y
		);
		//if (!IsValidRange) 
		//	goto error;

		RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
		if (!World) goto error;

		RTPosition TargetPosition = { 0, 0 };
		RTWorldTraceMovement(
			Runtime,
			World,
			PacketData->PositionBegin.X,
			PacketData->PositionBegin.Y,
			PacketData->PositionEnd.X,
			PacketData->PositionEnd.Y,
			&TargetPosition.X,
			&TargetPosition.Y,
            Character->Movement.CollisionMask,
			0
		);

		RTMovementEndDeadReckoning(Runtime, &Character->Movement);
        RTMovementSetPosition(Runtime, &Character->Movement, TargetPosition.X, TargetPosition.Y);
		Character->SyncMask.Info = true;

		SocketSend(Socket, Connection, Response);

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Notification = PacketBufferInit(NotificationPacketBuffer, S2C, NFY_SKILL_TO_CHARACTER);
		Notification->SkillIndex = Packet->SkillIndex;

		S2C_DATA_NFY_SKILL_GROUP_MOVEMENT* NotificationData = PacketBufferAppendStruct(NotificationPacketBuffer, S2C_DATA_NFY_SKILL_GROUP_MOVEMENT);
		NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
		NotificationData->Entity = Character->ID;
		NotificationData->PositionEnd.X = Character->Movement.PositionEnd.X;
		NotificationData->PositionEnd.Y = Character->Movement.PositionEnd.Y;
		BroadcastToWorld(
			Context,
			RTRuntimeGetWorldByCharacter(Runtime, Character),
			kEntityIDNull,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y,
			Notification
		);
		return;
	}
	else if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_ASTRAL) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_ASTRAL);
		if (Packet->Length < PacketLength) goto error;

		C2S_DATA_SKILL_GROUP_ASTRAL* PacketData = (C2S_DATA_SKILL_GROUP_ASTRAL*)&Packet->Data[0];
		PacketLength += sizeof(UInt16) * PacketData->InventorySlotCount;
		if (Packet->Length != PacketLength) goto error;

		if (SkillData->Intensity == RUNTIME_SKILL_INTENSITY_ASTRAL_WEAPON) {
			RTCharacterToggleAstralWeapon(Runtime, Character, PacketData->IsActivation, true);
		}
		else if (SkillData->Intensity == RUNTIME_SKILL_INTENSITY_ASTRAL_VEHICLE) {
			RTCharacterToggleAstralVehicle(Runtime, Character, PacketData->IsActivation, SkillSlot->ID);
		}
		else if (SkillData->Intensity == RUNTIME_SKILL_INTENSITY_ASTRAL_AURA) {
			if (PacketData->IsActivation) {
				RTCharacterStartAuraMode(Runtime, Character, SkillData->SkillID);
			}
			else {
				RTCharacterCancelAuraMode(Runtime, Character);
			}
		}
		
		S2C_DATA_SKILL_GROUP_ASTRAL* ResponseData = PacketBufferAppendStruct(ResponsePacketBuffer, S2C_DATA_SKILL_GROUP_ASTRAL);
		ResponseData->CurrentMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		ResponseData->IsActivation = PacketData->IsActivation;
		ResponseData->Unknown2 = PacketData->Unknown2;
		SocketSend(Socket, Connection, Response);
		return;
	}
	else if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_BATTLE_MODE) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_BATTLE_MODE);
		if (Packet->Length != PacketLength) goto error;

		C2S_DATA_SKILL_GROUP_BATTLE_MODE* PacketData = (C2S_DATA_SKILL_GROUP_BATTLE_MODE*)&Packet->Data[0];

		if (PacketData->IsActivation) {
			RTCharacterStartBattleMode(Runtime, Character, SkillData->SkillID);
		}
		else {
			RTCharacterCancelBattleMode(Runtime, Character);
		}

		S2C_DATA_SKILL_GROUP_BATTLE_MODE* ResponseData = PacketBufferAppendStruct(ResponsePacketBuffer, S2C_DATA_SKILL_GROUP_BATTLE_MODE);
		ResponseData->CurrentMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		ResponseData->IsActivation = RTCharacterIsBattleModeActive(Runtime, Character);
		SocketSend(Socket, Connection, Response);
		return;
	}
	else {
		goto error;
	}

	// TODO: Apply dash by attack skill if needed

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(CANCEL_BATTLE_MODE_SYNERGY) {
	if (!Character) goto error;

	return;

error:
	SocketDisconnect(Socket, Connection);
}