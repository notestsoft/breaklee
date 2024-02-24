#include "Character.h"
#include "Mob.h"
#include "Runtime.h"
#include "Force.h"
#include "World.h"
#include "WorldManager.h"

RTRuntimeRef RTRuntimeCreate(
    AllocatorRef Allocator,
    RTEventCallback Callback,
    Void* UserData
) {
    RTRuntimeRef Runtime = (RTRuntimeRef)AllocatorAllocate(Allocator, sizeof(struct _RTRuntime));
    if (!Runtime) FatalError("Memory allocation failed!");
    memset(Runtime, 0, sizeof(struct _RTRuntime));

    for (Int32 X = 0; X < RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH; X++) {
        for (Int32 Y = 0; Y < RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH; Y++) {
            Int32 Index = X + Y * RUNTIME_MOVEMENT_MAX_DISTANCE_LENGTH;
            Runtime->MovementDistanceCache[Index] = (Float32)sqrt((Float64)X * (Float64)X + (Float64)Y * (Float64)Y);
        }
    }

    Runtime->Context = RTRuntimeDataContextCreate();
    Runtime->WorldManager = RTWorldManagerCreate(
        Runtime,
        RUNTIME_MEMORY_MAX_WORLD_DATA_COUNT,
        RUNTIME_MEMORY_MAX_GLOBAL_WORLD_CONTEXT_COUNT,
        RUNTIME_MEMORY_MAX_PARTY_WORLD_CONTEXT_COUNT,
        RUNTIME_MEMORY_MAX_CHARACTER_COUNT
    );
    Runtime->Callback = Callback;
    Runtime->UserData = UserData;
    return Runtime;
}

Void RTRuntimeDestroy(
    RTRuntimeRef Runtime
) {
    RTRuntimeDataContextDestroy(Runtime->Context);
    RTWorldManagerDestroy(Runtime->WorldManager);
    AllocatorDeallocate(Runtime->Allocator, Runtime);
}

Bool RTRuntimeLoadData(
    RTRuntimeRef Runtime,
    CString RuntimeDataPath,
    CString ServerDataPath
) {
    // TODO: This should be configured in xml file
    RTInitForceEffectFormulas();
 
    return RTRuntimeDataContextLoad(Runtime->Context, RuntimeDataPath, ServerDataPath);
}

Void RTRuntimeUpdate(
    RTRuntimeRef Runtime
) {
    RTWorldManagerUpdate(Runtime->WorldManager);
    /* Movement Debugging
    for (Int32 Index = 0; Index < Runtime->Characters.Count; Index++) {
        RTCharacterRef Character = (RTCharacterRef)ArrayGetElementAtIndex(&Runtime->Characters, Index);
        if (Character->Movement.Flags & RUNTIME_MOVEMENT_IS_DEAD_RECKONING) {
            RTMovementUpdateDeadReckoning(Runtime, &Character->Movement);

            LogMessageFormat(
                LOG_LEVEL_INFO,
                "(%d, %d)",
                Character->Movement.PositionCurrent.X,
                Character->Movement.PositionCurrent.Y
            );
        }
    }
    */
}

Void RTRuntimeBroadcastEvent(
    RTRuntimeRef Runtime,
    Int32 EventType,
    RTWorldContextRef World,
    RTEntityID SourceID,
    RTEntityID TargetID,
    Int32 X,
    Int32 Y
) {
    Runtime->Event.Type = EventType;
    Runtime->Event.World = World;
    Runtime->Event.SourceID = SourceID;
    Runtime->Event.TargetID = TargetID;
    Runtime->Event.X = X;
    Runtime->Event.Y = Y;
    Runtime->Callback(Runtime, &Runtime->Event, Runtime->UserData);
}

Void RTRuntimeBroadcastEventData(
    RTRuntimeRef Runtime,
    Int32 EventType,
    RTWorldContextRef World,
    RTEntityID SourceID,
    RTEntityID TargetID,
    Int32 X,
    Int32 Y,
    RTEventData Data
) {
    Runtime->Event.Type = EventType;
    Runtime->Event.World = World;
    Runtime->Event.SourceID = SourceID;
    Runtime->Event.TargetID = TargetID;
    Runtime->Event.X = X;
    Runtime->Event.Y = Y;
    Runtime->Event.Data = Data;
    Runtime->Callback(Runtime, &Runtime->Event, Runtime->UserData);
}

RTEntityID RTRuntimeCreateEntity(
    RTRuntimeRef Runtime,
    UInt8 WorldIndex,
    UInt8 EntityType
) {
    assert(Runtime->EntityCount < RUNTIME_MEMORY_MAX_ENTITY_COUNT);

    for (Int32 Index = 0; Index < RUNTIME_MEMORY_MAX_ENTITY_COUNT; Index += 1) {
        if (!RTEntityIsNull(Runtime->Entities[Index])) continue;

        Runtime->Entities[Index].EntityIndex = Index + 1;
        Runtime->Entities[Index].WorldIndex = WorldIndex;
        Runtime->Entities[Index].EntityType = EntityType;
        Runtime->EntityCount += 1;
        return Runtime->Entities[Index];
    }

    FatalError("Maximum amount of entities reached!");
    return kEntityIDNull;
}

Void RTRuntimeDeleteEntity(
    RTRuntimeRef Runtime,
    RTEntityID Entity
) {
    Int32 Index = Entity.EntityIndex - 1;

    assert(Index < RUNTIME_MEMORY_MAX_ENTITY_COUNT);
    assert(RTEntityIsEqual(Runtime->Entities[Index], Entity));

    Runtime->Entities[Index] = kEntityIDNull;
    Runtime->EntityCount -= 1;
}

RTPartyRef RTRuntimeCreateParty(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 PartyType
) {
    assert(Runtime->PartyCount < RUNTIME_MEMORY_MAX_PARTY_COUNT);
    assert(RTEntityIsNull(Character->PartyID));

    RTEntityID Entity = RTRuntimeCreateEntity(Runtime, 0, RUNTIME_ENTITY_TYPE_PARTY);
    Int32 Index = Entity.EntityIndex - 1;

    RTPartyRef Party = &Runtime->Parties[Runtime->PartyCount];
    memset(Party, 0, sizeof(struct _RTParty));
    Party->ID = Entity;
    Party->LeaderID = Character->ID;
    Party->PartyType = PartyType;
    Party->MemberCount = 1;
    Party->Members[0].SlotIndex = 0;
    Party->Members[0].MemberID = Character->ID;

    Runtime->PartyCount += 1;
    return Party;
}

Void RTRuntimeDestroyParty(
    RTRuntimeRef Runtime,
    RTPartyRef Party
) {
    assert(!RTEntityIsNull(Party->ID));

    for (Index Index = 0; Index < Party->MemberCount; Index += 1) {
        RTEntityID MemberID = Party->Members[Index].MemberID;
        if (!RTWorldManagerHasCharacter(Runtime->WorldManager, MemberID)) continue;

        RTCharacterRef Member = RTWorldManagerGetCharacter(Runtime->WorldManager, MemberID);
        if (!RTEntityIsEqual(Member->PartyID, Party->ID)) continue;

        // TODO: Send event notifications...
        Member->PartyID = kEntityIDNull;
    }

    assert(!RTWorldContextHasParty(Runtime->WorldManager, Party->ID));

    RTRuntimeDeleteEntity(Runtime, Party->ID);
    memset(Party, 0, sizeof(struct _RTParty));
    // TODO: The party has to be removed from the memory!!!
}

RTWorldContextRef RTRuntimeGetWorldByID(
    RTRuntimeRef Runtime,
    Int32 WorldID
) {
    return RTWorldContextGetGlobal(Runtime->WorldManager, WorldID);
}

RTWorldContextRef RTRuntimeGetWorldByParty(
    RTRuntimeRef Runtime,
    RTEntityID PartyID
) {
    return RTWorldContextGetParty(Runtime->WorldManager, PartyID);
}

RTWorldContextRef RTRuntimeGetWorldByCharacter(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    if (!RTEntityIsNull(Character->PartyID)) {
        RTWorldContextRef WorldContext = RTWorldContextGetParty(Runtime->WorldManager, Character->PartyID);
        if (WorldContext->WorldData->WorldIndex == Character->Info.Position.WorldID) return WorldContext;
    }

    return RTWorldContextGetGlobal(Runtime->WorldManager, Character->Info.Position.WorldID);
}

RTNpcRef RTRuntimeGetNpcByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
) {
    for (Int32 Index = 0; Index < Runtime->NpcCount; Index++) {
        RTNpcRef Npc = &Runtime->Npcs[Index];
        if (Npc->WorldID == WorldID && Npc->ID == NpcID) return Npc;
    }

    return NULL;
}

RTWarpRef RTRuntimeGetWarpByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID,
    Int32 WarpIndex
) {
    for (Int32 Index = 0; Index < Runtime->WarpCount; Index++) {
        RTWarpRef Warp = &Runtime->Warps[Index];
        if (Warp->WorldID == WorldID && Warp->NpcID == NpcID && Warp->Index == WarpIndex) return Warp;
    }

    return NULL;
}

RTWarpRef RTRuntimeGetWarpByIndex(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 WarpIndex
) {
    if (WarpIndex < 0 || WarpIndex >= Runtime->WarpCount) return NULL;

    return &Runtime->Warps[WarpIndex];
}

RTPartyRef RTRuntimeGetParty(
    RTRuntimeRef Runtime,
    RTEntityID Entity
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_PARTY);
    assert(Entity.EntityIndex > 0);

    // TODO: Add an indexing solution for time & space efficiency
    for (Int32 Index = 0; Index < Runtime->PartyCount; Index += 1) {
        RTPartyRef Party = &Runtime->Parties[Index];
        if (RTEntityIsEqual(Party->ID, Entity)) {
            return Party;
        }
    }

    return NULL;
}

RTWorldItemRef RTRuntimeGetItem(
    RTRuntimeRef Runtime,
    RTEntityID Entity
) {
    assert(Entity.EntityType == RUNTIME_ENTITY_TYPE_ITEM);

    RTWorldContextRef World = RTRuntimeGetWorldByID(Runtime, Entity.WorldIndex);
    if (!World) return NULL;

    return RTWorldGetItemByEntity(Runtime, World, Entity);
}

RTWorldItemRef RTRuntimeGetItemByIndex(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 ItemID
) {
    RTWorldContextRef World = RTRuntimeGetWorldByID(Runtime, WorldID);
    assert(World);
   
    for (Int32 Index = 0; Index < World->ItemCount; Index++) {
        RTWorldItemRef Item = &World->Items[Index];
        if (Item->Index == ItemID) return Item;
    }

    return NULL;
}

RTItemDataRef RTRuntimeGetItemDataByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemIndex
) {
    UInt32 ItemID = (ItemIndex & RUNTIME_ITEM_MASK_INDEX);
    for (Int32 Index = 0; Index < Runtime->ItemDataCount; Index++) {
        RTItemDataRef Item = &Runtime->ItemData[Index];
        if (Item->ItemID == ItemID) {
            return Item;
        }
    }

    return NULL;
}

RTQuestDataRef RTRuntimeGetQuestByIndex(
    RTRuntimeRef Runtime,
    Int32 QuestIndex
) {
    for (Int32 Index = 0; Index < Runtime->QuestDataCount; Index++) {
        RTQuestDataRef Quest = &Runtime->QuestData[Index];
        if (Quest->ID == QuestIndex) {
            return Quest;
        }
    }

    return NULL;
}

RTQuestRewardItemSetDataRef RTRuntimeGetQuestRewardItemSetByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemSetIndex
) {
    if (ItemSetIndex < 1) return NULL;

    for (Int32 Index = 0; Index < Runtime->QuestRewardItemSetDataCount; Index++) {
        RTQuestRewardItemSetDataRef ItemSet = &Runtime->QuestRewardItemSetData[Index];
        if (ItemSet->ID == ItemSetIndex) {
            return ItemSet;
        }
    }

    return NULL;
}

RTQuestRewardItemDataRef RTRuntimeGetQuestRewardItemByIndex(
    RTRuntimeRef Runtime,
    Int32 ItemSetIndex,
    Int32 ItemIndex,
    Int32 BattleStyleIndex
) {
    RTQuestRewardItemSetDataRef ItemSet = RTRuntimeGetQuestRewardItemSetByIndex(Runtime, ItemSetIndex);
    if (!ItemSet) return NULL;

    for (Int32 Index = 0; Index < ItemSet->Count; Index++) {
        RTQuestRewardItemDataRef Item = &ItemSet->Items[Index];
        if (Item->Index == ItemIndex && (Item->BattleStyleIndex == BattleStyleIndex || Item->BattleStyleIndex == 0)) {
            return Item;
        }
    }

    return NULL;
}

RTShopDataRef RTRuntimeGetShopByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
) {
    for (Int32 Index = 0; Index < Runtime->ShopDataCount; Index++) {
        RTShopDataRef Shop = &Runtime->ShopData[Index];
        if (Shop->WorldID == WorldID && Shop->NpcID == NpcID) {
            return Shop;
        }
    }

    return NULL;
}

RTShopDataRef RTRuntimeGetShopByIndex(
    RTRuntimeRef Runtime,
    Int32 ShopIndex
) {
    for (Int32 Index = 0; Index < Runtime->ShopDataCount; Index++) {
        RTShopDataRef Shop = &Runtime->ShopData[Index];
        if (Shop->Index == ShopIndex) {
            return Shop;
        }
    }

    return NULL;
}

RTTrainerDataRef RTRuntimeGetTrainerByWorldNpcID(
    RTRuntimeRef Runtime,
    Int32 WorldID,
    Int32 NpcID
) {
    for (Int32 Index = 0; Index < Runtime->TrainerDataCount; Index++) {
        RTTrainerDataRef Trainer = &Runtime->TrainerData[Index];
        if (Trainer->WorldID == WorldID && Trainer->NpcID == NpcID) {
            return Trainer;
        }
    }

    return NULL;
}

RTCharacterSkillDataRef RTRuntimeGetCharacterSkillDataByID(
    RTRuntimeRef Runtime,
    Int32 SkillID
) {
    for (Int32 Index = 0; Index < Runtime->CharacterSkillDataCount; Index++) {
        if (Runtime->CharacterSkillData[Index].SkillID == SkillID) {
            return &Runtime->CharacterSkillData[Index];
        }
    }

    return NULL;
}

RTSkillLevelDataRef RTRuntimeGetSkillLevelDataByID(
    RTRuntimeRef Runtime,
    Int32 SkillID,
    Int32 SkillLevel
) {
    RTCharacterSkillDataRef SkillData = RTRuntimeGetCharacterSkillDataByID(Runtime, SkillID);
    if (!SkillData) return NULL;

    for (Int32 Index = 0; Index < SkillData->SkillLevelCount; Index++) {
        if (SkillData->SkillLevels[Index].StartLevel <= SkillLevel && SkillLevel <= SkillData->SkillLevels[Index].EndLevel) {
            return &SkillData->SkillLevels[Index];
        }
    }

    return NULL;
}

RTBattleStyleLevelFormulaDataRef RTRuntimeGetBattleStyleLevelFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    return &Runtime->BattleStyleLevelFormulaData[BattleStyleIndex - 1];

}

RTBattleStyleClassFormulaDataRef RTRuntimeGetBattleStyleClassFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    return &Runtime->BattleStyleClassFormulaData[BattleStyleIndex - 1];
}

RTBattleStyleSlopeDataRef RTRuntimeGetBattleStyleSlopeData(
    RTRuntimeRef Runtime,
    Int32 SlopeID,
    Int32 Penalty
) {
    RTBattleStyleSlopeFormulaDataRef Formula = RTRuntimeGetBattleStyleSlopeFormulaData(Runtime, SlopeID);
    assert(Formula && Formula->SlopeCount > 0);

    Int32 SlopePenalty = 0;
    Int32 SlopeIndex = 0;
    for (Int32 Index = 0; Index < Formula->SlopeCount; Index++) {
        if (Formula->Slopes[Index].Penalty <= Penalty && Formula->Slopes[Index].Penalty >= SlopePenalty) {
            SlopePenalty = Formula->Slopes[Index].Penalty;
            SlopeIndex = Index;
        }
    }

    return &Formula->Slopes[SlopeIndex];
}

RTBattleStyleSlopeFormulaDataRef RTRuntimeGetBattleStyleSlopeFormulaData(
    RTRuntimeRef Runtime,
    Int32 SlopeID
) {
    for (Int32 Index = 0; Index < Runtime->SlopeFormulaDataCount; Index++) {
        if (Runtime->BattleStyleSlopeFormulaData[Index].SlopeID == SlopeID) {
            return &Runtime->BattleStyleSlopeFormulaData[Index];
        }
    }

    return NULL;
}

RTBattleStyleStatsFormulaDataRef RTRuntimeGetBattleStyleStatsFormulaData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    return &Runtime->BattleStyleStatsFormulaData[BattleStyleIndex - 1];
}

RTBattleStyleSkillRankDataRef RTRuntimeGetBattleStyleSkillRankData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex,
    Int32 SkillRank
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    RTBattleStyleSkillFormulaDataRef FormulaData = &Runtime->BattleStyleSkillFormulaData[BattleStyleIndex - 1];
    for (Int32 Index = 0; Index < FormulaData->SkillRankCount; Index++) {
        if (FormulaData->SkillRanks[Index].SkillRank == SkillRank) {
            return &FormulaData->SkillRanks[Index];
        }
    }

    return NULL;
}

RTBattleStyleRankDataRef RTRuntimeGetBattleStyleRankData(
    RTRuntimeRef Runtime,
    Int32 BattleStyleIndex,
    Int32 Level
) {
    assert(RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MIN <= BattleStyleIndex && BattleStyleIndex <= RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_MAX);

    RTBattleStyleRankFormulaDataRef FormulaData = &Runtime->BattleStyleRankFormulaData[BattleStyleIndex - 1];
    for (Int32 Index = 0; Index < FormulaData->RankCount; Index++) {
        if (FormulaData->Ranks[Index].Level == Level) {
            return &FormulaData->Ranks[Index];
        }
    }

    return NULL;
}

RTWorldContextRef RTRuntimeOpenDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Index WorldIndex,
    Index DungeonIndex
) {
    RTWorldDataRef WorldData = RTWorldDataGet(Runtime->WorldManager, WorldIndex);
    assert(
        WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON ||
        WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON
    );

    // TODO: Cleanup previous dungeon, for now we assert to avoid to open a dungeon in a dungeon?
    RTWorldContextRef CurrentWorld = RTRuntimeGetWorldByCharacter(Runtime, Character);
    assert(CurrentWorld->WorldData->Type != RUNTIME_WORLD_TYPE_QUEST_DUNGEON);

    if (RTWorldContextPartyIsFull(Runtime->WorldManager)) return NULL;

    if (RTEntityIsNull(Character->PartyID)) {
        RTPartyRef Party = RTRuntimeCreateParty(Runtime, Character, RUNTIME_PARTY_TYPE_SOLO_DUNGEON);
        Character->PartyID = Party->ID;
    }
    
    RTWorldContextRef PartyWorld = RTWorldContextCreateParty(Runtime->WorldManager, WorldIndex, DungeonIndex, Character->PartyID);
    return PartyWorld;
}

Void RTRuntimeCloseDungeon(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    assert(!RTEntityIsNull(Character->PartyID) && Character->PartyID.EntityType == RUNTIME_ENTITY_TYPE_PARTY);

    RTWorldContextRef WorldContext = RTWorldContextGetParty(Runtime->WorldManager, Character->PartyID);
    if (!WorldContext) return;

    RTPartyRef Party = RTRuntimeGetParty(Runtime, Character->PartyID);
    assert(Party);

    if (Party->PartyType == RUNTIME_PARTY_TYPE_SOLO_DUNGEON) {
        assert(WorldContext->DungeonIndex != Character->Info.Position.DungeonIndex);

        RTWorldContextDestroyParty(Runtime->WorldManager, Character->PartyID);
        RTRuntimeDestroyParty(Runtime, Party);
    }
    else {
        // TODO: Check if any party member is still in dungeon and do proper cleanup...
    }
}

RTDungeonDataRef RTRuntimeGetDungeonDataByID(
    RTRuntimeRef Runtime,
    Index DungeonID
) {
    for (Int32 Index = 0; Index < Runtime->DungeonDataCount; Index++) {
        RTDungeonDataRef DungeonData = &Runtime->DungeonData[Index];
        if (DungeonData->DungeonID == DungeonID) {
            return DungeonData;
        }
    }

    return NULL;
}

RTMissionDungeonPatternPartDataRef RTRuntimeGetPatternPartByID(
    RTRuntimeRef Runtime,
    Int32 PatternPartID
) {
    for (Int32 Index = 0; Index < Runtime->MissionDungeonPatternPartDataCount; Index++) {
        RTMissionDungeonPatternPartDataRef PatternPartData = &Runtime->MissionDungeonPatternPartData[Index];
        if (PatternPartData->ID == PatternPartID) return PatternPartData;
    }

    return NULL;
}

Void RTRuntimeBroadcastCharacterData(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    UInt32 Type
) {
    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);
    assert(World);

    RTEventData EventData = { 0 };
    EventData.CharacterData.Type = Type;

    RTRuntimeBroadcastEventData(
        Runtime,
        RUNTIME_EVENT_CHARACTER_DATA,
        World,
        kEntityIDNull,
        Character->ID,
        Character->Movement.PositionCurrent.X,
        Character->Movement.PositionCurrent.Y,
        EventData
    );
}
