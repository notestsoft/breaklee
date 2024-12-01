#include "Runtime.h"
#include "Dungeon.h"
#include "Mob.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "PartyManager.h"
#include "World.h"
#include "WorldManager.h"
Bool RTDungeonIsPatternPartCompleted(
    RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
    RTDungeonDataRef DungeonData
) {
    if (WorldContext->PatternPartIndex < 0) return true;

    // TODO: Check mission items

    for (Int Index = 0; Index < WorldContext->MissionMobCount; Index += 1) {
        if (WorldContext->MissionMobs[Index].Count < WorldContext->MissionMobs[Index].MaxCount) {
            return false;
        }
    }

    return true;
}

Bool RTDungeonStartNextPatternPart(
    RTWorldContextRef WorldContext
) {
    RTRuntimeRef Runtime = WorldContext->WorldManager->Runtime;
    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);

    if (!RTDungeonIsPatternPartCompleted(Runtime, WorldContext, DungeonData)) return false;

    if (WorldContext->PatternPartIndex >= 0) {
        RTPartyRef Party = RTRuntimeGetParty(Runtime, WorldContext->Party);
        assert(Party);

        NOTIFICATION_DATA_DUNGEON_PATTERN_PART_COMPLETED* Notification = RTNotificationInit(DUNGEON_PATTERN_PART_COMPLETED);
        Notification->PatternPartIndex = WorldContext->PatternPartIndex;
        RTNotificationDispatchToParty(Notification, Party);
    }

    Int32 PatternPartIndex = WorldContext->PatternPartIndex + 1;
    if (PatternPartIndex >= DungeonData->PatternPartCount) {
        return true;
    }

    RTMissionDungeonPatternPartDataRef PatternPartData = RTRuntimeGetPatternPartByID(
        Runtime,
        DungeonData->PatternPartIndices[PatternPartIndex]
    );

    RTWorldSetMobTable(Runtime, WorldContext, PatternPartData->MobTable);

    WorldContext->MissionItemCount = 0;
    memset(WorldContext->MissionItems, 0, sizeof(struct _RTQuestUnitItemData) * RUNTIME_MAX_QUEST_COUNTER_COUNT);

    WorldContext->MissionMobCount = PatternPartData->MissionMobCount;
    memcpy(WorldContext->MissionMobs, PatternPartData->MissionMobs, sizeof(struct _RTQuestUnitMobData) * RUNTIME_MAX_QUEST_COUNTER_COUNT);

    for (Int Index = 0; Index < WorldContext->MissionMobCount; Index++) {
        WorldContext->MissionMobs[Index].Count = 0;
    }
    
    WorldContext->PatternPartIndex += 1;

    return true;
}

Bool RTDungeonStart(
	RTWorldContextRef WorldContext
) {
    RTRuntimeRef Runtime = WorldContext->WorldManager->Runtime;

    if (WorldContext->Active) return true;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);
    assert(DungeonData);

    WorldContext->PatternPartIndex = -1;
    WorldContext->DungeonStartTimestamp = GetTimestampMs();
    WorldContext->DungeonTimeout = GetTimestampMs() + DungeonData->MissionTimeout * 1000;
    WorldContext->TimerIndex = -1;
    WorldContext->TimerTimeout = UINT64_MAX;

    RTDungeonStartNextPatternPart(WorldContext);

    // TODO: Generate maze 

    WorldContext->Active = true;

    if (DungeonData->TimerData.Active && DungeonData->TimerData.ItemID > 0) {
        RTPartyRef Party = RTRuntimeGetParty(Runtime, WorldContext->Party);
        assert(Party);

        NOTIFICATION_DATA_DUNGEON_TIMER_INFO* Notification = RTNotificationInit(DUNGEON_TIMER_INFO);
        Notification->Type = DUNGEON_TIMER_INFO_TYPE_DROP;
        Notification->Drop.ItemID = DungeonData->TimerData.ItemID;
        Notification->Drop.ItemCount = WorldContext->TimerItemCount;
        RTNotificationDispatchToParty(Notification, Party);
    }

    // RTDungeonStartNextTimer(WorldContext);

    for (Int Index = 0; Index < DungeonData->StartKillMobCount; Index += 1) {
        RTEntityID MobID = { 0 };
        MobID.EntityIndex = DungeonData->StartKillMobList[Index];
        MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
        MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

        RTMobRef Mob = RTWorldContextGetMob(WorldContext, MobID);
        assert(Mob);

        RTWorldDespawnMobEvent(Runtime, WorldContext, Mob, 1);
    }

	return true;
}

Bool RTDungeonUpdate(
    RTWorldContextRef World
) {
    RTRuntimeRef Runtime = World->WorldManager->Runtime;
    Timestamp Timestamp = GetTimestampMs();
    if (World->DungeonTimeout <= Timestamp) {
        return RTDungeonFail(World);
    }

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
    assert(DungeonData);

    if (DungeonData->TimerData.Active && World->TimerTimeout < Timestamp) {
        if (World->TimerIndex + 1 < DungeonData->TimerData.TimerCount) {
            RTTimerDataRef TimerData = &DungeonData->TimerData.Timers[World->TimerIndex + 1];

            RTEntityID MobID = { 0 };
            MobID.EntityIndex = TimerData->TargetMobIndex;
            MobID.WorldIndex = World->WorldData->WorldIndex;
            MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

            RTMobRef Mob = RTWorldContextGetMob(World, MobID);
            assert(Mob);

            Bool IsAlive = RTMobIsAlive(Mob);
            if (!IsAlive && TimerData->TargetEvent == RUNTIME_DUNGEON_TIMER_TARGET_EVENT_TYPE_SPAWN) {
                RTWorldSpawnMob(Runtime, World, Mob);
            }

            if (IsAlive && TimerData->TargetEvent == RUNTIME_DUNGEON_TIMER_TARGET_EVENT_TYPE_DESPAWN) {
                RTWorldDespawnMob(Runtime, World, Mob);
            }
        }
        else {
            World->TimerTimeout = UINT64_MAX;
        }
    }

    return true;
}

Bool RTDungeonPause(
    RTWorldContextRef World
) {
    RTRuntimeRef Runtime = World->WorldManager->Runtime;
    RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, World->Party);
    if (!Party) return false;
    if (Party->MemberCount > 1) return false;
    if (World->Paused) return false;

    World->Paused = true;
    World->PauseTimestamp = GetTimestampMs();

    return true;
}

Bool RTDungeonResume(
    RTWorldContextRef World
) {
    RTRuntimeRef Runtime = World->WorldManager->Runtime;
    RTPartyRef Party = RTPartyManagerGetParty(Runtime->PartyManager, World->Party);
    if (!Party) return false;
    if (Party->MemberCount > 1) return false;
    if (!World->Paused) return false;

    Timestamp ElapsedTime = GetTimestampMs() - World->PauseTimestamp;
    World->DungeonTimeout += ElapsedTime;
    World->NextItemUpdateTimestamp += ElapsedTime;
    World->TimerTimeout += ElapsedTime;
    World->Paused = false;

    return true;
}

Bool RTDungeonEnd(
    RTWorldContextRef World
) {
    RTRuntimeRef Runtime = World->WorldManager->Runtime;

    if (!World->Active || World->Closed) return true;

    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
    assert(DungeonData);

    if (!RTDungeonIsPatternPartCompleted(Runtime, World, DungeonData)) return false;

    Int32 PatternPartIndex = World->PatternPartIndex + 1;
    if (PatternPartIndex >= DungeonData->PatternPartCount) {
        World->Active = false;
        World->Closed = true;
        World->Cleared = true;

        RTPartyRef Party = RTRuntimeGetParty(Runtime, World->Party);
        if (Party) {
            for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
                RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Party->Members[Index].CharacterIndex);
                if (!Character) continue;
                if (Character->Data.Info.WorldIndex != World->WorldData->WorldIndex) continue;
                if (Character->Data.Info.DungeonIndex != DungeonData->DungeonIndex) continue;

                RTCharacterDungeonQuestFlagClear(Character, DungeonData->DungeonIndex);
            }
        }

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

Bool RTWorldContextCheckMobState(
    RTWorldContextRef WorldContext,
    Int32 MobIndex,
    Bool IsAlive
) {
    RTEntityID MobID = { 0 };
    MobID.EntityIndex = MobIndex;
    MobID.WorldIndex = WorldContext->WorldData->WorldIndex;
    MobID.EntityType = RUNTIME_ENTITY_TYPE_MOB;

    RTMobRef Mob = RTWorldContextGetMob(WorldContext, MobID);
    if (!Mob) return false;

    if (IsAlive != RTMobIsAlive(Mob)) return false;
    if (!IsAlive && !Mob->IsDead) return false;

    return true;
}

Bool RTWorldContextCheckMobListState(
    RTWorldContextRef WorldContext,
    Int32 MobIndexCount,
    Int32* MobIndexList,
    Bool IsAlive
) {
    for (Int Index = 0; Index < MobIndexCount; Index += 1) {
        if (!RTWorldContextCheckMobState(WorldContext, MobIndexList[Index], IsAlive)) return false;
    }

    return true;
}

Bool RTDungeonTriggerEvent(
    RTWorldContextRef World,
    RTEntityID TriggerSource,
    Int EventTriggerIndex
) {
    assert(
        World->WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON ||
        World->WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON
    );
    
    RTRuntimeRef Runtime = World->WorldManager->Runtime;
    Bool Triggered = false;
    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
    // TODO: Assign a dungeon data pointer to the WorldContext so it doesn't have to be looked up on runtime
    assert(DungeonData);

    ArrayRef TriggerGroup = (ArrayRef)DictionaryLookup(DungeonData->TriggerGroups, &EventTriggerIndex);
    assert(TriggerGroup);

    for (Int TriggerIndex = 0; TriggerIndex < ArrayGetElementCount(TriggerGroup); TriggerIndex += 1) {
        RTDungeonTriggerDataRef TriggerData = (RTDungeonTriggerDataRef)ArrayGetElementAtIndex(TriggerGroup, TriggerIndex);
        
        Trace("TriggerEvent(Index=%d)", EventTriggerIndex);
        for (Int Offset = 0; Offset < TriggerData->LiveMobCount; Offset++)
            Trace("CheckLiveMob(%d, %d)", TriggerData->LiveMobIndexList[Offset], RTWorldContextCheckMobState(World, TriggerData->LiveMobIndexList[Offset], true));

        for (Int Offset = 0; Offset < TriggerData->DeadMobCount; Offset++)
            Trace("CheckDeadMob(%d, %d)", TriggerData->DeadMobIndexList[Offset], RTWorldContextCheckMobState(World, TriggerData->DeadMobIndexList[Offset], false));

        if (TriggerData->Type > 0 && !RTWorldContextCheckMobListState(World, TriggerData->LiveMobCount, TriggerData->LiveMobIndexList, true)) continue;
        if (TriggerData->Type > 1 && !RTWorldContextCheckMobListState(World, TriggerData->DeadMobCount, TriggerData->DeadMobIndexList, false)) continue;
        // TODO: Evaluate TriggerData->NpcIndex

        Int ActionGroupIndex = TriggerData->ActionGroupIndex;
        ArrayRef ActionGroup = (ArrayRef)DictionaryLookup(DungeonData->ActionGroups, &ActionGroupIndex);
        assert(ActionGroup);

        for (Int ActionIndex = 0; ActionIndex < ArrayGetElementCount(ActionGroup); ActionIndex += 1) {
            RTDungeonTriggerActionDataRef ActionData = (RTDungeonTriggerActionDataRef)ArrayGetElementAtIndex(ActionGroup, ActionIndex);

            if (ActionData->TargetAction == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_EVENT_CALL) {
                RTDungeonTriggerEvent(World, TriggerSource, ActionData->TargetMobIndex);
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

            if (ActionData->TargetAction == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_SOFT_DELETE) {
                Mob->IsPermanentDeath = true;
                Triggered = true;
            }

            if (ActionData->TargetAction == RUNTIME_DUNGEON_TRIGGER_ACTION_TYPE_KILL_BY_LINK_MOB) {
                Mob->EventDespawnLinkID = TriggerSource;
                RTWorldDespawnMobEvent(Runtime, World, Mob, ActionData->Delay);
                Triggered = true;
            }

            if (!Triggered) {
                Warn("Unknown trigger type (%d) given!", ActionData->TargetAction);
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

    NOTIFICATION_DATA_DUNGEON_TIME_CONTROL* Notification = RTNotificationInit(DUNGEON_TIME_CONTROL);
    Notification->TimeAddition = Value;
    RTNotificationDispatchToParty(Notification, Party);
}

Void RTDungeonUpdateTimer(
    RTWorldContextRef WorldContext,
    Int32 TargetEvent,
    Int32 TargetMobIndex
) {
    RTRuntimeRef Runtime = WorldContext->WorldManager->Runtime;
    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);
    assert(DungeonData);

    if (!DungeonData->TimerData.Active) return;

    if (WorldContext->TimerIndex + 1 < DungeonData->TimerData.TimerCount) {
        RTTimerDataRef TimerData = &DungeonData->TimerData.Timers[WorldContext->TimerIndex + 1];
        if (TimerData->TargetEvent == TargetEvent && TimerData->TargetMobIndex == TargetMobIndex) {
            WorldContext->TimerIndex += 1;

            if (TimerData->Interval > 0) {
                WorldContext->TimerTimeout = GetTimestampMs() + TimerData->Interval;
            }
            else {
                WorldContext->TimerTimeout = UINT64_MAX;
            }

            RTPartyRef Party = RTRuntimeGetParty(Runtime, WorldContext->Party);
            assert(Party);

            NOTIFICATION_DATA_DUNGEON_TIMER* Notification = RTNotificationInit(DUNGEON_TIMER);
            Notification->TimerType = TimerData->Type;
            Notification->Interval = TimerData->Interval;
            RTNotificationDispatchToParty(Notification, Party);
        }
    }
}

Void RTDungeonUpdateTimerMobHP(
    RTWorldContextRef WorldContext,
    RTMobRef Mob
) {
    RTRuntimeRef Runtime = WorldContext->WorldManager->Runtime;
    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);
    assert(DungeonData);

    assert(Mob->ID.WorldIndex == DungeonData->WorldIndex);

    if (!DungeonData->TimerData.Active) return;

    RTPartyRef Party = RTRuntimeGetParty(Runtime, WorldContext->Party);
    assert(Party);

    NOTIFICATION_DATA_DUNGEON_TIMER_INFO* Notification = RTNotificationInit(DUNGEON_TIMER_INFO);
    Notification->Type = DUNGEON_TIMER_INFO_TYPE_MOB;
    Notification->Mob.EntityID = Mob->ID.EntityIndex;
    Notification->Mob.CurrentHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    Notification->Mob.MaxHP = Mob->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    RTNotificationDispatchToParty(Notification, Party);
}

Void RTDungeonUpdateTimerItemCount(
    RTWorldContextRef WorldContext,
    RTItem ItemID,
    UInt64 ItemOptions
) {
    RTRuntimeRef Runtime = WorldContext->WorldManager->Runtime;
    RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, WorldContext->DungeonIndex);
    assert(DungeonData);

    if (!DungeonData->TimerData.Active) return;
    if (DungeonData->TimerData.ItemID != (ItemID.ID & RUNTIME_ITEM_MASK_INDEX)) return;

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemID.ID);
    assert(ItemData);

    UInt64 ItemStackSizeMask = RTItemDataGetStackSizeMask(ItemData);
    Int64 ItemStackSize = ItemOptions & ItemStackSizeMask;
    WorldContext->TimerItemCount += ItemStackSize;

    RTPartyRef Party = RTRuntimeGetParty(Runtime, WorldContext->Party);
    assert(Party);

    NOTIFICATION_DATA_DUNGEON_TIMER_INFO* Notification = RTNotificationInit(DUNGEON_TIMER_INFO);
    Notification->Type = DUNGEON_TIMER_INFO_TYPE_DROP;
    Notification->Drop.ItemID = DungeonData->TimerData.ItemID;
    Notification->Drop.ItemCount = WorldContext->TimerItemCount;
    RTNotificationDispatchToParty(Notification, Party);
}
