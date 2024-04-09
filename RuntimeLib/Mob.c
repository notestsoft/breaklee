#include "Mob.h"
#include "Movement.h"
#include "Script.h"
#include "Runtime.h"
#include "WorldManager.h"

// TODO: Gate mob can attack player which leads to receiving a lot of damage!

Void RTMobInit(
	RTRuntimeRef Runtime,
	RTMobRef Mob
) {
	assert(Mob && Mob->SpeciesData);

	Mob->Attributes.Seed = (Int32)PlatformGetTickCount();
	Mob->Attributes.AttackTimeout = 0;
	memset(Mob->Attributes.Values, 0, sizeof(Mob->Attributes.Values));
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = Mob->SpeciesData->MoveSpeed;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] = Mob->SpeciesData->HP;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = Mob->SpeciesData->HP;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] = Mob->SpeciesData->AttackRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE] = Mob->SpeciesData->Defense;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE] = Mob->SpeciesData->DefenseRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_EXP] = (Int32)Mob->SpeciesData->Exp;
	Mob->HPTriggerThreshold = (Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] * 300) / 1000;
	Mob->SpecialAttack = NULL;
	Mob->SpecialAttackSkill = NULL;
	Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
	memset(&Mob->Aggro, 0, sizeof(struct _RTMobAggroData));
}

Void RTMobInitFromSpeciesData(
	RTRuntimeRef Runtime,
	RTMobRef Mob,
	RTMobSpeciesDataRef MobSpeciesData
) {
	assert(Mob && MobSpeciesData);

	Mob->SpeciesData = MobSpeciesData;
	Mob->Attributes.Seed = (Int32)PlatformGetTickCount();
	Mob->Attributes.AttackTimeout = 0;
	memset(Mob->Attributes.Values, 0, sizeof(Mob->Attributes.Values));
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = Mob->SpeciesData->MoveSpeed;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] = Mob->SpeciesData->HP;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = Mob->SpeciesData->HP;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_ATTACK_RATE] = Mob->SpeciesData->AttackRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE] = Mob->SpeciesData->Defense;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_DEFENSE_RATE] = Mob->SpeciesData->DefenseRate;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_EXP] = (Int32)Mob->SpeciesData->Exp;
	Mob->HPTriggerThreshold = (Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] * 300) / 1000;
	Mob->SpecialAttack = NULL;
	Mob->SpecialAttackSkill = NULL;
	Mob->ActiveSkill = &Mob->SpeciesData->DefaultSkill;
}

Bool RTMobCanMove(RTMobRef Mob) {
	return Mob->SpeciesData->CanMove;
}

Bool RTMobCanAttack(RTMobRef Mob) {
	return Mob->SpeciesData->CanAttack;
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

    if (Mob->IsKilled) {
        return false;
    }

	return Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] > 0;
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
	Int32 Index = 0;

	while (Index < Mob->Aggro.Count) {
		RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Mob->Aggro.Entities[Index]);
		if (!Character) {
			Int32 TailLength = Mob->Aggro.Count - Index - 1;
			if (TailLength > 0) {
				memmove(&Mob->Aggro.Entities[Index], &Mob->Aggro.Entities[Index + 1], sizeof(RTEntityID));
				memmove(&Mob->Aggro.ReceivedDamage[Index], &Mob->Aggro.ReceivedDamage[Index + 1], sizeof(Int64));
			}

			Mob->Aggro.Count -= 1;
			continue;
		}

		Int32 Distance = RTMovementDistance(&Character->Movement, &Mob->Movement);
		if (!Character || 
			Character->Info.Position.WorldID != WorldContext->WorldData->WorldIndex ||
			!RTCharacterIsAlive(Runtime, Character) ||
			Distance > Mob->SpeciesData->AlertRange) {
			Int32 TailLength = Mob->Aggro.Count - Index - 1;
			if (TailLength > 0) {
				memmove(&Mob->Aggro.Entities[Index], &Mob->Aggro.Entities[Index + 1], sizeof(RTEntityID));
				memmove(&Mob->Aggro.ReceivedDamage[Index], &Mob->Aggro.ReceivedDamage[Index + 1], sizeof(Int64));
			}

			Mob->Aggro.Count -= 1;
			continue;
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

        RTEventData EventData = { 0 };
        EventData.MobMovementEnd.PositionCurrentX = Mob->Movement.PositionCurrent.X;
        EventData.MobMovementEnd.PositionCurrentY = Mob->Movement.PositionCurrent.Y;

		RTRuntimeBroadcastEventData(
			Runtime,
			(Mob->IsChasing) ? RUNTIME_EVENT_MOB_CHASE_END : RUNTIME_EVENT_MOB_MOVEMENT_END,
			World,
			kEntityIDNull,
			Mob->ID,
			Mob->Movement.PositionCurrent.X,
			Mob->Movement.PositionCurrent.Y,
            EventData
		);
	}
}

Void RTMobApplyDamage(
	RTRuntimeRef Runtime,
	RTWorldContextRef World,
	RTMobRef Mob,
	RTEntityID Source,
	Int32 Damage
) {
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] -= Damage;
	Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT] = MAX(0, Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT]);
	
	if (Source.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
		Bool Found = false;
		for (Int32 Index = 0; Index < Mob->Aggro.Count; Index += 1) {
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

		RTMobCancelMovement(Runtime, World, Mob);
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
		assert(false && "Not Implemented!");
	}

	// TODO: Check SkillGroup for attack patterns: single, aoe, movement, heal, buff, debuff

	struct _RTBattleResult Result = { 0 };
	RTCalculateNormalAttackResult(
		Runtime,
		RUNTIME_BATTLE_SKILL_TYPE_SWORD,
		Mob->SpeciesData->Level,
		&Mob->Attributes,
		Character->Info.Basic.Level,
		&Character->Attributes,
		&Result
	);

	RTCharacterAddHP(Runtime, Character, -Result.AppliedDamage, false);
	Mob->NextTimestamp = PlatformGetTickCount() + Mob->ActiveSkill->Interval;

	RTEventData EventData = { 0 };
	EventData.MobAttack.IsDefaultSkill = true;
	EventData.MobAttack.ResultCount = 1;
	EventData.MobAttack.Results[0].Entity = Character->ID;
	EventData.MobAttack.Results[0].Result = Result.AttackType;
	EventData.MobAttack.Results[0].IsDead = Result.IsDead;
	EventData.MobAttack.Results[0].AppliedDamage = Result.AppliedDamage;
	EventData.MobAttack.Results[0].TargetHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];

	RTRuntimeBroadcastEventData(
		Runtime,
		RUNTIME_EVENT_MOB_ATTACK,
		World,
		Mob->ID,
		Character->ID,
		Mob->Movement.PositionCurrent.X,
		Mob->Movement.PositionCurrent.Y,
		EventData
	);
}

struct _RTMobFindNearbyTargetArguments {
	RTWorldManagerRef WorldManager;
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

		Int32 LevelDifference = Character->Info.Basic.Level - Arguments->Mob->SpeciesData->Level;
		if (LevelDifference > RUNTIME_MOB_MAX_FIND_LEVEL_DIFFERENCE) {
			return;
		}
	}

	if (Distance < Arguments->Distance) {
		Arguments->Distance = Distance;
		Arguments->Target = Entity;
	}
}

Void RTMobUpdate(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob
) {
	Timestamp Timestamp = PlatformGetTickCount();
	if (Timestamp < Mob->NextTimestamp) {
		return;
	}

	RTMobOnEvent(Runtime, WorldContext, Mob, MOB_EVENT_UPDATE);
	
	if (Mob->EventSpawnTimestamp > 0 && Mob->EventSpawnTimestamp <= Timestamp) {
		Mob->EventSpawnTimestamp = 0;
		return RTWorldSpawnMob(Runtime, WorldContext, Mob);
	}

	if (Mob->EventDespawnTimestamp > 0 && Mob->EventDespawnTimestamp <= Timestamp) {
		Mob->EventDespawnTimestamp = 0;

		if (Mob->IsSpawned) {
			Mob->IsSpawned = false;
			Mob->NextTimestamp = Timestamp + Mob->Spawn.SpawnInterval;
			RTWorldDespawnMob(Runtime, WorldContext, Mob);
		}

		return;
	}

	if (!RTMobIsAlive(Mob) && Mob->IsSpawned) {
		Mob->IsSpawned = false;
		Mob->NextTimestamp = Timestamp + Mob->Spawn.SpawnInterval;
		return RTWorldDespawnMob(Runtime, WorldContext, Mob);
	}

	if (!RTMobIsAlive(Mob)) {
		if (RTMobCanRespawn(Mob)) {
			Mob->Aggro.Count = 0;
			memset(&Mob->Aggro.Entities, 0, sizeof(Mob->Aggro.Entities));

			Mob->IsKilled = false;
			Mob->IsSpawned = true;
			Mob->RemainingSpawnCount = MAX(0, Mob->RemainingSpawnCount - 1);
			Mob->NextTimestamp = Timestamp;
			return RTWorldSpawnMob(Runtime, WorldContext, Mob);
		}

		return;
	}

	RTMovementUpdateDeadReckoning(Runtime, &Mob->Movement);

	if (Mob->Movement.IsMoving) {
		if (Mob->Movement.PositionCurrent.X == Mob->Movement.PositionEnd.X &&
			Mob->Movement.PositionCurrent.Y == Mob->Movement.PositionEnd.Y) {
			RTMovementEndDeadReckoning(Runtime, &Mob->Movement);
            
            RTEventData EventData = { 0 };
            EventData.MobMovementEnd.PositionCurrentX = Mob->Movement.PositionCurrent.X;
            EventData.MobMovementEnd.PositionCurrentY = Mob->Movement.PositionCurrent.Y;

			RTRuntimeBroadcastEventData(
				Runtime,
				(Mob->IsChasing) ? RUNTIME_EVENT_MOB_CHASE_END : RUNTIME_EVENT_MOB_MOVEMENT_END,
				WorldContext,
				kEntityIDNull,
				Mob->ID,
				Mob->Movement.PositionCurrent.X,
				Mob->Movement.PositionCurrent.Y,
                EventData
			);

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
	if (RTEntityIsNull(Target) && Mob->RemainingFindCount > 0) {
		if (RTMobIsAggressive(Mob)) {
			struct _RTMobFindNearbyTargetArguments Arguments = { 0 };
			Arguments.WorldManager = WorldContext->WorldManager;
			Arguments.Mob = Mob;
			Arguments.Distance = Mob->SpeciesData->AlertRange;
			Arguments.Target = kEntityIDNull;
			RTWorldContextEnumerateEntitiesInRange(
				WorldContext,
				RUNTIME_ENTITY_TYPE_CHARACTER,
				Mob->Movement.PositionCurrent.X,
				Mob->Movement.PositionCurrent.Y,
				Mob->SpeciesData->AlertRange,
				Mob->Movement.CollisionMask,
				Mob->Movement.IgnoreMask,
				&_RTMobFindNearbyTargetProc,
				&Arguments
			);

			if (!RTEntityIsNull(Arguments.Target)) {
				assert(Mob->Aggro.Count < RUNTIME_MEMORY_MAX_MOB_AGGRO_COUNT);

				Mob->Aggro.Entities[Mob->Aggro.Count] = Arguments.Target;
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
		RTCharacterRef Character = RTWorldManagerGetCharacter(WorldContext->WorldManager, Target);
		assert(Character);

		Int32 Distance = RTMovementDistance(&Mob->Movement, &Character->Movement);
		RTMobUpdateActiveSkill(Runtime, WorldContext, Mob, Distance);

		if (Distance <= Mob->ActiveSkill->Range) {
			RTMobAttackTarget(Runtime, WorldContext, Mob, Character);
			return;
		}
		else {
			Int32 SpawnDistance = RTCalculateDistance(
				Mob->Movement.PositionCurrent.X,
				Mob->Movement.PositionCurrent.Y,
				Mob->Spawn.AreaX + Mob->Spawn.AreaWidth / 2,
				Mob->Spawn.AreaY + Mob->Spawn.AreaHeight / 2
			);

			if (SpawnDistance < Mob->SpeciesData->ChaseRange && SpawnDistance < Mob->SpeciesData->LimitRangeB) {
				TargetPositionX = Character->Movement.PositionCurrent.X;
				TargetPositionY = Character->Movement.PositionCurrent.Y;
			}
		}
	}

	if (RTMobCanMove(Mob) && !RTMobIsUnmovable(Mob)) {
		if (TargetPositionX < 0 || TargetPositionY < 0) {
			TargetPositionX = RandomRange(&WorldContext->Seed, Mob->Spawn.AreaX, Mob->Spawn.AreaX + Mob->Spawn.AreaWidth);
			TargetPositionY = RandomRange(&WorldContext->Seed, Mob->Spawn.AreaY, Mob->Spawn.AreaY + Mob->Spawn.AreaHeight);
			Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = Mob->SpeciesData->MoveSpeed;
			Mob->Movement.Speed = Mob->SpeciesData->MoveSpeed;
			Mob->IsChasing = false;
		}
		else {
			Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED] = Mob->SpeciesData->ChaseSpeed;
			Mob->Movement.Speed = Mob->SpeciesData->ChaseSpeed;
			Mob->IsChasing = true;
		}

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
        
        RTEventData EventData = { 0 };
        EventData.MobMovementBegin.TickCount = Mob->Movement.TickCount;
        EventData.MobMovementBegin.PositionBeginX = Mob->Movement.PositionBegin.X;
        EventData.MobMovementBegin.PositionBeginY = Mob->Movement.PositionBegin.Y;
        EventData.MobMovementBegin.PositionEndX = Mob->Movement.PositionEnd.X;
        EventData.MobMovementBegin.PositionEndY = Mob->Movement.PositionEnd.Y;
        
		RTRuntimeBroadcastEventData(
			Runtime,
			(Mob->IsChasing) ? RUNTIME_EVENT_MOB_CHASE_BEGIN : RUNTIME_EVENT_MOB_MOVEMENT_BEGIN,
			WorldContext,
			kEntityIDNull,
			Mob->ID,
			Mob->Movement.PositionCurrent.X,
			Mob->Movement.PositionCurrent.Y,
            EventData
		);

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

	RTScriptCall(Mob->Script, Event, Runtime, World, Mob, NULL);
}