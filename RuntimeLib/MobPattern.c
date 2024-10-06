#include "Mob.h"
#include "MobPattern.h"
#include "World.h"

Void RTMobPatternInsertActionState(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern,
	RTMobTriggerDataRef TriggerData,
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

	// TODO: Perform Runtime Action
	RTMobPatternActionExecuted(Runtime, WorldContext, Mob, MobPattern, ActionState->ActionGroupData->Index);
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

	// TODO: Perform Runtime Cancellation
}

Void RTMobPatternStart(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
) {
	if (MobPattern->IsRunning) return;
	
	MobPattern->IsRunning = true;
	MobPattern->IsIdle = true;
	MobPattern->StartTimestamp = GetTimestampMs();
	MobPattern->EventTriggerMask = 0;
	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef MobTrigger = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		TriggerState->IsRunning = false;
		TriggerState->Timestamp = MobPattern->StartTimestamp;
		MobPattern->EventTriggerMask |= (1 << MobTrigger->Type);
	}
}

Void RTMobPatternStop(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
) {

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
			else {
				ArrayRemoveElementAtIndex(MobPattern->ActionStates, Index);
				Index -= 1;
				continue;
			}
		}

		if (ActionState->IsRunning) {
			break;
		}
	}
}

Void RTMobPatternTimeElapsed(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob,
	RTMobPatternRef MobPattern
) {
	if (!(MobPattern->EventTriggerMask & (1 << RUNTIME_MOB_TRIGGER_TYPE_TIME_ELAPSED))) return;

	Timestamp ElapsedTime = GetTimestampMs() - MobPattern->StartTimestamp;

	for (Int32 TriggerIndex = 0; TriggerIndex < MobPattern->Data->TriggerCount; TriggerIndex += 1) {
		RTMobTriggerDataRef TriggerData = &MobPattern->Data->Triggers[TriggerIndex];
		RTMobTriggerStateRef TriggerState = &MobPattern->TriggerStates[TriggerIndex];
		if (TriggerState->IsRunning) continue;
		if (TriggerData->Type != RUNTIME_MOB_TRIGGER_TYPE_TIME_ELAPSED) continue;
		if (TriggerData->Parameters.TimeElapsed.Duration >= ElapsedTime) continue;

		RTMobTriggerEnqueue(Runtime, WorldContext, Mob, MobPattern, TriggerData, TriggerState);
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

Void RTMobPatternActionExecuted(
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
		// TODO: Check second parameter!

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
