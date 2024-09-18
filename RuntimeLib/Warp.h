#pragma once

#include "Base.h"
#include "Constants.h"
#include "Entity.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
	RUNTIME_WARP_TYPE_GATE = 0,
	RUNTIME_WARP_TYPE_QUEST_DUNGEON,
	RUNTIME_WARP_TYPE_DUNGEON,
};

struct _RTWarp {
	Int32 Index;
	Int32 Type;
	Int32 WorldID;
	Int32 NpcID;
	Int32 TargetID;
	Int32 Level;
	Int32 Fee;
	UInt32 WarpItemID;
	UInt32 WarpItemOption;
	Int32 QuestID;
};

typedef struct {
	UInt16 X;
	UInt16 Y;
	Int32 Fee;
	Int32 WorldIndex;
	Int32 Level;
} RTWarpPointResult;

#pragma pack(pop)

Bool RTCharacterConsumeReturnStone(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 SlotIndex
);

RTWarpPointResult RTRuntimeGetWarpPoint(
	RTRuntimeRef Runtime,
	RTCharacterRef Character,
	Int32 WarpIndex
);

Bool RTRuntimeWarpCharacter(
	RTRuntimeRef Runtime,
	RTEntityID Entity,
	Int32 NpcID,
	Int32 WarpPositionX,
	Int32 WarpPositionY,
	Int32 WarpWorldID,
	Int32 WarpIndex,
	Int32 SlotIndex
);

EXTERN_C_END
