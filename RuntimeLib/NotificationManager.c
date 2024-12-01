#include "Runtime.h"
#include "NotificationManager.h"
#include "WorldManager.h"

struct _RTNotificationCommandContext {
    RTNotificationCallback Callback;
    Void* UserData;
};
typedef struct _RTNotificationCommandContext* RTNotificationCommandContextRef;

struct _RTNotificationManager {
    AllocatorRef Allocator;
    RTRuntimeRef Runtime;
    DictionaryRef CommandRegistry;
};

static UInt8 kSharedNotificationBuffer[RUNTIME_MAX_NOTIFICATION_BUFFER_LENGTH] = { 0 };
static RTNotificationRef kSharedNotification = (RTNotificationRef)kSharedNotificationBuffer;

RTNotificationManagerRef RTNotificationManagerCreate(
    RTRuntimeRef Runtime
) {
    RTNotificationManagerRef NotificationManager = AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTNotificationManager));
    NotificationManager->Allocator = Runtime->Allocator;
    NotificationManager->Runtime = Runtime;
    NotificationManager->CommandRegistry = IndexDictionaryCreate(Runtime->Allocator, 64);
    return NotificationManager;
}

Void RTNotificationManagerDestroy(
    RTNotificationManagerRef NotificationManager
) {
    DictionaryDestroy(NotificationManager->CommandRegistry);
    AllocatorDeallocate(NotificationManager->Allocator, NotificationManager);
}

Void RTNotificationManagerRegisterCallback(
    RTNotificationManagerRef NotificationManager,
    Int32 Command,
    RTNotificationCallback Callback,
    Void* UserData
) {
    Int Key = Command;
    assert(!DictionaryLookup(NotificationManager->CommandRegistry, &Key));
    
    struct _RTNotificationCommandContext Context = { 0 };
    Context.Callback = Callback;
    Context.UserData = UserData;
    DictionaryInsert(NotificationManager->CommandRegistry, &Key, &Context, sizeof(struct _RTNotificationCommandContext));
}

Void RTNotificationManagerDispatchToCharacter(
    RTNotificationManagerRef NotificationManager,
    Void* Notification,
    RTCharacterRef Character
) {
    assert(Character);
    Int Key = ((RTNotificationRef)Notification)->Command;
    RTNotificationCommandContextRef Context = (RTNotificationCommandContextRef)DictionaryLookup(NotificationManager->CommandRegistry, &Key);
    if (Context) Context->Callback(
        NotificationManager->Runtime,
        Character,
        (RTNotificationRef)Notification,
        Context->UserData
    );
}

Void RTNotificationManagerDispatchToParty(
    RTNotificationManagerRef NotificationManager,
    Void* Notification,
    RTPartyRef Party
) {
    RTRuntimeRef Runtime = NotificationManager->Runtime;
    for (Int Index = 0; Index < Party->MemberCount; Index += 1) {
        RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Runtime->WorldManager, Party->Members[Index].CharacterIndex);
        if (!Character) continue;

        RTNotificationManagerDispatchToCharacter(NotificationManager, Notification, Character);
    }
}

Void RTNotificationManagerDispatchToChunk(
    RTNotificationManagerRef NotificationManager,
    Void* Notification,
    RTWorldChunkRef WorldChunk
) {
    for (Int Index = 0; Index < ArrayGetElementCount(WorldChunk->Characters); Index += 1) {
        RTEntityID Entity = *(RTEntityID*)ArrayGetElementAtIndex(WorldChunk->Characters, Index);
        RTCharacterRef Character = RTWorldManagerGetCharacter(WorldChunk->Runtime->WorldManager, Entity);
        if (!Character) continue;
        RTNotificationManagerDispatchToCharacter(NotificationManager, Notification, Character);
    }
}

Void RTNotificationManagerDispatchToNearby(
    RTNotificationManagerRef NotificationManager,
    Void* Notification,
    RTWorldChunkRef WorldChunk
) {
    RTRuntimeRef Runtime = WorldChunk->WorldContext->WorldManager->Runtime;
    if (!RTEntityIsNull(WorldChunk->WorldContext->Party)) {
        RTPartyRef Party = RTRuntimeGetParty(Runtime, WorldChunk->WorldContext->Party);
        RTNotificationManagerDispatchToParty(NotificationManager, Notification, Party);
        return;
    }

    Int32 StartChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 StartChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY - RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkX = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkX + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));
    Int32 EndChunkY = MAX(0, MIN(RUNTIME_WORLD_CHUNK_COUNT - 1, WorldChunk->ChunkY + RUNTIME_WORLD_CHUNK_VISIBLE_RADIUS));

    for (Int DeltaChunkX = StartChunkX; DeltaChunkX <= EndChunkX; DeltaChunkX += 1) {
        for (Int DeltaChunkY = StartChunkY; DeltaChunkY <= EndChunkY; DeltaChunkY += 1) {
            Int WorldChunkIndex = DeltaChunkX + DeltaChunkY * RUNTIME_WORLD_CHUNK_COUNT;
            assert(WorldChunkIndex < RUNTIME_WORLD_CHUNK_COUNT * RUNTIME_WORLD_CHUNK_COUNT);
            
            RTWorldChunkRef NearbyWorldChunk = &WorldChunk->WorldContext->Chunks[WorldChunkIndex];
            RTNotificationManagerDispatchToChunk(NotificationManager, Notification, NearbyWorldChunk);
        }
    }
}

RTNotificationRef _RTNotificationInit(
    Int32 Length,
    Int32 Command
) {
    assert(Length <= RUNTIME_MAX_NOTIFICATION_BUFFER_LENGTH);
    memset(kSharedNotification, 0, Length);
    kSharedNotification->Length = Length;
    kSharedNotification->Command = Command;
    return kSharedNotification;
}

Void* RTNotificationAppend(
    Void* Notification,
    Int Length
) {
    assert(((RTNotificationRef)Notification)->Length + Length <= RUNTIME_MAX_NOTIFICATION_BUFFER_LENGTH);
    Void* Memory = ((UInt8*)Notification) + ((RTNotificationRef)Notification)->Length;
    memset(Memory, 0, Length);
    ((RTNotificationRef)Notification)->Length += Length;
    return Memory;
}

Void* RTNotificationAppendCopy(
    Void* Notification,
    Void* Source,
    Int Length
) {
    Void* Memory = RTNotificationAppend(Notification, Length);
    memcpy(Memory, Source, Length);
    return Memory;
}

CString RTNotificationAppendCString(
    Void* Notification,
    CString Value
) {
    return (CString)RTNotificationAppendCopy(Notification, Value, strlen(Value) + 1);
}
