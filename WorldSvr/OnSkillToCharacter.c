#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_CHARACTER) {
	if (!Character) goto error;

	// TODO: Move logic to Runtime!

	//if (!RTCharacterIsAlive(Runtime, Character)) goto error;

	RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);

	if (Packet->SlotIndex < 0 || Packet->SlotIndex > RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT) {
		goto error;
	}

	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotBySlotIndex(Runtime, Character, Packet->SlotIndex);
	if (!SkillSlot) SkillSlot = RTCharacterGetSkillSlotBySkillIndex(Runtime, Character, Packet->SkillIndex);
	if (!SkillSlot) goto error;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
	assert(SkillData);

	Bool UseMoveStep = false;
	Bool UseStun = false;
	Bool UseHitCount = false;
	Bool UseDebuff = false;

	// TODO: Add skill cast time and cooldown checks

	// TODO: Calculate timing of combo skill

	PacketBufferRef ResponsePacketBuffer = SocketGetNextPacketBuffer(Socket);
	PacketBufferRef NotificationPacketBuffer = SocketGetNextPacketBuffer(Socket);

	S2C_DATA_SKILL_TO_CHARACTER* Response = PacketBufferInit(ResponsePacketBuffer, S2C, SKILL_TO_CHARACTER);
	Response->SkillIndex = Packet->SkillIndex;

	Int32 RequiredMP = RTCharacterCalculateRequiredMP(
		Runtime,
		Character,
		SkillData->Mp[0],
		SkillData->Mp[1],
		SkillSlot->Level // TODO: This level has to be set to the item grade for the astral skill as it can be level 0
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) {
		S2C_DATA_NFY_ERROR* Error = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_ERROR);
		Error->ErrorCommand = Packet->Command;
		Error->ErrorSubCommand = SkillData->SkillGroup;
		Error->ErrorCode = 14; // Insufficient MP
		SocketSend(Socket, Connection, Error);
		return;
	}

	if (SkillData->RageValue < 0) {
		if (!RTCharacterConsumeRage(Runtime, Character, ABS(SkillData->RageValue))) goto error;
	}

	RTCharacterAddMP(Runtime, Character, -RequiredMP, false);

	RTMovementUpdateDeadReckoning(Context->Runtime, &Character->Movement);

	if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_ATTACK) {
		Timestamp CooldownInterval = RTCharacterGetCooldownInterval(Runtime, Character, SkillData->CooldownIndex);
		if (CooldownInterval > Context->Config.WorldSvr.CooldownErrorTolerance) goto error;

		Bool IsNationWar = false; // TODO: Set correct value for nation war!
		RTCharacterSetCooldown(Runtime, Character, SkillData->CooldownIndex, 0, IsNationWar);

		Int32 PacketLength = sizeof(C2S_DATA_SKILL_TO_CHARACTER) + sizeof(C2S_DATA_SKILL_GROUP_ATTACK);
		if (Packet->Length < PacketLength) goto error;

		C2S_DATA_SKILL_GROUP_ATTACK* PacketData = (C2S_DATA_SKILL_GROUP_ATTACK*)&Packet->Data[0];
		PacketLength += sizeof(C2S_DATA_SKILL_TO_CHARACTER_TARGET) * PacketData->TargetCount;
		if (Packet->Length != PacketLength) goto error;

		S2C_DATA_SKILL_GROUP_ATTACK* ResponseData = PacketBufferAppendStruct(ResponsePacketBuffer, S2C_DATA_SKILL_GROUP_ATTACK);
		
		Int32 TargetCount = 0;
		Int32 ReceivedSkillExp = 0;

		RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

		for (Int Index = 0; Index < PacketData->TargetCount; Index += 1) {
			C2S_DATA_SKILL_TO_CHARACTER_TARGET* Target = &PacketData->Data[Index];
			S2C_DATA_SKILL_TO_CHARACTER_TARGET* TargetResponse = PacketBufferAppendStruct(ResponsePacketBuffer, S2C_DATA_SKILL_TO_CHARACTER_TARGET);
			
			RTCharacterRef TargetCharacter = RTWorldManagerGetCharacter(Runtime->WorldManager, Target->Entity);
			ClientContextRef TargetClient = ServerGetClientByIndex(Context, TargetCharacter->CharacterIndex, NULL);

			if (!TargetCharacter) continue;
			if (!TargetClient) continue;
			if (!RTCharacterIsAlive(Runtime, TargetCharacter)) continue;

			TargetResponse->CharacterIndex = TargetCharacter->CharacterIndex;

			RTBattleResult Result = RTCalculateSkillAttackResult(
				Runtime,
				World,
				SkillSlot->Level,
				SkillData,
				true,
				&Character->Attributes,
				&TargetCharacter->Attributes,
				&Character->Movement,
				&TargetCharacter->Movement
			);

			RTCharacterApplyDamage(Runtime, TargetCharacter, TargetCharacter->ID, Result.AppliedDamage);

			if (Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL || Result.AttackType == RUNTIME_ATTACK_TYPE_CRITICAL) {
				RTCharacterAddRage(Runtime, Character, MAX(0, SkillData->RageValue));
			}
			
			if (Character->Data.BattleModeInfo.Info.BattleModeIndex < 1 && Character->Data.BattleModeInfo.Info.AuraModeIndex < 1) {
				Int32 SpReward = RTCalculateSPReward((Int32)Result.SkillExp, 0, RUNTIME_COMBO_TIMING_INVALID);
				RTCharacterAddSP(Runtime, Character, SpReward);
			}

			TargetResponse->AttackType = Result.AttackType;
			TargetResponse->AppliedDamage = (UInt32)Result.AppliedDamage;
			TargetResponse->TotalDamage = (UInt32)Result.TotalDamage;
			TargetResponse->AdditionalDamage = (UInt32)Result.AdditionalDamage; // TODO: Check why additional damage is being displayed wrong by client
			TargetResponse->HP = TargetCharacter->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT]; //- (UInt32)Result.AppliedDamage;
			TargetResponse->Unknown3 = (UInt32)Result.IsDead;

			if (TargetClient) {
				S2C_DATA_NFY_SKILLTOMTON* NotificationTomTon = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_SKILLTOMTON);
				NotificationTomTon->SkillIndex = Response->SkillIndex;
				NotificationTomTon->Damage = (UInt32)Result.AppliedDamage;
				NotificationTomTon->Unknown1 = (UInt32)Result.IsDead;
				SocketSend(Context->ClientSocket, TargetClient->Connection, NotificationTomTon);
			}

			TargetCount += 1;
		}


		ResponseData->CharacterHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		ResponseData->CharacterMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
		ResponseData->CharacterSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];

		ResponseData->CharacterMaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
		ResponseData->AccumulatedExp = Character->Data.Info.Exp;
		ResponseData->AccumulatedOxp = Character->Data.OverlordMasteryInfo.Info.Exp;
		ResponseData->ReceivedSkillExp = ReceivedSkillExp;
		ResponseData->TargetCount = TargetCount;

		S2C_DATA_NFY_SKILL_TO_CHARACTER* Notification = PacketBufferInit(NotificationPacketBuffer, S2C, NFY_SKILL_TO_CHARACTER);
		Notification->SkillIndex = Response->SkillIndex;

		S2C_DATA_NFY_SKILL_GROUP_ATTACK* NotificationData = PacketBufferAppendStruct(NotificationPacketBuffer, S2C_DATA_NFY_SKILL_GROUP_ATTACK);
		
		NotificationData->TargetCount = ResponseData->TargetCount;
		NotificationData->CharacterIndex = (UInt32)Character->CharacterIndex;
		NotificationData->PositionSet.X = PacketData->PositionSet.X;
		NotificationData->PositionSet.Y = PacketData->PositionSet.Y;
		NotificationData->CharacterHP = ResponseData->CharacterHP;
		NotificationData->Shield = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE];

		for (Int Index = 0; Index < TargetCount; Index++) {
			S2C_DATA_SKILL_TO_CHARACTER_TARGET* TargetResponse = &ResponseData->Data[Index];
			S2C_DATA_NFY_SKILL_TO_CHARACTER_TARGET* TargetNotification = 
				PacketBufferAppendStruct(NotificationPacketBuffer, S2C_DATA_NFY_SKILL_TO_CHARACTER_TARGET);

			TargetNotification->CharacterIndex = (UInt32)TargetResponse->CharacterIndex;
			TargetNotification->AttackType = TargetResponse->AttackType;
			TargetNotification->HP = TargetResponse->HP;
			TargetNotification->Unknown3 = TargetResponse->Unknown3;
		}

		SocketSend(Socket, Connection, Response);

		BroadcastToWorld(
			Context,
			World,
			kEntityIDNull,
			Character->Data.Info.PositionX,
			Character->Data.Info.PositionY,
			Notification
		);
		return;

	} else if (SkillData->SkillGroup == RUNTIME_SKILL_GROUP_MOVEMENT) {
		Timestamp CooldownInterval = RTCharacterGetCooldownInterval(Runtime, Character, SkillData->CooldownIndex);
		if (CooldownInterval > Context->Config.WorldSvr.CooldownErrorTolerance) goto error;

		Bool IsNationWar = false; // TODO: Set correct value for nation war!
		RTCharacterSetCooldown(Runtime, Character, SkillData->CooldownIndex, 0, IsNationWar);

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
				Timestamp CooldownInterval = RTCharacterGetCooldownInterval(Runtime, Character, SkillData->CooldownIndex);
				if (CooldownInterval > Context->Config.WorldSvr.CooldownErrorTolerance) goto error;

				RTCharacterStartAuraMode(Runtime, Character, SkillData->SkillID);
			}
			else {
				RTCharacterCancelAuraMode(Runtime, Character);
			}
		}

		Bool IsNationWar = false; // TODO: Set correct value for nation war!
		RTCharacterSetCooldown(Runtime, Character, SkillData->CooldownIndex, 0, IsNationWar);
		
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
			Timestamp CooldownInterval = RTCharacterGetCooldownInterval(Runtime, Character, SkillData->CooldownIndex);
			if (CooldownInterval > Context->Config.WorldSvr.CooldownErrorTolerance) goto error;

			RTCharacterStartBattleMode(Runtime, Character, SkillData->SkillID);
		}
		else {
			RTCharacterCancelBattleMode(Runtime, Character);
		}

		Bool IsNationWar = false; // TODO: Set correct value for nation war!
		RTCharacterSetCooldown(Runtime, Character, SkillData->CooldownIndex, 0, IsNationWar);

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