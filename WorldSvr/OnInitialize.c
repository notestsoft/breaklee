#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

Void SendEventList(
    ServerContextRef Context,
    ClientContextRef Client
) {
    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);

    S2C_DATA_NFY_EVENT_LIST* EventList = PacketBufferInit(PacketBuffer, S2C, NFY_EVENT_LIST);
    EventList->Count = Context->Runtime->Context->EventCount;

    for (Int Index = 0; Index < Context->Runtime->Context->EventCount; Index += 1) {
        RTDataEventRef EventData = &Context->Runtime->Context->EventList[Index];
        PacketBufferAppendValue(PacketBuffer, UInt32, EventData->ID);
    }

    SocketSend(Context->ClientSocket, Client->Connection, EventList);
}

Void SendEventInfo(
    ServerContextRef Context,
    ClientContextRef Client
) {
    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);

    S2C_DATA_NFY_EVENT_INFO* EventInfo = PacketBufferInitExtended(PacketBuffer, S2C, NFY_EVENT_INFO);
    EventInfo->HasEventInfo = (Context->Runtime->Context->EventCount > 0) ? 1 : 0;

    S2C_DATA_NFY_EVENT_INFO_HEADER* EventInfoHeader = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_NFY_EVENT_INFO_HEADER);
    EventInfoHeader->EventCount = Context->Runtime->Context->EventCount;

    for (Int EventIndex = 0; EventIndex < Context->Runtime->Context->EventCount; EventIndex += 1) {
        RTDataEventRef EventData = &Context->Runtime->Context->EventList[EventIndex];

        S2C_DATA_NFY_EVENT_INFO_EVENT* EventInfoEvent = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT);
        EventInfoEvent->EventIndex = EventData->ID;
        EventInfoEvent->EventType = EventData->Type;
        EventInfoEvent->EventFlags = EventData->UseFlags;
        EventInfoEvent->EventStartTimestamp = GetTimestamp() - 1000000;
        EventInfoEvent->EventEndTimestamp = GetTimestamp() + 1000000;
        EventInfoEvent->EventUpdateTimestamp = 0;
        EventInfoEvent->WorldIndex = EventData->WorldIndex;
        EventInfoEvent->NpcIndex = EventData->NpcIndex;
        PacketBufferAppendCString(PacketBuffer, EventData->Description);

        S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO* EventItemInfo = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO);
        EventItemInfo->ItemCount = EventData->EventItemCount;

        for (Int ItemIndex = 0; ItemIndex < EventData->EventItemCount; ItemIndex += 1) {
            RTDataEventItemRef EventItem = &EventData->EventItemList[ItemIndex];

            S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO_DATA* EventItemData = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO_DATA);
            EventItemData->ItemID.Serial = EventItem->ItemID;
            EventItemData->ItemOptions = EventItem->ItemOptions;
            EventItemData->ExternalID = EventItem->ExternalID;
            EventItemData->TextureItemID = EventItem->TextureItemID;
            PacketBufferAppendCString(PacketBuffer, EventItem->Title);
            PacketBufferAppendCString(PacketBuffer, EventItem->Description);
        }

        EventItemInfo = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO);
        EventItemInfo->ItemCount = 0;

        EventItemInfo = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_NFY_EVENT_INFO_EVENT_ITEM_INFO);
        EventItemInfo->ItemCount = 0;
    }

    SocketSend(Context->ClientSocket, Client->Connection, EventInfo);
}

CLIENT_PROCEDURE_BINDING(INITIALIZE) {
    if (Character) goto error;
	
    if (!(Client->Flags & CLIENT_FLAGS_CHARACTER_INDEX_LOADED) || Client->AccountID < 1) goto error;

    IPC_W2D_DATA_GET_CHARACTER* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, GET_CHARACTER);
    Request->Header.SourceConnectionID = Connection->ID;
    Request->Header.Source = Server->IPCSocket->NodeID;
    Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
    Request->CharacterIndex = Packet->CharacterIndex;
    IPCSocketUnicast(Server->IPCSocket, Request);
    return;

error:
	SocketDisconnect(Socket, Connection);
}

IPC_PROCEDURE_BINDING(D2W, GET_CHARACTER) {
    if (!ClientConnection || !Client) goto error;
    if (!Packet->Success) goto error;

    Character = RTWorldManagerCreateCharacter(Context->Runtime->WorldManager, Packet->CharacterIndex);
    Character->DungeonEntryItemSlotIndex = -1;
    Character->SyncMask.RawValue = 0;
    Character->SyncTimestamp = PlatformGetTickCount();
    Character->Data.AccountInfo = Client->AccountInfo;
    Character->Data.Info = Packet->Character.CharacterInfo;
    Character->Data.StyleInfo = Packet->Character.CharacterStyleInfo;
    Character->Data.DailyResetInfo = Packet->Character.DailyResetInfo;

    if (Runtime->InstantWarManager) {
        RTInstantWarSetPosition(Runtime, Runtime->InstantWarManager, Character);
    }

    UInt8* Memory = &Packet->Data[0];

    Character->Data.EquipmentInfo.Info = Packet->Character.EquipmentInfo;
    if (Packet->Character.EquipmentInfo.EquipmentSlotCount > 0) {
        Int32 Length = sizeof(struct _RTItemSlot) * Packet->Character.EquipmentInfo.EquipmentSlotCount;
        memcpy(Character->Data.EquipmentInfo.EquipmentSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.EquipmentInfo.InventorySlotCount > 0) {
        Int32 Length = sizeof(struct _RTItemSlot) * Packet->Character.EquipmentInfo.InventorySlotCount;
        memcpy(Character->Data.EquipmentInfo.InventorySlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.EquipmentInfo.LinkSlotCount > 0) {
        Int32 Length = sizeof(struct _RTEquipmentLinkSlot) * Packet->Character.EquipmentInfo.LinkSlotCount;
        memcpy(Character->Data.EquipmentInfo.LinkSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.EquipmentInfo.LockSlotCount > 0) {
        Int32 Length = sizeof(struct _RTEquipmentLockSlot) * Packet->Character.EquipmentInfo.LockSlotCount;
        memcpy(Character->Data.EquipmentInfo.LockSlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.InventoryInfo.Info = Packet->Character.InventoryInfo;
    if (Packet->Character.InventoryInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTItemSlot) * Packet->Character.InventoryInfo.SlotCount;
        memcpy(Character->Data.InventoryInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.VehicleInventoryInfo.Info = Packet->Character.VehicleInventoryInfo;
    if (Packet->Character.VehicleInventoryInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTItemSlot) * Packet->Character.VehicleInventoryInfo.SlotCount;
        memcpy(Character->Data.VehicleInventoryInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.SkillSlotInfo.Info = Packet->Character.SkillSlotInfo;
    if (Packet->Character.SkillSlotInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTSkillSlot) * Packet->Character.SkillSlotInfo.SlotCount;
        memcpy(Character->Data.SkillSlotInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.QuickSlotInfo.Info = Packet->Character.QuickSlotInfo;
    if (Packet->Character.QuickSlotInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTQuickSlot) * Packet->Character.QuickSlotInfo.SlotCount;
        memcpy(Character->Data.QuickSlotInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.MercenaryInfo.Info = Packet->Character.MercenaryInfo;
    if (Packet->Character.MercenaryInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTMercenarySlot) * Packet->Character.MercenaryInfo.SlotCount;
        memcpy(Character->Data.MercenaryInfo.Slots, Memory, Length);
        Memory += Length;
    }

    // UInt16 ItemPeriodCount;

    Character->Data.AbilityInfo.Info = Packet->Character.AbilityInfo;
    if (Packet->Character.AbilityInfo.EssenceAbilityCount > 0) {
        Int32 Length = sizeof(struct _RTEssenceAbilitySlot) * Packet->Character.AbilityInfo.EssenceAbilityCount;
        memcpy(Character->Data.AbilityInfo.EssenceAbilitySlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.AbilityInfo.BlendedAbilityCount > 0) {
        Int32 Length = sizeof(struct _RTBlendedAbilitySlot) * Packet->Character.AbilityInfo.BlendedAbilityCount;
        memcpy(Character->Data.AbilityInfo.BlendedAbilitySlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.AbilityInfo.KarmaAbilityCount > 0) {
        Int32 Length = sizeof(struct _RTKarmaAbilitySlot) * Packet->Character.AbilityInfo.KarmaAbilityCount;
        memcpy(Character->Data.AbilityInfo.KarmaAbilitySlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.BlessingBeadInfo.Info = Packet->Character.BlessingBeadInfo;
    if (Packet->Character.BlessingBeadInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTBlessingBeadSlot) * Packet->Character.BlessingBeadInfo.SlotCount;
        memcpy(Character->Data.BlessingBeadInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.PremiumServiceInfo.Info = Packet->Character.PremiumServiceInfo;
    if (Packet->Character.PremiumServiceInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTPremiumServiceSlot) * Packet->Character.PremiumServiceInfo.SlotCount;
        memcpy(Character->Data.PremiumServiceInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.QuestInfo.Info = Packet->Character.QuestInfo;
    if (Packet->Character.QuestInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTQuestSlot) * Packet->Character.QuestInfo.SlotCount;
        memcpy(Character->Data.QuestInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.DailyQuestInfo.Info = Packet->Character.DailyQuestInfo;
    if (Packet->Character.DailyQuestInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTDailyQuestSlot) * Packet->Character.DailyQuestInfo.SlotCount;
        memcpy(Character->Data.DailyQuestInfo.Slots, Memory, Length);
        Memory += Length;
    }
    
    Character->Data.HelpInfo = Packet->Character.HelpInfo;

    Character->Data.AppearanceInfo.Info = Packet->Character.AppearanceInfo;
    if (Packet->Character.AppearanceInfo.EquipmentAppearanceCount > 0) {
        Int32 Length = sizeof(struct _RTItemSlotAppearance) * Packet->Character.AppearanceInfo.EquipmentAppearanceCount;
        memcpy(Character->Data.AppearanceInfo.EquipmentSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.AppearanceInfo.InventoryAppearanceCount > 0) {
        Int32 Length = sizeof(struct _RTItemSlotAppearance) * Packet->Character.AppearanceInfo.InventoryAppearanceCount;
        memcpy(Character->Data.AppearanceInfo.InventorySlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.AchievementInfo.Info = Packet->Character.AchievementInfo;
    if (Packet->Character.AchievementInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTAchievementSlot) * Packet->Character.AchievementInfo.SlotCount;
        memcpy(Character->Data.AchievementInfo.Slots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.AchievementInfo.RewardSlotCount > 0) {
        Int32 Length = sizeof(struct _RTAchievementRewardSlot) * Packet->Character.AchievementInfo.RewardSlotCount;
        memcpy(Character->Data.AchievementInfo.RewardSlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.CraftInfo.Info = Packet->Character.CraftInfo;
    if (Packet->Character.CraftInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTCraftSlot) * Packet->Character.CraftInfo.SlotCount;
        memcpy(Character->Data.CraftInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.RequestCraftInfo.Info = Packet->Character.RequestCraftInfo;
    if (Packet->Character.RequestCraftInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTRequestCraftSlot) * Packet->Character.RequestCraftInfo.SlotCount;
        memcpy(Character->Data.RequestCraftInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.CooldownInfo.Info = Packet->Character.CooldownInfo;
    if (Packet->Character.CooldownInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTCooldownSlot) * Packet->Character.CooldownInfo.SlotCount;
        memcpy(Character->Data.CooldownInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.UpgradeInfo = Packet->Character.UpgradeInfo;

    Character->Data.GoldMeritMasteryInfo.Info = Packet->Character.GoldMeritMasteryInfo;
    if (Packet->Character.GoldMeritMasteryInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTGoldMeritMasterySlot) * Packet->Character.GoldMeritMasteryInfo.SlotCount;
        memcpy(Character->Data.GoldMeritMasteryInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.PlatinumMeritMasteryInfo.Info = Packet->Character.PlatinumMeritMasteryInfo;
    if (Packet->Character.PlatinumMeritMasteryInfo.TotalMemorizeCount > 0) {
        Int32 Length = sizeof(struct _RTPlatinumMeritExtendedMemorizeSlot) * Packet->Character.PlatinumMeritMasteryInfo.TotalMemorizeCount;
        memcpy(Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount > 0) {
        Int32 Length = sizeof(struct _RTPlatinumMeritUnlockedSlot) * Packet->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount;
        memcpy(Character->Data.PlatinumMeritMasteryInfo.UnlockedSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.PlatinumMeritMasteryInfo.MasterySlotCount > 0) {
        Int32 Length = sizeof(struct _RTPlatinumMeritMasterySlot) * Packet->Character.PlatinumMeritMasteryInfo.MasterySlotCount;
        memcpy(Character->Data.PlatinumMeritMasteryInfo.MasterySlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount > 0) {
        Int32 Length = sizeof(struct _RTPlatinumMeritSpecialMasterySlot) * Packet->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount;
        memcpy(Character->Data.PlatinumMeritMasteryInfo.SpecialMasterySlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.DiamondMeritMasteryInfo.Info = Packet->Character.DiamondMeritMasteryInfo;
    if (Packet->Character.DiamondMeritMasteryInfo.TotalMemorizeCount > 0) {
        Int32 Length = sizeof(struct _RTDiamondMeritExtendedMemorizeSlot) * Packet->Character.DiamondMeritMasteryInfo.TotalMemorizeCount;
        memcpy(Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.DiamondMeritMasteryInfo.UnlockedSlotCount > 0) {
        Int32 Length = sizeof(struct _RTDiamondMeritUnlockedSlot) * Packet->Character.DiamondMeritMasteryInfo.UnlockedSlotCount;
        memcpy(Character->Data.DiamondMeritMasteryInfo.UnlockedSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.DiamondMeritMasteryInfo.MasterySlotCount > 0) {
        Int32 Length = sizeof(struct _RTDiamondMeritMasterySlot) * Packet->Character.DiamondMeritMasteryInfo.MasterySlotCount;
        memcpy(Character->Data.DiamondMeritMasteryInfo.MasterySlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.DiamondMeritMasteryInfo.SpecialMasterySlotCount > 0) {
        Int32 Length = sizeof(struct _RTDiamondMeritSpecialMasterySlot) * Packet->Character.DiamondMeritMasteryInfo.SpecialMasterySlotCount;
        memcpy(Character->Data.DiamondMeritMasteryInfo.SpecialMasterySlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.AchievementExtendedInfo.Info = Packet->Character.AchievementExtendedInfo;
    if (Packet->Character.AchievementExtendedInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTAchievementExtendedRewardSlot) * Packet->Character.AchievementExtendedInfo.SlotCount;
        memcpy(Character->Data.AchievementExtendedInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Client->AccountInfo.ForceGem = Packet->Character.ForceGem;

    Character->Data.WarpServiceInfo.Info = Packet->Character.WarpServiceInfo;
    if (Packet->Character.WarpServiceInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTWarpServiceSlot) * Packet->Character.WarpServiceInfo.SlotCount;
        memcpy(Character->Data.WarpServiceInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.OverlordMasteryInfo.Info = Packet->Character.OverlordMasteryInfo;
    if (Packet->Character.OverlordMasteryInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTOverlordMasterySlot) * Packet->Character.OverlordMasteryInfo.SlotCount;
        memcpy(Character->Data.OverlordMasteryInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.HonorMedalInfo.Info = Packet->Character.HonorMedalInfo;
    if (Packet->Character.HonorMedalInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTHonorMedalSlot) * Packet->Character.HonorMedalInfo.SlotCount;
        memcpy(Character->Data.HonorMedalInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.ForceWingInfo.Info = Packet->Character.ForceWingInfo;
    if (Packet->Character.ForceWingInfo.PresetSlotCount > 0) {
        Int32 Length = sizeof(struct _RTForceWingPresetSlot) * Packet->Character.ForceWingInfo.PresetSlotCount;
        memcpy(Character->Data.ForceWingInfo.PresetSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.ForceWingInfo.TrainingSlotCount > 0) {
        Int32 Length = sizeof(struct _RTForceWingTrainingSlot) * Packet->Character.ForceWingInfo.TrainingSlotCount;
        memcpy(Character->Data.ForceWingInfo.TrainingSlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.GiftboxInfo.Info = Packet->Character.GiftBoxInfo;
    if (Packet->Character.GiftBoxInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTGiftBoxSlot) * Packet->Character.GiftBoxInfo.SlotCount;
        memcpy(Character->Data.GiftboxInfo.Slots, Memory, Length);
        Memory += Length;
        Length = sizeof(struct _RTGiftBoxRewardSlot) * Packet->Character.GiftBoxInfo.SlotCount;
        memcpy(Character->Data.GiftboxInfo.RewardSlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.CollectionInfo.Info = Packet->Character.CollectionInfo;
    if (Packet->Character.CollectionInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTCollectionSlot) * Packet->Character.CollectionInfo.SlotCount;
        memcpy(Character->Data.CollectionInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.TransformInfo.Info = Packet->Character.TransformInfo;
    if (Packet->Character.TransformInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTTransformSlot) * Packet->Character.TransformInfo.SlotCount;
        memcpy(Character->Data.TransformInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.SecretShopInfo = Packet->Character.SecretShopData;

    Character->Data.AuraMasteryInfo.Info = Packet->Character.AuraMasteryInfo;
    if (Packet->Character.AuraMasteryInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTAuraMasterySlot) * Packet->Character.AuraMasteryInfo.SlotCount;
        memcpy(Character->Data.AuraMasteryInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.TranscendenceInfo.Info = Packet->Character.TranscendenceInfo;
    if (Packet->Character.TranscendenceInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTTranscendenceSlot) * Packet->Character.TranscendenceInfo.SlotCount;
        memcpy(Character->Data.TranscendenceInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.DamageBoosterInfo.Info = Packet->Character.DamageBoosterInfo;
    if (Packet->Character.DamageBoosterInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTDamageBoosterSlot) * Packet->Character.DamageBoosterInfo.SlotCount;
        memcpy(Character->Data.DamageBoosterInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.ResearchSupportInfo = Packet->Character.ResearchSupportInfo;

    Character->Data.StellarMasteryInfo.Info = Packet->Character.StellarMasteryInfo;
    if (Packet->Character.StellarMasteryInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTStellarMasterySlot) * Packet->Character.StellarMasteryInfo.SlotCount;
        memcpy(Character->Data.StellarMasteryInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.MythMasteryInfo.Info = Packet->Character.MythMasteryInfo;
    if (Packet->Character.MythMasteryInfo.MasterySlotCount > 0) {
        Int32 Length = sizeof(struct _RTMythMasterySlot) * Packet->Character.MythMasteryInfo.MasterySlotCount;
        memcpy(Character->Data.MythMasteryInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.NewbieSupportInfo.Info = Packet->Character.NewbieSupportInfo;
    if (Packet->Character.NewbieSupportInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTNewbieSupportSlot) * Packet->Character.NewbieSupportInfo.SlotCount;
        memcpy(Character->Data.NewbieSupportInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.EventPassInfo.Info = Packet->Character.EventPassInfo;
    if (Packet->Character.EventPassInfo.MissionSlotCount > 0) {
        Int32 Length = sizeof(struct _RTEventPassMissionSlot) * Packet->Character.EventPassInfo.MissionSlotCount;
        memcpy(Character->Data.EventPassInfo.MissionSlots, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.EventPassInfo.RewardSlotCount > 0) {
        Int32 Length = sizeof(struct _RTEventPassRewardSlot) * Packet->Character.EventPassInfo.RewardSlotCount;
        memcpy(Character->Data.EventPassInfo.RewardSlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.CostumeWarehouseInfo.Info = Packet->Character.CostumeWarehouseInfo;
    if (Packet->Character.CostumeWarehouseInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTAccountCostumeSlot) * Packet->Character.CostumeWarehouseInfo.SlotCount;
        memcpy(Character->Data.CostumeWarehouseInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.CostumeInfo.Info = Packet->Character.CostumeInfo;
    if (Packet->Character.CostumeInfo.PageCount > 0) {
        Int32 Length = sizeof(struct _RTCostumePage) * Packet->Character.CostumeInfo.PageCount;
        memcpy(Character->Data.CostumeInfo.Pages, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.CostumeInfo.AppliedSlotCount > 0) {
        Int32 Length = sizeof(struct _RTAppliedCostumeSlot) * Packet->Character.CostumeInfo.AppliedSlotCount;
        memcpy(Character->Data.CostumeInfo.AppliedSlots, Memory, Length);
        Memory += Length;
    }

    Character->Data.ExplorationInfo.Info = Packet->Character.ExplorationInfo;
    if (Packet->Character.ExplorationInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTExplorationSlot) * Packet->Character.ExplorationInfo.SlotCount;
        memcpy(Character->Data.ExplorationInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.AnimaMasteryInfo.Info = Packet->Character.AnimaMasteryInfo;
    if (Packet->Character.AnimaMasteryInfo.PresetCount > 0) {
        Int32 Length = sizeof(struct _RTAnimaMasteryPresetData) * Packet->Character.AnimaMasteryInfo.PresetCount;
        memcpy(Character->Data.AnimaMasteryInfo.PresetData, Memory, Length);
        Memory += Length;
    }

    if (Packet->Character.AnimaMasteryInfo.StorageCount > 0) {
        Int32 Length = sizeof(struct _RTAnimaMasteryCategoryData) * Packet->Character.AnimaMasteryInfo.StorageCount;
        memcpy(Character->Data.AnimaMasteryInfo.CategoryData, Memory, Length);
        Memory += Length;
    }

    Character->Data.PresetInfo = Packet->Character.PresetInfo;
    Character->Data.SettingsInfo = Packet->SettingsInfo;
    Character->Data.WarehouseInfo.Info = Packet->WarehouseInfo;

    if (Packet->WarehouseInfo.SlotCount > 0) {
        Int32 Length = sizeof(struct _RTItemSlot) * Packet->WarehouseInfo.SlotCount;
        memcpy(Character->Data.WarehouseInfo.Slots, Memory, Length);
        Memory += Length;
    }

    Character->Data.BattleModeInfo = Packet->Character.BattleModeInfo;
    Character->Data.BuffInfo.Info = Packet->Character.BuffInfo;

    Int32 BuffSlotCount = (
        Packet->Character.BuffInfo.SkillBuffCount +
        Packet->Character.BuffInfo.PotionBuffCount +
        Packet->Character.BuffInfo.GmBuffCount +
        Packet->Character.BuffInfo.ForceCaliburBuffCount +
        Packet->Character.BuffInfo.UnknownBuffCount2 +
        Packet->Character.BuffInfo.ForceWingBuffCount +
        Packet->Character.BuffInfo.FirePlaceBuffCount
    );
    if (BuffSlotCount > 0) {
        Int32 Length = sizeof(struct _RTBuffSlot) * BuffSlotCount;
        memcpy(Character->Data.BuffInfo.Slots, Memory, Length);
        Memory += Length;
    }

    CStringCopySafe(Character->Name, RUNTIME_CHARACTER_MAX_NAME_LENGTH + 1, Packet->Character.Name);

    RTCharacterInitialize(Runtime, Character);
    
    Client->CharacterIndex = Packet->CharacterIndex;

    PacketBufferRef PacketBuffer = SocketGetNextPacketBuffer(Context->ClientSocket);
    S2C_DATA_INITIALIZE* Response = PacketBufferInitExtended(PacketBuffer, S2C, INITIALIZE);
    /* Server Info */

    //Response->WorldType = (UInt32)Context->Runtime->Config.IsInstantWar != 0 ? Context->Runtime->Config.IsInstantWar : Context->Runtime->Environment.RawValue;

    Response->WorldType = (UInt32)Context->Runtime->Environment.RawValue;
    Response->IsWarehousePasswordSet = Packet->Character.IsWarehousePasswordSet;
    Response->IsInventoryPasswordSet = Packet->Character.IsInventoryPasswordSet;
    Response->IsWarehouseLocked = Packet->Character.IsWarehouseLocked;
    Response->IsInventoryLocked = Packet->Character.IsInventoryLocked;
    Response->Server.ServerID = Context->Config.WorldSvr.GroupIndex;
    Response->Server.WorldServerID = Context->Config.WorldSvr.NodeIndex;
    Response->Server.PlayerCount = SocketGetConnectionCount(Context->ClientSocket);
    Response->Server.MaxPlayerCount = Context->Config.WorldSvr.MaxConnectionCount;
    memcpy(Response->Server.Address.Host, Context->Config.WorldSvr.Host, strlen(Context->Config.WorldSvr.Host));
    Response->Server.Address.Port = Context->Config.WorldSvr.Port;
    //Response->Server.WorldType = Context->Runtime->Config.IsInstantWar != 0 ? Context->Runtime->Config.IsInstantWar : Context->Runtime->Environment.RawValue;
    Response->Server.WorldType = Context->Runtime->Environment.RawValue;
    Response->Entity = Character->ID;
    Response->CharacterInfo = Character->Data.Info;
    // TODO: Check if max hp or base hp is requested here...
    Response->CharacterInfo.BaseHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_MAX];
    Response->CharacterInfo.MaxMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_MAX];
    Response->CharacterInfo.MaxSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_MAX];
    Response->CharacterInfo.MaxBP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_MAX];
    Response->CharacterInfo.MaxRage = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_MAX];
    Response->CharacterInfo.CurrentHP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_HP_CURRENT];
    Response->CharacterInfo.CurrentMP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_MP_CURRENT];
    Response->CharacterInfo.CurrentSP = Character->Attributes.Values[RUNTIME_ATTRIBUTE_SP_CURRENT];
    Response->CharacterInfo.CurrentBP = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_BP_CURRENT];
    Response->CharacterInfo.CurrentRage = (UInt32)Character->Attributes.Values[RUNTIME_ATTRIBUTE_RAGE_CURRENT];
    memcpy(Response->ChatServerAddress.Host, Context->Config.ChatSvr.Host, strlen(Context->Config.ChatSvr.Host));
    Response->ChatServerAddress.Port = Context->Config.ChatSvr.Port;
    memcpy(Response->AuctionServerAddress.Host, Context->Config.AuctionSvr.Host, strlen(Context->Config.AuctionSvr.Host));
    Response->AuctionServerAddress.Port = Context->Config.AuctionSvr.Port;
    memcpy(Response->PartyServerAddress.Host, Context->Config.PartySvr.Host, strlen(Context->Config.PartySvr.Host));
    Response->PartyServerAddress.Port = Context->Config.PartySvr.Port;
    Response->PartyServerUDPPort = 0;
    Response->CharacterStyleInfo = Character->Data.StyleInfo;
    memcpy(&Response->ResearchSupportInfo, &Character->Data.ResearchSupportInfo, sizeof(struct _RTCharacterResearchSupportInfo));
    Response->NameLength = strlen(Character->Name) + 1;
    CString Name = (CString)PacketBufferAppend(PacketBuffer, strlen(Character->Name));
    memcpy(Name, Character->Name, strlen(Character->Name));

    Response->EquipmentInfo = Character->Data.EquipmentInfo.Info;
    if (Character->Data.EquipmentInfo.Info.EquipmentSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.EquipmentInfo.EquipmentSlots,
            sizeof(struct _RTItemSlot) * Character->Data.EquipmentInfo.Info.EquipmentSlotCount
        );
    }

    if (Character->Data.EquipmentInfo.Info.InventorySlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.EquipmentInfo.InventorySlots,
            sizeof(struct _RTItemSlot) * Character->Data.EquipmentInfo.Info.InventorySlotCount
        );
    }

    if (Character->Data.EquipmentInfo.Info.LinkSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.EquipmentInfo.LinkSlots,
            sizeof(struct _RTEquipmentLinkSlot) * Character->Data.EquipmentInfo.Info.LinkSlotCount
        );
    }

    if (Character->Data.EquipmentInfo.Info.LockSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.EquipmentInfo.LockSlots,
            sizeof(struct _RTEquipmentLockSlot) * Character->Data.EquipmentInfo.Info.LockSlotCount
        );
    }

    Response->InventoryInfo = Character->Data.InventoryInfo.Info;
    if (Character->Data.InventoryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.InventoryInfo.Slots,
            sizeof(struct _RTItemSlot) * Character->Data.InventoryInfo.Info.SlotCount
        );
    }

    Response->SkillSlotInfo = Character->Data.SkillSlotInfo.Info;
    if (Character->Data.SkillSlotInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.SkillSlotInfo.Slots,
            sizeof(struct _RTSkillSlot) * Character->Data.SkillSlotInfo.Info.SlotCount
        );
    }

    Response->QuickSlotInfo = Character->Data.QuickSlotInfo.Info;
    if (Character->Data.QuickSlotInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.QuickSlotInfo.Slots,
            sizeof(struct _RTQuickSlot) * Character->Data.QuickSlotInfo.Info.SlotCount
        );
    }

    Response->AbilityInfo = Character->Data.AbilityInfo.Info;
    if (Character->Data.AbilityInfo.Info.EssenceAbilityCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AbilityInfo.EssenceAbilitySlots,
            sizeof(struct _RTEssenceAbilitySlot) * Character->Data.AbilityInfo.Info.EssenceAbilityCount
        );
    }

    if (Character->Data.AbilityInfo.Info.BlendedAbilityCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AbilityInfo.BlendedAbilitySlots,
            sizeof(struct _RTBlendedAbilitySlot) * Character->Data.AbilityInfo.Info.BlendedAbilityCount
        );
    }

    if (Character->Data.AbilityInfo.Info.KarmaAbilityCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AbilityInfo.KarmaAbilitySlots,
            sizeof(struct _RTKarmaAbilitySlot) * Character->Data.AbilityInfo.Info.KarmaAbilityCount
        );
    }

    Response->BlessingBeadInfo = Character->Data.BlessingBeadInfo.Info;
    if (Character->Data.BlessingBeadInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.BlessingBeadInfo.Slots,
            sizeof(struct _RTBlessingBeadSlot) * Character->Data.BlessingBeadInfo.Info.SlotCount
        );
    }

    Response->PremiumServiceInfo = Character->Data.PremiumServiceInfo.Info;
    if (Character->Data.PremiumServiceInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.PremiumServiceInfo.Slots,
            sizeof(struct _RTPremiumServiceSlot) * Character->Data.PremiumServiceInfo.Info.SlotCount
        );
    }

    Response->QuestInfo = Character->Data.QuestInfo.Info;
    for (Int Index = 0; Index < Character->Data.QuestInfo.Info.SlotCount; Index += 1) {
        RTQuestSlotRef QuestSlot = &Character->Data.QuestInfo.Slots[Index];
        if (!QuestSlot->QuestIndex) continue;

        RTQuestDataRef Quest = RTRuntimeGetQuestByIndex(Runtime, QuestSlot->QuestIndex);
        assert(Quest);

        UInt16 NpcActionFlags = 0;
        for (Int Index = 0; Index < QuestSlot->NpcActionIndex; Index += 1) {
            NpcActionFlags |= 1 << Index;
        }

        S2C_DATA_INITIALIZE_QUEST_INDEX* QuestResponse = PacketBufferAppendStruct(PacketBuffer, S2C_DATA_INITIALIZE_QUEST_INDEX);
        QuestResponse->QuestIndex = QuestSlot->QuestIndex;
        QuestResponse->NpcActionFlags = NpcActionFlags;
        QuestResponse->DisplayNotice = QuestSlot->DisplayNotice;
        QuestResponse->DisplayOpenNotice = QuestSlot->DisplayOpenNotice;
        QuestResponse->QuestSlotIndex = QuestSlot->SlotIndex;

        Int32 CounterCount = Quest->MissionMobCount + Quest->MissionItemCount;
        for (Int CounterIndex = 0; CounterIndex < CounterCount; CounterIndex++) {
            PacketBufferAppendValue(PacketBuffer, UInt8, QuestSlot->Counter[CounterIndex]);
        }
    }

    Response->DailyQuestInfo = Character->Data.DailyQuestInfo.Info;
    if (Character->Data.DailyQuestInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.DailyQuestInfo.Slots,
            sizeof(struct _RTDailyQuestSlot) * Character->Data.DailyQuestInfo.Info.SlotCount
        );
    }

    Response->MercenaryInfo = Character->Data.MercenaryInfo.Info;
    if (Character->Data.MercenaryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.MercenaryInfo.Slots,
            sizeof(struct _RTMercenarySlot) * Character->Data.MercenaryInfo.Info.SlotCount
        );
    }

    Response->AppearanceInfo = Character->Data.AppearanceInfo.Info;
    if (Character->Data.AppearanceInfo.Info.EquipmentAppearanceCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AppearanceInfo.EquipmentSlots,
            sizeof(struct _RTItemSlotAppearance) * Character->Data.AppearanceInfo.Info.EquipmentAppearanceCount
        );
    }

    if (Character->Data.AppearanceInfo.Info.InventoryAppearanceCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AppearanceInfo.InventorySlots,
            sizeof(struct _RTItemSlotAppearance) * Character->Data.AppearanceInfo.Info.InventoryAppearanceCount
        );
    }
    
    Response->AchievementInfo = Character->Data.AchievementInfo.Info;
    if (Character->Data.AchievementInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AchievementInfo.Slots,
            sizeof(struct _RTAchievementSlot) * Character->Data.AchievementInfo.Info.SlotCount
        );
    }

    if (Character->Data.AchievementInfo.Info.RewardSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AchievementInfo.RewardSlots,
            sizeof(struct _RTAchievementRewardSlot) * Character->Data.AchievementInfo.Info.RewardSlotCount
        );
    }

    Response->AchievementExtendedInfo = Character->Data.AchievementExtendedInfo.Info;
    if (Character->Data.AchievementExtendedInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AchievementExtendedInfo.Slots,
            sizeof(struct _RTAchievementExtendedRewardSlot) * Character->Data.AchievementExtendedInfo.Info.SlotCount
        );
    }

    Response->ForceGem = Character->Data.AccountInfo.ForceGem;

    // TODO: BuffData

    Response->CraftInfo = Character->Data.CraftInfo.Info;
    // TODO: CraftData
    
    Response->RequestCraftInfo = Character->Data.RequestCraftInfo.Info;
    if (Character->Data.RequestCraftInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.RequestCraftInfo.Slots,
            sizeof(struct _RTRequestCraftSlot) * Character->Data.RequestCraftInfo.Info.SlotCount
        );
    }

    Response->CooldownInfo = Character->Data.CooldownInfo.Info;
    if (Character->Data.CooldownInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.CooldownInfo.Slots,
            sizeof(struct _RTCooldownSlot) * Character->Data.CooldownInfo.Info.SlotCount
        );
    }

    Response->VehicleInventoryInfo = Character->Data.VehicleInventoryInfo.Info;
    if (Character->Data.VehicleInventoryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.VehicleInventoryInfo.Slots,
            sizeof(struct _RTItemSlot) * Character->Data.VehicleInventoryInfo.Info.SlotCount
        );
    }

    Response->GoldMeritMasteryInfo = Character->Data.GoldMeritMasteryInfo.Info;
    if (Character->Data.GoldMeritMasteryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.GoldMeritMasteryInfo.Slots,
            sizeof(struct _RTGoldMeritMasterySlot) * Character->Data.GoldMeritMasteryInfo.Info.SlotCount
        );
    }

    Response->PlatinumMeritMasteryInfo = Character->Data.PlatinumMeritMasteryInfo.Info;
    if (Character->Data.PlatinumMeritMasteryInfo.Info.TotalMemorizeCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots,
            sizeof(struct _RTPlatinumMeritExtendedMemorizeSlot) * Character->Data.PlatinumMeritMasteryInfo.Info.TotalMemorizeCount
        );
    }

    if (Character->Data.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.PlatinumMeritMasteryInfo.UnlockedSlots,
            sizeof(struct _RTPlatinumMeritUnlockedSlot) * Character->Data.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount
        );
    }

    if (Character->Data.PlatinumMeritMasteryInfo.Info.MasterySlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.PlatinumMeritMasteryInfo.MasterySlots,
            sizeof(struct _RTPlatinumMeritMasterySlot) * Character->Data.PlatinumMeritMasteryInfo.Info.MasterySlotCount
        );
    }

    if (Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.PlatinumMeritMasteryInfo.SpecialMasterySlots,
            sizeof(struct _RTPlatinumMeritSpecialMasterySlot) * Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount
        );
    }

    Response->DiamondMeritMasteryInfo = Character->Data.DiamondMeritMasteryInfo.Info;
    if (Character->Data.DiamondMeritMasteryInfo.Info.TotalMemorizeCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots,
            sizeof(struct _RTPlatinumMeritExtendedMemorizeSlot) * Character->Data.DiamondMeritMasteryInfo.Info.TotalMemorizeCount
        );
    }

    if (Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.DiamondMeritMasteryInfo.UnlockedSlots,
            sizeof(struct _RTPlatinumMeritUnlockedSlot) * Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount
        );
    }

    if (Character->Data.DiamondMeritMasteryInfo.Info.MasterySlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.DiamondMeritMasteryInfo.MasterySlots,
            sizeof(struct _RTPlatinumMeritMasterySlot) * Character->Data.DiamondMeritMasteryInfo.Info.MasterySlotCount
        );
    }

    if (Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.DiamondMeritMasteryInfo.SpecialMasterySlots,
            sizeof(struct _RTPlatinumMeritSpecialMasterySlot) * Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount
        );
    }

    Response->WarpServiceInfo = Character->Data.WarpServiceInfo.Info;
    if (Character->Data.WarpServiceInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.WarpServiceInfo.Slots,
            sizeof(struct _RTWarpServiceSlot) * Character->Data.WarpServiceInfo.Info.SlotCount
        );
    }

    Response->OverlordMasteryInfo = Character->Data.OverlordMasteryInfo.Info;
    if (Character->Data.OverlordMasteryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.OverlordMasteryInfo.Slots,
            sizeof(struct _RTOverlordMasterySlot) * Character->Data.OverlordMasteryInfo.Info.SlotCount
        );
    }

    Response->HonorMedalInfo = Character->Data.HonorMedalInfo.Info;
    if (Character->Data.HonorMedalInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.HonorMedalInfo.Slots,
            sizeof(struct _RTHonorMedalSlot) * Character->Data.HonorMedalInfo.Info.SlotCount
        );
    }

    Response->ForceWingInfo = Character->Data.ForceWingInfo.Info;
    if (Character->Data.ForceWingInfo.Info.PresetSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.ForceWingInfo.PresetSlots,
            sizeof(struct _RTForceWingPresetSlot) * Character->Data.ForceWingInfo.Info.PresetSlotCount
        );
    }

    if (Character->Data.ForceWingInfo.Info.TrainingSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.ForceWingInfo.TrainingSlots,
            sizeof(struct _RTForceWingTrainingSlot) * Character->Data.ForceWingInfo.Info.TrainingSlotCount
        );
    }

    Response->GiftBoxInfo = Character->Data.GiftboxInfo.Info;
    if (Character->Data.GiftboxInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.GiftboxInfo.Slots,
            sizeof(struct _RTGiftBoxSlot) * Character->Data.GiftboxInfo.Info.SlotCount
        );
    }

    Response->CollectionInfo = Character->Data.CollectionInfo.Info;
    if (Character->Data.CollectionInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.CollectionInfo.Slots,
            sizeof(struct _RTCollectionSlot) * Character->Data.CollectionInfo.Info.SlotCount
        );
    }

    Response->TransformInfo = Character->Data.TransformInfo.Info;
    if (Character->Data.TransformInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.TransformInfo.Slots,
            sizeof(struct _RTTransformSlot) * Character->Data.TransformInfo.Info.SlotCount
        );
    }

    Response->TranscendenceInfo = Character->Data.TranscendenceInfo.Info;
    if (Character->Data.TranscendenceInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.TranscendenceInfo.Slots,
            sizeof(struct _RTTranscendenceSlot) * Character->Data.TranscendenceInfo.Info.SlotCount
        );
    }

    // TODO: Add missing data...

    Response->StellarMasteryInfo = Character->Data.StellarMasteryInfo.Info;
    if (Character->Data.StellarMasteryInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.StellarMasteryInfo.Slots,
            sizeof(struct _RTStellarMasterySlot) * Character->Data.StellarMasteryInfo.Info.SlotCount
        );
    }

    Response->MythMasteryInfo = Character->Data.MythMasteryInfo.Info;
    if (Character->Data.MythMasteryInfo.Info.MasterySlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.MythMasteryInfo.Slots,
            sizeof(struct _RTMythMasterySlot) * Character->Data.MythMasteryInfo.Info.MasterySlotCount
        );
    }

    Response->NewbieSupportInfo = Character->Data.NewbieSupportInfo.Info;
    if (Character->Data.NewbieSupportInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.NewbieSupportInfo.Slots,
            sizeof(struct _RTNewbieSupportSlot) * Character->Data.NewbieSupportInfo.Info.SlotCount
        );
    }

    Response->EventPassInfo = Character->Data.EventPassInfo.Info;
    if (Character->Data.EventPassInfo.Info.MissionPageCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.EventPassInfo.MissionPages,
            sizeof(struct _RTEventPassMissionPage) * Character->Data.EventPassInfo.Info.MissionPageCount
        );
    }

    if (Character->Data.EventPassInfo.Info.MissionSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.EventPassInfo.MissionSlots,
            sizeof(struct _RTEventPassMissionSlot) * Character->Data.EventPassInfo.Info.MissionSlotCount
        );
    }

    if (Character->Data.EventPassInfo.Info.RewardSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.EventPassInfo.RewardSlots,
            sizeof(struct _RTEventPassRewardSlot) * Character->Data.EventPassInfo.Info.RewardSlotCount
        );
    }

    Response->CostumeWarehouseInfo = Character->Data.CostumeWarehouseInfo.Info;
    if (Character->Data.CostumeWarehouseInfo.Info.SlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.CostumeWarehouseInfo.Slots,
            sizeof(struct _RTAccountCostumeSlot) * Character->Data.CostumeWarehouseInfo.Info.SlotCount
        );
    }

    Response->CostumeInfo = Character->Data.CostumeInfo.Info;
    if (Character->Data.CostumeInfo.Info.PageCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.CostumeInfo.Pages,
            sizeof(struct _RTCostumePage) * Character->Data.CostumeInfo.Info.PageCount
        );
    }

    if (Character->Data.CostumeInfo.Info.AppliedSlotCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.CostumeInfo.AppliedSlots,
            sizeof(struct _RTAppliedCostumeSlot) * Character->Data.CostumeInfo.Info.AppliedSlotCount
        );
    }

    Response->AnimaMasteryInfo = Character->Data.AnimaMasteryInfo.Info;
    if (Character->Data.AnimaMasteryInfo.Info.PresetCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AnimaMasteryInfo.PresetData,
            sizeof(struct _RTAnimaMasteryPresetData) * Character->Data.AnimaMasteryInfo.Info.PresetCount
        );
    }

    if (Character->Data.AnimaMasteryInfo.Info.StorageCount > 0) {
        PacketBufferAppendCopy(
            PacketBuffer,
            Character->Data.AnimaMasteryInfo.CategoryData,
            sizeof(struct _RTAnimaMasteryCategoryData) * Character->Data.AnimaMasteryInfo.Info.StorageCount
        );
    }

//    Response->MythMasteryInfo.HasRolledMasterySlot = 1;
//    Response->MythMasteryInfo.RolledMasterySlot.MasteryIndex = 0;
//    Response->MythMasteryInfo.RolledMasterySlot.SlotIndex = 1;
//    Response->MythMasteryInfo.RolledMasterySlot.Tier = 74;
//    Response->MythMasteryInfo.RolledMasterySlot.Grade = 1;
//    Response->MythMasteryInfo.RolledMasterySlot.ForceEffectIndex = 211;
//    Response->MythMasteryInfo.RolledMasterySlot.ForceValue = 1;
//    Response->MythMasteryInfo.RolledMasterySlot.ForceValueType = 1;
//    Response->MythMasteryInfo.Unknown[0] = 1;
//    Response->MythMasteryInfo.Unknown[1] = 0;
//    Response->MythMasteryInfo.Unknown[2] = 1;
//    Response->MythMasteryInfo.Unknown[3] = 74;
//    Response->MythMasteryInfo.Unknown[4] = 1;
//    Response->MythMasteryInfo.Unknown[5] = 211;
//    Response->MythMasteryInfo.Unknown[9] = 1;
    SocketSend(Context->ClientSocket, ClientConnection, Response);
    
    RTWorldContextRef WorldContext = RTRuntimeGetWorldByCharacter(Runtime, Character);
    RTWorldSpawnCharacter(Runtime, WorldContext, Character->ID);

    // Rename Card
    // RTRuntimeDataEventHotReload(Runtime->Context, "Event\\Events45.xml");

    // Hero Leap
    // RTRuntimeDataEventHotReload(Runtime->Context, "Event\\Events106.xml");

    // Lottery
    // RTRuntimeDataEventHotReload(Runtime->Context, "Event\\Events34.xml");

    // TODO: Move event data to database and trigger request on init
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

    IPC_W2C_DATA_CLIENT_CONNECT* ChatRequest = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2C, CLIENT_CONNECT);
    ChatRequest->Header.Source = Server->IPCSocket->NodeID;
    ChatRequest->Header.SourceConnectionID = Client->Connection->ID;
    ChatRequest->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
    ChatRequest->Header.Target.Type = IPC_TYPE_CHAT;
    ChatRequest->CharacterIndex = Character->CharacterIndex;
    IPCSocketUnicast(Server->IPCSocket, ChatRequest);
    return;

error:
	if (ClientConnection) {
		SocketDisconnect(Context->ClientSocket, ClientConnection);
	}
}
