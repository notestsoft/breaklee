#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(SKILL_TO_MOB) {
	if (!Character) goto error;

	// TODO: Move logic to Runtime!

	if (!RTCharacterIsAlive(Runtime, Character)) goto error;

	RTMovementEndDeadReckoning(Runtime, &Character->Movement);

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

	if (Packet->TargetCount < 1) goto error;

	Int32 RequiredMP = RTCharacterCalculateRequiredMP(
		Runtime,
		Character,
		SkillData->Mp[0],
		SkillData->Mp[1],
		SkillSlot->Level
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) {
		S2C_DATA_NFY_ERROR* Error = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_ERROR);
		Error->ErrorCommand = Packet->Command;
		Error->ErrorSubCommand = SkillData->SkillGroup;
		Error->ErrorCode = 14; // Insufficient MP
		SocketSend(Socket, Connection, Error);
		return;
	}

    RTCharacterAddMP(Runtime, Character, -RequiredMP, false);
    
	if (SkillData->RageValue < 0) {
		if (!RTCharacterConsumeRage(Runtime, Character, ABS(SkillData->RageValue))) goto error;
	}

	// TODO: Apply dash by skill if needed

	RTMovementUpdateDeadReckoning(Context->Runtime, &Character->Movement);

	Int32 CharacterPositionError = RTCalculateDistance(
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Packet->PositionCharacter.X,
		Packet->PositionCharacter.Y
	);
	/*
	if (CharacterPositionError > 1) 
		goto error;
	*/

	PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_SKILL_TO_MOB* Response = PacketBufferInit(PacketBuffer, S2C, SKILL_TO_MOB);

	Int32 TargetCount = 0;
	Int32 ReceivedSkillExp = 0;

	RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	// TODO: Add evaluation for each target
	for (Int Index = 0; Index < Packet->TargetCount; Index += 1) {
		C2S_DATA_SKILL_TO_MOB_TARGET* Target = &Packet->Data[Index];
		S2C_DATA_SKILL_TO_MOB_TARGET* TargetResponse = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_SKILL_TO_MOB_TARGET);
		TargetResponse->Entity = Target->Entity;
		TargetResponse->EntityIDType = Target->EntityIDType;

		RTMobRef Mob = RTWorldContextGetMob(World, Target->Entity);
		if (!Mob || Mob->IsDead || Index >= SkillData->MaxTarget) {
			TargetResponse->AttackType = RUNTIME_ATTACK_TYPE_MISS;
			continue;
		}
		/*
		Bool IsValidRange = RTCheckSkillTargetDistance(
			Skill,
			Mob->Data->Radius,
			Mob->Data->Scale,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.X,
			Mob->Movement.PositionCurrent.X,
			Mob->Movement.PositionCurrent.Y
		);
		if (!IsValidRange) {
			TargetResponse->AttackType = RUNTIME_ATTACK_TYPE_MISS;
			continue;
		}
		*/

		RTBattleResult Result = RTCalculateSkillAttackResult(
			Runtime,
			World,
			SkillSlot->Level,
			SkillData,
			true,
			&Character->Attributes,
			&Mob->Attributes,
			&Character->Movement,
			&Mob->Movement
		);

		if (Index == 0) {
			if (Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL || Result.AttackType == RUNTIME_ATTACK_TYPE_CRITICAL) {
				RTCharacterAddRage(Runtime, Character, MAX(0, SkillData->RageValue));
			}
		}

		RTMobApplyDamage(Runtime, World, Mob, Character->ID, Result.AppliedDamage, Result.Delay);

		if (Mob->Pattern && (Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL || Result.AttackType == RUNTIME_ATTACK_TYPE_CRITICAL)) {
			RTMobPatternSkillReceived(Runtime, World, Mob, Mob->Pattern, SkillData->SkillID);
		}

		RTCharacterAddExp(Runtime, Character, Result.Exp);

		ReceivedSkillExp += RTCharacterAddSkillExp(Runtime, Character, Result.SkillExp);

		if (Character->Data.BattleModeInfo.Info.BattleModeIndex < 1 && Character->Data.BattleModeInfo.Info.AuraModeIndex < 1) {
			Int32 SpReward = RTCalculateSPReward((Int32)Result.SkillExp, 0, RUNTIME_COMBO_TIMING_INVALID);
			RTCharacterAddSP(Runtime, Character, SpReward);
		}

		TargetResponse->AttackType = Result.AttackType;
		TargetResponse->MobAppliedDamage = (UInt32)Result.AppliedDamage;
		TargetResponse->MobTotalDamage = (UInt32)Result.TotalDamage;
		TargetResponse->MobAdditionalDamage = (UInt32)Result.AdditionalDamage; // TODO: Check why additional damage is being displayed wrong by client
		TargetResponse->MobHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		TargetResponse->Unknown3 = 1;

		// TODO: This should be calculated globally inside the mob logic when it dies by a bfx effect and no active attack!
		if (Result.IsDead) {
			if (RTCharacterIncrementQuestMobCounter(Runtime, Character, Mob->Spawn.MobSpeciesIndex)) {
				S2C_DATA_NFY_QUEST_MOB_KILL* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, NFY_QUEST_MOB_KILL);
				Notification->MobSpeciesIndex = Mob->Spawn.MobSpeciesIndex;
				Notification->SkillIndex = 0;
				SocketSend(Socket, Connection, Notification);
			}
		}

		TargetCount += 1;
	}

	Response->SkillIndex = Packet->SkillIndex;
	Response->CharacterHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Response->CharacterMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
	Response->CharacterSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
	Response->Unknown4 = -1;
	Response->CharacterMaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
	Response->AccumulatedExp = Character->Data.Info.Exp;
	Response->AccumulatedOxp = Character->Data.OverlordMasteryInfo.Info.Exp;
	Response->ReceivedSkillExp = ReceivedSkillExp;
	Response->AXP = Character->Data.AbilityInfo.Info.Axp;
	Response->AP = Character->Data.AbilityInfo.Info.AP;
	Response->TargetCount = TargetCount;
	SocketSend(Socket, Connection, Response);

	PacketBuffer = SocketGetNextPacketBuffer(Socket);
	S2C_DATA_NFY_SKILL_TO_MOB* Notification = PacketBufferInit(PacketBuffer, S2C, NFY_SKILL_TO_MOB);
	Notification->SkillIndex = Response->SkillIndex;
	Notification->TargetCount = Response->TargetCount;
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->PositionSet.X = Character->Data.Info.PositionX;
	Notification->PositionSet.Y = Character->Data.Info.PositionY;
	Notification->CharacterHP = Response->CharacterHP;
	Notification->Shield = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE];

	for (Int Index = 0; Index < Response->TargetCount; Index++) {
		S2C_DATA_SKILL_TO_MOB_TARGET* TargetResponse = &Response->Data[Index];
		S2C_DATA_NFY_SKILL_TO_MOB_TARGET* TargetNotification = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_NFY_SKILL_TO_MOB_TARGET);
		TargetNotification->Entity = TargetResponse->Entity;
		TargetNotification->EntityIDType = TargetResponse->EntityIDType;
		TargetNotification->AttackType = TargetResponse->AttackType;
		TargetNotification->MobHP = TargetResponse->MobHP;
		TargetNotification->Unknown3 = 1;
	}

	BroadcastToWorld(
		Context,
		World,
		kEntityIDNull,
		Character->Data.Info.PositionX,
		Character->Data.Info.PositionY,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
