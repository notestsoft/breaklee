#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
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

	// TODO: Packet could eventually contains SkillIndex but no slotindex
	RTSkillSlotRef SkillSlot = RTCharacterGetSkillSlotByIndex(Runtime, Character, Packet->SlotIndex);
	if (!SkillSlot) goto error;

	RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillSlot->ID);
	assert(SkillData);

	Bool UseMoveStep = false;
	Bool UseStun = false;
	Bool UseHitCount = false;
	Bool UseDebuff = false;

	// TODO: Add skill cast time and cooldown checks

	// TODO: Calculate timing of combo skill

	if (Packet->TargetCount < 1 || (Packet->TargetCount > 1 && !SkillData->Multi)) goto error;

	Int32 RequiredMP = RTCharacterCalculateRequiredMP(
		Runtime,
		Character,
		SkillData->Mp[0],
		SkillData->Mp[1],
		SkillSlot->Level
	);

	if (Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT] < RequiredMP) {
		// TODO: Send error notification
		// NotifyLastErrCode(pUserCtx, CSC_SKILLTOMOBS, pUserSkillData->_sgGroup, EC_MP_INSUFFICIENCY);
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

	S2C_DATA_SKILL_TO_MOB* Response = PacketBufferInit(Connection->PacketBuffer, S2C, SKILL_TO_MOB);
	Response->SkillIndex = Packet->SkillIndex;
	Response->CharacterHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Response->CharacterMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
	Response->CharacterSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
	Response->Unknown4 = -1;
	Response->CharacterMaxHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];

	Int32 TargetCount = 0;
	Int32 ReceivedSkillExp = 0;

	RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

	// TODO: Add evaluation for each target
	for (Int32 Index = 0; Index < Packet->TargetCount; Index += 1) {
		C2S_DATA_SKILL_TO_MOB_TARGET* Target = &Packet->Data[Index];
		S2C_DATA_SKILL_TO_MOB_TARGET* TargetResponse = PacketBufferAppendStruct(Connection->PacketBuffer, S2C_DATA_SKILL_TO_MOB_TARGET);
		TargetResponse->Entity = Target->Entity;
		TargetResponse->EntityIDType = Target->EntityIDType;

		RTMobRef Mob = RTWorldContextGetMob(World, Target->Entity);
		if (!Mob || Mob->IsDead) {
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
		struct _RTBattleResult Result = { 0 };
		RTCalculateSkillAttackResult(
			Runtime,
			SkillSlot->Level,
			SkillData,
			Character->Info.Basic.Level,
			&Character->Attributes,
			Mob->SpeciesData->Level,
			&Mob->Attributes,
			&Result
		);

		if (Index == 0) {
			if (Result.AttackType == RUNTIME_ATTACK_TYPE_NORMAL || Result.AttackType == RUNTIME_ATTACK_TYPE_CRITICAL) {
				RTCharacterAddRage(Runtime, Character, MAX(0, SkillData->RageValue));
			}
		}

		RTMobApplyDamage(Runtime, World, Mob, Character->ID, Result.AppliedDamage);
		RTCharacterAddExp(Runtime, Character, Result.Exp);

		ReceivedSkillExp += RTCharacterAddSkillExp(Runtime, Character, Result.SkillExp);

		TargetResponse->AttackType = Result.AttackType;
		TargetResponse->MobAppliedDamage = Result.AppliedDamage;
		TargetResponse->MobTotalDamage = Result.TotalDamage;
		TargetResponse->MobAdditionalDamage = Result.AdditionalDamage; // TODO: Check why additional damage is being displayed wrong by client
		TargetResponse->MobHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		TargetResponse->Unknown3 = 1;

		// TODO: This should be calculated globally inside the mob logic when it dies by a bfx effect and no active attack!
		if (Result.IsDead) {
			if (RTCharacterIncrementQuestMobCounter(Runtime, Character, Mob->Spawn.MobSpeciesIndex)) {
				S2C_DATA_NFY_QUEST_MOB_KILL* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_QUEST_MOB_KILL);
				Notification->MobSpeciesIndex = Mob->Spawn.MobSpeciesIndex;
				Notification->SkillIndex = 0;
				Notification->Unknown1 = 0;
				SocketSend(Socket, Connection, Notification);
			}

			Mob->DropOwner = Character->ID;
		}

		TargetCount += 1;
	}

	Response->AccumulatedExp = Character->Info.Basic.Exp;
	Response->AccumulatedOxp = Character->Info.Overlord.Exp;
	Response->ReceivedSkillExp = ReceivedSkillExp;
	Response->AXP = Character->Info.Ability.Exp;
	Response->AP = Character->Info.Ability.Point;
	Response->TargetCount = TargetCount;
	SocketSend(Socket, Connection, Response);

	if (ReceivedSkillExp > 0) {
		RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

		RTRuntimeBroadcastEvent(
			Runtime,
			RUNTIME_EVENT_CHARACTER_UPDATE_SKILL_STATUS,
			World,
			kEntityIDNull,
			Character->ID,
			Character->Movement.PositionCurrent.X,
			Character->Movement.PositionCurrent.Y
		);
	}

	S2C_DATA_NFY_SKILL_TO_MOB* Notification = PacketBufferInit(Context->ClientSocket->PacketBuffer, S2C, NFY_SKILL_TO_MOB);
	Notification->SkillIndex = Response->SkillIndex;
	Notification->TargetCount = Response->TargetCount;
	Notification->CharacterIndex = (UInt32)Client->CharacterIndex;
	Notification->PositionSet.X = Character->Info.Position.X;
	Notification->PositionSet.Y = Character->Info.Position.Y;
	Notification->CharacterHP = Response->CharacterHP;

	for (Int32 Index = 0; Index < Response->TargetCount; Index++) {
		S2C_DATA_SKILL_TO_MOB_TARGET* TargetResponse = &Response->Data[Index];
		S2C_DATA_NFY_SKILL_TO_MOB_TARGET* TargetNotification = PacketBufferAppendStruct(Context->ClientSocket->PacketBuffer, S2C_DATA_NFY_SKILL_TO_MOB_TARGET);
		TargetNotification->Entity = TargetResponse->Entity;
		TargetNotification->EntityIDType = TargetResponse->EntityIDType;
		TargetNotification->AttackType = TargetResponse->AttackType;
		TargetNotification->MobHP = TargetResponse->MobHP;
		TargetNotification->Unknown3 = 1;
	}

	return BroadcastToWorld(
		Context,
		World,
		kEntityIDNull,
		Character->Info.Position.X,
		Character->Info.Position.Y,
		Notification
	);
	
error:
	return SocketDisconnect(Socket, Connection);
}
