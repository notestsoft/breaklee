#include "Mob.h"
#include "MobPattern.h"
#include "Movement.h"
#include "Script.h"
#include "Runtime.h"
#include "World.h"
#include "WorldManager.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Void RTMobInit(
	RTRuntimeRef Runtime,
	RTMobRef Mob
) {
	assert(Mob && Mob->SpeciesData);

	Mob->Attributes.Seed = (Int32)PlatformGetTickCount();
	memset(Mob->Attributes.Values, 0, sizeof(Mob->Attributes.Values));
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_EXP] = Mob->SpeciesData->Exp;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_LEVEL] = (Mob->Spawn.Level > 1) ? Mob->Spawn.Level : Mob->SpeciesData->Level;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = Mob->SpeciesData->MoveSpeed;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] = Mob->SpeciesData->HP;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = Mob->SpeciesData->HP;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] = Mob->SpeciesData->AttackRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE] = Mob->SpeciesData->Defense;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE] = Mob->SpeciesData->DefenseRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION] = Mob->SpeciesData->DamageReduction;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY] = Mob->SpeciesData->Accuracy;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION] = Mob->SpeciesData->Penetration;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_RATE_RESIST] = Mob->SpeciesData->ResistCriticalRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ACCURACY_RESIST] = Mob->SpeciesData->IgnoreAccuracy;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REDUCTION_RESIST] = Mob->SpeciesData->IgnoreDamageReduction;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_PENETRATION_RESIST] = Mob->SpeciesData->IgnorePenetration;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ABSOLUTE_DAMAGE] = Mob->SpeciesData->AbsoluteDamage;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ALL_SKILL_AMP_RESIST] = Mob->SpeciesData->ResistSkillAmp;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_CRITICAL_DAMAGE_RESIST] = Mob->SpeciesData->ResistCriticalDamage;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_SUPPRESSION_RESIST] = Mob->SpeciesData->ResistSuppression;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_SILENCE_RESIST] = Mob->SpeciesData->ResistSilence;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_REGEN] = Mob->SpeciesData->HPRecharge;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_REFLECT] = Mob->SpeciesData->ReflectDamage;
	Mob->NextRegenTimestamp = GetTimestampMs() + RUNTIME_REGENERATION_INTERVAL;
	Mob->HPTriggerThreshold = (Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] * 300) / 1000;
	Mob->SpecialAttack = NULL;
	Mob->SpecialAttackSkill = NULL;
	Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
	memset(&Mob->Aggro, 0, sizeof(struct _RTMobAggroData));

	Mob->AlertRange = Mob->SpeciesData->AlertRange;
	Mob->ChaseRange = Mob->SpeciesData->ChaseRange;
	Mob->LimitRangeB = Mob->SpeciesData->LimitRangeB;

	if (Mob->EnemyCount < 1) {
		Mob->EnemyCount = 1;
		Mob->Enemies[0] = -1;
	}
	else {
		Mob->AlertRange = INT32_MAX;
		Mob->ChaseRange = INT32_MAX;
		Mob->LimitRangeB = INT32_MAX;
	}

	Mob->IsAttackingCharacter = Mob->EnemyCount < 1;
	for (Int Index = 0; Index < Mob->EnemyCount; Index += 1) {
		if (Mob->Enemies[Index] < 0) {
			Mob->IsAttackingCharacter = true;
			break;
		}
	}

	RTMobPatrolInit(Runtime, Mob);
}

Bool RTMobCanMove(RTMobRef Mob) {
	return Mob->SpeciesData->CanMove;
}

Bool RTMobCanAttack(RTMobRef Mob) {
	return Mob->SpeciesData->CanAttack && !Mob->Spawn.IsMissionGate;
}

Bool RTMobCanRespawn(RTMobRef Mob) {
	if (!Mob->Spawn.SpawnDefault) {
		return false;
	}

	if (Mob->IsPermanentDeath) {
		return false;
	}

	return Mob->IsInfiniteSpawn || Mob->RemainingSpawnCount > 0;
}

Bool RTMobIsAlive(RTMobRef Mob) {
	if (!Mob->IsSpawned) {
		return false;
	}

    if (Mob->IsDead) {
        return false;
    }

	return Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] > 0;
}

Bool RTMobIsImmune(
	RTMobRef Mob
) {
	return Mob->Attributes.Values[RUNTIME_ATTRIBUTE_EVASION_COMPLETE] > 0;
}

Bool RTMobIsUnmovable(RTMobRef Mob) {
	return false;
}

Bool RTMobIsAggressive(RTMobRef Mob) {
	return !(Mob->SpeciesData->AggressiveType == RUNTIME_MOB_AGGRESSIVE_TYPE_PASSIVE);
}

Void RTMobUpdateActiveSkill(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob,
	Int32 TargetDistance
) {
	Mob->SpecialAttack = NULL;
	Mob->SpecialAttackSkill = NULL;

	if (Mob->SpeciesData->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_NONE) {
		Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
	}

	if (Mob->SpeciesData->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_HP_TRIGGER) {
		if (Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] < Mob->HPTriggerThreshold) {
			Mob->ActiveSkill = &Mob->SpeciesData->SpecialSkill;
		}
	}

	if (Mob->SpeciesData->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_SITUATIONAL) {
		assert(false && "Implementation missing!");
	}

	if (Mob->SpeciesData->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_BY_REACH) {
		if (Mob->SpeciesData->SpecialSkill.Reach >= TargetDistance) {
			Mob->ActiveSkill = &Mob->SpeciesData->SpecialSkill;
		}
		else if (Mob->SpeciesData->DefaultSkill.Reach >= TargetDistance) {
			Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
		}
	}

	if (Mob->SpeciesData->AttackPattern == RUNTIME_MOB_ATTACK_PATTERN_TYPE_SELECTIVE) {
		RTDataMobAttackIndexRef AttackIndex = RTRuntimeDataMobAttackIndexGet(Runtime->Context, Mob->SpeciesData->MobSpeciesIndex);
		if (AttackIndex && AttackIndex->MobAttackLinkCount > 0) {
			Int32 AttackLinkIndex = RandomRange(&World->Seed, 0, AttackIndex->MobAttackLinkCount - 1);
			RTDataMobAttackLinkRef AttackLink = &AttackIndex->MobAttackLinkList[AttackLinkIndex];
            RTDataMobAttackRef Attack = RTRuntimeDataMobAttackGet(Runtime->Context, AttackLink->MobAttackID);
			Mob->SpecialAttack = &Attack->MobAttackDataList[0];
			assert(Mob->SpecialAttack);

			if (Mob->SpecialAttack->SkillID > 0) {
				Mob->SpecialAttackSkill = RTRuntimeDataMobSkillGet(Runtime->Context, Mob->SpecialAttack->SkillID);
				assert(Mob->SpecialAttackSkill);
			}
		}
		else {
			Int32 Random = RandomRange(&World->Seed, 0, INT32_MAX);
			if (Random > INT32_MAX / 2) {
				Mob->ActiveSkill = &Mob->SpeciesData->SpecialSkill;
			}
			else {
				Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
			}
		}
	}
}

RTEntityID RTMobGetMaxAggroTarget(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob
) {
	Int32 MaxAggroIndex = -1;
	Int Index = 0;

	while (Index < Mob->Aggro.Count) {
		RTEntityID EnemyID = Mob->Aggro.Entities[Index];
		if (EnemyID.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
			RTCharacterRef Enemy = RTWorldManagerGetCharacter(WorldContext->WorldManager, EnemyID);
			if (!Enemy) {
				Int32 TailLength = Mob->Aggro.Count - Index - 1;
				if (TailLength > 0) {
					memmove(
						&Mob->Aggro.Entities[Index],
						&Mob->Aggro.Entities[Index + 1],
						TailLength * sizeof(RTEntityID)
					);
					memmove(
						&Mob->Aggro.ReceivedDamage[Index],
						&Mob->Aggro.ReceivedDamage[Index + 1],
						TailLength * sizeof(Int64)
					);
				}

				Mob->Aggro.Count -= 1;
				continue;
			}

			Int32 Distance = RTMovementDistance(&Enemy->Movement, &Mob->Movement);
			if (!Enemy ||
				Enemy->Data.Info.WorldIndex != WorldContext->WorldData->WorldIndex ||
				!RTCharacterIsAlive(Runtime, Enemy) ||
				Distance > Mob->AlertRange) {
				Int32 TailLength = Mob->Aggro.Count - Index - 1;
				if (TailLength > 0) {
					memmove(
						&Mob->Aggro.Entities[Index],
						&Mob->Aggro.Entities[Index + 1],
						TailLength * sizeof(RTEntityID)
					);
					memmove(
						&Mob->Aggro.ReceivedDamage[Index],
						&Mob->Aggro.ReceivedDamage[Index + 1],
						TailLength * sizeof(Int64)
					);
				}

				Mob->Aggro.Count -= 1;
				continue;
			}
		}

		if (EnemyID.EntityType == RUNTIME_ENTITY_TYPE_MOB) {
			RTMobRef Enemy = RTWorldContextGetMob(WorldContext, EnemyID);
			if (!Enemy) {
				Int32 TailLength = Mob->Aggro.Count - Index - 1;
				if (TailLength > 0) {
					memmove(
						&Mob->Aggro.Entities[Index],
						&Mob->Aggro.Entities[Index + 1],
						TailLength * sizeof(RTEntityID)
					);
					memmove(
						&Mob->Aggro.ReceivedDamage[Index],
						&Mob->Aggro.ReceivedDamage[Index + 1],
						TailLength * sizeof(Int64)
					);
				}

				Mob->Aggro.Count -= 1;
				continue;
			}

			Int32 Distance = RTMovementDistance(&Enemy->Movement, &Mob->Movement);
			if (!Enemy ||
				!RTMobIsAlive(Enemy) ||
				Distance > Mob->AlertRange) {
				Int32 TailLength = Mob->Aggro.Count - Index - 1;
				if (TailLength > 0) {
					memmove(
						&Mob->Aggro.Entities[Index],
						&Mob->Aggro.Entities[Index + 1],
						TailLength * sizeof(RTEntityID)
					);
					memmove(
						&Mob->Aggro.ReceivedDamage[Index],
						&Mob->Aggro.ReceivedDamage[Index + 1],
						TailLength * sizeof(Int64)
					);
				}

				Mob->Aggro.Count -= 1;
				continue;
			}
		}

		if (MaxAggroIndex < 0) {
			MaxAggroIndex = Index;
		}
		else if (Mob->Aggro.ReceivedDamage[MaxAggroIndex] < Mob->Aggro.ReceivedDamage[Index]) {
			MaxAggroIndex = Index;
		}

		Index += 1;
	}

	if (MaxAggroIndex < 0) {
		return kEntityIDNull;
	}

	return Mob->Aggro.Entities[MaxAggroIndex];
}

Void RTMobCancelMovement(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob
) {
	if (Mob->Movement.IsMoving) {
		Mob->Movement.PositionEnd.X = Mob->Movement.PositionCurrent.X;
		Mob->Movement.PositionEnd.Y = Mob->Movement.PositionCurrent.Y;
		RTMovementEndDeadReckoning(Runtime, &Mob->Movement);

		if (Mob->IsChasing) {
			NOTIFICATION_DATA_MOB_CHASE_END* Notification = RTNotificationInit(MOB_CHASE_END);
			Notification->Entity = Mob->ID;
			Notification->PositionCurrentX = Mob->Movement.PositionCurrent.X;
			Notification->PositionCurrentY = Mob->Movement.PositionCurrent.Y;
			RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
		}
		else {
			NOTIFICATION_DATA_MOB_MOVE_END* Notification = RTNotificationInit(MOB_MOVE_END);
			Notification->Entity = Mob->ID;
			Notification->PositionCurrentX = Mob->Movement.PositionCurrent.X;
			Notification->PositionCurrentY = Mob->Movement.PositionCurrent.Y;
			RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
		}
	}
}

Void RTMobApplyDamage(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTEntityID Source,
	Int64 Damage,
	Timestamp Delay
) {
	Int64 PreviousHp = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Int64 TotalDamage = MIN(Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT], Damage);
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] -= Damage;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MAX(0, Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT]);
	
	Bool Found = false;
	for (Int Index = 0; Index < Mob->Aggro.Count; Index += 1) {
		if (RTEntityIsEqual(Mob->Aggro.Entities[Index], Source)) {
			Mob->Aggro.ReceivedDamage[Index] += Damage;
			Found = true;
			break;
		}
	}

	if (!Found && Mob->Aggro.Count < RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT) {
		Mob->Aggro.Entities[Mob->Aggro.Count] = Source;
		Mob->Aggro.ReceivedDamage[Mob->Aggro.Count] = Damage;
		Mob->Aggro.Count += 1;
	}

	RTMobCancelMovement(Runtime, WorldContext, Mob);

	if (Mob->Script && TotalDamage > 0) RTScriptCall(
		Mob->Script,
		MOB_EVENT_DAMAGE,
		LUA_TLIGHTUSERDATA, Runtime,
		LUA_TLIGHTUSERDATA, WorldContext,
		LUA_TLIGHTUSERDATA, Mob,
		LUA_TNUMBER, TotalDamage,
		NULL
	);

	if (Mob->IsTimerMob) {
		RTDungeonUpdateTimerMobHP(WorldContext, Mob);
	}

	if (Mob->Pattern) {
		RTMobPatternHpChanged(Runtime, WorldContext, Mob, Mob->Pattern, PreviousHp, Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT]);
	}

	if (!RTMobIsAlive(Mob)) {
		Mob->DropOwner = Source;
		Mob->NextTimestamp = GetTimestampMs() + Mob->Spawn.SpawnInterval;
		RTWorldDespawnMobEvent(Runtime, WorldContext, Mob, Delay);
	}
}

Void RTMobAttackTarget(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob,
	RTCharacterRef Character
) {
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK] = RandomRange(
		&World->Seed, 
		Mob->ActiveSkill->PhysicalAttackMin, 
		Mob->ActiveSkill->PhysicalAttackMax
	);

	if (Mob->SpecialAttackSkill) {
		// assert(false && "Not Implemented!");
	}

	// TODO: Check SkillGroup for attack patterns: single, aoe, movement, heal, buff, debuff

	RTBattleResult Result = RTCalculateNormalAttackResult(
		Runtime,
		World,
		RUNTIME_SKILL_DAMAGE_TYPE_SWORD,
		true,
		&Mob->Attributes,
		&Character->Attributes,
		&Mob->Movement,
		&Character->Movement
	);

	RTCharacterAddHP(Runtime, Character, -Result.AppliedDamage, false);
	Mob->NextTimestamp = GetTimestampMs() + Mob->ActiveSkill->Interval;

	{
		NOTIFICATION_DATA_MOB_ATTACK_AOE* Notification = RTNotificationInit(MOB_ATTACK_AOE);
		Notification->Entity = Mob->ID;
		Notification->IsDefaultSkill = true;
		Notification->Unknown1 = 0;
		Notification->MobHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		Notification->Unknown2 = 0;
		Notification->TargetCount = 1;

		NOTIFICATION_DATA_MOB_ATTACK_AOE_TARGET* NotificationTarget = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOB_ATTACK_AOE_TARGET);
		NotificationTarget->CharacterIndex = (UInt32)Character->CharacterIndex;
		NotificationTarget->IsDead = Result.IsDead;
		NotificationTarget->Result = Result.AttackType;
		NotificationTarget->AppliedDamage = (UInt32)Result.AppliedDamage;
		NotificationTarget->TargetHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		NotificationTarget->TargetShield = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
		memset(NotificationTarget->Unknown1, 0, 29);
		RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
	}
}

Void RTMobAttackTargetMob(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob,
	RTMobRef Target
) {
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK] = RandomRange(
		&World->Seed,
		Mob->ActiveSkill->PhysicalAttackMin,
		Mob->ActiveSkill->PhysicalAttackMax
	);

	if (Mob->SpecialAttackSkill) {
		// assert(false && "Not Implemented!");
	}

	// TODO: Check SkillGroup for attack patterns: single, aoe, movement, heal, buff, debuff

	RTBattleResult Result = RTCalculateNormalAttackResult(
		Runtime,
		World,
		RUNTIME_SKILL_DAMAGE_TYPE_SWORD,
		true,
		&Mob->Attributes,
		&Target->Attributes,
		&Mob->Movement,
		&Target->Movement
	);

	RTMobApplyDamage(Runtime, World, Target, Mob->ID, Result.AppliedDamage, Result.Delay);
	Mob->NextTimestamp = GetTimestampMs() + Mob->ActiveSkill->Interval;

	// TODO: Check which packet has to be sent for mob attacking another mob
	{
		NOTIFICATION_DATA_MOB_ATTACK_AOE* Notification = RTNotificationInit(MOB_ATTACK_AOE);
		Notification->Entity = Mob->ID;
		Notification->IsDefaultSkill = true;
		Notification->Unknown1 = 0;
		Notification->MobHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		Notification->Unknown2 = 0;
		Notification->TargetCount = 1;

		NOTIFICATION_DATA_MOB_ATTACK_AOE_TARGET* NotificationTarget = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOB_ATTACK_AOE_TARGET);
		NotificationTarget->CharacterIndex = (UInt32)Target->ID.Serial;
		NotificationTarget->IsDead = Result.IsDead;
		NotificationTarget->Result = Result.AttackType;
		NotificationTarget->AppliedDamage = (UInt32)Result.AppliedDamage;
		NotificationTarget->TargetHP = Target->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		NotificationTarget->TargetShield = (UInt32)Target->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
		memset(NotificationTarget->Unknown1, 0, 29);
		RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
	}
}

struct _RTMobFindNearbyTargetArguments {
	RTWorldManagerRef WorldManager;
	RTWorldContextRef WorldContext;
	RTMobRef Mob;
	Int32 Distance;
	RTEntityID Target;
};

Void _RTMobFindNearbyTargetProc(
	RTEntityID Entity,
	UInt16 Distance,
	Void* Userdata
) {
	struct _RTMobFindNearbyTargetArguments* Arguments = (struct _RTMobFindNearbyTargetArguments*)Userdata;

	if (Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
		RTCharacterRef Character = RTWorldManagerGetCharacter(
			Arguments->WorldManager,
			Entity
		);

		Int32 LevelDifference = (Int32)(Character->Attributes.Values[RUNTIME_ATTRIBUTE_LEVEL] - Arguments->Mob->Attributes.Values[RUNTIME_ATTRIBUTE_LEVEL]);
		if (Arguments->WorldContext->WorldData->Type == RUNTIME_WORLD_TYPE_GLOBAL &&
			LevelDifference > RUNTIME_MOB_MAX_FIND_LEVEL_DIFFERENCE) {
			return;
		}
	}

	if (Distance < Arguments->Distance) {
		Arguments->Distance = Distance;
		Arguments->Target = Entity;
	}
}

RTEntityID RTMobFindNearbyTarget(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob
) {
	RTEntityID Target = kEntityIDNull;
	Int32 Distance = Mob->AlertRange;

	for (Int Index = 0; Index < Mob->EnemyCount; Index += 1) {
		Int32 EnemyIndex = Mob->Enemies[Index];
		if (EnemyIndex < 0) {
			struct _RTMobFindNearbyTargetArguments Arguments = { 0 };
			Arguments.WorldManager = WorldContext->WorldManager;
			Arguments.WorldContext = WorldContext;
			Arguments.Mob = Mob;
			Arguments.Distance = Distance;
			Arguments.Target = Target;

			RTWorldContextEnumerateEntitiesInRange(
				WorldContext,
				RUNTIME_ENTITY_TYPE_CHARACTER,
				Mob->Movement.PositionCurrent.X,
				Mob->Movement.PositionCurrent.Y,
				Mob->AlertRange,
				Mob->Movement.CollisionMask,
				Mob->Movement.IgnoreMask,
				&_RTMobFindNearbyTargetProc,
				&Arguments
			);

			Target = Arguments.Target;
			Distance = Arguments.Distance;
		}
		else {
			RTEntityID TargetMobID = {
				.EntityIndex = EnemyIndex,
				.WorldIndex = WorldContext->WorldData->WorldIndex,
				.EntityType = RUNTIME_ENTITY_TYPE_MOB,
			};
			RTMobRef TargetMob = RTWorldContextGetMob(WorldContext, TargetMobID);
			assert(TargetMob);

			if (RTMobIsAlive(TargetMob)) {
				Int32 TargetDistance = RTCalculateDistance(
					Mob->Movement.PositionCurrent.X,
					Mob->Movement.PositionCurrent.Y,
					TargetMob->Movement.PositionCurrent.X,
					TargetMob->Movement.PositionCurrent.Y
				);

				if (TargetDistance < Distance) {
					Target = TargetMobID;
					Distance = TargetDistance;
				}
			}
		}
	}

	return Target;
}

Void RTMobUpdate(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob
) {
	if (RTMobIsAlive(Mob) && Mob->Movement.IsMoving && Mob->Movement.WorldChunk) {
		assert(Mob->Movement.WorldContext);

		RTMovementRef Movement = &Mob->Movement;
		RTWorldChunkRef WorldChunk = Movement->WorldChunk;
		RTWorldChunkRef NewChunk = RTWorldContextGetChunk(Movement->WorldContext, Movement->PositionCurrent.X, Movement->PositionCurrent.Y);
		if (WorldChunk != NewChunk) {
			RTWorldChunkRemove(Movement->WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE);
			RTWorldChunkInsert(NewChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE);

			Movement->WorldChunk = NewChunk;
		}
	}

	Timestamp Timestamp = GetTimestampMs();

	if (RTMobIsAlive(Mob) && Mob->NextRegenTimestamp <= Timestamp && Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_REGEN] > 0) {
		Mob->NextRegenTimestamp = Timestamp + RUNTIME_REGENERATION_INTERVAL;
		RTMobHeal(Runtime, WorldContext, Mob, Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_REGEN]);
	}

	if (RTMobIsAlive(Mob)) {
		RTMobUpdateBuffs(Runtime, Mob);
	}

	if (Timestamp < Mob->NextTimestamp) {
		return;
	}

	if (Mob->EventSpawnTimestamp > 0) {
		if (Mob->EventSpawnTimestamp > Timestamp) return;

		Mob->EventSpawnTimestamp = 0;
		RTWorldSpawnMob(Runtime, WorldContext, Mob);
		return;
	}

	if (Mob->EventDespawnTimestamp > 0) {
		if (Mob->EventDespawnTimestamp > Timestamp) return;

		Mob->EventDespawnTimestamp = 0;

		if (Mob->IsSpawned) {
			Mob->NextTimestamp = Timestamp + Mob->Spawn.SpawnInterval;
			RTWorldDespawnMob(Runtime, WorldContext, Mob);
		}

		return;
	}

	if (Mob->EventRespawnTimestamp > 0) {
		if (Mob->EventRespawnTimestamp > Timestamp) return;

		Mob->EventRespawnTimestamp = 0;

		if (Mob->IsSpawned) {
			RTWorldRespawnMob(Runtime, WorldContext, Mob);
		}
	}

	RTMobOnEvent(Runtime, WorldContext, Mob, MOB_EVENT_UPDATE);

	if (!RTMobIsAlive(Mob) && Mob->IsSpawned) {
		Mob->NextTimestamp = Timestamp + Mob->Spawn.SpawnInterval;
		RTWorldDespawnMob(Runtime, WorldContext, Mob);
		return;
	}

	if (!RTMobIsAlive(Mob)) {
		if (RTMobCanRespawn(Mob)) {
			Mob->Aggro.Count = 0;
			memset(&Mob->Aggro.Entities, 0, sizeof(Mob->Aggro.Entities));

			Mob->RemainingSpawnCount = MAX(0, Mob->RemainingSpawnCount - 1);
			Mob->NextTimestamp = Timestamp;
			RTWorldSpawnMobEvent(Runtime, WorldContext, Mob, Mob->Spawn.SpawnInterval);
		}

		return;
	}

	RTMovementUpdateDeadReckoning(Runtime, &Mob->Movement);

	// TODO: Check if this has to be more percise then NextTimestamp
	if (Mob->Pattern) {
		RTMobPatternTimeElapsed(Runtime, WorldContext, Mob, Mob->Pattern);
		RTMobPatternUpdate(Runtime, WorldContext, Mob, Mob->Pattern);
	}

	if (Mob->Movement.IsMoving) {
		Bool Finished = (
			!Mob->Movement.IsDeadReckoning ||
			(Mob->Movement.PositionCurrent.X == Mob->Movement.PositionEnd.X && Mob->Movement.PositionCurrent.Y == Mob->Movement.PositionEnd.Y)
		);

		if (Finished) {
			RTMovementEndDeadReckoning(Runtime, &Mob->Movement);
            
			if (Mob->IsChasing) {
				NOTIFICATION_DATA_MOB_CHASE_END* Notification = RTNotificationInit(MOB_CHASE_END);
				Notification->Entity = Mob->ID;
				Notification->PositionCurrentX = Mob->Movement.PositionCurrent.X;
				Notification->PositionCurrentY = Mob->Movement.PositionCurrent.Y;
				RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
			}
			else {
				NOTIFICATION_DATA_MOB_MOVE_END* Notification = RTNotificationInit(MOB_MOVE_END);
				Notification->Entity = Mob->ID;
				Notification->PositionCurrentX = Mob->Movement.PositionCurrent.X;
				Notification->PositionCurrentY = Mob->Movement.PositionCurrent.Y;
				RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
			}

			Mob->RemainingFindCount = Mob->SpeciesData->FindCount;
		}

		if (Mob->IsChasing) {
			Mob->NextTimestamp = Timestamp + Mob->SpeciesData->ChaseInterval;
		}
		else {
			Mob->NextTimestamp = Timestamp + Mob->SpeciesData->MoveInterval;
		}

		return;
	}

	RTEntityID Target = RTMobGetMaxAggroTarget(Runtime, WorldContext, Mob);
	Bool IsIdle = RTEntityIsNull(Target);

	if (Mob->IsIdle != IsIdle) {
		Mob->IsIdle = IsIdle;
		
		if (Mob->Pattern) {
			RTMobPatternStateChanged(Runtime, WorldContext, Mob, Mob->Pattern, IsIdle);
		}
	}

	if (RTEntityIsNull(Target) && Mob->RemainingFindCount > 0) {
		if (RTMobIsAggressive(Mob)) {
			RTEntityID NewTarget = RTMobFindNearbyTarget(Runtime, WorldContext, Mob);
			if (!RTEntityIsNull(NewTarget)) {
				assert(Mob->Aggro.Count < RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT);

				Mob->Aggro.Entities[Mob->Aggro.Count] = NewTarget;
				Mob->Aggro.ReceivedDamage[Mob->Aggro.Count] = 0;
				Mob->Aggro.Count += 1;
				Mob->RemainingFindCount = Mob->SpeciesData->FindCount;
				return;
			}
		}

		Mob->RemainingFindCount = MAX(0, Mob->RemainingFindCount - 1);
		Mob->NextTimestamp = Timestamp + Mob->SpeciesData->FindInterval;
		return;
	}

	Int32 TargetPositionX = -1;
	Int32 TargetPositionY = -1;

	if (!RTEntityIsNull(Target) && RTMobCanAttack(Mob)) {
		if (Target.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
			RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Target);
			assert(Character);

			Int32 Distance = RTMovementDistance(&Mob->Movement, &Character->Movement);
			RTMobUpdateActiveSkill(Runtime, WorldContext, Mob, Distance);

			if (Mob->AggroTargetDistance != Distance) {
				Mob->AggroTargetDistance = Distance;
				if (Mob->Pattern) RTMobPatternDistanceChanged(Runtime, WorldContext, Mob, Mob->Pattern);
			}

			Mob->AggroTargetDistance = Distance;

			if (Distance <= Mob->ActiveSkill->Distance) {
				if (!Mob->Pattern) RTMobAttackTarget(Runtime, WorldContext, Mob, Character);
				return;
			}
			else {
				Int32 SpawnDistance = RTCalculateDistance(
					Mob->Movement.PositionCurrent.X,
					Mob->Movement.PositionCurrent.Y,
					Mob->Spawn.AreaX + Mob->Spawn.AreaWidth / 2,
					Mob->Spawn.AreaY + Mob->Spawn.AreaHeight / 2
				);

				if (SpawnDistance < Mob->ChaseRange && SpawnDistance < Mob->LimitRangeB) {
					TargetPositionX = Character->Movement.PositionCurrent.X;
					TargetPositionY = Character->Movement.PositionCurrent.Y;

					Int32 DeltaX = TargetPositionX - Mob->Movement.PositionCurrent.X;
					Int32 DeltaY = TargetPositionY - Mob->Movement.PositionCurrent.Y;
					Int32 AbsDeltaX = ABS(DeltaX);
					Int32 AbsDeltaY = ABS(DeltaY);
					Int32 OffsetPoint = 0;
					Int32 OffsetX = 0;
					Int32 OffsetY = 0;

					if (AbsDeltaX >= AbsDeltaY) {
						OffsetPoint = AbsDeltaX - Mob->ActiveSkill->Offset;
						if (OffsetPoint < 0) {
							TargetPositionX = Mob->Movement.PositionCurrent.X;
							TargetPositionY = Mob->Movement.PositionCurrent.Y;
						}
						else {
							OffsetX = OffsetPoint;
							OffsetY = (AbsDeltaY * OffsetX + (AbsDeltaX >> 1)) / AbsDeltaX;

							if (DeltaX < 0) OffsetX = -OffsetX;
							if (DeltaY < 0) OffsetY = -OffsetY;

							TargetPositionX = Mob->Movement.PositionCurrent.X + OffsetX;
							TargetPositionY = Mob->Movement.PositionCurrent.Y + OffsetY;
						}
					}
					else {
						OffsetPoint = AbsDeltaY - Mob->ActiveSkill->Offset;
						if (OffsetPoint < 0) {
							TargetPositionX = Mob->Movement.PositionCurrent.X;
							TargetPositionY = Mob->Movement.PositionCurrent.Y;
						}
						else {
							OffsetY = OffsetPoint;
							OffsetX = (AbsDeltaX * OffsetY + (AbsDeltaY >> 1)) / AbsDeltaY;

							if (DeltaX < 0) OffsetX = -OffsetX;
							if (DeltaY < 0) OffsetY = -OffsetY;

							TargetPositionX = Mob->Movement.PositionCurrent.X + OffsetX;
							TargetPositionY = Mob->Movement.PositionCurrent.Y + OffsetY;
						}
					}
				}
			}
		}
		else if (Target.EntityType == RUNTIME_ENTITY_TYPE_MOB) {
			RTMobRef TargetMob = RTWorldContextGetMob(WorldContext, Target);
			assert(TargetMob);

			Int32 Distance = RTMovementDistance(&Mob->Movement, &TargetMob->Movement);
			RTMobUpdateActiveSkill(Runtime, WorldContext, Mob, Distance);

			if (Mob->AggroTargetDistance != Distance) {
				Mob->AggroTargetDistance = Distance;
				if (Mob->Pattern) RTMobPatternDistanceChanged(Runtime, WorldContext, Mob, Mob->Pattern);
			}

			if (Distance <= Mob->ActiveSkill->Distance) {
				RTMobAttackTargetMob(Runtime, WorldContext, Mob, TargetMob);
				return;
			}
			else {
				Int32 SpawnDistance = RTCalculateDistance(
					Mob->Movement.PositionCurrent.X,
					Mob->Movement.PositionCurrent.Y,
					Mob->Spawn.AreaX + Mob->Spawn.AreaWidth / 2,
					Mob->Spawn.AreaY + Mob->Spawn.AreaHeight / 2
				);

				if (SpawnDistance < Mob->ChaseRange && SpawnDistance < Mob->LimitRangeB) {
					TargetPositionX = TargetMob->Movement.PositionCurrent.X;
					TargetPositionY = TargetMob->Movement.PositionCurrent.Y;
				}
			}
		}
	}

	Bool IsPatrolling = RTMobPatrolUpdate(Runtime, WorldContext, Mob);
	if (IsPatrolling) {
		Mob->NextTimestamp = Timestamp + Mob->SpeciesData->MoveInterval;
		return;
	}

	if (RTMobCanMove(Mob) && !RTMobIsUnmovable(Mob) && !IsPatrolling) {
		Bool IsChasing = true;
		if (TargetPositionX < 0 || TargetPositionY < 0) {
			TargetPositionX = RandomRange(&WorldContext->Seed, Mob->Spawn.AreaX, Mob->Spawn.AreaX + Mob->Spawn.AreaWidth);
			TargetPositionY = RandomRange(&WorldContext->Seed, Mob->Spawn.AreaY, Mob->Spawn.AreaY + Mob->Spawn.AreaHeight); 
			IsChasing = false;
		}

		UInt32 CollisionMask = Mob->Movement.CollisionMask | RUNTIME_WORLD_TILE_USER | RUNTIME_WORLD_TILE_MOBS;
		if (RTWorldIsTileColliding(Runtime, WorldContext, TargetPositionX, TargetPositionY, CollisionMask)) {
			Mob->NextTimestamp = Timestamp + RUNTIME_MOB_MIN_PHASE_TIMEOUT;
			return;
		}

		Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = IsChasing ? Mob->SpeciesData->ChaseSpeed : Mob->SpeciesData->MoveSpeed;
		Mob->IsChasing = IsChasing;

		RTMovementSetSpeed(Runtime, &Mob->Movement, (Int32)Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED]);
		Mob->Movement.PositionEnd.X = TargetPositionX;
		Mob->Movement.PositionEnd.Y = TargetPositionY;

		Bool PathFound = RTMovementFindPath(
			Runtime,
			WorldContext,
			&Mob->Movement
		);

		if (!PathFound) {
			if (Mob->Movement.WaypointCount != 2) {
				Mob->RemainingFindCount = Mob->SpeciesData->FindCount;
				return;
			}

			Mob->Movement.PositionEnd.X = Mob->Movement.Waypoints[1].X;
			Mob->Movement.PositionEnd.Y = Mob->Movement.Waypoints[1].Y;
		}

		RTMovementStartDeadReckoning(Runtime, &Mob->Movement);

		if (Mob->IsChasing) {
			NOTIFICATION_DATA_MOB_CHASE_BEGIN* Notification = RTNotificationInit(MOB_CHASE_BEGIN);
			Notification->Entity = Mob->ID;
			Notification->TickCount = Mob->Movement.TickCount;
			Notification->PositionBeginX = Mob->Movement.PositionBegin.X;
			Notification->PositionBeginY = Mob->Movement.PositionBegin.Y;
			Notification->PositionEndX = Mob->Movement.PositionEnd.X;
			Notification->PositionEndY = Mob->Movement.PositionEnd.Y;
			RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
		}
		else {
			NOTIFICATION_DATA_MOB_MOVE_BEGIN* Notification = RTNotificationInit(MOB_MOVE_BEGIN);
			Notification->Entity = Mob->ID;
			Notification->TickCount = Mob->Movement.TickCount;
			Notification->PositionBeginX = Mob->Movement.PositionBegin.X;
			Notification->PositionBeginY = Mob->Movement.PositionBegin.Y;
			Notification->PositionEndX = Mob->Movement.PositionEnd.X;
			Notification->PositionEndY = Mob->Movement.PositionEnd.Y;
			RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
		}
        
		if (Mob->IsChasing) {
			Mob->NextTimestamp = Timestamp + Mob->SpeciesData->ChaseInterval;
		}
		else {
			Mob->NextTimestamp = Timestamp + Mob->SpeciesData->MoveInterval;
		}

		return;
	}

	Mob->RemainingFindCount = Mob->SpeciesData->FindCount;
}

Void RTMobOnEvent(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob,
	CString Event
) {
	if (!Mob->Script) return;

	RTScriptCall(
        Mob->Script, 
        Event, 
        LUA_TLIGHTUSERDATA, Runtime, 
        LUA_TLIGHTUSERDATA, World, 
        LUA_TLIGHTUSERDATA, Mob,
        NULL
    );
}

Void RTMobStartSpecialAction(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	Int32 SpecialActionIndex
) {
	if (Mob->SpecialActionIndex > 0) {
		RTMobCancelSpecialAction(Runtime, WorldContext, Mob);
	}

	Timestamp CurrentTimestamp = GetTimestampMs();

	Mob->SpecialActionIndex = SpecialActionIndex;
	Mob->SpecialActionStartTimestamp = CurrentTimestamp;

	NOTIFICATION_DATA_MOB_PATTERN_SPECIAL_ACTION* Notification = RTNotificationInit(MOB_PATTERN_SPECIAL_ACTION);
	Notification->Count = 1;

	NOTIFICATION_DATA_MOB_PATTERN_SPECIAL_ACTION_MOB* NotificationTarget = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOB_PATTERN_SPECIAL_ACTION_MOB);
	NotificationTarget->MobID = Mob->ID;
	NotificationTarget->ActionIndex = Mob->SpecialActionIndex;
	NotificationTarget->ActionElapsedTime = (UInt32)(CurrentTimestamp - Mob->SpecialActionStartTimestamp);
	RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
}

Void RTMobCancelSpecialAction(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob
) {
	if (!Mob->SpecialActionIndex) return;

	Timestamp CurrentTimestamp = GetTimestampMs();

	Mob->SpecialActionIndex = 0;
	Mob->SpecialActionStartTimestamp = CurrentTimestamp;

	NOTIFICATION_DATA_MOB_PATTERN_SPECIAL_ACTION* Notification = RTNotificationInit(MOB_PATTERN_SPECIAL_ACTION);
	Notification->Count = 1;

	NOTIFICATION_DATA_MOB_PATTERN_SPECIAL_ACTION_MOB* NotificationTarget = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOB_PATTERN_SPECIAL_ACTION_MOB);
	NotificationTarget->MobID = Mob->ID;
	NotificationTarget->ActionIndex = Mob->SpecialActionIndex;
	NotificationTarget->ActionElapsedTime = (UInt32)(CurrentTimestamp - Mob->SpecialActionStartTimestamp);
	RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
}

Void RTMobAttack(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	Int32 AttackIndex,
	Int64 AttackRate,
	Int64 PhysicalAttackMin,
	Int64 PhysicalAttackMax,
	Int32 TargetType,
	Int32 TargetIndex
) {
	RTDataMobAttackRef MobAttack = RTRuntimeDataMobAttackGet(Runtime->Context, AttackIndex);
	assert(MobAttack && MobAttack->MobAttackDataCount > 0);

	RTDataMobAttackDataRef MobAttackData = &MobAttack->MobAttackDataList[0];
	RTDataMobSkillRef MobSkillData = (MobAttackData->SkillID > 0) ? RTRuntimeDataMobSkillGet(Runtime->Context, MobAttackData->SkillID) : NULL;

	NOTIFICATION_DATA_MOB_PATTERN_ATTACK* Notification = RTNotificationInit(MOB_PATTERN_ATTACK);
	Notification->SourceIndex = Mob->ID.Serial;
	Notification->AttackIndex = AttackIndex;
	Notification->X = Mob->Movement.PositionCurrent.X;
	Notification->Y = Mob->Movement.PositionCurrent.Y;
	Notification->ReflectResult = 0;
	Notification->CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Notification->ReflectDamage = 0;
	Notification->Unknown3 = 0;

	RTBattleAttributesRef Attacker = &Mob->Attributes;
	RTBattleAttributesRef Defender = NULL;

	if (TargetType == RUNTIME_MOB_ATTACK_TARGET_TYPE_ENEMY_1 ||
		TargetType == RUNTIME_MOB_ATTACK_TARGET_TYPE_ENEMY_2 ||
		TargetType == RUNTIME_MOB_ATTACK_TARGET_TYPE_ENEMY_DEFERRED ||
		TargetType == RUNTIME_MOB_ATTACK_TARGET_TYPE_ENEMY_FIX_POSITION) {
		// TODO: Check Enemies array & also filter MaxAggroTarget to character only
		RTEntityID TargetID = RTMobGetMaxAggroTarget(Runtime, WorldContext, Mob);
	}
	else if (TargetType == RUNTIME_MOB_ATTACK_TARGET_TYPE_SELF) {
		Defender = &Mob->Attributes;
	}
	else if (TargetType == RUNTIME_MOB_ATTACK_TARGET_TYPE_TARGET) {
		RTEntityID TargetID = {
			.EntityIndex = TargetIndex,
			.WorldIndex = WorldContext->WorldData->WorldIndex,
			.EntityType = RUNTIME_ENTITY_TYPE_MOB
		};
		RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
		Defender = &Target->Attributes;
	}
	else if (TargetType == RUNTIME_MOB_ATTACK_TARGET_TYPE_MISSION_GATE) {
		for (Int Index = 0; Index < Mob->EnemyCount; Index += 1) {
			if (Mob->Enemies[Index] >= 0) {
				RTEntityID TargetID = {
					.EntityIndex = Mob->Enemies[Index],
					.WorldIndex = WorldContext->WorldData->WorldIndex,
					.EntityType = RUNTIME_ENTITY_TYPE_MOB
				};
				RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
				if (Target->Spawn.IsMissionGate > 0) {
					Defender = &Target->Attributes;
					break;
				}
			}
		}
	}

	if (!Defender) {
		Trace("MobAttack(%d, %d) no target found", Mob->ID.EntityIndex, AttackIndex);
		return;
	}

	RTEntityID Target = RTMobGetMaxAggroTarget(Runtime, WorldContext, Mob);
	if (Target.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
		RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Target);
		assert(Character);

		NOTIFICATION_DATA_MOB_PATTERN_ATTACK_TARGET* NotificationTarget = RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOB_PATTERN_ATTACK_TARGET);
		NotificationTarget->TargetEntityType = Target.EntityType;
		NotificationTarget->TargetIndex = Character->CharacterIndex;
		NotificationTarget->IsDead = RTCharacterIsAlive(Runtime, Character) ? 0 : 1;
		NotificationTarget->AttackType = 0;
		NotificationTarget->AppliedDamage = 0;
		NotificationTarget->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
		NotificationTarget->CurrentShield = Character->Attributes.Values[RUNTIME_ATTRIBUTE_DAMAGE_ABSORB];
		NotificationTarget->Unknown1 = 0;
		NotificationTarget->AdditionalDamage = 0;
		NotificationTarget->BfxIndex = 0;
		NotificationTarget->BfxDuration = 0;

		Notification->TargetCount += 1;
	}

	RTNotificationAppendStruct(Notification, NOTIFICATION_DATA_MOB_PATTERN_ATTACK_TARGET);
	RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
}

Void RTMobHeal(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	Int64 Amount
) {
	if (!RTMobIsAlive(Mob)) return;
	if (Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] >= Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX]) return;

	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] += Amount;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MIN(Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT], Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX]);

	NOTIFICATION_DATA_MOB_SPECIAL_BUFF* Notification = RTNotificationInit(MOB_SPECIAL_BUFF);
	Notification->MobID = Mob->ID;
	Notification->CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Notification->ReceivedDamage = 0;
	RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
}

Void RTMobCurse(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	Int64 Amount
) {
	RTMobApplyDamage(Runtime, WorldContext, Mob, kEntityIDNull, Amount, 0);

	NOTIFICATION_DATA_MOB_SPECIAL_BUFF* Notification = RTNotificationInit(MOB_SPECIAL_BUFF);
	Notification->MobID = Mob->ID;
	Notification->CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
	Notification->ReceivedDamage = (UInt32)Amount;
	RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
}
