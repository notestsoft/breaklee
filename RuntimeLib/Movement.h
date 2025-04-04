#pragma once

#include "Base.h"
#include "Constants.h"
#include "Entity.h"

EXTERN_C_BEGIN

struct _RTMovement {
    RTWorldContextRef WorldContext;
    RTWorldChunkRef WorldChunk;
    RTEntityID Entity;
    UInt32 TickCount;
    UInt32 Flags;
    UInt32 CollisionMask;
    UInt32 IgnoreMask;
    Bool IsMoving;
    Bool IsDeadReckoning;
    Int32 WaypointIndex;
    Int32 WaypointCount;
    RTPosition Waypoints[RUNTIME_MOVEMENT_MAX_WAYPOINT_COUNT]; // TODO: Add support for dynamic waypoint speeds
    RTPosition PositionBegin;
    RTPosition PositionCurrent;
    RTPosition PositionEnd;
    RTPosition PositionTile;
    Float32 Base;
    Float32 Distance;
    Float32 Speed;
    Float32 Sin;
    Float32 Cos;
};

Void RTMovementInitialize(
    RTRuntimeRef Runtime,
    RTMovementRef Movement,
    RTEntityID Entity,
    Int32 X,
    Int32 Y,
    Int32 Speed,
    UInt32 CollisionMask
);

Void RTMovementStartDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
);

Void RTMovementRestartDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
);

Void RTMovementEndDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
);

Void RTMovementUpdateDeadReckoning(
    RTRuntimeRef Runtime,
    RTMovementRef Movement
);

Void RTMovementSetSpeed(
    RTRuntimeRef Runtime,
    RTMovementRef Movement,
    Int32 Speed
);

Void RTMovementSetPosition(
    RTRuntimeRef Runtime,
    RTMovementRef Movement,
    Int32 X,
    Int32 Y
);

Int32 RTMovementDistance(
    RTMovementRef Lhs,
    RTMovementRef Rhs
);

Bool RTMovementIsInRange(
    RTRuntimeRef Runtime,
    RTMovementRef Movement,
    Int32 X,
    Int32 Y
);

Bool RTMovementFindPath(
    RTRuntimeRef Runtime,
    RTWorldContextRef World,
    RTMovementRef Movement
);

EXTERN_C_END
