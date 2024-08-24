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

	// TODO: Packet could eventually contains SkillIndex but no slotindex
	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, Packet->SlotIndex);
	if (!SkillSlot) goto error;

	// 3, 32

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
	assert(SkillData);

	// TODO: Add SkillData validations
	// TODO: Add skill cast time and cooldown checks

	Int32 RequiredMP = RTCharacterCalculateRequiredMP(
		Runtime,
		Character,
		SkillData->Mp[0],
		SkillData->Mp[1],
		SkillSlot->Level
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) {
		// TODO: Send error notification
		return;
	}

    RTCharacterAddMP(Runtime, Character, -RequiredMP, false);

	S2C_DATA_SKILL_TO_CHARACTER* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SKILL_TO_CHARACTER);
	Response->SkillIndex = Packet->SkillIndex;

	if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_MOVEMENT) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_MOVEMENT);
		if (Packet->Length != PacketLength) goto error;
		
		C2S_DATA_SKILL_GROUP_MOVEMENT* PacketData = (C2S_DATA_SKILL_GROUP_MOVEMENT*)&Packet->Data[0];

		S2C_DATA_SKILL_GROUP_MOVEMENT* ResponseData = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_SKILL_GROUP_MOVEMENT);
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

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_SKILL_TO_CHARACTER);
		Notification->SkillIndex = Packet->SkillIndex;

		S2C_DATA_NFY_SKILL_GROUP_MOVEMENT* NotificationData = PacketBufferAppendStruct(Context->ClientSocket->PacketBuffer, S2C_DATA_NFY_SKILL_GROUP_MOVEMENT);
		NotificationData->CharacterIndex = (UInt32)Client->CharacterIndex;
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
		if (Packet->Length != PacketLength) goto error;

		C2S_DATA_SKILL_GROUP_ASTRAL* PacketData = (C2S_DATA_SKILL_GROUP_ASTRAL*)&Packet->Data[0];

		// TODO: Activate, deactivate battle mode, do runtime validations
		Character->Data.StyleInfo.ExtendedStyle.IsAstralWeaponActive = PacketData->IsActivation;

		S2C_DATA_SKILL_GROUP_ASTRAL* ResponseData = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_SKILL_GROUP_ASTRAL);
		ResponseData->CurrentMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		ResponseData->IsActivation = PacketData->IsActivation;
		ResponseData->Unknown2 = PacketData->Unknown2;
		SocketSend(Socket, Connection, Response);

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_SKILL_TO_CHARACTER);
		Notification->SkillIndex = Packet->SkillIndex;

		S2C_DATA_NFY_SKILL_GROUP_ASTRAL_WEAPON* NotificationData = PacketBufferAppendStruct(Context->ClientSocket->PacketBuffer, S2C_DATA_NFY_SKILL_GROUP_ASTRAL_WEAPON);
		NotificationData->CharacterIndex = (UInt32)Client->CharacterIndex;
		NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
		NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
		NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
		NotificationData->IsActivation = PacketData->IsActivation;
		NotificationData->Unknown2 = PacketData->Unknown2;

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
	else if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_BATTLE_MODE) {
		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_BATTLE_MODE);
		if (Packet->Length != PacketLength) goto error;

		C2S_DATA_SKILL_GROUP_BATTLE_MODE* PacketData = (C2S_DATA_SKILL_GROUP_BATTLE_MODE*)&Packet->Data[0];

		// TODO: Activate, deactivate battle mode, do runtime validations

		if (PacketData->IsActivation) {
			Character->Data.StyleInfo.ExtendedStyle.BattleModeFlags |= (1 << (SkillData->Intensity - 1));
			Character->BattleModeSkillIndex = SkillData->SkillID;
			Character->BattleModeTimeout = PlatformGetTickCount() + 90000; // TODO: Check where the duration is stored in
		}
		else {
			Character->Data.StyleInfo.ExtendedStyle.BattleModeFlags &= ~(1 << (SkillData->Intensity - 1));
		}

		S2C_DATA_SKILL_GROUP_BATTLE_MODE* ResponseData = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_SKILL_GROUP_BATTLE_MODE);
		ResponseData->CurrentMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		ResponseData->IsActivation = PacketData->IsActivation;
		SocketSend(Socket, Connection, Response);

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_SKILL_TO_CHARACTER);
		Notification->SkillIndex = Packet->SkillIndex;

		S2C_DATA_NFY_SKILL_GROUP_BATTLE_MODE* NotificationData = PacketBufferAppendStruct(Context->ClientSocket->PacketBuffer, S2C_DATA_NFY_SKILL_GROUP_BATTLE_MODE);
		NotificationData->CharacterIndex = (UInt32)Client->CharacterIndex;
		NotificationData->CharacterStyle = Character->Data.StyleInfo.Style.RawValue;
		NotificationData->CharacterLiveStyle = Character->Data.StyleInfo.LiveStyle.RawValue;
		NotificationData->CharacterExtendedStyle = Character->Data.StyleInfo.ExtendedStyle.RawValue;
		NotificationData->IsActivation = PacketData->IsActivation;

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
	else {
		goto error;
	}

	// TODO: Apply dash by attack skill if needed

	SocketSend(Socket, Connection, Response);
	return;

	/* TODO: Send notification S2C_DATA_SKILL_TO_CHARACTER_UPDATE
	
	S2C_DATA_SKILL_TO_MOB_UPDATE* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, SKILL_TO_MOB_UPDATE);
	Notification->SkillIndex = Response->SkillIndex;
	Notification->TargetCount = Response->TargetCount;
	Notification->CharacterID = Character->ID;
	Notification->SetPositionX = Character->Data.Info.PositionX;
	Notification->SetPositionY = Character->Data.Info.PositionY;
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
		Character->Data.Info.PositionX,
		Character->Data.Info.PositionY,
		Notification
	);
	*/

error:
	SocketDisconnect(Socket, Connection);
}

