#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_TARGET) {
	if (!Character) goto error;

	// TODO: Move logic to Runtime!
	/*
	if (!RTCharacterIsAlive(Runtime, Character)) goto error;

	RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);
	
	if (Packet->SlotIndex < 0 || Packet->SlotIndex > RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) {
		goto error;
	}

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySlotIndex(Runtime, Character, Packet->SlotIndex);
	if (!SkillSlot) SkillSlot = RTCharacterGetSkillSlotBySkillIndex(Runtime, Character, Packet->SkillIndex);
	if (!SkillSlot) goto error;
	*/
	/*
	C2S_DATA_SKILL_TO_TARGET_WING_TARGET
		S2C_DATA_SKILL_TO_CHARACTER_WING
	*/
	S2C_DATA_SKILL_TO_TARGET_BUFF* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SKILL_TO_TARGET_BUFF);
	Response->SkillIndex = Packet->SkillIndex;
	Response->SkillLevel = 1;
	Response->Unknown1 = 123;
	Response->Unknown2 = 234;
	Response->Unknown3 = 345;
	Response->Unknown4 = 456;
	Response->Unknown5 = 255;
	Response->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Response->CurrentMP = (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];

	Response->TargetCount = 1;
	S2C_DATA_SKILL_TO_TARGET_BUFF_TARGET* ResponseTarget = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_SKILL_TO_TARGET_BUFF_TARGET);
	ResponseTarget->SourceIndex = 1;
	ResponseTarget->TargetIndex = (UInt32)Character->CharacterIndex;
	ResponseTarget->TargetType = RUNTIME_ENTITY_TYPE_CHARACTER;
	ResponseTarget->Result = 1;
	ResponseTarget->Unknown1 = 1;
	ResponseTarget->Unknown2 = 1;

	for (Int32 Index = 0; Index < 39; Index++) {
		ResponseTarget->Unknown3[Index] = Index + 1;
	}

	SocketSend(Socket, Connection, Response);

	static Int32 Offset = 0;

	S2C_DATA_NFY_SKILL_TO_TARGET_BUFF* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_SKILL_TO_TARGET_BUFF);
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->SkillIndex = Packet->SkillIndex;
	Notification->SkillLevel = 1;
	Notification->Unknown2 = 0; // Only takes 0 or 1, moves the slot to second row (penet potion slot)
	Notification->Unknown3 = 0;
	Notification->Unknown4 = 0;
	Notification->Unknown5 = 0; // Flag indicating if the skill is stackable
	Notification->Unknown6 = 0;
	Notification->Unknown7 = 0;
	Notification->Unknown8 = 0;
	Notification->TargetCount = 1;
	//Notification->EffectorItemID = 33554677;
	//Notification->EffectorItemOptions = 460544;

	S2C_DATA_NFY_SKILL_TO_TARGET_BUFF_TARGET* NotificationTarget = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_NFY_SKILL_TO_TARGET_BUFF_TARGET);
	NotificationTarget->TargetIndex = (UInt32)Character->CharacterIndex;
	NotificationTarget->TargetType = RUNTIME_ENTITY_TYPE_CHARACTER;
	NotificationTarget->Result = 1;
	NotificationTarget->UnknownA[0] = 0;
	NotificationTarget->UnknownA[3] = Packet->SkillIndex;
	NotificationTarget->UnknownTimer = 1000000;

	for (Int32 Index = 0; Index < 8; Index++)
		NotificationTarget->Unknown4[Index] = RandomRange(&Offset, 0, UINT8_MAX);

	SocketSend(Socket, Connection, Notification);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
