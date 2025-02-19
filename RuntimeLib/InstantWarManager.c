#include "Runtime.h"
#include "InstantWarManager.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"
#include "WorldManager.h"
#include <Shared/Enumerations.h>

RTInstantWarManagerRef RTInstantWarManagerCreate(
    RTRuntimeRef Runtime,
    UInt64 WorldType,
    UInt64 EntryValue
) {
    RTInstantWarManagerRef InstantWarManager = AllocatorAllocate(Runtime->Allocator, sizeof(struct _RTInstantWarManager));
    if (!InstantWarManager) Fatal("Memory allocation failed");

    InstantWarManager->Allocator = Runtime->Allocator;
    InstantWarManager->Runtime = Runtime;
    InstantWarManager->WorldType = WorldType;
    InstantWarManager->EntryValue = EntryValue;
    InstantWarManager->WarMapRef = RTRuntimeDataWarMapGet(Runtime->Context, WorldType >> 40);
    
    return InstantWarManager;
}

Void RTInstantWarSetPosition(
    RTRuntimeRef Runtime,
    RTInstantWarManagerRef InstantWarManager,
    RTCharacterRef Character
) {
    Character->Data.Info.WorldIndex = InstantWarManager->WarMapRef->LobbyWorldId;
    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, World->WorldData->DeadWarpIndex);
    Character->Data.Info.PositionX = WarpPoint.X;
    Character->Data.Info.PositionY = WarpPoint.Y;
    Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
    Character->Data.Info.DungeonIndex = 0;
    Character->SyncMask.Info = true;
}

Void RTInstantWarManagerDestroy(
    RTInstantWarManagerRef InstantWarManager
) {
    // TODO: Check if all running PvPs have to be cleaned up!!!
    AllocatorDeallocate(InstantWarManager->Allocator, InstantWarManager);
}

Void RTInstantWarManagerUpdate(
    RTInstantWarManagerRef InstantWarManager,
    UInt64 WorldType
) {

    RTRuntimeRef Runtime = InstantWarManager->Runtime;
    
    return 0;

error:
    return 1;

}

