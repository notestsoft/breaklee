#include "Mob.h"
#include "MobPatrol.h"
#include "Movement.h"
#include "Script.h"
#include "Runtime.h"
#include "WorldManager.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

RTMobPatrolBranchDataRef RTMobPatrolGetBranchData(
	RTMobPatrolDataRef MobPatrol,
	Int32 BranchIndex
) {
	for (Int Index = 0; Index < ArrayGetElementCount(MobPatrol->Branches); Index += 1) {
		RTMobPatrolBranchDataRef BranchData = (RTMobPatrolBranchDataRef)ArrayGetElementAtIndex(MobPatrol->Branches, Index);
		if (BranchData->Index == BranchIndex) return BranchData;
	}

	return NULL;
}

Void RTMobPatrolInit(
	RTRuntimeRef Runtime,
	RTMobRef Mob
) {
	memset(&Mob->Patrol, 0, sizeof(struct _RTMobPatrol));
	if (!Mob->Spawn.MobPatrolIndex) return;

	Int MemoryPoolIndex = Mob->Spawn.MobPatrolIndex;
	Mob->Patrol.Data = (RTMobPatrolDataRef)MemoryPoolFetch(Runtime->MobPatrolDataPool, MemoryPoolIndex);
	Mob->Patrol.WaypointIndex = 0;

	if (ArrayGetElementCount(Mob->Patrol.Data->Branches) > 0) {
		RTMobPatrolBranchDataRef BranchData = (RTMobPatrolBranchDataRef)ArrayGetElementAtIndex(Mob->Patrol.Data->Branches, 0);
		Mob->Patrol.BranchIndex = BranchData->Index;
	}
}

Bool RTMobPatrolUpdate(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob
) {
	if (!Mob->Patrol.Data) return false;

	Timestamp CurrentTimestamp = GetTimestampMs();
	if (Mob->Movement.IsMoving) {
		Mob->Patrol.MovementTimestamp = CurrentTimestamp;
		return true;
	}

	RTMobPatrolBranchDataRef BranchData = RTMobPatrolGetBranchData(Mob->Patrol.Data, Mob->Patrol.BranchIndex);
	if (!BranchData) return false;
	if (Mob->Patrol.MovementTimestamp + Mob->Patrol.WaypointDelay > CurrentTimestamp) return true;
	
	if (Mob->Patrol.WaypointIndex >= 0 && Mob->Patrol.WaypointIndex < ArrayGetElementCount(BranchData->Waypoints)) {
		RTMobPatrolWaypointDataRef WaypointData = (RTMobPatrolWaypointDataRef)ArrayGetElementAtIndex(BranchData->Waypoints, Mob->Patrol.WaypointIndex);
		
		if (WaypointData->Type == RUNTIME_MOB_PATROL_TYPE_WALK) {
			Mob->Movement.PositionEnd.X = WaypointData->X;
			Mob->Movement.PositionEnd.Y = WaypointData->Y;
			Mob->IsChasing = false;

			Bool PathFound = RTMovementFindPath(Runtime, WorldContext, &Mob->Movement);
			if (!PathFound && Mob->Movement.WaypointCount == 2) {
				Mob->Movement.PositionEnd.X = Mob->Movement.Waypoints[1].X;
				Mob->Movement.PositionEnd.Y = Mob->Movement.Waypoints[1].Y;
			}
			
			if (PathFound || (!PathFound && Mob->Movement.WaypointCount == 2)) {
				RTMovementStartDeadReckoning(Runtime, &Mob->Movement);

				NOTIFICATION_DATA_MOB_MOVE_BEGIN* Notification = RTNotificationInit(MOB_MOVE_BEGIN);
				Notification->Entity = Mob->ID;
				Notification->TickCount = Mob->Movement.TickCount;
				Notification->PositionBeginX = Mob->Movement.PositionBegin.X;
				Notification->PositionBeginY = Mob->Movement.PositionBegin.Y;
				Notification->PositionEndX = Mob->Movement.PositionEnd.X;
				Notification->PositionEndY = Mob->Movement.PositionEnd.Y;
				RTNotificationDispatchToNearby(Notification, Mob->Movement.WorldChunk);
			}
		}

		if (WaypointData->Type == RUNTIME_MOB_PATROL_TYPE_WARP) {
			RTWorldChunkRemove(Mob->Movement.WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
			RTWorldTileDecreaseMobCount(Runtime, WorldContext, Mob->Movement.PositionTile.X, Mob->Movement.PositionTile.Y);
			RTMovementInitialize(
				Runtime,
				&Mob->Movement,
				Mob->ID,
				WaypointData->X,
				WaypointData->Y,
				(Int32)Mob->Attributes.Values[RUNTIME_ATTRIBUTE_MOVEMENT_SPEED],
				RUNTIME_WORLD_TILE_WALL | RUNTIME_WORLD_TILE_TOWN
			);
			RTWorldTileIncreaseMobCount(Runtime, WorldContext, Mob->Movement.PositionTile.X, Mob->Movement.PositionTile.Y);
			Mob->Movement.WorldContext = WorldContext;
			Mob->Movement.WorldChunk = RTWorldContextGetChunk(WorldContext, Mob->Movement.PositionCurrent.X, Mob->Movement.PositionCurrent.Y);
			RTWorldChunkInsert(Mob->Movement.WorldChunk, Mob->ID, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
		}

		Mob->Patrol.WaypointDelay = WaypointData->Delay;
		Mob->Patrol.MovementTimestamp = CurrentTimestamp;
		Mob->Patrol.WaypointIndex += 1;
		Mob->RemainingFindCount = Mob->SpeciesData->FindCount;

		if (Mob->Patrol.WaypointIndex >= ArrayGetElementCount(BranchData->Waypoints) && BranchData->LinkCount > 0) {
			Int32 Seed = (Int32)PlatformGetTickCount();
			Mob->Patrol.BranchIndex = BranchData->LinkList[RandomRange(&Seed, 0, BranchData->LinkCount - 1)];
			Mob->Patrol.WaypointIndex = 0;
		}

		return true;
	}
	
	return false;
}
