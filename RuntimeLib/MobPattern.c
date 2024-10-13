#include "Mob.h"
#include "MobPattern.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "World.h"

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
	Trace("EnqueueAction(%d, %d, %d) on Mob(%d:%llu:%llu)",
		TriggerData->Index,
		ActionGroupData->Index,
		ActionData->Index,
		Mob->ID.EntityIndex,
		WorldContext->DungeonIndex,
		WorldContext->WorldData->WorldIndex
	);

	Int32 StateCount = ArrayGetElementCount(MobPattern->ActionStates);
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
	Timestamp CurrentTimestamp = GetTimestampMs();
	Int32 Seed = (Int32)PlatformGetTickCount();
	for (Int32 Index = 0; Index < TriggerData->ActionGroupCount; Index += 1) {
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
	Trace("StartAction(%d, %d, %d) on Mob(%d:%llu:%llu)",
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
			Notification->TargetIndex = Target->CharacterIndex;
			Notification->PositionX = ActionState->ActionData->Parameters.WarpTarget.PositionX;
			Notification->PositionY = ActionState->ActionData->Parameters.WarpTarget.PositionY;
			RTNotificationDispatchToNearby(Notification, Target->Movement.WorldChunk);
		}
		else if (TargetID.EntityType == RUNTIME_ENTITY_TYPE_MOB) {
			RTMobRef Target = (RTMobRef)TargetContext;
			RTWorldChunkRemove(Target->Movement.WorldChunk, TargetID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
			RTMovementInitialize(
				Runtime,
				&Target->Movement,
				ActionState->ActionData->Parameters.WarpTarget.PositionX,
				ActionState->ActionData->Parameters.WarpTarget.PositionY,
				Target->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED],
				RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN
			);
			RTWorldChunkRef WorldChunk = RTWorldContextGetChunk(WorldContext, Target->Movement.PositionCurrent.X, Target->Movement.PositionCurrent.Y);
			RTWorldChunkInsert(WorldChunk, TargetID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
		}
		else {
			UNREACHABLE("Unexpected target type given for action WARP_TARGET!");
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_WARP_SELF) {
		RTWorldChunkRemove(Mob->Movement.WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
		RTMovementInitialize(
			Runtime,
			&Mob->Movement,
			ActionState->ActionData->Parameters.WarpTarget.PositionX,
			ActionState->ActionData->Parameters.WarpTarget.PositionY,
			Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED],
			RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN
		);
		Mob->Movement.WorldChunk = RTWorldContextGetChunk(WorldContext, Mob->Movement.PositionCurrent.X, Mob->Movement.PositionCurrent.Y);
		RTWorldChunkInsert(Mob->Movement.WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SPAWN_MOB) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_HEAL_TARGET) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_1 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_2 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_EVASION_SELF_3) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SPECIAL_ACTION) {
		RTMobStartSpecialAction(Runtime, WorldContext, Mob, ActionState->ActionData->Parameters.SpecialAction.ActionIndex);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_1 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_2) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_CANCEL_ACTION && !ActionState->ActionData->Parameters.CancelAction.ActionFinished) {
		Timestamp CurrentTimestamp = GetTimestampMs();
		for (Int32 Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
			RTMobActionStateRef ActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, Index);
			if (ActionState->ActionData->Index != ActionState->ActionData->Parameters.CancelAction.ActionIndex) continue;

			ActionState->EndTimestamp = CurrentTimestamp;
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_DESPAWN_MOB) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_RESPAWN_SELF) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SOCIAL_ACTION) {

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
	Trace("CancelAction(%d, %d, %d) on Mob(%d:%llu:%llu)",
		ActionState->TriggerData->Index,
		ActionState->ActionGroupData->Index,
		ActionState->ActionData->Index,
		Mob->ID.EntityIndex,
		WorldContext->DungeonIndex,
		WorldContext->WorldData->WorldIndex
	);

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_WARP_TARGET) {
		
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_WARP_SELF) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SPAWN_MOB) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_HEAL_TARGET) {
		if (ActionState->ActionData->Parameters.Heal.WorldType == 0 && WorldContext->WorldData->Type != RUNTIME_WORLD_TYPE_GLOBAL) goto end;

		RTEntityID TargetID = {
			.EntityIndex = ActionState->ActionData->Parameters.Heal.MobIndex,
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

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SPECIAL_ACTION) {
		RTMobCancelSpecialAction(Runtime, WorldContext, Mob);
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_1 ||
		ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_ATTACK_2) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_CANCEL_ACTION && ActionState->ActionData->Parameters.CancelAction.ActionFinished) {
		Timestamp CurrentTimestamp = GetTimestampMs();
		for (Int32 Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
			RTMobActionStateRef ActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, Index);
			if (ActionState->ActionData->Index != ActionState->ActionData->Parameters.CancelAction.ActionIndex) continue;

			ActionState->EndTimestamp = CurrentTimestamp;
		}
	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_DESPAWN_MOB) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_RESPAWN_SELF) {

	}

	if (ActionState->ActionData->Type == RUNTIME_MOB_ACTION_TYPE_SOCIAL_ACTION) {

	}

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
	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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
	MobPattern->StartTimestamp = 0;
	MobPattern->SpawnTimestamp = 0;
	MobPattern->EventTriggerMask = 0;
	ArrayRemoveAllElements(MobPattern->ActionStates, true);
	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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

	Int32 Seed = PlatformGetTickCount();
	Timestamp CurrentTimestamp = GetTimestampMs();
	for (Int32 Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
		RTMobActionStateRef ActionState = (RTMobActionStateRef)ArrayGetElementAtIndex(MobPattern->ActionStates, Index);
		if (!ActionState->IsRunning && ActionState->StartTimestamp <= CurrentTimestamp) {
			ActionState->IsRunning = true;
			RTMobPatternStartAction(Runtime, WorldContext, Mob, MobPattern, ActionState);
		}

		if (ActionState->IsRunning && ActionState->EndTimestamp <= CurrentTimestamp) {
			ActionState->IsRunning = false;
			RTMobPatternCancelAction(Runtime, WorldContext, Mob, MobPattern, ActionState);

			if (ActionState->ActionData->Loop) {
				ActionState->ActionData = &ActionState->ActionGroupData->Actions[RandomRange(&Seed, 0, ActionState->ActionGroupData->ActionCount - 1)];
				ActionState->StartTimestamp = CurrentTimestamp + ActionState->ActionData->LoopDelay;
				ActionState->EndTimestamp = ActionState->StartTimestamp + ActionState->ActionData->Duration;
			}
		}

		if (ActionState->IsRunning) {
			break;
		}
	}

	for (Int32 Index = 0; Index < ArrayGetElementCount(MobPattern->ActionStates); Index += 1) {
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
	if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_START_TIME | 1 << RUNTIME_MOB_TRIGGER_TYPE_SPAWN_TIME))) return;

	Timestamp StartElapsedTime = GetTimestampMs() - MobPattern->StartTimestamp;
	Timestamp SpawnElapsedTime = GetTimestampMs() - MobPattern->SpawnTimestamp;

	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;

		if ((MobPattern->StartTimestamp > 0 && TriggerData->Type == RUNTIME_MOB_TRIGGER_TYPE_START_TIME && TriggerData->Parameters.TimeElapsed.Duration <= StartElapsedTime) ||
			(MobPattern->SpawnTimestamp > 0 && TriggerData->Type == RUNTIME_MOB_TRIGGER_TYPE_SPAWN_TIME && TriggerData->Parameters.TimeElapsed.Duration <= SpawnElapsedTime)) {
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

	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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

	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;
		if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_ACTION_EXECUTED) continue;
		if (TriggerData->Parameters.ActionExecuted.ActionIndex != ActionIndex) continue;
		if (TriggerData->Parameters.ActionExecuted.ActionFinished) continue;

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

	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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
	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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

		for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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

		for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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

		for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
			RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
			RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
			if (TriggerState->IsRunning) continue;
			if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_LINK_MOB_DESPAWNED) continue;
			if (TriggerData->Parameters.LinkMobSpawned.LinkMobIndex != Mob->LinkMobIndex) continue;

			RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
		}
	}
	else {
		if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_MOB_DESPAWNED))) return;

		for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
			RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
			RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
			if (TriggerState->IsRunning) continue;
			if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_MOB_DESPAWNED) continue;
			if (TriggerData->Parameters.MobSpawned.WorldIndex && TriggerData->Parameters.MobSpawned.WorldIndex != Mob->ID.WorldIndex) continue;
			if (TriggerData->Parameters.MobSpawned.DungeonIndex && TriggerData->Parameters.MobSpawned.DungeonIndex != WorldContext->DungeonIndex) continue;
			if (TriggerData->Parameters.MobSpawned.EntityIndex != Mob->ID.EntityIndex) continue;

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

	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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
	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
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
