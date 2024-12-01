#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_TARGET) {
	if (!Character) goto error;

	RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
	if (!WorldContext) goto error;

	if (!RTCharacterIsAlive(Runtime, Character)) goto error;

	RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySkillIndex(Runtime, Character, Packet->SkillIndex); 
	if (!SkillSlot) goto error;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
	if (!SkillData) goto error;

	// TODO: Add SkillData validations
	// TODO: Add skill cast time and cooldown checks
	// TODO: Consume SP

	Int32 RequiredMP = RTCharacterCalculateRequiredMP(
		Runtime,
		Character,
		SkillData->Mp[0],
		SkillData->Mp[1],
		SkillSlot->Level // TODO: This level has to be set to the item grade for the astral skill as it can be level 0
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) {
		// TODO: Send error notification
		goto error;
	}

	PacketBufferRef ResponsePacketBuffer = SocketGetNextPacketBuffer(Socket);
	PacketBufferRef NotificationPacketBuffer = SocketGetNextPacketBuffer(Socket);

	if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_BUFF || SkillData->SkillGroup == RUNTIME_SKILL_GROUP_DEBUFF) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_TARGET) + sizeof(C2S_DATA_SKILL_TO_TARGET_BUFF);
		if (Packet->Length < PacketLength) goto error;

		C2S_DATA_SKILL_TO_TARGET_BUFF* PacketData = (C2S_DATA_SKILL_TO_TARGET_BUFF*)&Packet->Data[0];
		PacketLength += sizeof(C2S_DATA_SKILL_TO_TARGET_BUFF_TARGET) * PacketData->TargetCount + sizeof(C2S_DATA_SKILL_TO_TARGET_BUFF_TAIL);
		if (Packet->Length != PacketLength) goto error;
		if (SkillSlot->Index != PacketData->SlotIndex) goto error;

		RTCharacterAddMP(Runtime, Character, -RequiredMP, false);

		S2C_DATA_SKILL_TO_TARGET_BUFF* Response = PacketBufferInit(ResponsePacketBuffer, S2C, SKILL_TO_TARGET_BUFF);
		Response->SkillIndex = Packet->SkillIndex;

		S2C_DATA_NFY_SKILL_TO_TARGET_BUFF* Notification = PacketBufferInit(NotificationPacketBuffer, S2C, NFY_SKILL_TO_TARGET_BUFF);
		Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
		Notification->SkillIndex = Packet->SkillIndex;

		C2S_DATA_SKILL_TO_TARGET_BUFF_TARGET* PacketTargetData = (C2S_DATA_SKILL_TO_TARGET_BUFF_TARGET*)(&Packet->Data[0] + sizeof(C2S_DATA_SKILL_TO_TARGET_BUFF));
		C2S_DATA_SKILL_TO_TARGET_BUFF_TAIL* PacketTailData = (C2S_DATA_SKILL_TO_TARGET_BUFF_TAIL*)((UInt8*)PacketTargetData + sizeof(C2S_DATA_SKILL_TO_TARGET_BUFF_TARGET) * PacketData->TargetCount);

		Response->SkillLevel = SkillSlot->Level;
		Response->BuffType = 0;
		Response->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		Response->CurrentMP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		Response->CurrentSP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
		Response->TargetCount = PacketData->TargetCount;

		Notification->SkillLevel = SkillSlot->Level;
		Notification->Unknown1 = (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_DEBUFF) ? 1 : 0;
		Notification->BuffType = 0;
		Notification->Unknown2 = 0;
		Notification->TargetCount = PacketData->TargetCount;
		Notification->ItemID = 0;
		Notification->EffectorItemID = 0;
		Notification->EffectorItemOptions = 0;
		Notification->SkillTranscendenceLevel = 0;
		Notification->SkillTranscendenceIndex = 0;

		// TODO: Validate Targets

		for (Int TargetIndex = 0; TargetIndex < PacketData->TargetCount; TargetIndex += 1) {
			RTEntityID TargetID = PacketTargetData[TargetIndex].TargetID;
			UInt8 TargetIDType = PacketTargetData[TargetIndex].TargetType;

			S2C_DATA_SKILL_TO_TARGET_BUFF_TARGET* ResponseTarget = PacketBufferAppendStruct(ResponsePacketBuffer, S2C_DATA_SKILL_TO_TARGET_BUFF_TARGET);
			ResponseTarget->TargetIndex = TargetID.Serial;
			ResponseTarget->TargetType = TargetIDType;

			S2C_DATA_NFY_SKILL_TO_TARGET_BUFF_TARGET* NotificationTarget = PacketBufferAppendStruct(NotificationPacketBuffer, S2C_DATA_NFY_SKILL_TO_TARGET_BUFF_TARGET);
			NotificationTarget->TargetIndex = TargetID.Serial;
			NotificationTarget->TargetType = TargetIDType;
			
			if (TargetIndex >= SkillData->MaxTarget) continue;

			if (TargetIDType == RUNTIME_ENTITY_TYPE_CHARACTER) {
				RTCharacterRef Target = RTWorldManagerGetCharacter(Runtime->WorldManager, TargetID);
				if (Target) {
					ResponseTarget->TargetIndex = (UInt32)Target->CharacterIndex;
					ResponseTarget->Result = RTCharacterApplyBuff(Runtime, Target, SkillSlot, SkillData, RUNTIME_BUFF_SLOT_TYPE_SKILL);
					ResponseTarget->Duration = Target->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
					NotificationTarget->TargetIndex = ResponseTarget->TargetIndex;
					NotificationTarget->Result = ResponseTarget->Result;
					NotificationTarget->Duration = Target->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
				}
			}
			else if (TargetIDType == RUNTIME_ENTITY_TYPE_MOB) {
				RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
				RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
				if (Target) {
					ResponseTarget->TargetIndex = Target->ID.Serial;
					ResponseTarget->Result = RTMobApplyBuff(Runtime, Character, Target, SkillSlot, SkillData);
					ResponseTarget->Duration = Target->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
					NotificationTarget->Result = ResponseTarget->Result;
					NotificationTarget->Duration = Target->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
				}
			}
			else {
				goto error;
			}
		}

		S2C_DATA_SKILL_TO_TARGET_BUFF_TAIL* ResponseTail = PacketBufferAppendStruct(ResponsePacketBuffer, S2C_DATA_SKILL_TO_TARGET_BUFF_TAIL);
		S2C_DATA_NFY_SKILL_TO_TARGET_BUFF_TAIL* NotificationTail = PacketBufferAppendStruct(NotificationPacketBuffer, S2C_DATA_NFY_SKILL_TO_TARGET_BUFF_TAIL);

		SocketSend(Socket, Connection, Response);

		if (!RTEntityIsNull(WorldContext->Party)) {
			BroadcastToParty(Context, WorldContext->Party, Notification);
		}
		else { 
			BroadcastToWorld(
				Context,
				RTRuntimeGetWorldByCharacter(Runtime, Character),
				kEntityIDNull,
				Character->Movement.PositionCurrent.X,
				Character->Movement.PositionCurrent.Y,
				Notification
			);
		}
	}
	else if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_WING) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_TARGET) + sizeof(C2S_DATA_SKILL_TO_TARGET_WING);
		if (Packet->Length < PacketLength) goto error;

		C2S_DATA_SKILL_TO_TARGET_WING* PacketData = (C2S_DATA_SKILL_TO_TARGET_WING*)&Packet->Data[0];
		if (SkillSlot->Index != PacketData->SlotIndex) goto error;

		RTCharacterAddMP(Runtime, Character, -RequiredMP, false);
		RTCharacterApplyBuff(Runtime, Character, SkillSlot, SkillData, RUNTIME_BUFF_SLOT_TYPE_FORCE_WING);

		S2C_DATA_SKILL_TO_CHARACTER* Response = PacketBufferInit(ResponsePacketBuffer, S2C, SKILL_TO_CHARACTER);
		Response->SkillIndex = Packet->SkillIndex;

		S2C_DATA_SKILL_GROUP_WING* ResponseData = PacketBufferAppendStruct(ResponsePacketBuffer, S2C_DATA_SKILL_GROUP_WING);
		ResponseData->CurrentMP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		ResponseData->CurrentSP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
		SocketSend(Socket, Connection, Response);
	}
	else {
		goto error;
	}

	return;

error:
	SocketDisconnect(Socket, Connection);
}

CLIENT_PROCEDURE_BINDING(REMOVE_BUFF) {
	if (!Character) goto error;

	S2C_DATA_REMOVE_BUFF* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, REMOVE_BUFF);
	Response->SkillIndex = Packet->SkillIndex;

	if (!RTCharacterRemoveBuff(Runtime, Character, Packet->SkillIndex)) {
		Response->Result = 1;
	}

	SocketSend(Socket, Connection, Response);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
