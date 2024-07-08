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
    RTWorldContextRef WorldContext;
    Index WorldIndex;
    Index WorldInstanceIndex;
	Index ReferenceCount;
    Int32 ChunkX;
    Int32 ChunkY;
    ArrayRef Characters;
	ArrayRef Mobs;
	ArrayRef Items;
    Timestamp NextItemUpdateTimestamp;
};

typedef Void* RTWorldChunkIteratorRef;

Void RTWorldChunkInitialize(
	RTRuntimeRef Runtime,
    RTWorldContextRef WorldContext,
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

Void RTWorldChunkUpdate(
	RTWorldChunkRef Chunk,
	RTEntityID Entity
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
