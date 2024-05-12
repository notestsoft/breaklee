#include "Event.h"
#include "Movement.h"
#include "Runtime.h"
#include "WorldChunk.h"
#include "WorldManager.h"

Void RTWorldChunkInitialize(
	RTRuntimeRef Runtime,
	RTWorldChunkRef Chunk,
    Index WorldIndex,
    Index WorldInstanceIndex,
    Int32 ChunkX,
    Int32 ChunkY
) {
    Chunk->Runtime = Runtime;
    Chunk->WorldIndex = WorldIndex;
    Chunk->WorldInstanceIndex = WorldInstanceIndex;
	Chunk->ReferenceCount = 0;
    Chunk->ChunkX = ChunkX;
    Chunk->ChunkY = ChunkY;
	Chunk->Characters = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
	Chunk->Mobs = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
	Chunk->Items = ArrayCreateEmpty(Runtime->Allocator, sizeof(RTEntityID), 8);
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
	RTEntityID Entity,
    Int32 Reason
) {
	ArrayRef Container = RTWorldChunkGetContainer(Chunk, Entity);
	assert(!ArrayContainsElement(Container, &Entity));
	ArrayAppendElement(Container, &Entity);
    RTWorldChunkNotify(Chunk, Entity, Reason, true);
}

Void RTWorldChunkMove(
    RTWorldChunkRef NewChunk,
    RTMovementRef Movement
) {
    RTWorldChunkRef PreviousChunk = Movement->WorldChunk;
    RTWorldChunkRemove(Movement->WorldChunk, Movement->Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_MOVE);
    RTWorldChunkInsert(NewChunk, Movement->Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_MOVE);
    Movement->WorldChunk = NewChunk;
    
    RTCharacterRef Character = RTWorldManagerGetCharacter(NewChunk->Runtime->WorldManager, Movement->Entity);
    assert(Character);
    
    RTWorldContextRef WorldContext = RTWorldContextGet(NewChunk->Runtime->WorldManager, NewChunk->WorldIndex, NewChunk->WorldInstanceIndex);
    assert(WorldContext);
    
    RTEventData EventData = { 0 };
    EventData.UserList.CharacterIndex = Character->CharacterIndex;
    EventData.UserList.PreviousChunkX = PreviousChunk->ChunkX;
    EventData.UserList.PreviousChunkY = PreviousChunk->ChunkY;
    EventData.UserList.CurrentChunkX = NewChunk->ChunkX;
    EventData.UserList.CurrentChunkY = NewChunk->ChunkY;
    EventData.UserList.Reason = RUNTIME_WORLD_CHUNK_UPDATE_REASON_MOVE;
    EventData.UserList.IsInsertion = true;
    
    RTRuntimeBroadcastEventData(
		NewChunk->Runtime,
		RUNTIME_EVENT_USER_LIST,
		WorldContext,
		kEntityIDNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
        EventData
	);
}

Void RTWorldChunkRemove(
	RTWorldChunkRef Chunk,
	RTEntityID Entity,
    Int32 Reason
) {
	ArrayRef Container = RTWorldChunkGetContainer(Chunk, Entity);
	assert(ArrayContainsElement(Container, &Entity));
	ArrayRemoveElement(Container, &Entity);
    RTWorldChunkNotify(Chunk, Entity, Reason, false);
}

Void RTWorldChunkNotify(
    RTWorldChunkRef Chunk,
    RTEntityID Entity,
    Int32 Reason,
    Bool IsInsertion
) {
    if (Reason == RUNTIME_WORLD_CHUNK_UPDATE_REASON_NONE) return;
    if (Entity.EntityType != RUNTIME_ENTITY_TYPE_CHARACTER) return;

    RTCharacterRef Character = RTWorldManagerGetCharacter(Chunk->Runtime->WorldManager, Entity);
    assert(Character);
    
    RTWorldContextRef WorldContext = RTWorldContextGet(Chunk->Runtime->WorldManager, Chunk->WorldIndex, Chunk->WorldInstanceIndex);
    assert(WorldContext);
    
    RTEventData EventData = { 0 };
    EventData.UserList.CharacterIndex = Character->CharacterIndex;
    EventData.UserList.PreviousChunkX = -1;
    EventData.UserList.PreviousChunkY = -1;
    EventData.UserList.CurrentChunkX = Chunk->ChunkX;
    EventData.UserList.CurrentChunkY = Chunk->ChunkY;
    EventData.UserList.Reason = Reason;
    EventData.UserList.IsInsertion = IsInsertion;
    
    RTRuntimeBroadcastEventData(
		Chunk->Runtime,
		RUNTIME_EVENT_USER_LIST,
		WorldContext,
		kEntityIDNull,
		Character->ID,
		Character->Movement.PositionCurrent.X,
		Character->Movement.PositionCurrent.Y,
        EventData
	);
}
