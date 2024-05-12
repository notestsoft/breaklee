#pragma once

#include "Base.h"
#include "Entity.h"

EXTERN_C_BEGIN

enum {
    RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE,
    RUNTIME_WORLD_CHUNK_UPDATE_REASON_INIT,
    RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP,
    RUNTIME_WORLD_CHUNK_UPDATE_REASON_MOVE,
};

struct _RTWorldChunk {
    RTRuntimeRef Runtime;
    Index WorldIndex;
    Index WorldInstanceIndex;
	Index ReferenceCount;
    Int32 ChunkX;
    Int32 ChunkY;
	ArrayRef Characters;
	ArrayRef Mobs;
	ArrayRef Items;
};

typedef Void* RTWorldChunkIteratorRef;

Void RTWorldChunkInitialize(
	RTRuntimeRef Runtime,
	RTWorldChunkRef Chunk,
    Index WorldIndex,
    Index WorldInstanceIndex,
    Int32 ChunkX,
    Int32 ChunkY
);

Void RTWorldChunkDeinitialize(
	RTWorldChunkRef Chunk
);

Void RTWorldChunkInsert(
	RTWorldChunkRef Chunk,
	RTEntityID Entity,
    Int32 Reason
);

Void RTWorldChunkMove(
    RTWorldChunkRef NewChunk,
    RTMovementRef Movement
);

Void RTWorldChunkRemove(
	RTWorldChunkRef Chunk,
	RTEntityID Entity,
    Int32 Reason
);

Void RTWorldChunkNotify(
    RTWorldChunkRef Chunk,
    RTEntityID Entity,
    Int32 Reason,
    Bool IsInsertion
);

EXTERN_C_END
