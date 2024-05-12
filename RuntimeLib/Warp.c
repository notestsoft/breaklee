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
    assert(Character->Info.Profile.Nation <= 3);

    RTDataWarpPointRef WarpPoint = &Runtime->Context->WarpPointList[WarpIndex - 1];
    RTPosition Positions[] = {
        { WarpPoint->X, WarpPoint->Y },
        { WarpPoint->Nation1X, WarpPoint->Nation1Y },
        { WarpPoint->Nation2X, WarpPoint->Nation2Y },
        { WarpPoint->Nation3X, WarpPoint->Nation2Y },
    };
    RTWarpPointResult Result = { 0 };
    Result.X = Positions[Character->Info.Profile.Nation].X;
    Result.Y = Positions[Character->Info.Profile.Nation].Y;
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

    if (RUNTIME_NPC_ID_RESERVED_BEGIN <= NpcID && NpcID <= RUNTIME_NPC_ID_RESERVED_END) {
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
            Character->Info.Position.X = WarpPoint.X;
            Character->Info.Position.Y = WarpPoint.Y;
            Character->Info.Position.WorldID = WarpPoint.WorldIndex;
            Character->Info.Position.DungeonIndex = TargetWorld->DungeonIndex;
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

            RTItemSlotRef Slot = RTInventoryGetSlot(Runtime, &Character->InventoryInfo, SlotIndex);
            if (!Slot) return false;

            // TODO: Check return stone cooldown time!!!

            if (Slot->Item.Serial == RUNTIME_ITEM_SERIAL_WARP_CONSUMABLE) {
                if (Slot->ItemOptions > 0) {
                    Slot->ItemOptions -= 1;

                    if (Slot->ItemOptions < 1) {
                        RTInventoryClearSlot(Runtime, &Character->InventoryInfo, SlotIndex);
                    }
                }
                else {
                    return false;
                }
            }
            else if (Slot->Item.Serial == RUNTIME_ITEM_SERIAL_WARP_PERMANENT) {
                // TODO: Check if item is expired
            }
            else {
                return false;
            }

            RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, World->WorldData->ReturnWarpIndex);
            RTWorldContextRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
            assert(NewWorld);

            RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

            Character->Info.Position.X = WarpPoint.X;
            Character->Info.Position.Y = WarpPoint.Y;
            Character->Info.Position.WorldID = WarpPoint.WorldIndex;
            Character->Info.Position.DungeonIndex = NewWorld->DungeonIndex;

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

        case RUNTIME_NPC_ID_NAVIGATION: return false;
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

            Character->Info.Position.X = WarpPoint.X;
            Character->Info.Position.Y = WarpPoint.Y;
            Character->Info.Position.WorldID = WarpPoint.WorldIndex;
            Character->Info.Position.DungeonIndex = TargetWorld->DungeonIndex;

            RTMovementInitialize(
                Runtime,
                &Character->Movement,
                WarpPoint.X,
                WarpPoint.Y,
                RUNTIME_MOVEMENT_SPEED_BASE,
                RUNTIME_WORLD_TILE_WALL
            );

            if (World->Closed) {
                assert(Character->Info.Position.DungeonIndex != World->DungeonIndex);
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
        // TODO: Enable this code to check for gm nation
        // if (Character->Info.Profile.Nation != 3) return false;

        RTWorldContextRef TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpWorldID);
        if (RTWorldIsTileColliding(Runtime, TargetWorld, WarpPositionX, WarpPositionY, Character->Movement.CollisionMask)) return false;

        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

        Character->Info.Position.X = WarpPositionX;
        Character->Info.Position.Y = WarpPositionY;
        Character->Info.Position.WorldID = WarpWorldID;
        Character->Info.Position.DungeonIndex = 0;

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

    RTNpcRef Npc = RTRuntimeGetNpcByWorldNpcID(Runtime, Character->Info.Position.WorldID, NpcID);
    if (!Npc) return false;

    // TODO: Distance check if not working for quest dungeons from npcs...
    //if (!RTMovementIsInRange(Runtime, &Character->Movement, Npc->X, Npc->Y)) return false;

    RTWarpRef Warp = RTRuntimeGetWarpByWorldNpcID(Runtime, Character->Info.Position.WorldID, NpcID, WarpIndex);
    if (!Warp) return false;
    assert(Warp->Index == WarpIndex);

    if (Character->Info.Basic.Level < Warp->Level) return false;
    if (Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] < Warp->Fee) return false;

    if (Warp->Type == RUNTIME_WARP_TYPE_GATE) {
        RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, Warp->TargetID);

        RTWorldContextRef TargetWorld = World;
        if (World->WorldData->WorldIndex != WarpPoint.WorldIndex) {
            TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
            assert(TargetWorld);
        }

        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Warp->Fee;
        Character->Info.Position.X = WarpPoint.X;
        Character->Info.Position.Y = WarpPoint.Y;
        Character->Info.Position.WorldID = WarpPoint.WorldIndex;
        Character->Info.Position.DungeonIndex = TargetWorld->DungeonIndex;

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

        if (Character->Info.Basic.Level < QuestDungeonData->EntryConditionLevel) return false;

        // TODO: Check if party has other dungeon
        // TODO: Check MaxPlayerCount & Party Size
        // TODO: Check EntryConditionClass

        RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, QuestDungeonData->EntryWarpID);
        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);

        RTWorldContextRef DungeonWorld = RTRuntimeOpenDungeon(Runtime, Character, WarpPoint.WorldIndex, QuestDungeonData->DungeonID);
        if (!DungeonWorld) return false;

        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Warp->Fee;
        Character->Info.Position.X = WarpPoint.X;
        Character->Info.Position.Y = WarpPoint.Y;
        Character->Info.Position.WorldID = WarpPoint.WorldIndex;
        Character->Info.Position.DungeonIndex = DungeonWorld->DungeonIndex;

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

        if (Character->Info.Basic.Level < DungeonData->EntryConditionLevel) return false;

        // TODO: Check if party has other dungeon
        // TODO: Check MaxPlayerCount & Party Size
        // TODO: Check EntryConditionClass

        RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, DungeonData->EntryWarpID);
        RTWorldDespawnCharacter(Runtime, World, Entity, RUNTIME_WORLD_CHUNK_UPDATE_REASON_WARP);
        
        RTWorldContextRef DungeonWorld = RTRuntimeOpenDungeon(Runtime, Character, WarpPoint.WorldIndex, DungeonData->DungeonID);
        if (!DungeonWorld) return false;

        Character->Info.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ] -= Warp->Fee;
        Character->Info.Position.X = WarpPoint.X;
        Character->Info.Position.Y = WarpPoint.Y;
        Character->Info.Position.WorldID = WarpPoint.WorldIndex;
        Character->Info.Position.DungeonIndex = DungeonWorld->DungeonIndex;
        Character->DungeonEntryItemSlotIndex = SlotIndex;

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
