#pragma once

#include "Base.h"
#include "Entity.h"
#include "Movement.h"

EXTERN_C_BEGIN

enum {
	RUNTIME_MOB_PATROL_TYPE_WALK,
	RUNTIME_MOB_PATROL_TYPE_WARP,
};

typedef struct _RTMobPatrolWaypointData* RTMobPatrolWaypointDataRef;
typedef struct _RTMobPatrolBranchData* RTMobPatrolBranchDataRef;
typedef struct _RTMobPatrolData* RTMobPatrolDataRef;

struct _RTMobPatrolWaypointData {
	Int32 Type;
	Int32 X;
	Int32 Y;
	Int32 Delay;
};

struct _RTMobPatrolBranchData {
	Int32 Index;
	Int32 LinkCount;
	Int32 LinkList[RUNTIME_MOB_PATROL_MAX_LINK_COUNT];
	ArrayRef Waypoints;
};

struct _RTMobPatrolData {
	Int32 Index;
	ArrayRef Branches;
};

struct _RTMobPatrol {
	RTMobPatrolDataRef Data;
	Int32 BranchIndex;
	Int32 WaypointIndex;
	Int32 WaypointDelay;
	Timestamp MovementTimestamp;
};

Void RTMobPatrolInit(
	RTRuntimeRef Runtime,
	RTMobRef Mob
);

Bool RTMobPatrolUpdate(
	RTRuntimeRef Runtime,
	RTWorldContextRef WorldContext,
	RTMobRef Mob
);

EXTERN_C_END
