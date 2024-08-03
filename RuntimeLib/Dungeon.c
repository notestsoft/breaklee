#include "Runtime.h"
#include "Dungeon.h"
#include "World.h"
#include "WorldManager.h"
#include "Mob.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Bool RTDungeonIsPatternPartCompleted(
    RTWorldContextRef World
) {
    if (World->PatternPartIndex == -1) return true;

    // TODO: Check mission items

    for (Int32 Index = 0; Index < World->MissionMobCount; Index++) {
        if (World->MissionMobs[Index].Count < World->MissionMobs[Index].MaxCount) {
            return false;
        }
    }

    return true;
}

Bool RTDungeonStartNextPatternPart(
    RTWorldContextRef World
) {
    RTRuntimeRef Runtime = World->WorldManager->Runtime;
    
    if (!RTDungeonIsPatternPartCompleted(World)) return false;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);

    if (World->PatternPartIndex >= 0) {
        RTPartyRef Party = RTRuntimeGetParty(Runtime, World->Party);
        assert(Party);

        NOTIFICATION_DATA_DUNGEON_PATTERN_PART_COMPLETED* Notification = RTNotificationInit(DUNGEON_PATTERN_PART_COMPLETED);
        Notification->PatternPartIndex = World->PatternPartIndex;
        RTNotificationDispatchToParty(Notification, Party);
    }

    Int32 PatternPartIndex = World->PatternPartIndex + 1;
    if (PatternPartIndex >= DungeonData->PatternPartCount) {
        return true;
    }

    RTMissionDungeonPatternPartDataRef PatternPartData = RTRuntimeGetPatternPartByID(
        Runtime,
        DungeonData->PatternPartIndices[PatternPartIndex]
    );

    RTWorldSetMobTable(Runtime, World, PatternPartData->MobTable);

    World->MissionItemCount = 0;
    memset(World->MissionItems, 0, sizeof(struct _RTQuestUnitItemData) * RUNTIME_MAX_QUEST_COUNTER_COUNT);

    World->MissionMobCount = PatternPartData->MissionMobCount;
    memcpy(World->MissionMobs, PatternPartData->MissionMobs, sizeof(struct _RTQuestUnitMobData) * RUNTIME_MAX_QUEST_COUNTER_COUNT);

    for (Int32 Index = 0; Index < World->MissionMobCount; Index++) {
        World->MissionMobs[Index].Count = 0;
    }
    
    World->PatternPartIndex += 1;

    return true;
}

Bool RTDungeonStart(
	RTWorldContextRef World
) {
    RTRuntimeRef Runtime = World->WorldManager->Runtime;

    if (World->Active) return true;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
    assert(DungeonData);

    World->PatternPartIndex = -1;
    World->DungeonTimeout = GetTimestamp() + DungeonData->MissionTimeout;
    RTDungeonStartNextPatternPart(World);

    // TODO: Generate maze 

    World->Active = true;
	return true;
}

Bool RTDungeonUpdate(
    RTWorldContextRef World
) {
    Timestamp Timestamp = GetTimestamp();
    if (World->DungeonTimeout <= Timestamp) {
        return RTDungeonFail(World);
    }

    return true;
}

Bool RTDungeonEnd(
    RTWorldContextRef World
) {
    RTRuntimeRef Runtime = World->WorldManager->Runtime;

    if (!World->Active || World->Closed) return true;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
    assert(DungeonData);

    if (!RTDungeonIsPatternPartCompleted(World)) return false;

    Int32 PatternPartIndex = World->PatternPartIndex + 1;
    if (PatternPartIndex >= DungeonData->PatternPartCount) {
        World->Active = false;
        World->Closed = true;
        World->Cleared = true;
        return true;
    }

    return false;
}

Bool RTDungeonFail(
    RTWorldContextRef World
) {
    if (!World->Active || World->Closed) return true;

    World->Active = false;
    World->Closed = true;

    // TODO: Add dungeon fail related notifications
    // TODO: Apply dungeon fail penalty

    return true;
}

Bool RTWorldContextCheckMobListState(
    RTWorldContextRef WorldContext,
    Int32 MobIndexCount,
    Int32* MobIndexList,
    Bool IsAlive
) {
    for (Int32 Index = 0; Index < MobIndexCount; Index += 1) {
        RTEntityID MobID = { 0 };
        MobID.EntityIndex = MobIndexList[Index];
        MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
        MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        RTMobRef Mob = RTWorldContextGetMob(WorldContext, MobID);
        if (!Mob || RTMobIsAlive(Mob) != IsAlive) return false;
    }

    return true;
}

Bool RTDungeonTriggerEvent(
    RTWorldContextRef World,
    Index TriggerIndex
) {
    assert(
        World->WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON ||
        World->WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON
    );
    
    RTRuntimeRef Runtime = World->WorldManager->Runtime;
    Bool Triggered = false;
    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
    assert(DungeonData);

    ArrayRef TriggerGroup = (ArrayRef)DictionaryLookup(DungeonData->TriggerGroups, &TriggerIndex);
    assert(TriggerGroup);

    for (Int32 TriggerIndex = 0; TriggerIndex < ArrayGetElementCount(TriggerGroup); TriggerIndex += 1) {
        RTDungeonTriggerDataRef TriggerData = (RTDungeonTriggerDataRef)ArrayGetElementAtIndex(TriggerGroup, TriggerIndex);
        if (!RTWorldContextCheckMobListState(World, TriggerData->LiveMobCount, TriggerData->LiveMobIndexList, true)) continue;
        if (!RTWorldContextCheckMobListState(World, TriggerData->DeadMobCount, TriggerData->DeadMobIndexList, false)) continue;

        // TODO: Check how TriggerData->Type is supposed to work
        // TODO: Evaluate TriggerData->NpcIndex

        Index ActionGroupIndex = TriggerData->ActionGroupIndex;
        ArrayRef ActionGroup = (ArrayRef)DictionaryLookup(DungeonData->ActionGroups, &ActionGroupIndex);
        assert(ActionGroup);

        for (Int32 ActionIndex = 0; ActionIndex < ArrayGetElementCount(ActionGroup); ActionIndex += 1) {
            RTDungeonTriggerActionDataRef ActionData = (RTDungeonTriggerActionDataRef)ArrayGetElementAtIndex(ActionGroup, ActionIndex);

            if (ActionData->TargetAction == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_EVENT_CALL) {
                RTDungeonTriggerEvent(World, ActionData->TargetMobIndex);
                Triggered = true;
                continue;
            }

            RTEntityID MobID = { 0 };
            MobID.EntityIndex = ActionData->TargetMobIndex;
            MobID.WorldIndex = World->WorldData->WorldIndex;
            MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

            RTMobRef Mob = RTWorldContextGetMob(World, MobID);
            assert(Mob);

            if (ActionData->TargetAction == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_SPAWN) {
                if (Mob->IsPermanentDeath) continue;
                if (RTMobIsAlive(Mob)) continue;

                RTWorldSpawnMobEvent(Runtime, World, Mob, ActionData->Delay);
                Triggered = true;
            }

            if (ActionData->TargetAction == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_KILL) {
                if (!RTMobIsAlive(Mob)) continue;

                RTWorldDespawnMobEvent(Runtime, World, Mob, ActionData->Delay);
                Triggered = true;
            }

            if (ActionData->TargetAction == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_REVIVE) {
                if (RTMobIsAlive(Mob)) continue;

                RTWorldSpawnMobEvent(Runtime, World, Mob, ActionData->Delay);
                Triggered = true;
            }

            if (ActionData->TargetAction == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_DELETE) {
                Mob->IsPermanentDeath = true;
                RTWorldDespawnMobEvent(Runtime, World, Mob, ActionData->Delay);
                Triggered = true;
            }
        }
    }

    return Triggered;
}

Void RTDungeonAddTime(
    RTWorldContextRef WorldContext,
    Int32 Value
) {
    WorldContext->DungeonTimeout += Value;

    RTRuntimeRef Runtime = WorldContext->WorldManager->Runtime;
    RTPartyRef Party = RTRuntimeGetParty(Runtime, WorldContext->Party);
    assert(Party);

    NOTIFICATION_DATA_NFY_DUNGEON_TIMER* Notification = RTNotificationInit(NFY_DUNGEON_TIMER);
    Notification->TimeAddition = Value;
    RTNotificationDispatchToParty(Notification, Party);
}
