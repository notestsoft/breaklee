#include "WorldChunk.h"

Void RTWorldChunkInitialize(
	AllocatorRef Allocator,
	RTWorldChunkRef Chunk
) {
	Chunk->ReferenceCount = 0;
	Chunk->Characters = ArrayCreateEmpty(Allocator, sizeof(RTEntityID), 8);
	Chunk->Mobs = ArrayCreateEmpty(Allocator, sizeof(RTEntityID), 8);
	Chunk->Items = ArrayCreateEmpty(Allocator, sizeof(RTEntityID), 8);
}

Void RTWorldChunkDeinitialize(
	RTWorldChunkRef Chunk
) {
	ArrayDestroy(Chunk->Characters);
	ArrayDestroy(Chunk->Mobs);
	ArrayDestroy(Chunk->Items);
}

ArrayRef RTWorldChunkGetContainer(
	RTWorldChunkRef Chunk,
	RTEntityID Entity
) {
	switch (Entity.EntityType) {
	case RUNTIME_ENTITY_TYPE_CHARACTER:
		return Chunk->Characters;

	case RUNTIME_ENTITY_TYPE_MOB:
		return Chunk->Mobs;

	case RUNTIME_ENTITY_TYPE_ITEM:
		return Chunk->Items;

	default:
		UNREACHABLE("Invalid entity type given for world chunk!");
	}
}

Void RTWorldChunkInsert(
	RTWorldChunkRef Chunk,
	RTEntityID Entity
) {
	ArrayRef Container = RTWorldChunkGetContainer(Chunk, Entity);
	assert(!ArrayContainsElement(Container, &Entity));
	ArrayAppendElement(Container, &Entity);
}

Void RTWorldChunkRemove(
	RTWorldChunkRef Chunk,
	RTEntityID Entity
) {
	ArrayRef Container = RTWorldChunkGetContainer(Chunk, Entity);
	assert(ArrayContainsElement(Container, &Entity));
	ArrayRemoveElement(Container, &Entity);
}
