#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_CHARACTER) {
	if (!Character) goto error;

	// TODO: Move logic to Runtime!

	if (!RTCharacterIsAlive(Runtime, Character)) goto error;

	RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);

	if (Packet->SlotIndex < 0 || Packet->SlotIndex > RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT ||
		Packet->SkillIndex < 0 || Packet->SkillIndex >= Runtime->CharacterSkillDataCount) {
		goto error;
	}

	// TODO: Packet could eventually contains SkillIndex but no slotindex
	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, Packet->SlotIndex);
	if (!SkillSlot) goto error;

	// 3, 32

	RTCharacterSkillDataRef Skill = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
	assert(Skill);

	// TODO: Add skill cast time and cooldown checks

	Int32 RequiredMP = RTCharacterCalculateRequiredMP(
		Runtime,
		Character,
		Skill->Mp[0],
		Skill->Mp[1],
		SkillSlot->Level
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) {
		// TODO: Send error notification
		return;
	}

    Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] -= RequiredMP;

	S2C_DATA_SKILL_TO_CHARACTER* Response = PacketInit(S2C_DATA_SKILL_TO_CHARACTER);
	Response->Command = S2C_SKILL_TO_CHARACTER;
	Response->SkillIndex = Packet->SkillIndex;

	if (Skill->SkillGroup == RUNTIME_SKILL_GROUP_MOVEMENT) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_MOVEMENT);
		if (Packet->Signature.Length != PacketLength) goto error;
		
		C2S_DATA_SKILL_GROUP_MOVEMENT* PacketData = (C2S_DATA_SKILL_GROUP_MOVEMENT*)&Packet->Data[0];

		S2C_DATA_SKILL_GROUP_MOVEMENT* ResponseData = PacketAppendStruct(S2C_DATA_SKILL_GROUP_MOVEMENT);
		ResponseData->CharacterMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];

		Int32 CharacterPositionError = RTCalculateDistance(
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y,
			PacketData->PositionBegin.X,
			PacketData->PositionBegin.Y
		);
		//if (CharacterPositionError > 1) 
		//	goto error;
		
		Bool IsValidRange = RTCheckSkillTargetDistance(
			Skill,
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
		Character->SyncMask |= RUNTIME_CHARACTER_SYNC_INFO;
		Character->SyncPriority |= RUNTIME_CHARACTER_SYNC_PRIORITY_LOW;

		SocketSend(Socket, Connection, Response);

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Notification = PacketInit(S2C_DATA_NFY_SKILL_TO_CHARACTER);
		Notification->Command = S2C_NFY_SKILL_TO_CHARACTER;
		Notification->SkillIndex = Packet->SkillIndex;

		S2C_DATA_NFY_SKILL_GROUP_MOVEMENT* NotificationData = PacketAppendStruct(S2C_DATA_NFY_SKILL_GROUP_MOVEMENT);
		NotificationData->CharacterIndex = Client->CharacterIndex;
		NotificationData->Entity = Character->ID;
		NotificationData->PositionEnd.X = Character->Movement.PositionEnd.X;
		NotificationData->PositionEnd.Y = Character->Movement.PositionEnd.Y;

		return BroadcastToWorld(
			Context,
			RTRuntimeGetWorldByCharacter(Runtime, Character),
			kEntityIDNull,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y,
			Notification
		);
	}
	else if (Skill->SkillGroup == RUNTIME_SKILL_GROUP_ASTRAL) {
		// TODO: Activate astral weapon
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_ASTRAL);
		if (Packet->Signature.Length != PacketLength) goto error;

		C2S_DATA_SKILL_GROUP_ASTRAL* PacketData = (C2S_DATA_SKILL_GROUP_ASTRAL*)&Packet->Data[0];

		S2C_DATA_SKILL_GROUP_ASTRAL* ResponseData = PacketAppendStruct(S2C_DATA_SKILL_GROUP_ASTRAL);
		ResponseData->Unknown1 = PacketData->Unknown1;
		ResponseData->Unknown2 = PacketData->Unknown2;

		SocketSend(Socket, Connection, Response);

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Notification = PacketInit(S2C_DATA_NFY_SKILL_TO_CHARACTER);
		Notification->Command = S2C_NFY_SKILL_TO_CHARACTER;
		Notification->SkillIndex = Packet->SkillIndex;

		S2C_DATA_NFY_SKILL_GROUP_ASTRAL_WEAPON* NotificationData = PacketAppendStruct(S2C_DATA_NFY_SKILL_GROUP_ASTRAL_WEAPON);
		NotificationData->CharacterIndex = Client->CharacterIndex;
		NotificationData->Entity = Character->ID;
		NotificationData->Position.X = Character->Movement.PositionCurrent.X;
		NotificationData->Position.Y = Character->Movement.PositionCurrent.Y;
		NotificationData->Unknown1 = PacketData->Unknown1;
		NotificationData->Unknown2 = PacketData->Unknown2;

		return BroadcastToWorld(
			Context,
			RTRuntimeGetWorldByCharacter(Runtime, Character),
			kEntityIDNull,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y,
			Notification
		);
	}
	else if (Skill->SkillGroup == RUNTIME_SKILL_GROUP_BATTLE_MODE) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_BATTLE_MODE);
		if (Packet->Signature.Length != PacketLength) goto error;

		C2S_DATA_SKILL_GROUP_BATTLE_MODE* PacketData = (C2S_DATA_SKILL_GROUP_BATTLE_MODE*)&Packet->Data[0];

		// TODO: Activate, deactivate battle mode

		S2C_DATA_SKILL_GROUP_BATTLE_MODE* ResponseData = PacketAppendStruct(S2C_DATA_SKILL_GROUP_BATTLE_MODE);
		ResponseData->CurrentMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		ResponseData->IsActivation = PacketData->IsActivation;

		SocketSend(Socket, Connection, Response);

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Notification = PacketInit(S2C_DATA_NFY_SKILL_TO_CHARACTER);
		Notification->Command = S2C_NFY_SKILL_TO_CHARACTER;
		Notification->SkillIndex = Packet->SkillIndex;

		S2C_DATA_NFY_SKILL_GROUP_BATTLE_MODE* NotificationData = PacketAppendStruct(S2C_DATA_NFY_SKILL_GROUP_BATTLE_MODE);
		NotificationData->CharacterIndex = Client->CharacterIndex;
		NotificationData->CharacterStyle = SwapUInt32(Character->Info.Style.RawValue);
		NotificationData->CharacterLiveStyle = SwapUInt32(0); // TODO: Add character livestyle
		NotificationData->StyleExtension = 0; // TODO: Check what StyleExtension is good for
		NotificationData->IsActivation = PacketData->IsActivation;

		return BroadcastToWorld(
			Context,
			RTRuntimeGetWorldByCharacter(Runtime, Character),
			kEntityIDNull,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y,
			Notification
		);
	}
	else {
		goto error;
	}

	// TODO: Apply dash by attack skill if needed

	return SocketSend(Socket, Connection, Response);

	/* TODO: Send notification S2C_DATA_SKILL_TO_CHARACTER_UPDATE
	
	S2C_DATA_SKILL_TO_MOB_UPDATE* Notification = PacketInit(S2C_DATA_SKILL_TO_MOB_UPDATE);
	Notification->Command = S2C_SKILL_TO_MOB_UPDATE;
	Notification->SkillIndex = Response->SkillIndex;
	Notification->TargetCount = Response->TargetCount;
	Notification->CharacterID = Character->ID;
	Notification->SetPositionX = Character->Info.Position.X;
	Notification->SetPositionY = Character->Info.Position.Y;
	Notification->CharacterHP = Response->CharacterHP;

	for (Int32 Index = 0; Index < Response->TargetCount; Index++) {
		S2C_DATA_SKILL_TO_MOB_TARGET* TargetResponse = &Response->Data[Index];
		S2C_DATA_SKILL_TO_MOB_TARGET_UPDATE* TargetNotification = PacketAppendStruct(S2C_DATA_SKILL_TO_MOB_TARGET_UPDATE);
		TargetNotification->EntityID = TargetResponse->EntityID;
		TargetNotification->WorldID = TargetResponse->WorldID;
		TargetNotification->EntityType = TargetResponse->EntityType;
		TargetNotification->AttackType = TargetResponse->AttackType;
		TargetNotification->MobHP = TargetResponse->MobHP;
		TargetNotification->Unknown3 = 1;
	}

	return BroadcastToWorld(
		Server,
		World,
        kEntityNull,
		Character->ID,
		Character->Info.Position.X,
		Character->Info.Position.Y,
		Notification
	);
	*/

error:
	return SocketDisconnect(Socket, Connection);
}

