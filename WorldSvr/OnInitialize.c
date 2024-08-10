#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

Void SendCharacterStatus(
    ServerContextRef Context,
    SocketRef Socket,
    SocketConnectionRef Connection,
    RTCharacterRef Character
) {
    S2C_DATA_NFY_CHARACTER_STATUS* Notification = PacketBufferInit(Connection->PacketBuffer, S2C, NFY_CHARACTER_STATUS);
    Notification->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    Notification->CurrentMP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
    SocketSend(Socket, Connection, Notification);
}

Void SendEventList(
    ServerContextRef Context,
    ClientContextRef Client
) {
    S2C_DATA_NFY_EVENT_LIST* EventList = PacketBufferInit(Client->Connection->PacketBuffer, S2C, NFY_EVENT_LIST);
    EventList->Count = Context->Runtime->Context->EventCount;

    for (Index Index = 0; Index < Context->Runtime->Context->EventCount; Index += 1) {
        RTDataEventRef EventData = &Context->Runtime->Context->EventList[Index];
        PacketBufferAppendValue(Client->Connection->PacketBuffer, UInt32, EventData->ID);
    }

    SocketSend(Context->ClientSocket, Client->Connection, EventList);
}

Void SendEventInfo(
    ServerContextRef Context,
    ClientContextRef Client
) {
    S2C_DATA_NFY_EVENT_INFO* EventInfo = PacketBufferInitExtended(Client->Connection->PacketBuffer, S2C, NFY_EVENT_INFO);
    EventInfo->HasEventInfo = (Context->Runtime->Context->EventCount > 0) ? 1 : 0;

    S2C_DATA_NFY_EVENT_INFO_HEADER* EventInfoHeader = PacketBufferAppendStruct(Client->Connection->PacketBuffer, S2C_DATA_NFY_EVENT_INFO_HEADER);
    EventInfoHeader->EventCount = Context->Runtime->Context->EventCount;

    for (Int32 EventIndex = 0; EventIndex < Context->Runtime->Context->EventCount; EventIndex += 1) {
        RTDataEventRef EventData = &Context->Runtime->Context->EventList[EventIndex];

        S2C_DATA_NFY_EVENT_INFO_EVENT* EventInfoEvent = PacketBufferAppendStruct(Client->Connection->PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT);
        EventInfoEvent->EventIndex = EventData->ID;
        EventInfoEvent->EventType = EventData->Type;
        EventInfoEvent->EventFlags = EventData->UseFlags;
        EventInfoEvent->EventStartTimestamp = GetTimestamp() - 1000000;
        EventInfoEvent->EventEndTimestamp = GetTimestamp() + 1000000;
        EventInfoEvent->EventUpdateTimestamp = 0;
        EventInfoEvent->WorldIndex = EventData->WorldIndex;
        EventInfoEvent->NpcIndex = EventData->NpcIndex;
        PacketBufferAppendCString(Client->Connection->PacketBuffer, EventData->Description);

        S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO* EventItemInfo = PacketBufferAppendStruct(Client->Connection->PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO);
        EventItemInfo->ItemCount = EventData->EventItemCount;

        for (Index ItemIndex = 0; ItemIndex < EventData->EventItemCount; ItemIndex += 1) {
            RTDataEventItemRef EventItem = &EventData->EventItemList[ItemIndex];

            S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO_DATA* EventItemData = PacketBufferAppendStruct(Client->Connection->PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO_DATA);
            EventItemData->ItemID.Serial = EventItem->ItemID;
            EventItemData->ItemOptions = EventItem->ItemOptions;
            EventItemData->ExternalID = EventItem->ExternalID;
            EventItemData->TextureItemID = EventItem->TextureItemID;
            PacketBufferAppendCString(Client->Connection->PacketBuffer, EventItem->Title);
            PacketBufferAppendCString(Client->Connection->PacketBuffer, EventItem->Description);
        }

        EventItemInfo = PacketBufferAppendStruct(Client->Connection->PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO);
        EventItemInfo->ItemCount = 0;

        EventItemInfo = PacketBufferAppendStruct(Client->Connection->PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO);
        EventItemInfo->ItemCount = 0;
    }

    SocketSend(Context->ClientSocket, Client->Connection, EventInfo);
}

CLIENT_PROCEDURE_BINDING(INITIALIZE) {
    if (Character) goto error;
	
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->Account.AccountID < 1) goto error;

    IPC_W2D_DATA_GET_CHARACTER* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, GET_CHARACTER);
    Request->Header.SourceConnectionID = Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->Account.AccountID;
    Request->CharacterID = (Packet->CharacterIndex - Packet->CharacterIndex % MAX_CHARACTER_COUNT) / MAX_CHARACTER_COUNT;
    Request->CharacterIndex = Packet->CharacterIndex;
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, GET_CHARACTER) {
    if (!ClientConnection || !Client) goto error;
    if (!Packet->Success) goto error;

    if (Context->Config.WorldSvr.DebugSetQuestFlags) {
        memset(Packet->CharacterData.QuestInfo.Info.FinishedQuests, 0xFF, RUNTIME_CHARACTER_MAX_QUEST_FLAG_COUNT);
    }

    Packet->CharacterData.Info.Profile.Nation = 1;

    assert(ClientConnection->ID < RUNTIME_MEMORY_MAX_CHARACTER_COUNT);

    Character = RTWorldManagerCreateCharacter(Context->Runtime->WorldManager, Packet->CharacterIndex);

    RTCharacterInitialize(
        Runtime,
        Character,
        Packet->CharacterName,
        &Packet->CharacterData
    );

    Client->CharacterDatabaseID = Packet->CharacterID;
    Client->CharacterIndex = Packet->CharacterIndex;
    CStringCopySafe(Client->CharacterName, MAX_CHARACTER_NAME_LENGTH + 1, Packet->CharacterName);

    S2C_DATA_INITIALIZE* Response = PacketBufferInitExtended(ClientConnection->PacketBuffer, S2C, INITIALIZE);
    /* Server Info */
    Response->WorldType = (UInt32)Runtime->Environment.RawValue;
    Response->Server.ServerID = Context->Config.WorldSvr.GroupIndex;
    Response->Server.WorldServerID = Context->Config.WorldSvr.NodeIndex;
    Response->Server.PlayerCount = SocketGetConnectionCount(Context->ClientSocket);
    Response->Server.MaxPlayerCount = Context->Config.WorldSvr.MaxConnectionCount;
    memcpy(Response->Server.Address.Host, Context->Config.WorldSvr.Host, strlen(Context->Config.WorldSvr.Host));
    Response->Server.Address.Port = Context->Config.WorldSvr.Port;
    Response->Server.WorldType = (UInt32)Runtime->Environment.RawValue;

    /* Character Info */
    Response->WorldIndex = Packet->CharacterData.Info.Position.WorldID;
    Response->DungeonIndex = (UInt32)Packet->CharacterData.Info.Position.DungeonIndex;
    Response->Position.X = Packet->CharacterData.Info.Position.X;
    Response->Position.Y = Packet->CharacterData.Info.Position.Y;
    Response->Exp = Packet->CharacterData.Info.Basic.Exp;
    Response->Alz = Packet->CharacterData.Info.Alz;
    Response->Wexp = Packet->CharacterData.Info.Honor.Exp;
    Response->Level = Packet->CharacterData.Info.Basic.Level;
    Response->STR = Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_STR];
    Response->DEX = Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_DEX];
    Response->INT = Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_INT];
    Response->PNT = Packet->CharacterData.Info.Stat[RUNTIME_CHARACTER_STAT_PNT];
    Response->SkillRank = Packet->CharacterData.Info.Skill.Rank;
    Response->SkillLevel = Packet->CharacterData.Info.Skill.Level;
    Response->SkillExp = (UInt32)Packet->CharacterData.Info.Skill.Exp;
    Response->SkillPoint = Packet->CharacterData.Info.Skill.Point;
    Response->RestExp = 0;
    Response->Nation = Packet->CharacterData.Info.Profile.Nation;
    Response->HonorPoint = Packet->CharacterData.Info.Honor.Point;
    Response->WarpMask = Packet->CharacterData.Info.Profile.WarpMask;
    Response->MapsMask = Packet->CharacterData.Info.Profile.MapsMask;

    memcpy(Response->ChatServerAddress.Host, Context->Config.ChatSvr.Host, strlen(Context->Config.ChatSvr.Host));
    Response->ChatServerAddress.Port = Context->Config.ChatSvr.Port;
    memcpy(Response->AuctionServerAddress.Host, Context->Config.AuctionSvr.Host, strlen(Context->Config.AuctionSvr.Host));
    Response->AuctionServerAddress.Port = Context->Config.AuctionSvr.Port;
    memcpy(Response->PartyServerAddress.Host, Context->Config.PartySvr.Host, strlen(Context->Config.PartySvr.Host));
    Response->PartyServerAddress.Port = Context->Config.PartySvr.Port;

    Response->CharacterStyle = SwapUInt32(Packet->CharacterData.Info.Style.RawValue);
    Response->CharacterLiveStyle = SwapUInt32(Packet->CharacterData.Info.LiveStyle.RawValue);
    Response->ForceGem = Packet->CharacterData.AccountInfo.ForceGem;

    memcpy(&Response->ResearchSupportInfo, &Packet->CharacterData.ResearchSupportInfo, sizeof(struct _RTCharacterResearchSupportInfo));

    Response->NameLength = strlen(Packet->CharacterName) + 1;
    CString Name = (CString)PacketBufferAppend(ClientConnection->PacketBuffer, strlen(Packet->CharacterName));
    memcpy(Name, Packet->CharacterName, strlen(Packet->CharacterName));

    Response->EquipmentInfo = Packet->CharacterData.EquipmentInfo.Info;
    if (Packet->CharacterData.EquipmentInfo.Info.EquipmentSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.EquipmentInfo.EquipmentSlots,
            sizeof(struct _RTItemSlot) * Packet->CharacterData.EquipmentInfo.Info.EquipmentSlotCount
        );
    }

    if (Packet->CharacterData.EquipmentInfo.Info.InventorySlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.EquipmentInfo.InventorySlots,
            sizeof(struct _RTItemSlot) * Packet->CharacterData.EquipmentInfo.Info.InventorySlotCount
        );
    }

    if (Packet->CharacterData.EquipmentInfo.Info.LinkSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.EquipmentInfo.LinkSlots,
            sizeof(struct _RTEquipmentLinkSlot) * Packet->CharacterData.EquipmentInfo.Info.LinkSlotCount
        );
    }

    if (Packet->CharacterData.EquipmentInfo.Info.LockSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.EquipmentInfo.LockSlots,
            sizeof(struct _RTEquipmentLockSlot) * Packet->CharacterData.EquipmentInfo.Info.LockSlotCount
        );
    }

    Response->InventoryInfo = Packet->CharacterData.InventoryInfo.Info;
    if (Packet->CharacterData.InventoryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.InventoryInfo.Slots,
            sizeof(struct _RTItemSlot) * Packet->CharacterData.InventoryInfo.Info.SlotCount
        );
    }

    Response->SkillSlotInfo = Packet->CharacterData.SkillSlotInfo.Info;
    if (Packet->CharacterData.SkillSlotInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.SkillSlotInfo.Skills,
            sizeof(struct _RTSkillSlot) * Packet->CharacterData.SkillSlotInfo.Info.SlotCount
        );
    }

    Response->QuickSlotInfo = Packet->CharacterData.QuickSlotInfo.Info;
    if (Packet->CharacterData.QuickSlotInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.QuickSlotInfo.QuickSlots,
            sizeof(struct _RTQuickSlot) * Packet->CharacterData.QuickSlotInfo.Info.SlotCount
        );
    }

    Response->AbilityInfo = Packet->CharacterData.AbilityInfo.Info;
    if (Packet->CharacterData.AbilityInfo.Info.EssenceAbilityCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AbilityInfo.EssenceAbilitySlots,
            sizeof(struct _RTEssenceAbilitySlot) * Packet->CharacterData.AbilityInfo.Info.EssenceAbilityCount
        );
    }

    if (Packet->CharacterData.AbilityInfo.Info.BlendedAbilityCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AbilityInfo.BlendedAbilitySlots,
            sizeof(struct _RTBlendedAbilitySlot) * Packet->CharacterData.AbilityInfo.Info.BlendedAbilityCount
        );
    }

    if (Packet->CharacterData.AbilityInfo.Info.KarmaAbilityCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AbilityInfo.KarmaAbilitySlots,
            sizeof(struct _RTKarmaAbilitySlot) * Packet->CharacterData.AbilityInfo.Info.KarmaAbilityCount
        );
    }
    
    Response->BlessingBeadInfo = Packet->CharacterData.BlessingBeadInfo.Info;
    if (Packet->CharacterData.BlessingBeadInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.BlessingBeadInfo.Slots,
            sizeof(struct _RTBlessingBeadSlot) * Packet->CharacterData.BlessingBeadInfo.Info.SlotCount
        );
    }

    Response->PremiumServiceInfo = Packet->CharacterData.PremiumServiceInfo.Info;
    if (Packet->CharacterData.PremiumServiceInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.PremiumServiceInfo.Slots,
            sizeof(struct _RTPremiumServiceSlot) * Packet->CharacterData.PremiumServiceInfo.Info.SlotCount
        );
    }

    Response->QuestInfo = Packet->CharacterData.QuestInfo.Info;
    for (Int32 Index = 0; Index < Packet->CharacterData.QuestInfo.Info.SlotCount; Index += 1) {
        RTQuestSlotRef QuestSlot = &Packet->CharacterData.QuestInfo.Slots[Index];
        if (!QuestSlot->QuestIndex) continue;

        RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
        assert(Quest);

        UInt16 NpcActionFlags = 0;
        for (Int32 Index = 0; Index < QuestSlot->NpcActionIndex; Index += 1) {
            NpcActionFlags |= 1 << Index;
        }

        S2C_DATA_INITIALIZE_QUEST_INDEX* QuestResponse = PacketBufferAppendStruct(ClientConnection->PacketBuffer, S2C_DATA_INITIALIZE_QUEST_INDEX);
        QuestResponse->QuestIndex = QuestSlot->QuestIndex;
        QuestResponse->NpcActionFlags = NpcActionFlags;
        QuestResponse->DisplayNotice = QuestSlot->DisplayNotice;
        QuestResponse->DisplayOpenNotice = QuestSlot->DisplayOpenNotice;
        QuestResponse->QuestSlotIndex = QuestSlot->SlotIndex;

        Int32 CounterCount = Quest->MissionMobCount + Quest->MissionItemCount;
        for (Int32 CounterIndex = 0; CounterIndex < CounterCount; CounterIndex++) {
            PacketBufferAppendValue(ClientConnection->PacketBuffer, UInt8, QuestSlot->Counter[CounterIndex]);
        }
    }

    Response->DailyQuestInfo = Packet->CharacterData.DailyQuestInfo.Info;
    if (Packet->CharacterData.DailyQuestInfo.Info.Count > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.DailyQuestInfo.Slots,
            sizeof(struct _RTDailyQuestSlot) * Packet->CharacterData.DailyQuestInfo.Info.Count
        );
    }

    Response->MercenaryInfo = Packet->CharacterData.MercenaryInfo.Info;
    if (Packet->CharacterData.BlessingBeadInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.MercenaryInfo.Slots,
            sizeof(struct _RTMercenarySlot) * Packet->CharacterData.MercenaryInfo.Info.SlotCount
        );
    }

    Response->AppearanceInfo = Packet->CharacterData.AppearanceInfo.Info;
    if (Packet->CharacterData.AppearanceInfo.Info.EquipmentAppearanceCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AppearanceInfo.EquipmentSlots,
            sizeof(struct _RTItemSlotAppearance) * Packet->CharacterData.AppearanceInfo.Info.EquipmentAppearanceCount
        );
    }

    if (Packet->CharacterData.AppearanceInfo.Info.InventoryAppearanceCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AppearanceInfo.InventorySlots,
            sizeof(struct _RTItemSlotAppearance) * Packet->CharacterData.AppearanceInfo.Info.InventoryAppearanceCount
        );
    }
    
    Response->AchievementInfo = Packet->CharacterData.AchievementInfo.Info;
    if (Packet->CharacterData.AchievementInfo.Info.AchievementCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AchievementInfo.AchievementSlots,
            sizeof(struct _RTAchievementSlot) * Packet->CharacterData.AchievementInfo.Info.AchievementCount
        );
    }

    if (Packet->CharacterData.AchievementInfo.Info.AchievementCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AchievementInfo.AchievementRewardSlots,
            sizeof(struct _RTAchievementRewardSlot) * Packet->CharacterData.AchievementInfo.Info.AchievementRewardCount
        );
    }

    Response->AchievementExtendedInfo = Packet->CharacterData.AchievementExtendedInfo.Info;
    if (Packet->CharacterData.AchievementExtendedInfo.Info.AchievementExtendedRewardCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AchievementExtendedInfo.AchievementExtendedRewardSlots,
            sizeof(struct _RTAchievementExtendedRewardSlot) * Packet->CharacterData.AchievementExtendedInfo.Info.AchievementExtendedRewardCount
        );
    }

    // TODO: BuffData

    Response->CraftInfo = Packet->CharacterData.CraftInfo.Info;
    // TODO: CraftData
    
    Response->RequestCraftInfo = Packet->CharacterData.RequestCraftInfo.Info;
    // TODO: RequestCraftData

    Response->VehicleInventoryInfo = Packet->CharacterData.VehicleInventoryInfo.Info;
    if (Packet->CharacterData.VehicleInventoryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.VehicleInventoryInfo.Slots,
            sizeof(struct _RTItemSlot) * Packet->CharacterData.VehicleInventoryInfo.Info.SlotCount
        );
    }

    Response->GoldMeritMasteryInfo = Packet->CharacterData.GoldMeritMasteryInfo.Info;
    if (Packet->CharacterData.GoldMeritMasteryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.GoldMeritMasteryInfo.Slots,
            sizeof(struct _RTGoldMeritMasterySlot) * Packet->CharacterData.GoldMeritMasteryInfo.Info.SlotCount
        );
    }

    Response->PlatinumMeritMasteryInfo = Packet->CharacterData.PlatinumMeritMasteryInfo.Info;
    if (Packet->CharacterData.PlatinumMeritMasteryInfo.Info.ExtendedMemorizeCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots,
            sizeof(struct _RTPlatinumMeritExtendedMemorizeSlot) * Packet->CharacterData.PlatinumMeritMasteryInfo.Info.ExtendedMemorizeCount
        );
    }

    if (Packet->CharacterData.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.PlatinumMeritMasteryInfo.UnlockedSlots,
            sizeof(struct _RTPlatinumMeritUnlockedSlot) * Packet->CharacterData.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount
        );
    }

    if (Packet->CharacterData.PlatinumMeritMasteryInfo.Info.MasterySlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.PlatinumMeritMasteryInfo.MasterySlots,
            sizeof(struct _RTPlatinumMeritMasterySlot) * Packet->CharacterData.PlatinumMeritMasteryInfo.Info.MasterySlotCount
        );
    }

    if (Packet->CharacterData.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.PlatinumMeritMasteryInfo.SpecialMasterySlots,
            sizeof(struct _RTPlatinumMeritSpecialMasterySlot) * Packet->CharacterData.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount
        );
    }

    Response->DiamondMeritMasteryInfo = Packet->CharacterData.DiamondMeritMasteryInfo.Info;
    if (Packet->CharacterData.DiamondMeritMasteryInfo.Info.ExtendedMemorizeCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.DiamondMeritMasteryInfo.ExtendedMemorizeSlots,
            sizeof(struct _RTPlatinumMeritExtendedMemorizeSlot) * Packet->CharacterData.DiamondMeritMasteryInfo.Info.ExtendedMemorizeCount
        );
    }

    if (Packet->CharacterData.DiamondMeritMasteryInfo.Info.UnlockedSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.DiamondMeritMasteryInfo.UnlockedSlots,
            sizeof(struct _RTPlatinumMeritUnlockedSlot) * Packet->CharacterData.DiamondMeritMasteryInfo.Info.UnlockedSlotCount
        );
    }

    if (Packet->CharacterData.DiamondMeritMasteryInfo.Info.MasterySlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.DiamondMeritMasteryInfo.MasterySlots,
            sizeof(struct _RTPlatinumMeritMasterySlot) * Packet->CharacterData.DiamondMeritMasteryInfo.Info.MasterySlotCount
        );
    }

    if (Packet->CharacterData.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.DiamondMeritMasteryInfo.SpecialMasterySlots,
            sizeof(struct _RTPlatinumMeritSpecialMasterySlot) * Packet->CharacterData.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount
        );
    }

    Response->WarpServiceInfo = Packet->CharacterData.WarpServiceInfo.Info;
    if (Packet->CharacterData.WarpServiceInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.WarpServiceInfo.Slots,
            sizeof(struct _RTWarpServiceSlot) * Packet->CharacterData.WarpServiceInfo.Info.SlotCount
        );
    }

    Response->OverlordMasteryInfo = Packet->CharacterData.OverlordMasteryInfo.Info;
    if (Packet->CharacterData.OverlordMasteryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.OverlordMasteryInfo.Slots,
            sizeof(struct _RTOverlordMasterySlot) * Packet->CharacterData.OverlordMasteryInfo.Info.SlotCount
        );
    }

    Response->HonorMedalInfo = Packet->CharacterData.HonorMedalInfo.Info;
    if (Packet->CharacterData.HonorMedalInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.HonorMedalInfo.Slots,
            sizeof(struct _RTHonorMedalSlot) * Packet->CharacterData.HonorMedalInfo.Info.SlotCount
        );
    }

    Response->ForceWingInfo = Packet->CharacterData.ForceWingInfo.Info;
    if (Packet->CharacterData.ForceWingInfo.Info.PresetSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.ForceWingInfo.PresetSlots,
            sizeof(struct _RTForceWingPresetSlot) * Packet->CharacterData.ForceWingInfo.Info.PresetSlotCount
        );
    }

    if (Packet->CharacterData.ForceWingInfo.Info.TrainingSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.ForceWingInfo.TrainingSlots,
            sizeof(struct _RTForceWingTrainingSlot) * Packet->CharacterData.ForceWingInfo.Info.TrainingSlotCount
        );
    }

    Response->GiftBoxInfo = Character->Data.GiftboxInfo.Info;
    if (Character->Data.GiftboxInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Character->Data.GiftboxInfo.Slots,
            sizeof(struct _RTGiftBoxSlot) * Character->Data.GiftboxInfo.Info.SlotCount
        );
    }

    Response->CollectionInfo = Packet->CharacterData.CollectionInfo.Info;
    if (Packet->CharacterData.CollectionInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.CollectionInfo.Slots,
            sizeof(struct _RTCollectionSlot) * Packet->CharacterData.CollectionInfo.Info.SlotCount
        );
    }

    Response->TransformInfo = Packet->CharacterData.TransformInfo.Info;
    if (Packet->CharacterData.TransformInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.TransformInfo.Slots,
            sizeof(struct _RTTransformSlot) * Packet->CharacterData.TransformInfo.Info.SlotCount
        );
    }

    Response->TranscendenceInfo = Packet->CharacterData.TranscendenceInfo.Info;
    if (Packet->CharacterData.TranscendenceInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.TranscendenceInfo.Slots,
            sizeof(struct _RTTranscendenceSlot) * Packet->CharacterData.TranscendenceInfo.Info.SlotCount
        );
    }

    // TODO: Add missing data...

    Response->StellarMasteryInfo = Packet->CharacterData.StellarMasteryInfo.Info;
    if (Packet->CharacterData.StellarMasteryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.StellarMasteryInfo.Slots,
            sizeof(struct _RTStellarMasterySlot) * Packet->CharacterData.StellarMasteryInfo.Info.SlotCount
        );
    }

    Response->MythMasteryInfo = Packet->CharacterData.MythMasteryInfo.Info;
    if (Packet->CharacterData.MythMasteryInfo.Info.PropertySlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.MythMasteryInfo.Slots,
            sizeof(struct _RTMythMasterySlot) * Packet->CharacterData.MythMasteryInfo.Info.PropertySlotCount
        );
    }

    Response->NewbieSupportInfo = Packet->CharacterData.NewbieSupportInfo.Info;
    if (Packet->CharacterData.NewbieSupportInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.NewbieSupportInfo.Slots,
            sizeof(struct _RTNewbieSupportSlot) * Packet->CharacterData.NewbieSupportInfo.Info.SlotCount
        );
    }

    Response->EventPassInfo = Packet->CharacterData.EventPassInfo.Info;
    if (Packet->CharacterData.EventPassInfo.Info.MissionSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.EventPassInfo.MissionSlots,
            sizeof(struct _RTEventPassMissionSlot) * Packet->CharacterData.EventPassInfo.Info.MissionSlotCount
        );
    }

    if (Packet->CharacterData.EventPassInfo.Info.RewardSlotCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.EventPassInfo.RewardSlots,
            sizeof(struct _RTEventPassRewardSlot) * Packet->CharacterData.EventPassInfo.Info.RewardSlotCount
        );
    }

    Response->CostumeWarehouseInfo = Packet->CharacterData.CostumeWarehouseInfo.Info;

    Response->CostumeInfo = Packet->CharacterData.CostumeInfo.Info;
    /*
    
struct _RTAccountCostumeSlot {
    UInt32 ItemID;
    UInt32 Unknown1;
};

struct _RTCostumePage {
    Int32 PageIndex;
    UInt32 CostumeSlots[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT];
};

struct _RTAppliedCostumeSlot {
    UInt32 ItemID;
};

struct _RTCharacterCostumeWarehouseInfo {
    Int32 AccountCostumeSlotCount;
    struct _RTAccountCostumeSlot AccountCostumeSlots[RUNTIME_CHARACTER_MAX_ACCOUNT_COSTUME_SLOT_COUNT];
};

struct _RTCharacterCostumeInfo {
    Int32 ActivePageIndex;
    Int32 PageCount;
    struct _RTCostumePage Pages[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_COUNT];
    Int32 AppliedSlotCount;
    struct _RTAppliedCostumeSlot AppliedSlots[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT];
};
*/

    Response->AnimaMasteryInfo = Packet->CharacterData.AnimaMasteryInfo.Info;
    if (Packet->CharacterData.AnimaMasteryInfo.Info.PresetCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AnimaMasteryInfo.PresetData,
            sizeof(struct _RTAnimaMasteryPresetData) * Packet->CharacterData.AnimaMasteryInfo.Info.PresetCount
        );
    }

    if (Packet->CharacterData.AnimaMasteryInfo.Info.StorageCount > 0) {
        PacketBufferAppendCopy(
            ClientConnection->PacketBuffer,
            Packet->CharacterData.AnimaMasteryInfo.CategoryData,
            sizeof(struct _RTAnimaMasteryCategoryData) * Packet->CharacterData.AnimaMasteryInfo.Info.StorageCount
        );
    }

    Response->Entity = Character->ID;

    RTWorldContextRef World = RTRuntimeGetWorldByCharacter(Runtime, Character);

    if ((World->WorldData->Type == RUNTIME_WORLD_TYPE_QUEST_DUNGEON ||
        World->WorldData->Type == RUNTIME_WORLD_TYPE_DUNGEON) &&
        World->DungeonIndex != Character->Data.Info.Position.DungeonIndex) {

        RTDungeonDataRef DungeonData = RTRuntimeGetDungeonDataByID(Runtime, Character->Data.Info.Position.DungeonIndex);
        if (!DungeonData) goto error;

        RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, DungeonData->FailWarpNpcID);
        RTWorldContextRef TargetWorld = World;
        if (World->WorldData->WorldIndex != WarpPoint.WorldIndex) {
            TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
            assert(TargetWorld);
        }

        World = TargetWorld;
        Character->Data.Info.Position.X = WarpPoint.X;
        Character->Data.Info.Position.Y = WarpPoint.Y;
        Character->Data.Info.Position.WorldID = WarpPoint.WorldIndex;
        Character->Data.Info.Position.DungeonIndex = TargetWorld->DungeonIndex;
        Response->WorldIndex = Character->Data.Info.Position.WorldID;
        Response->Position.X = Character->Data.Info.Position.X;
        Response->Position.Y = Character->Data.Info.Position.Y;
    }

    // TODO: Check if max hp or base hp is requested here...
    Response->BaseHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    Response->MaxMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
    Response->MaxSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX];
    Response->MaxBP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX];
    Response->MaxRage = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX];

    Response->CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    Response->CurrentMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
    Response->CurrentSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
    Response->CurrentBP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
    Response->CurrentRage = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];

    Response->MapsMask = 0xFFFFFFFF;
    Response->WarpMask = 0xFFFFFFFF;
    /*
    Response->Unknown7[0] = 113;
    Response->Unknown7[1] = 201;
    Response->Unknown7[2] = 9;
    Response->UnknownSkillRank = Response->SkillRank + 1;
    Response->Unknown1[55] = 1;
    Response->Unknown1[58] = 20;
    */
    //memset(Response->DungeonFlagInfo.Flags, 0xFF, 640);

    /* this contains counters
    Int32 Seed = Character->Data.Info.Position.X;
    for (Int32 Index = 0; Index < 282; Index++)
        Response->Unknown14[Index] = Random(&Seed);
        */

    SocketSend(Context->ClientSocket, ClientConnection, Response);

    if (!RTCharacterIsAlive(Runtime, Character)) {
        RTWarpPointResult WarpPoint = RTRuntimeGetWarpPoint(Runtime, Character, World->WorldData->DeadWarpIndex);
        RTWorldContextRef TargetWorld = World;
        if (World->WorldData->WorldIndex != WarpPoint.WorldIndex) {
            TargetWorld = RTRuntimeGetWorldByID(Runtime, WarpPoint.WorldIndex);
            assert(TargetWorld);
        }
        
        Character->Data.Info.Resource.HP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
        Character->Data.Info.Resource.MP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
        Character->Data.Info.Position.X = WarpPoint.X;
        Character->Data.Info.Position.Y = WarpPoint.Y;
        Character->Data.Info.Position.WorldID = WarpPoint.WorldIndex;
        Character->Data.Info.Position.DungeonIndex = TargetWorld->DungeonIndex;
        RTCharacterInitializeAttributes(Runtime, Character);

        RTMovementInitialize(
            Runtime,
            &Character->Movement,
            WarpPoint.X,
            WarpPoint.Y,
            RUNTIME_MOVEMENT_SPEED_BASE,
            RUNTIME_WORLD_TILE_WALL
        );
    }

    RTWorldSpawnCharacter(Runtime, World, Character->ID);
    
    RTRuntimeDataEventHotReload(
        Context->Runtime->Context,
        Context->Config.WorldSvr.RuntimeDataPath,
        Context->Config.WorldSvr.ServerDataPath,
        "Event.xml"
    );

    // TODO: Move event data to database and trigger request on init
    SendCharacterStatus(Context, Context->ClientSocket, Client->Connection, Character);
    SendEventInfo(Context, Client);
    SendEventList(Context, Client);
    SendGiftBoxPricePoolList(Context, Client);
    SendPremiumServiceList(Context, Context->ClientSocket, Client->Connection);
    BroadcastUserList(Server, Context);

    IPC_W2P_DATA_CLIENT_CONNECT* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2P, CLIENT_CONNECT);
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.SourceConnectionID = Client->Connection->ID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_PARTY;
    Request->CharacterIndex = Character->CharacterIndex;
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
	if (ClientConnection) {
		SocketDisconnect(Context->ClientSocket, ClientConnection);
	}
}
