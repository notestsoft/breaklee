#include "Force.h"
#include "Mob.h"
#include "MobPattern.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "World.h"
#include "WorldManager.h"

RTMobPatternSpawnDataRef RTMobPatternDataGetSpawnData(
	RTMobPatternDataRef MobPatternData,
	Int32 LinkMobIndex
) {
	for (Int Index = 0; Index < ArrayGetElementCount(MobPatternData->MobPool); Index += 1) {
		RTMobPatternSpawnDataRef SpawnData = (RTMobPatternSpawnDataRef)ArrayGetElementAtIndex(MobPatternData->MobPool, Index);
		if (SpawnData->Index == LinkMobIndex) return SpawnData;
	}

	return NULL;
}

Void RTMobPatternCancelAction(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobActionStateRef ActionState
);

Void RTMobPatternInsertActionState(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobTriggerDataRef TriggerData,
	RTMobTriggerStateRef TriggerState,
	RTMobActionGroupDataRef ActionGroupData,
	RTMobActionDataRef ActionData
) {
	Trace("EnqueueAction(%d, %d, %d) on Mob(%d:%d:%d)",
		TriggerData->Index,
		ActionGroupData->Index,
		ActionData->Index,
		Mob->ID.EntityIndex,
		WorldContext->DungeonIndex,
		WorldContext->WorldData->WorldIndex
	);

	Int32 StateCount = (Int32)ArrayGetElementCount(MobPattern->ActionStates);
	Int32 LowIndex = 0;
	Int32 HighIndex = StateCount - 1;
	Int32 InsertionIndex = StateCount;

	while (LowIndex <= HighIndex) {
		Int32 MidIndex = (LowIndex + HighIndex) / 2;
		RTMobActionStateRef ActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, MidIndex);
		if (ActionState->ActionData->Priority == ActionData->Priority) {
			InsertionIndex = MidIndex;
			break;
		}
		else if (ActionState->ActionData->Priority < ActionData->Priority) {
			LowIndex = MidIndex + 1;
		}
		else {
			HighIndex = MidIndex - 1;
		}
	}

    // TODO: Why is this overriding?
	InsertionIndex = LowIndex;
	struct _RTMobActionState ActionState = { 0 };
	ActionState.TriggerData = TriggerData;
	ActionState.TriggerState = TriggerState;
	ActionState.ActionGroupData = ActionGroupData;
	ActionState.ActionData = ActionData;
	ActionState.StartTimestamp = GetTimestampMs() + ActionGroupData->Delay;
	ActionState.EndTimestamp = ActionState.StartTimestamp + ActionData->Duration;
 	ArrayInsertElementAtIndex(MobPattern->ActionStates, InsertionIndex, &ActionState);
}

Void RTMobTriggerEnqueue(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobTriggerDataRef TriggerData,
	RTMobTriggerStateRef TriggerState
) {
	Int32 Seed = (Int32)PlatformGetTickCount();
	for (Int Index = 0; Index < TriggerData->ActionGroupCount; Index += 1) {
		RTMobPatternInsertActionState(
			Runtime,
			WorldContext,
			Mob,
			MobPattern,
			TriggerData,
			TriggerState,
			&TriggerData->ActionGroups[Index],
			&TriggerData->ActionGroups[Index].Actions[RandomRange(&Seed, 0, TriggerData->ActionGroups[Index].ActionCount - 1)]
		);
	}

	TriggerState->IsRunning = true;
}

Void RTMobPatternStartAction(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobActionStateRef ActionState
) {
	Trace("StartAction(%d, %d, %d) on Mob(%d:%d:%d)",
		ActionState->TriggerData->Index,
		ActionState->ActionGroupData->Index,
		ActionState->ActionData->Index,
		Mob->ID.EntityIndex, 
		WorldContext->DungeonIndex,
		WorldContext->WorldData->WorldIndex
	);

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_WARP_TARGET) {
		if (Mob->Aggro.Count < 1) goto end;

		RTEntityID TargetID = Mob->Aggro.Entities[0];
		Void* TargetContext = RTWorldContextGetEntityContext(WorldContext, TargetID);
		if (!TargetContext) goto end;

		if (TargetID.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER) {
			RTCharacterRef Target = (RTCharacterRef)TargetContext;
			Target->MobPatternWarpX = ActionState->ActionData->Parameters.WarpTarget.PositionX;
			Target->MobPatternWarpY = ActionState->ActionData->Parameters.WarpTarget.PositionY;

			NOTIFICATION_DATA_MOB_PATTERN_WARP_TARGET* Notification = RTNotificationInit(MOB_PATTERN_WARP_TARGET);
			Notification->SourceIndex = Mob->ID.Serial;
			Notification->TargetIndex = (UInt32)Target->CharacterIndex;
			Notification->PositionX = ActionState->ActionData->Parameters.WarpTarget.PositionX;
			Notification->PositionY = ActionState->ActionData->Parameters.WarpTarget.PositionY;
			RTNotificationDispatchToNearby(Notification, Target->Movement.WorldChunk);
		}
		else if (TargetID.EntityType == RUNTIME_ENTITY_TYPE_MOB) {
			RTMobRef Target = (RTMobRef)TargetContext;
			RTWorldChunkRemove(Target->Movement.WorldChunk, TargetID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
			RTWorldTileDecreaseMobCount(Runtime, WorldContext, Target->Movement.PositionTile.X, Target->Movement.PositionTile.Y);
			RTMovementInitialize(
				Runtime,
				&Target->Movement,
				Target->ID,
				ActionState->ActionData->Parameters.WarpTarget.PositionX,
				ActionState->ActionData->Parameters.WarpTarget.PositionY,
				(Int32)Target->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED],
				RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN
			);
			RTWorldTileIncreaseMobCount(Runtime, WorldContext, Target->Movement.PositionTile.X, Target->Movement.PositionTile.Y);
			Target->Movement.WorldContext = WorldContext;
			Target->Movement.WorldChunk = RTWorldContextGetChunk(WorldContext, Target->Movement.PositionCurrent.X, Target->Movement.PositionCurrent.Y);
			RTWorldChunkInsert(Target->Movement.WorldChunk, TargetID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
		}
		else {
			UNREACHABLE("Unexpected target type given for action WARP_TARGET!");
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_WARP_SELF) {
		RTWorldChunkRemove(Mob->Movement.WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
		RTWorldTileDecreaseMobCount(Runtime, WorldContext, Mob->Movement.PositionTile.X, Mob->Movement.PositionTile.Y);
		RTMovementInitialize(
			Runtime,
			&Mob->Movement,
			Mob->ID,
			ActionState->ActionData->Parameters.WarpTarget.PositionX,
			ActionState->ActionData->Parameters.WarpTarget.PositionY,
			(Int32)Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED],
			RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN
		);
		RTWorldTileIncreaseMobCount(Runtime, WorldContext, Mob->Movement.PositionTile.X, Mob->Movement.PositionTile.Y);
		Mob->Movement.WorldContext = WorldContext;
		Mob->Movement.WorldChunk = RTWorldContextGetChunk(WorldContext, Mob->Movement.PositionCurrent.X, Mob->Movement.PositionCurrent.Y);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SPAWN_MOB) {
		Int32 LinkMobIndex = ActionState->ActionData->Parameters.LinkMobSpawn.LinkMobIndex;
		RTMobPatternSpawnDataRef SpawnData = RTMobPatternDataGetSpawnData(MobPattern->Data, LinkMobIndex);
		assert(SpawnData);

		// TODO: Check if it should create a new mob or respawn the previously spawned one
		RTWorldCreateMob(
			Runtime,
			WorldContext,
			Mob,
			LinkMobIndex,
			SpawnData->MobSpeciesIndex,
			SpawnData->AreaX,
			SpawnData->AreaY,
			SpawnData->AreaWidth,
			SpawnData->AreaHeight,
			SpawnData->Interval,
			SpawnData->Count,
			SpawnData->MobPatternIndex,
			SpawnData->Script,
			0
		);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_HEAL_TARGET) { 
		if (ActionState->ActionData->Parameters.Heal.WorldType == 0 && WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_GLOBAL) goto end;

		RTEntityID TargetID = {
			.EntityIndex = ActionState->ActionData->Parameters.AttackUp.MobIndex,
			.WorldIndex = WorldContext->WorldData->WorldIndex,
			.EntityType = RUNTIME_ENTITY_TYPE_MOB
		};
		RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
		if (!Target) goto end;

		Int64 Value = 0;
		if (ActionState->ActionData->Parameters.Heal.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_DECIMAL) {
			Value = ActionState->ActionData->Parameters.Heal.Value;
		}

		if (ActionState->ActionData->Parameters.Heal.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_PERCENT) {
			Value = (Target->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] * ActionState->ActionData->Parameters.Heal.Value) / 100;
		}

		RTMobHeal(Runtime, WorldContext, Target, Value);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_1 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_2 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_3) {
		RTMobApplyForceEffect(
			Runtime,
			Mob,
			kEntityIDNull,
			RUNTIME_FORCE_EFFECT_EVASION_COMPLETE,
			1,
			RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
		);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SPECIAL_ACTION) {
		RTMobStartSpecialAction(Runtime, WorldContext, Mob, ActionState->ActionData->Parameters.SpecialAction.ActionIndex);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_1 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_2) {
		RTMobAttack(
			Runtime,
			WorldContext,
			Mob,
			ActionState->ActionData->Parameters.Attack.AttackIndex,
			ActionState->ActionData->Parameters.Attack.AttackRate,
			ActionState->ActionData->Parameters.Attack.PhysicalAttackMin,
			ActionState->ActionData->Parameters.Attack.PhysicalAttackMax,
			ActionState->ActionData->TargetType,
			ActionState->ActionData->TargetParameters[0]
		);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_CANCEL_ACTION && !ActionState->ActionData->Parameters.CancelAction.ActionFinished) {
		Timestamp CurrentTimestamp = GetTimestampMs();
		for (Int Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
			RTMobActionStateRef TargetActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, Index);
			if (TargetActionState->ActionGroupData->Index != ActionState->ActionData->Parameters.CancelAction.ActionIndex) continue;

			TargetActionState->EndTimestamp = CurrentTimestamp;
			RTMobPatternCancelAction(Runtime, WorldContext, Mob, MobPattern, TargetActionState);
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_DESPAWN_LINK_MOB) {
		for (Int Index = 0; Index < ArrayGetElementCount(MobPattern->LinkMobs); Index += 1) {
			RTEntityID LinkMobID = *(RTEntityID*)ArrayGetElementAtIndex(MobPattern->LinkMobs, Index);
			RTMobRef LinkMob = RTWorldContextGetMob(WorldContext, LinkMobID);
			if (!LinkMob || LinkMob->LinkMobIndex != ActionState->ActionData->Parameters.LinkMobDespawn.LinkMobIndex) continue;

			RTWorldDespawnMobEvent(Runtime, WorldContext, LinkMob, 1);
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_RESPAWN_SELF) {
		RTWorldRespawnMobEvent(Runtime, WorldContext, Mob, 1);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SOCIAL_ACTION) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_UP_TARGET) {
		if (ActionState->ActionData->Parameters.AttackUp.WorldType == 0 && WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_GLOBAL) goto end;

		RTEntityID TargetID = {
			.EntityIndex = ActionState->ActionData->Parameters.AttackUp.MobIndex,
			.WorldIndex = WorldContext->WorldData->WorldIndex,
			.EntityType = RUNTIME_ENTITY_TYPE_MOB
		};
		RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
		if (!Target) goto end;

		if (ActionState->ActionData->Parameters.AttackUp.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_DECIMAL) {
			RTMobApplyForceEffect(
				Runtime,
				Target,
				kEntityIDNull,
				RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_1,
				ActionState->ActionData->Parameters.AttackUp.Value,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}

		if (ActionState->ActionData->Parameters.AttackUp.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_PERCENT) {
			RTMobApplyForceEffect(
				Runtime,
				Target,
				kEntityIDNull,
				RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_PERCENT,
				ActionState->ActionData->Parameters.AttackUp.Value,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_DEFENSE_UP_TARGET) {
		if (ActionState->ActionData->Parameters.DefenseUp.WorldType == 0 && WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_GLOBAL) goto end;

		RTEntityID TargetID = {
			.EntityIndex = ActionState->ActionData->Parameters.DefenseUp.MobIndex,
			.WorldIndex = WorldContext->WorldData->WorldIndex,
			.EntityType = RUNTIME_ENTITY_TYPE_MOB
		};
		RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
		if (!Target) goto end;

		if (ActionState->ActionData->Parameters.DefenseUp.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_DECIMAL) {
			RTMobApplyForceEffect(
				Runtime,
				Target,
				kEntityIDNull,
				RUNTIME_FORCE_EFFECT_DEFENSE_UP_1,
				ActionState->ActionData->Parameters.DefenseUp.Value,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}

		if (ActionState->ActionData->Parameters.DefenseUp.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_PERCENT) {
			RTMobApplyForceEffect(
				Runtime,
				Target,
				kEntityIDNull,
				RUNTIME_FORCE_EFFECT_DEFENSE_UP_PERCENT,
				ActionState->ActionData->Parameters.DefenseUp.Value,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_DESPAWN_MOB) {
		if (ActionState->ActionData->Parameters.MobDespawn.MobIndex < 0) {
			RTWorldDespawnMobEvent(Runtime, WorldContext, Mob, 1);
		}
		else {
			RTEntityID TargetID = {
				.EntityIndex = ActionState->ActionData->Parameters.MobDespawn.MobIndex,
				.WorldIndex = WorldContext->WorldData->WorldIndex,
				.EntityType = RUNTIME_ENTITY_TYPE_MOB,
			};
			RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
			if (Target) RTWorldDespawnMobEvent(Runtime, WorldContext, Target, 1);
		}
	}

end:

	// TODO: Perform Runtime Action
	RTMobPatternActionStarted(Runtime, WorldContext, Mob, MobPattern, ActionState->ActionGroupData->Index);
}

Void RTMobPatternCancelAction(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobActionStateRef ActionState
) {
	Trace("CancelAction(%d, %d, %d) on Mob(%d:%d:%d)",
		ActionState->TriggerData->Index,
		ActionState->ActionGroupData->Index,
		ActionState->ActionData->Index,
		Mob->ID.EntityIndex,
		WorldContext->DungeonIndex,
		WorldContext->WorldData->WorldIndex
	);

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_WARP_TARGET) { }

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_WARP_SELF) { }

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SPAWN_MOB) { }

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_HEAL_TARGET) { }

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_1 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_2 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_3) {
		RTMobCancelForceEffect(
			Runtime,
			Mob,
			kEntityIDNull,
			RUNTIME_FORCE_EFFECT_EVASION_COMPLETE,
			1,
			RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
		);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SPECIAL_ACTION) {
		RTMobCancelSpecialAction(Runtime, WorldContext, Mob);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_1 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_2) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_CANCEL_ACTION && ActionState->ActionData->Parameters.CancelAction.ActionFinished) {
		Timestamp CurrentTimestamp = GetTimestampMs();
		for (Int Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
			RTMobActionStateRef TargetActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, Index);
			if (TargetActionState->ActionGroupData->Index != ActionState->ActionData->Parameters.CancelAction.ActionIndex) continue;

			TargetActionState->EndTimestamp = CurrentTimestamp;
			RTMobPatternCancelAction(Runtime, WorldContext, Mob, MobPattern, TargetActionState);
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_DESPAWN_LINK_MOB) { }

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_RESPAWN_SELF) { }

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SOCIAL_ACTION) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_UP_TARGET) {
		if (ActionState->ActionData->Duration < 1) goto end;
		if (ActionState->ActionData->Parameters.AttackUp.WorldType == 0 && WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_GLOBAL) goto end;

		RTEntityID TargetID = {
			.EntityIndex = ActionState->ActionData->Parameters.Heal.MobIndex,
			.WorldIndex = WorldContext->WorldData->WorldIndex,
			.EntityType = RUNTIME_ENTITY_TYPE_MOB
		};
		RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
		if (!Target) goto end;

		if (ActionState->ActionData->Parameters.AttackUp.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_DECIMAL) {
			RTMobCancelForceEffect(
				Runtime,
				Target,
				kEntityIDNull,
				RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_1,
				ActionState->ActionData->Parameters.AttackUp.Value,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}

		if (ActionState->ActionData->Parameters.AttackUp.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_PERCENT) {
			RTMobCancelForceEffect(
				Runtime,
				Target,
				kEntityIDNull,
				RUNTIME_FORCE_EFFECT_FINAL_ATTACK_UP_PERCENT,
				ActionState->ActionData->Parameters.AttackUp.Value,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_DEFENSE_UP_TARGET) {
		if (ActionState->ActionData->Parameters.DefenseUp.WorldType == 0 && WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_GLOBAL) goto end;

		RTEntityID TargetID = {
			.EntityIndex = ActionState->ActionData->Parameters.DefenseUp.MobIndex,
			.WorldIndex = WorldContext->WorldData->WorldIndex,
			.EntityType = RUNTIME_ENTITY_TYPE_MOB
		};
		RTMobRef Target = RTWorldContextGetMob(WorldContext, TargetID);
		if (!Target) goto end;

		if (ActionState->ActionData->Parameters.DefenseUp.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_DECIMAL) {
			RTMobCancelForceEffect(
				Runtime,
				Target,
				kEntityIDNull,
				RUNTIME_FORCE_EFFECT_DEFENSE_UP_1,
				ActionState->ActionData->Parameters.DefenseUp.Value,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}

		if (ActionState->ActionData->Parameters.DefenseUp.ValueType == RUNTIME_MOB_PATTERN_VALUE_TYPE_PERCENT) {
			RTMobCancelForceEffect(
				Runtime,
				Target,
				kEntityIDNull,
				RUNTIME_FORCE_EFFECT_DEFENSE_UP_PERCENT,
				ActionState->ActionData->Parameters.DefenseUp.Value,
				RUNTIME_FORCE_VALUE_TYPE_ADDITIVE
			);
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_DESPAWN_MOB) { }

end:
	// TODO: Perform Runtime Cancellation
	RTMobPatternActionFinished(Runtime, WorldContext, Mob, MobPattern, ActionState->ActionGroupData->Index);
}

Void RTMobPatternSpawn(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
) {
	MobPattern->StartTimestamp = 0;
	MobPattern->SpawnTimestamp = GetTimestampMs();
	MobPattern->EventTriggerMask = 0;
	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		TriggerState->IsRunning = false;
		TriggerState->Timestamp = MobPattern->StartTimestamp;
		MobPattern->EventTriggerMask |= (1 << TriggerData->Type);
	}
}

Void RTMobPatternStop(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
) {
	for (Int Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
		RTMobActionStateRef ActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, Index);
		if (ActionState->IsRunning) {
			ActionState->IsRunning = false;
			RTMobPatternCancelAction(Runtime, WorldContext, Mob, MobPattern, ActionState);
		}
	}

	MobPattern->StartTimestamp = 0;
	MobPattern->SpawnTimestamp = 0;
	MobPattern->EventTriggerMask = 0;
	ArrayRemoveAllElements(MobPattern->ActionStates, true);
	ArrayRemoveAllElements(MobPattern->LinkMobs, true);

	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		TriggerState->IsRunning = false;
	}
}

Void RTMobPatternUpdate(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
) {
	assert(RTMobIsAlive(Mob));

	Int32 Seed = (Int32)PlatformGetTickCount();
	Timestamp CurrentTimestamp = GetTimestampMs();
	for (Int Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
		RTMobActionStateRef ActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, Index);
		if (!ActionState->IsRunning && ActionState->StartTimestamp <= CurrentTimestamp) {
			ActionState->IsRunning = true;
			RTMobPatternStartAction(Runtime, WorldContext, Mob, MobPattern, ActionState);
		}

		if (ActionState->IsRunning && ActionState->EndTimestamp <= CurrentTimestamp) {
			ActionState->IsRunning = false;
			RTMobPatternCancelAction(Runtime, WorldContext, Mob, MobPattern, ActionState);

			if (ActionState->ActionData->Loop > 0) {
				ActionState->ActionData = &ActionState->ActionGroupData->Actions[RandomRange(&Seed, 0, ActionState->ActionGroupData->ActionCount - 1)];
				ActionState->StartTimestamp = CurrentTimestamp + ActionState->ActionData->LoopDelay;
				ActionState->EndTimestamp = ActionState->StartTimestamp + ActionState->ActionData->Duration;
			}
		}

		if (ActionState->IsRunning) {
			break;
		}
	}

	for (Int Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
		RTMobActionStateRef ActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, Index);
		if (!ActionState->IsRunning && ActionState->EndTimestamp <= CurrentTimestamp) {
			ActionState->TriggerState->IsRunning = false;
			ArrayRemoveElementAtIndex(MobPattern->ActionStates, Index);
			Index -= 1;
		}
	}
}

Void RTMobPatternTimeElapsed(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
) {
	UInt32 TriggerMask = (
		1 << RUNTIME_MOB_TRIGGER_TYPE_START_TIME | 
		1 << RUNTIME_MOB_TRIGGER_TYPE_SPAWN_TIME |
		1 << RUNTIME_MOB_TRIGGER_TYPE_INSTANCE_TIME
	);

	if (!(MobPattern->EventTriggerMask & TriggerMask)) return;

	Timestamp StartElapsedTime = GetTimestampMs() - MobPattern->StartTimestamp;
	Timestamp SpawnElapsedTime = GetTimestampMs() - MobPattern->SpawnTimestamp;
	Timestamp InstanceElapsedTime = GetTimestampMs() - WorldContext->DungeonStartTimestamp;

	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;

		if ((MobPattern->StartTimestamp > 0 && TriggerData->Type == RUNTIME_MOB_TRIGGER_TYPE_START_TIME && TriggerData->Parameters.TimeElapsed.Duration <= StartElapsedTime) ||
			(MobPattern->SpawnTimestamp > 0 && TriggerData->Type == RUNTIME_MOB_TRIGGER_TYPE_SPAWN_TIME && TriggerData->Parameters.TimeElapsed.Duration <= SpawnElapsedTime) ||
			(WorldContext->DungeonStartTimestamp > 0 && TriggerData->Type == RUNTIME_MOB_TRIGGER_TYPE_INSTANCE_TIME && TriggerData->Parameters.TimeElapsed.Duration <= InstanceElapsedTime)) {
			RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
		}
	}
}

Void RTMobPatternHpChanged(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Int64 OldHp,
	Int64 NewHp
) {
	if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_HP_THRESHOLD))) return;

	Float32 OldHpPercent = 100.0f / Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] * OldHp;
	Float32 NewHpPercent = 100.0f / Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX] * NewHp;

	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;
		if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_HP_THRESHOLD) continue;
		if (TriggerData->Parameters.HpThreshold.HpPercent > OldHpPercent) continue;
		if (TriggerData->Parameters.HpThreshold.HpPercent < NewHpPercent) continue;

		RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
	}
}

Void RTMobPatternActionStarted(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Int32 ActionIndex
) {
	if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_ACTION_EXECUTED))) return;

	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;
		if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_ACTION_EXECUTED) continue;
		if (TriggerData->Parameters.ActionExecuted.ActionIndex != ActionIndex) continue;
		if (TriggerData->Parameters.ActionExecuted.ActionFinished > 0) continue;

		RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
	}
}

Void RTMobPatternActionFinished(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Int32 ActionIndex
) {
	if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_ACTION_EXECUTED))) return;

	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;
		if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_ACTION_EXECUTED) continue;
		if (TriggerData->Parameters.ActionExecuted.ActionIndex != ActionIndex) continue;
		if (!TriggerData->Parameters.ActionExecuted.ActionFinished) continue;

		RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
	}
}

Void RTMobPatternDistanceChanged(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
) {
	if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_DISTANCE_THRESHOLD_MAX | 1 << RUNTIME_MOB_TRIGGER_TYPE_DISTANCE_THRESHOLD_MIN))) return;

	Timestamp CurrentTimestamp = GetTimestampMs();
	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;

		Bool InDistance = false;
		if (TriggerData->Type == RUNTIME_MOB_TRIGGER_TYPE_DISTANCE_THRESHOLD_MAX) {
			InDistance = TriggerData->Parameters.DistanceThreshold.Distance > Mob->AggroTargetDistance;
		}
		else if (TriggerData->Type == RUNTIME_MOB_TRIGGER_TYPE_DISTANCE_THRESHOLD_MIN) {
			InDistance = TriggerData->Parameters.DistanceThreshold.Distance <= Mob->AggroTargetDistance;
		}
		else continue;

		if (!InDistance) {
			TriggerState->Timestamp = CurrentTimestamp;
			continue;
		}

		Timestamp ElapsedTime = CurrentTimestamp - TriggerState->Timestamp;
		if (TriggerData->Parameters.DistanceThreshold.Duration > ElapsedTime) continue;

		RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
	}
}

Void RTMobPatternMobSpawned(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobRef SpawnedMob
) {
	Bool IsLinkMob = RTEntityIsEqual(Mob->ID, SpawnedMob->LinkEntityID);
	if (IsLinkMob) {
		if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_SPAWNED))) return;

		for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
			RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
			RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
			if (TriggerState->IsRunning) continue;
			if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_SPAWNED) continue;
			if (TriggerData->Parameters.LinkMobSpawned.LinkMobIndex != Mob->LinkMobIndex) continue;

			RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
		}
	}
	else {
		if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_MOB_SPAWNED))) return;

		for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
			RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
			RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
			if (TriggerState->IsRunning) continue;
			if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_MOB_SPAWNED) continue;
			if (TriggerData->Parameters.MobSpawned.WorldIndex && TriggerData->Parameters.MobSpawned.WorldIndex != Mob->ID.WorldIndex) continue;
			if (TriggerData->Parameters.MobSpawned.DungeonIndex && TriggerData->Parameters.MobSpawned.DungeonIndex != WorldContext->DungeonIndex) continue;
			if (TriggerData->Parameters.MobSpawned.EntityIndex != Mob->ID.EntityIndex) continue;

			RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
		}
	}
}

Void RTMobPatternMobDespawned(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobRef DespawnedMob
) {
	Bool IsLinkMob = RTEntityIsEqual(Mob->ID, DespawnedMob->LinkEntityID);
	if (IsLinkMob) {
		if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_DESPAWNED))) return;

		for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
			RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
			RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
			if (TriggerState->IsRunning) continue;
			if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_DESPAWNED) continue;
			if (TriggerData->Parameters.LinkMobSpawned.LinkMobIndex != DespawnedMob->LinkMobIndex) continue;

			RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
		}
	}
	else {
		if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_MOB_DESPAWNED))) return;

		for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
			RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
			RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
			if (TriggerState->IsRunning) continue;
			if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_MOB_DESPAWNED) continue;
			if (TriggerData->Parameters.MobSpawned.WorldIndex && TriggerData->Parameters.MobSpawned.WorldIndex != DespawnedMob->ID.WorldIndex) continue;
			if (TriggerData->Parameters.MobSpawned.DungeonIndex && TriggerData->Parameters.MobSpawned.DungeonIndex != WorldContext->DungeonIndex) continue;
			if (TriggerData->Parameters.MobSpawned.EntityIndex != DespawnedMob->ID.EntityIndex) continue;

			RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
		}
	}
}

Void RTMobPatternSkillReceived(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Int32 SkillIndex
) {
	if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_SKILL_RECEIVED))) return;

	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;
		if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_SKILL_RECEIVED) continue;
		if (TriggerData->Parameters.SkillReceived.SkillIndex != SkillIndex) continue;

		RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
	}
}

Void RTMobPatternStateChanged(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	Bool IsIdle
) {
	if (!IsIdle && MobPattern->StartTimestamp < 1) {
		MobPattern->StartTimestamp = GetTimestampMs();
	}

	if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_TIME_IDLE))) return;

	Timestamp CurrentTimestamp = GetTimestampMs();
	for (Int TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;
		if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_TIME_IDLE) continue;

		if (!IsIdle) {
			TriggerState->Timestamp = CurrentTimestamp;
			continue;
		}

		Timestamp ElapsedTime = CurrentTimestamp - TriggerState->Timestamp;
		if (TriggerData->Parameters.DistanceThreshold.Duration > ElapsedTime) continue;
		
		RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
	}
}
