#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcs.h"
#include "Notification.h"
#include "Server.h"

CLIENT_PROCEDURE_BINDING(INITIALIZE) {
    if (Character) goto error;
	
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;
   
	IPC_DATA_WORLD_REQGETCHARACTER* Request = PacketInit(IPC_DATA_WORLD_REQGETCHARACTER);
	Request->Command = IPC_WORLD_REQGETCHARACTER;
	Request->ConnectionID = Connection->ID;
	Request->AccountID = Client->Account.AccountID;
    Request->CharacterID = (Packet->CharacterIndex - Packet->CharacterIndex % MAX_CHARACTER_COUNT) / MAX_CHARACTER_COUNT;
    Request->CharacterIndex = Packet->CharacterIndex;
	return SocketSendAll(Context->MasterSocket, Request);

error:
	return SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(OnWorldGetCharacter, IPC_WORLD_ACKGETCHARACTER, IPC_DATA_WORLD_ACKGETCHARACTER) {
    if (!ClientConnection || !Client) goto error;
    if (!Packet->Success) goto error;

    if (Context->Config.WorldSvr.DebugSetQuestFlags) {
        memset(Packet->Character.QuestFlagData.FinishedQuests, 0xFF, RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);
    }

    RTSkillSlotRef GmSkill = &Packet->Character.SkillSlotData.Skills[Packet->Character.SkillSlotData.Count];
    GmSkill->ID = 146;
    GmSkill->Level = 1;
    GmSkill->Index = 66;
    Packet->Character.SkillSlotData.Count += 1;

    /*
    RTWorldRef TargetWorld = RTRuntimeGetWorldByID(Runtime, Packet->Character.CharacterData.Position.WorldID);
    if (TargetWorld->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON) {
        RTDungeonDataRef QuestDungeonData = RTRuntimeGetDungeonDataByID(Runtime, TargetWorld->DungeonID);
        assert(QuestDungeonData);

        assert(0 <= QuestDungeonData->FailWarpNpcID - 1 && QuestDungeonData->FailWarpNpcID - 1 < Runtime->WarpIndexCount);
        RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[QuestDungeonData->FailWarpNpcID - 1];
        RTPositionRef Position = &WarpIndex->Target[0];
        RTWorldRef NewWorld = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
        assert(NewWorld);

        Packet->Character.CharacterData.Position.X = Position->X;
        Packet->Character.CharacterData.Position.Y = Position->Y;
        Packet->Character.CharacterData.Position.WorldID = WarpIndex->WorldID;
    }
    */

    S2C_DATA_INITIALIZE* Response = PacketInitExtended(S2C_DATA_INITIALIZE);
    Response->Command = S2C_INITIALIZE;

    // TODO: Populate correct data!!!

    /* Server Info */
    Response->WorldType = Context->Config.WorldSvr.WorldType;
    Response->Server.ServerID = Master->ServerID;
    Response->Server.WorldID = Context->Config.WorldSvr.WorldID;
    Response->Server.PlayerCount = SocketGetConnectionCount(Context->ClientSocket);
    Response->Server.MaxPlayerCount = Context->Config.WorldSvr.MaxConnectionCount;
    memcpy(Response->Server.Address.Host, Context->Config.WorldSvr.Host, strlen(Context->Config.WorldSvr.Host));
    Response->Server.Address.Port = Context->Config.WorldSvr.Port;
    Response->Server.WorldType = Context->Config.WorldSvr.WorldType;

    /* Character Info */
    Response->WorldIndex = Packet->Character.CharacterData.Position.WorldID;
    Response->DungeonIndex = Packet->Character.CharacterData.Position.DungeonIndex;
    Response->Position.X = Packet->Character.CharacterData.Position.X;
    Response->Position.Y = Packet->Character.CharacterData.Position.Y;
    Response->Exp = Packet->Character.CharacterData.Basic.Exp;
    Response->Alz = Packet->Character.CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_ALZ];
    Response->Wexp = Packet->Character.CharacterData.Honor.Exp;
    Response->Level = Packet->Character.CharacterData.Basic.Level;
    Response->STR = Packet->Character.CharacterData.Stat[RUNTIME_CHARACTER_STAT_STR];
    Response->DEX = Packet->Character.CharacterData.Stat[RUNTIME_CHARACTER_STAT_DEX];
    Response->INT = Packet->Character.CharacterData.Stat[RUNTIME_CHARACTER_STAT_INT];
    Response->PNT = Packet->Character.CharacterData.Stat[RUNTIME_CHARACTER_STAT_PNT];
    Response->SkillRank = Packet->Character.CharacterData.Skill.Rank;
    Response->SkillLevel = Packet->Character.CharacterData.Skill.Level;
    Response->SkillExp = (UInt32)Packet->Character.CharacterData.Skill.Exp;
    Response->SkillPoint = Packet->Character.CharacterData.Skill.Point;
    Response->HonorPoint = Packet->Character.CharacterData.Honor.Point;

    /* TODO: Populate Server host data */
    CString DummyHost = "127.0.0.1";
    memcpy(Response->ChatServerAddress.Host, DummyHost, strlen(DummyHost));
    Response->ChatServerAddress.Port = 0;
    memcpy(Response->AuctionServerAddress.Host, DummyHost, strlen(DummyHost));
    Response->AuctionServerAddress.Port = 0;
    memcpy(Response->PartyServerAddress.Host, DummyHost, strlen(DummyHost));
    Response->PartyServerAddress.Port = 0;

    Response->Nation = Packet->Character.CharacterData.Profile.Nation;
    Response->WarpMask = Packet->Character.CharacterData.Profile.WarpMask;
    Response->MapsMask = Packet->Character.CharacterData.Profile.MapsMask;
    Response->CharacterStyle = SwapUInt32(Packet->Character.CharacterData.Style.RawValue);
    Response->CharacterLiveStyle = 0;
    Response->AP = Packet->Character.CharacterData.Ability.Point;
    Response->Axp = Packet->Character.CharacterData.Ability.Exp;
    Response->AllAchievementScore = 0;
    Response->NormalAchievementScore = 0;
    Response->QuestAchievementScore = 0;
    Response->DungeonAchievementScore = 0;
    Response->ItemsAchievementScore = 0;
    Response->PvpAchievementScore = 0;
    Response->WarAchievementScore = 0;
    Response->HuntingAchievementScore = 0;
    Response->CraftAchievementScore = 0;
    Response->CommunityAchievementScore = 0;
    Response->SharedAchievementScore = 0;
    Response->SpecialAchievementScore = 0;
    Response->DisplayTitle = 0;
    Response->EventTitle = 0;
    Response->GuildTitle = 0;
    Response->WarTitle = 0;
    Response->CraftEnergy = 0;
    Response->SortingOrderMask = 0;
    Response->BasicCraftExp = 0;
    Response->GoldMeritExp = 0;
    Response->GoldMeritPoint = 0;
    Response->PlatinumMeritCount = 0;
    Response->PlatinumMeritExp = 0;
    Response->PlatinumMeritPoint[0] = 0;
    Response->PlatinumMeritPoint[1] = 0;
    Response->CharacterCreationDate = Packet->Character.CreationDate;
    Response->ForceGem = Packet->Character.CharacterData.Currency[RUNTIME_CHARACTER_CURRENCY_GEM];
    Response->OverlordLevel = Packet->Character.CharacterData.Overlord.Level;
    Response->OverlordExp = Packet->Character.CharacterData.Overlord.Exp;
    Response->OverlordPoint = Packet->Character.CharacterData.Overlord.Point;
    // Response->SpecialGiftboxPoint = 0;
    Response->TranscendencePoint = 0;
    Response->NameLength = strlen(Packet->Character.Name) + 1;
    CString Name = (CString)PacketAppendMemory(strlen(Packet->Character.Name));
    memcpy(Name, Packet->Character.Name, strlen(Packet->Character.Name));

    Response->EquipmentSlotCount = Packet->Character.EquipmentData.Count;
    if (Packet->Character.EquipmentData.Count > 0) {
        PacketAppendMemoryCopy(
            Packet->Character.EquipmentData.Slots,
            sizeof(struct _RTItemSlot) * Packet->Character.EquipmentData.Count
        );
    }

    //memset(Packet->Character.InventoryData.Inventory, 0, sizeof(struct _RTItemSlot) * Packet->Character.InventoryData.Count);
    //Packet->Character.InventoryData.Count = 0;

    Response->InventorySlotCount = Packet->Character.InventoryData.Count;
    if (Packet->Character.InventoryData.Count > 0) {
        PacketAppendMemoryCopy(
            Packet->Character.InventoryData.Slots,
            sizeof(struct _RTItemSlot) * Packet->Character.InventoryData.Count
        );
    }

    Response->SkillSlotCount = Packet->Character.SkillSlotData.Count;
    if (Packet->Character.SkillSlotData.Count > 0) {
        PacketAppendMemoryCopy(
            Packet->Character.SkillSlotData.Skills,
            sizeof(struct _RTSkillSlot) * Packet->Character.SkillSlotData.Count
        );
    }

    Response->QuickSlotCount = Packet->Character.QuickSlotData.Count;
    if (Packet->Character.QuickSlotData.Count > 0) {
        PacketAppendMemoryCopy(
            Packet->Character.QuickSlotData.QuickSlots,
            sizeof(struct _RTQuickSlot) * Packet->Character.QuickSlotData.Count
        );
    }

    Response->EssenceAbilityCount = Packet->Character.EssenceAbilityData.Count;
    for (Int32 Index = 0; Index < Packet->Character.EssenceAbilityData.Count; Index += 1) {
        RTEssenceAbilitySlotRef AbilitySlot = &Packet->Character.EssenceAbilityData.Slots[Index];

        S2C_DATA_INITIALIZE_ESSENCE_ABILITY_SLOT* ResponseSlot = PacketAppendStruct(S2C_DATA_INITIALIZE_ESSENCE_ABILITY_SLOT);
        ResponseSlot->AbilityID = AbilitySlot->AbilityID;
        ResponseSlot->Level = AbilitySlot->Level;
        ResponseSlot->Unknown1 = AbilitySlot->Unknown1;
    }

    Response->BlendedAbilityCount = 0;

    memcpy(&Response->QuestFlagInfo, &Packet->Character.QuestFlagData, sizeof(struct _RTCharacterQuestFlagInfo));
    memcpy(&Response->DungeonQuestFlagInfo, &Packet->Character.DungeonQuestFlagData, sizeof(struct _RTCharacterDungeonQuestFlagInfo));

    Response->QuestSlotCount = 0;
    for (Int32 Index = 0; Index < RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT; Index += 1) {
        RTQuestSlotRef QuestSlot = &Packet->Character.QuestSlotData.QuestSlot[Index];
        if (!QuestSlot->QuestIndex) continue;

        RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
        assert(Quest);

        UInt16 NpcActionFlags = 0;
        for (Int32 Index = 0; Index < QuestSlot->NpcActionIndex; Index += 1) {
            NpcActionFlags |= 1 << Index;
        }

        S2C_DATA_INITIALIZE_QUEST_INDEX* QuestResponse = PacketAppendStruct(S2C_DATA_INITIALIZE_QUEST_INDEX);
        QuestResponse->QuestIndex = QuestSlot->QuestIndex;
        QuestResponse->NpcActionFlags = NpcActionFlags;
        QuestResponse->DisplayNotice = QuestSlot->DisplayNotice;
        QuestResponse->DisplayOpenNotice = QuestSlot->DisplayOpenNotice;
        QuestResponse->QuestSlotIndex = Index;

        Int32 CounterCount = Quest->MissionMobCount + Quest->MissionItemCount;
        for (Int32 CounterIndex = 0; CounterIndex < CounterCount; CounterIndex++) {
            PacketAppendValue(UInt8, QuestSlot->Counter[CounterIndex]);
        }

        Response->QuestSlotCount += 1;
    }
    /*
    Response->PremiumServiceCount = 1;
    S2C_DATA_INITIALIZE_PREMIUM_SERVICE_SLOT* PremiumServiceSlot = PacketAppendStruct(S2C_DATA_INITIALIZE_PREMIUM_SERVICE_SLOT);
    PremiumServiceSlot->ServiceIndex = 1;
    PremiumServiceSlot->Unknown1 = 0;
    PremiumServiceSlot->Duration.Year = 1;
    */
    Response->BlessingBeadCount = 0;
    Response->MercenaryCount = 0;
    Response->EquipmentAppearanceCount = 0;
    Response->InventoryAppearanceCount = 0;
    Response->AchievementCount = 0;
    Response->TitleCount = 0;
    Response->ExtendedTitleCount = 0;
    Response->CraftCount = 0;

    Response->OverlordMasteryCount = Packet->Character.OverlordData.Count;
    for (Int32 Index = 0; Index < Packet->Character.OverlordData.Count; Index += 1) {
        S2C_DATA_OVERLORD_MASTERY_SLOT* MasterySlot = PacketAppendStruct(S2C_DATA_OVERLORD_MASTERY_SLOT);
        MasterySlot->MasteryIndex = Packet->Character.OverlordData.Slots[Index].MasteryIndex;
        MasterySlot->Level = Packet->Character.OverlordData.Slots[Index].Level;
    }

    // HonorMedalCount
    // ForceWingCount
    
    Response->SpecialGiftboxCount = 5;
    for (Int32 Index = 0; Index < 5; Index += 1) {
        RTGiftBoxInfoRef GiftBox = PacketAppendStruct(struct _RTGiftBoxInfo);
        GiftBox->Index = Index;
        GiftBox->ReceivedCount = 333;
        GiftBox->ElapsedTime = 0;
        GiftBox->CooldownTime = 1;
        GiftBox->Active = 0;
    }

    Response->CollectionCount = Packet->Character.CollectionData.Count;

    for (Int32 Index = 0; Index < Packet->Character.CollectionData.Count; Index += 1) {
        S2C_DATA_COLLECTION_SLOT* CollectionSlot = PacketAppendStruct(S2C_DATA_COLLECTION_SLOT);
        CollectionSlot->TypeID = Packet->Character.CollectionData.Slots[Index].TypeID;
        CollectionSlot->CollectionID = Packet->Character.CollectionData.Slots[Index].CollectionID;
        CollectionSlot->MissionID = Packet->Character.CollectionData.Slots[Index].MissionID;
        CollectionSlot->ReceivedReward = Packet->Character.CollectionData.Slots[Index].ReceivedReward;
        memcpy(&CollectionSlot->MissionItemCounts[0], &Packet->Character.CollectionData.Slots[Index].MissionItemCounts[0], sizeof(UInt16) * RUNTIME_CHARACTER_MAX_COLLECTION_ITEM_COUNT);
    }

    Response->TransformCount = 0;
    Response->TranscendenceCount = 0;

    assert(ClientConnection->ID < RUNTIME_MEMORY_MAX_CHARACTER_COUNT);

    Character = RTWorldManagerCreateCharacter(Context->Runtime->WorldManager, Packet->CharacterIndex);

    RTCharacterInitialize(
        Runtime, 
        Character,
        &Packet->Character.CharacterData,
        &Packet->Character.EquipmentData,
        &Packet->Character.InventoryData,
        &Packet->Character.SkillSlotData,
        &Packet->Character.QuickSlotData,
        &Packet->Character.QuestSlotData,
        &Packet->Character.QuestFlagData,
        &Packet->Character.DungeonQuestFlagData,
        &Packet->Character.EssenceAbilityData,
        &Packet->Character.OverlordData,
        &Packet->Character.CollectionData
    );

    Client->CharacterDatabaseID = Packet->Character.ID;
    Client->CharacterIndex = Packet->CharacterIndex;
    CStringCopySafe(Client->CharacterName, MAX_CHARACTER_NAME_LENGTH, Packet->Character.Name);

    Response->Entity = Character->ID;
    Response->CharacterIndex = Packet->CharacterIndex;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

    if ((World->WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON ||
        World->WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON) &&
        World->DungeonIndex != Character->Info.Position.DungeonIndex) {

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, Character->Info.Position.DungeonIndex);
        if (!DungeonData) goto error;

        assert(0 <= DungeonData->FailWarpNpcID - 1 && DungeonData->FailWarpNpcID - 1 < Runtime->WarpIndexCount);
        RTWarpIndexRef WarpIndex = &Runtime->WarpIndices[DungeonData->FailWarpNpcID - 1];
        RTPositionRef Position = &WarpIndex->Target[0];

        RTWorldContextRef TargetWorld = World;
        if (World->WorldData->WorldIndex != WarpIndex->WorldID) {
            TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpIndex->WorldID);
            assert(TargetWorld);
        }

        World = TargetWorld;
        Character->Info.Position.X = Position->X;
        Character->Info.Position.Y = Position->Y;
        Character->Info.Position.WorldID = WarpIndex->WorldID;
        Character->Info.Position.DungeonIndex = TargetWorld->DungeonIndex;
        Response->WorldIndex = Character->Info.Position.WorldID;
        Response->Position.X = Character->Info.Position.X;
        Response->Position.Y = Character->Info.Position.Y;
    }

    Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX] = 25000;
    Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT] = 25000;

    // TODO: Check if max hp or base hp is requested here...
    Response->BaseHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    Response->MaxMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
    Response->MaxSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX];
    Response->MaxBP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX];

    Response->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    Response->CurrentMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
    Response->CurrentSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
    Response->CurrentBP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
    Response->CurrentRage = Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
    
    /*
    Response->Unknown7[0] = 113;
    Response->Unknown7[1] = 201;
    Response->Unknown7[2] = 9;
    Response->UnknownSkillRank = Response->SkillRank + 1;
    Response->Unknown1[55] = 1;
    Response->Unknown1[58] = 20;
    */
    Response->Nation = 3;
    Response->MapsMask = 2101247;
    Response->WarpMask = 2101247;
    //memset(Response->DungeonFlagInfo.Flags, 0xFF, 640);

    SocketSend(Context->ClientSocket, ClientConnection, Response);

    RTWorldSpawnCharacter(Runtime, World, Character->ID);

    return;

error:
	if (ClientConnection) {
		return SocketDisconnect(Context->ClientSocket, ClientConnection);
	}
}
