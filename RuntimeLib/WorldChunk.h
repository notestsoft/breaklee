#pragma once

#include "Base.h"
#include "Entity.h"

EXTERN_C_BEGIN

struct _RTWorldChunk {
	Index ReferenceCount;
	ArrayRef Characters;
	ArrayRef Mobs;
	ArrayRef Items;
};

typedef Void* RTWorldChunkIteratorRef;

Void RTWorldChunkInitialize(
	AllocatorRef Allocator,
	RTWorldChunkRef Chunk
);

Void RTWorldChunkDeinitialize(
	RTWorldChunkRef Chunk
);

Void RTWorldChunkInsert(
	RTWorldChunkRef Chunk,
	RTEntityID Entity
);

Void RTWorldChunkRemove(
	RTWorldChunkRef Chunk,
	RTEntityID Entity
);

EXTERN_C_END
