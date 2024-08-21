#include "Inventory.h"
#include "Runtime.h"
#include "Warp.h"
#include "WorldManager.h"

RTWarpPointResult RTRuntimeGetWarpPoint(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 WarpIndex
) {
    assert(0 <= WarpIndex - 1 && WarpIndex - 1 < Runtime->Context->WarpPointCount);
    assert(Character->Data.StyleInfo.Nation <= 3);

    RTDataWarpPointRef WarpPoint = &Runtime->Context->WarpPointList[WarpIndex - 1];
    RTPosition Positions[] = {
        { WarpPoint->X, WarpPoint->Y },
        { WarpPoint->Nation1X, WarpPoint->Nation1Y },
        { WarpPoint->Nation2X, WarpPoint->Nation2Y },
        { WarpPoint->Nation3X, WarpPoint->Nation3Y },
    };
    RTWarpPointResult Result = { 0 };
    Result.X = Positions[Character->Data.StyleInfo.Nation].X;
    Result.Y = Positions[Character->Data.StyleInfo.Nation].Y;
    Result.Fee = WarpPoint->Fee;
    Result.WorldIndex = WarpPoint->WorldIndex;
    Result.Level = WarpPoint->Level;
    return Result;
}

Bool RTRuntimeWarpCharacter(
    RTRuntimeRef Runtime,
    RTEntityID Entity,
    Int32 NpcID,
    Int32 WarpPositionX,
    Int32 WarpPositionY,
    Int32 WarpWorldID,
    Int32 WarpIndex,
    Int32 SlotIndex
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_CHARACTER);

    RTCharacterRef Character = RTWorldManagerGetCharacter(Runtime->WorldManager, Entity);
    assert(Character);

    if (RTCharacterIsUnmovable(Runtime, Character)) return false;
    if ((Character->Movement.IsMoving) ||
        (Character->Movement.IsDeadReckoning)) {
        RTMovementEndDeadReckoning(Runtime, &Character->Movement);
    }

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    if (!World) return false;

    if ((RUNTIME_NPC_ID_RESERVED_BEGIN <= NpcID && NpcID <= RUNTIME_NPC_ID_RESERVED_END) ||
        (RUNTIME_NPC_ID_RESERVED_BEGIN2 <= NpcID && NpcID <= RUNTIME_NPC_ID_RESERVED_END2)) {
        switch (NpcID) {
        case RUNTIME_NPC_ID_WAR_0: return false;
        case RUNTIME_NPC_ID_WAR_1: return false;
        case RUNTIME_NPC_ID_WAR_WARP: return false;
        case RUNTIME_NPC_ID_WAR_BATTLEFIELD: return false;
        case RUNTIME_NPC_ID_WAR_LOBBY: return false;
        case RUNTIME_NPC_ID_DEAD: {
            RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, World->WorldData->DeadWarpIndex);
            RTWorldContextRef TargetWorld = World;
            if (World->WorldData->WorldIndex != WarpPoint.WorldIndex) {
                TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
                assert(TargetWorld);
            }

            RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

            RTCharacterSetHP(Runtime, Character, Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX], false);
            RTCharacterSetMP(Runtime, Character, (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX], false);
            Character->Data.Info.PositionX = WarpPoint.X;
            Character->Data.Info.PositionY = WarpPoint.Y;
            Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
            Character->Data.Info.DungeonIndex = TargetWorld->DungeonIndex;
            Character->SyncMask.Info = true;
            RTCharacterInitializeAttributes(Runtime, Character);

            RTMovementInitialize(
                Runtime,
                &Character->Movement,
                WarpPoint.X,
                WarpPoint.Y,
                RUNTIME_MOVEMENT_SPEED_BASE,
                RUNTIME_WORLD_TILE_WALL
            );

            RTWorldSpawnCharacterWithoutNotification(Runtime, TargetWorld, Entity);

            return true;
        }
        case RUNTIME_NPC_ID_RETURN: {
            // TODO: Check if character is in dungeon or war and return err

            RTItemSlotRef Slot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SlotIndex);
            if (!Slot) return false;

            RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID);
            if (!ItemData) return false;
            // TODO: Check return stone cooldown time!!!

            if (ItemData->ItemType == RUNTIME_ITEM_TYPE_RETURN_STONE) {
                if (Slot->ItemOptions > 0) {
                    Slot->ItemOptions -= 1;

                    if (Slot->ItemOptions < 1) {
                        RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SlotIndex);
                    }
                }
                else {
                    RTInventoryClearSlot(Runtime, &Character->Data.InventoryInfo, SlotIndex);
                }
            }
            else if (ItemData->ItemType == RUNTIME_ITEM_TYPE_RETURN_CORE) {
                // TODO: Check if item is expired
            }
            else {
                return false;
            }

            RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, World->WorldData->ReturnWarpIndex);
            RTWorldContextRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
            assert(NewWorld);

            RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

            Character->Data.Info.PositionX = WarpPoint.X;
            Character->Data.Info.PositionY = WarpPoint.Y;
            Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
            Character->Data.Info.DungeonIndex = NewWorld->DungeonIndex;
            Character->SyncMask.Info = true;
        
            RTMovementInitialize(
                Runtime,
                &Character->Movement,
                WarpPoint.X,
                WarpPoint.Y,
                RUNTIME_MOVEMENT_SPEED_BASE,
                RUNTIME_WORLD_TILE_WALL
            );

            RTWorldSpawnCharacterWithoutNotification(Runtime, NewWorld, Entity);

            return true;
        }

        case RUNTIME_NPC_ID_DUNGEON_WARP: {
            Int32 DungeonIndex = WarpPositionX << 8 | WarpPositionY;
            RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, DungeonIndex);
            if (!DungeonData) return false;

            if (Character->Data.Info.Level < DungeonData->EntryConditionLevel) return false;

            RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, DungeonData->FailWarpNpcID);
            RTWorldContextRef TargetWorld = World;
            if (World->WorldData->WorldIndex != WarpPoint.WorldIndex) {
                TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
                assert(TargetWorld);
            }

            if (RTWorldIsTileColliding(Runtime, TargetWorld, WarpPoint.X, WarpPoint.Y, Character->Movement.CollisionMask)) return false;

            RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

            Character->Data.Info.PositionX = WarpPoint.X;
            Character->Data.Info.PositionY = WarpPoint.Y;
            Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
            Character->Data.Info.DungeonIndex = TargetWorld->DungeonIndex;
            Character->SyncMask.Info = true;

            RTMovementInitialize(
                Runtime,
                &Character->Movement,
                WarpPoint.X,
                WarpPoint.Y,
                RUNTIME_MOVEMENT_SPEED_BASE,
                RUNTIME_WORLD_TILE_WALL
            );

            RTWorldSpawnCharacterWithoutNotification(Runtime, TargetWorld, Entity);

            return true;
        }

        case RUNTIME_NPC_ID_NAVIGATION: {
            Int32 WarpNpcID = WarpPositionX << 8 | WarpPositionY;
            RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, WarpNpcID);
            RTWorldContextRef TargetWorld = World;
            if (World->WorldData->WorldIndex != WarpPoint.WorldIndex) {
                TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
                assert(TargetWorld);
            }

            if (RTWorldIsTileColliding(Runtime, TargetWorld, WarpPoint.X, WarpPoint.Y, Character->Movement.CollisionMask)) return false;

            RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

            Character->Data.Info.PositionX = WarpPoint.X;
            Character->Data.Info.PositionY = WarpPoint.Y;
            Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
            Character->Data.Info.DungeonIndex = TargetWorld->DungeonIndex;
            Character->SyncMask.Info = true;

            RTMovementInitialize(
                Runtime,
                &Character->Movement,
                WarpPoint.X,
                WarpPoint.Y,
                RUNTIME_MOVEMENT_SPEED_BASE,
                RUNTIME_WORLD_TILE_WALL
            );

            RTWorldSpawnCharacterWithoutNotification(Runtime, TargetWorld, Entity);

            return true;
        }

        case RUNTIME_NPC_ID_UNKNOWN_1: return false;
        case RUNTIME_NPC_ID_UNKNOWN_2: return false;
        case RUNTIME_NPC_ID_QUEST_DUNGEON_WARP: return false;
        case RUNTIME_NPC_ID_QUEST_DUNGEON_EXIT: {
            if (World->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON) return false;

            RTDungeonDataRef QuestDungeonData = RTRuntimeGetDungeonDataByID(Runtime, World->DungeonIndex);
            if (!QuestDungeonData) return false;

            Int32 WarpNpcID = QuestDungeonData->FailWarpNpcID;
            if (World->Cleared) {
                WarpNpcID = QuestDungeonData->SuccessWarpNpcID;
            }
            else if (!RTCharacterIsAlive(Runtime, Character)) {
                WarpNpcID = QuestDungeonData->DeadWarpID;

                RTCharacterSetHP(Runtime, Character, Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX], false);
                RTCharacterSetMP(Runtime, Character, (Int32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX], false);
                RTCharacterInitializeAttributes(Runtime, Character);
            }

            RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, WarpNpcID);
            RTWorldContextRef TargetWorld = World;
            if (World->WorldData->WorldIndex != WarpPoint.WorldIndex) {
                TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
                assert(TargetWorld);
            }

            // TODO: Delete Dungeon Instance!
            RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

            if (!World->Cleared && World->ReferenceCount < 1) {
                RTDungeonFail(World);
            }

            Character->Data.Info.PositionX = WarpPoint.X;
            Character->Data.Info.PositionY = WarpPoint.Y;
            Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
            Character->Data.Info.DungeonIndex = TargetWorld->DungeonIndex;
            Character->SyncMask.Info = true;
        
            RTMovementInitialize(
                Runtime,
                &Character->Movement,
                WarpPoint.X,
                WarpPoint.Y,
                RUNTIME_MOVEMENT_SPEED_BASE,
                RUNTIME_WORLD_TILE_WALL
            );

            if (World->Closed) {
                assert(Character->Data.Info.DungeonIndex != World->DungeonIndex);
                // TODO: This doesn't seam to be sufficient or maybe there is a warp center bug?
                RTRuntimeCloseDungeon(Runtime, Character);
            }

            RTWorldSpawnCharacterWithoutNotification(Runtime, TargetWorld, Entity);

            return true;
        }
        case RUNTIME_NPC_ID_UNKNOWN_3: return false;
        case RUNTIME_NPC_ID_PRISON: return false;
        case RUNTIME_NPC_ID_UNKNOWN_4: return false;
        case RUNTIME_NPC_ID_UNKNOWN_5: return false;
        case RUNTIME_NPC_ID_GM: return false;
        }
    }
    
    if (NpcID == RUNTIME_NPC_ID_GM2) {
        if (Character->Data.StyleInfo.Nation != 3) return false;

        RTWorldContextRef TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpWorldID);
        if (!TargetWorld) return false;

        if (RTWorldIsTileColliding(Runtime, TargetWorld, WarpPositionX, WarpPositionY, Character->Movement.CollisionMask)) return false;

        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

        Character->Data.Info.PositionX = WarpPositionX;
        Character->Data.Info.PositionY = WarpPositionY;
        Character->Data.Info.WorldIndex = WarpWorldID;
        Character->Data.Info.DungeonIndex = 0;
        Character->SyncMask.Info = true;
        
        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            WarpPositionX,
            WarpPositionY,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );

        RTWorldSpawnCharacterWithoutNotification(Runtime, TargetWorld, Entity);

        return true;
    }

    if (NpcID == RUNTIME_NPC_ID_FRONTIER_STONE) {
        RTItemSlotRef Slot = RTInventoryGetSlot(Runtime, &Character->Data.InventoryInfo, SlotIndex);
        if (!Slot) return false;
        if (Slot->ItemOptions < 1) return false;

        RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, Slot->Item.ID);
        if (!ItemData) return false;
        if (ItemData->ItemType != RUNTIME_ITEM_TYPE_FRONTIER_STONE) return false;

        RTItemOptions ItemOptions = { .Serial = Slot->ItemOptions };
        WarpWorldID = ItemOptions.FrontierStone.WorldIndex;
        WarpPositionX = ItemOptions.FrontierStone.X;
        WarpPositionY = ItemOptions.FrontierStone.Y;

        RTWorldContextRef TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpWorldID);
        if (!TargetWorld) return false;
        if (TargetWorld->WorldData->WorldIndex != Character->Data.Info.WorldIndex) return false;

        if (RTWorldIsTileColliding(Runtime, TargetWorld, WarpPositionX, WarpPositionY, Character->Movement.CollisionMask)) return false;

        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

        Character->Data.Info.PositionX = WarpPositionX;
        Character->Data.Info.PositionY = WarpPositionY;
        Character->Data.Info.WorldIndex = WarpWorldID;
        Character->Data.Info.DungeonIndex = 0;
        Character->SyncMask.Info = true;

        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            WarpPositionX,
            WarpPositionY,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );

        RTWorldSpawnCharacterWithoutNotification(Runtime, TargetWorld, Entity);

        return true;
    }

    RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, Character->Data.Info.WorldIndex, NpcID);
    if (!Npc) return false;

    // TODO: Distance check if not working for quest dungeons from npcs...
    //if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) return false;

    RTWarpRef Warp = RTRuntimeGetWarpByWorldNpcID(Runtime, Character->Data.Info.WorldIndex, NpcID, WarpIndex);
    if (!Warp) return false;
    assert(Warp->Index == WarpIndex);

    if (Character->Data.Info.Level < Warp->Level) return false;
    if (Character->Data.Info.Alz< Warp->Fee) return false;

    if (Warp->Type == RUNTIME_WARP_TYPE_GATE) {
        RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, Warp->TargetID);

        RTWorldContextRef TargetWorld = World;
        if (World->WorldData->WorldIndex != WarpPoint.WorldIndex) {
            TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
            assert(TargetWorld);
        }

        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

        Character->Data.Info.Alz-= Warp->Fee;
        Character->Data.Info.PositionX = WarpPoint.X;
        Character->Data.Info.PositionY = WarpPoint.Y;
        Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
        Character->Data.Info.DungeonIndex = TargetWorld->DungeonIndex;
        Character->SyncMask.Info = true;
        
        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            WarpPoint.X,
            WarpPoint.Y,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );

        RTWorldSpawnCharacterWithoutNotification(Runtime, TargetWorld, Entity);

        return true;
    }
    else if (Warp->Type == RUNTIME_WARP_TYPE_QUEST_DUNGEON) {
        // TODO: Check quest dungeon flags of character if is already finished
        // TODO: Check if user is currently in quest dungeon and next quest dungeon index for success is matching

        if (!RTCharacterHasQuestDungeon(Runtime, Character, Warp->TargetID)) return false;

        RTDungeonDataRef QuestDungeonData = RTRuntimeGetDungeonDataByID(Runtime, Warp->TargetID);

        // TODO: Check and remove EntryItem

        if (Character->Data.Info.Level < QuestDungeonData->EntryConditionLevel) return false;

        // TODO: Check if party has other dungeon
        // TODO: Check MaxPlayerCount & Party Size
        // TODO: Check EntryConditionClass

        RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, QuestDungeonData->EntryWarpID);
        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

        RTWorldContextRef DungeonWorld = RTRuntimeOpenDungeon(Runtime, Character, WarpPoint.WorldIndex, QuestDungeonData->DungeonIndex);
        if (!DungeonWorld) return false;

        Character->Data.Info.Alz-= Warp->Fee;
        Character->Data.Info.PositionX = WarpPoint.X;
        Character->Data.Info.PositionY = WarpPoint.Y;
        Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
        Character->Data.Info.DungeonIndex = DungeonWorld->DungeonIndex;
        Character->SyncMask.Info = true;
        
        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            WarpPoint.X,
            WarpPoint.Y,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );

        RTWorldSpawnCharacterWithoutNotification(Runtime, DungeonWorld, Entity);

        /*
        Int32 FailWarpNpcID;
        Int32 DeadWarpID;
        Int32 SuccessWarpNpcID;
        Int32 WarpNpcSetID;
        Int32 DungeonType;
        Int32 MissionTimeout;
        */
        return true;
    }
    else if (Warp->Type == RUNTIME_WARP_TYPE_DUNGEON) {
        // TODO: Check if user is currently in dungeon and next dungeon index for success is matching

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, Warp->TargetID);

        // TODO: Check and remove EntryItem

        if (Character->Data.Info.Level < DungeonData->EntryConditionLevel) return false;

        // TODO: Check if party has other dungeon
        // TODO: Check MaxPlayerCount & Party Size
        // TODO: Check EntryConditionClass

        RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, DungeonData->EntryWarpID);
        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
        
        RTWorldContextRef DungeonWorld = RTRuntimeOpenDungeon(Runtime, Character, WarpPoint.WorldIndex, DungeonData->DungeonIndex);
        if (!DungeonWorld) return false;

        Character->Data.Info.Alz-= Warp->Fee;
        Character->Data.Info.PositionX = WarpPoint.X;
        Character->Data.Info.PositionY = WarpPoint.Y;
        Character->Data.Info.WorldIndex = WarpPoint.WorldIndex;
        Character->Data.Info.DungeonIndex = DungeonWorld->DungeonIndex;
        Character->DungeonEntryItemSlotIndex = SlotIndex;
        Character->SyncMask.Info = true;
        
        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            WarpPoint.X,
            WarpPoint.Y,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );

        RTWorldSpawnCharacterWithoutNotification(Runtime, DungeonWorld, Entity);

        /*
        Int32 FailWarpNpcID;
        Int32 DeadWarpID;
        Int32 SuccessWarpNpcID;
        Int32 WarpNpcSetID;
        Int32 DungeonType;
        Int32 MissionTimeout;
        */
        return true;
    }

    return false;
}
