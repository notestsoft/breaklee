#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(ATTACK_TO_MOB) {
	if (!Character) goto error;
	if (!RTCharacterIsAlive(Runtime, Character)) goto error;
	if (Packet->EntityIDType != RUNTIME_ENTITY_TYPE_MOB) goto error;

	// TODO: Check if mob is in attack range (auto miss target)
	// TODO: Check if attack cooldown is ok (auto miss target)

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
	RTMobRef Mob = RTWorldContextGetMob(World, Packet->Entity);
	if (!Mob) goto error;
	if (Mob->IsDead) goto error; // TODO: (auto miss target)

	RTBattleResult Result = RTCalculateNormalAttackResult(
		Runtime,
		World,
		RUNTIME_SKILL_DAMAGE_TYPE_SWORD,
		true,
		&Character->Attributes,
		&Mob->Attributes,
		&Character->Movement,
		&Mob->Movement
	);

	RTMobApplyDamage(Runtime, World, Mob, Character->ID, Result.AppliedDamage, Result.Delay);
	RTCharacterAddExp(Runtime, Character, Result.Exp);

	S2C_DATA_ATTACK_TO_MOB* Response = PacketBufferInit(SocketGetNextPacketBuffer(Socket), S2C, ATTACK_TO_MOB);
	Response->Entity = Packet->Entity;
	Response->EntityIDType = Packet->EntityIDType;
	Response->AttackType = Result.AttackType;
	Response->MobAppliedDamage = (UInt32)Result.AppliedDamage;
	Response->MobTotalDamage = (UInt32)Result.TotalDamage;
	Response->MobAdditionalDamage = (UInt32)Result.AdditionalDamage;
	Response->AccumulatedExp = Character->Data.Info.Exp;
	Response->AccumulatedOxp = Character->Data.OverlordMasteryInfo.Info.Exp;
	Response->CharacterHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Response->CharacterMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
	Response->CharacterSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
	Response->MobHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Response->Unknown1 = 1;
	Response->Unknown2 = 1;
	Response->Unknown3 = 1;
	Response->Unknown4 = 1;
	SocketSend(Socket, Connection, Response);

	// TODO: This should be calculated globally inside the mob logic when it dies by a bfx effect and no active attack!
	if (Result.IsDead) {
		if (RTCharacterIncrementQuestMobCounter(Runtime, Character, Mob->Spawn.MobSpeciesIndex)) {
			S2C_DATA_NFY_QUEST_MOB_KILL* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_QUEST_MOB_KILL);
			Notification->MobSpeciesIndex = Mob->Spawn.MobSpeciesIndex;
			Notification->SkillIndex = 0;
			SocketSend(Socket, Connection, Notification);
		}
	}

	S2C_DATA_NFY_ATTACK_TO_MOB* Notification = PacketBufferInit(SocketGetNextPacketBuffer(Context->ClientSocket), S2C, NFY_ATTACK_TO_MOB);
	Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
	Notification->Mob = Response->Entity;
	Notification->MobIDType = Response->EntityIDType;
	Notification->AttackType = Response->AttackType;
	Notification->MobHP = Response->MobHP;
	Notification->CharacterHP = Response->CharacterHP;

	BroadcastToWorld(
		Context,
		World,
		kEntityIDNull,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
		Notification
	);
	return;

error:
	SocketDisconnect(Socket, Connection);
}
