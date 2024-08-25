#include "ClientProtocol.h"
#include "ClientProcedures.h"
#include "ClientSocket.h"
#include "IPCProcedures.h"
#include "Notification.h"
#include "Server.h"

Void ServerSyncCharacter(
	ServerRef Server,
	ServerContextRef Context,
	ClientContextRef Client,
	RTCharacterRef Character
) {
	Character->SyncTimestamp = GetTimestampMs();

	IPC_W2D_DATA_DBSYNC* Request = IPCPacketBufferInit(Server->IPCSocket->PacketBuffer, W2D, DBSYNC);
	Request->Header.SourceConnectionID = Client->Connection->ID;
	Request->Header.Source = Server->IPCSocket->NodeID;
	Request->Header.Target.Group = Context->Config.WorldSvr.GroupIndex;
	Request->Header.Target.Type = IPC_TYPE_MASTERDB;
	Request->AccountID = Client->AccountID;
	Request->CharacterID = Client->CharacterDatabaseID;
	Request->SyncMask = Character->SyncMask;

	if (Character->SyncMask.AccountInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.AccountInfo, sizeof(struct _RTCharacterAccountInfo));
	}
	
	if (Character->SyncMask.GoldMeritMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.GoldMeritMasteryInfo.Info, sizeof(struct _RTGoldMeritMasteryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.GoldMeritMasteryInfo.Slots, sizeof(struct _RTGoldMeritMasterySlot) * Character->Data.GoldMeritMasteryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.PlatinumMeritMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.PlatinumMeritMasteryInfo.Info, sizeof(struct _RTPlatinumMeritMasteryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.PlatinumMeritMasteryInfo.ExtendedMemorizeSlots, sizeof(struct _RTPlatinumMeritExtendedMemorizeSlot) * Character->Data.PlatinumMeritMasteryInfo.Info.ExtendedMemorizeCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.PlatinumMeritMasteryInfo.UnlockedSlots, sizeof(struct _RTPlatinumMeritUnlockedSlot) * Character->Data.PlatinumMeritMasteryInfo.Info.UnlockedSlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.PlatinumMeritMasteryInfo.MasterySlots, sizeof(struct _RTPlatinumMeritMasterySlot) * Character->Data.PlatinumMeritMasteryInfo.Info.MasterySlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.PlatinumMeritMasteryInfo.SpecialMasterySlots, sizeof(struct _RTPlatinumMeritSpecialMasterySlot) * Character->Data.PlatinumMeritMasteryInfo.Info.SpecialMasterySlotCount);
	}

	if (Character->SyncMask.DiamondMeritMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.DiamondMeritMasteryInfo.Info, sizeof(struct _RTDiamondMeritMasteryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.DiamondMeritMasteryInfo.ExtendedMemorizeSlots, sizeof(struct _RTDiamondMeritExtendedMemorizeSlot) * Character->Data.DiamondMeritMasteryInfo.Info.ExtendedMemorizeCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.DiamondMeritMasteryInfo.UnlockedSlots, sizeof(struct _RTDiamondMeritUnlockedSlot) * Character->Data.DiamondMeritMasteryInfo.Info.UnlockedSlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.DiamondMeritMasteryInfo.MasterySlots, sizeof(struct _RTDiamondMeritMasterySlot) * Character->Data.DiamondMeritMasteryInfo.Info.MasterySlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.DiamondMeritMasteryInfo.SpecialMasterySlots, sizeof(struct _RTDiamondMeritSpecialMasterySlot) * Character->Data.DiamondMeritMasteryInfo.Info.SpecialMasterySlotCount);
	}

	if (Character->SyncMask.CollectionInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.CollectionInfo.Info, sizeof(struct _RTCollectionInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.CollectionInfo.Slots, sizeof(struct _RTCollectionSlot) * Character->Data.CollectionInfo.Info.SlotCount);
	}

	if (Character->SyncMask.ResearchSupportInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.ResearchSupportInfo, sizeof(struct _RTCharacterResearchSupportInfo));
	}

	if (Character->SyncMask.EventPassInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.EventPassInfo.Info, sizeof(struct _RTEventPassInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.EventPassInfo.MissionSlots, sizeof(struct _RTEventPassMissionSlot) * Character->Data.EventPassInfo.Info.MissionSlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.EventPassInfo.RewardSlots, sizeof(struct _RTEventPassRewardSlot) * Character->Data.EventPassInfo.Info.RewardSlotCount);
	}

	if (Character->SyncMask.CostumeWarehouseInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.CostumeWarehouseInfo.Info, sizeof(struct _RTCostumeWarehouseInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.CostumeWarehouseInfo.Slots, sizeof(struct _RTAccountCostumeSlot) * Character->Data.CostumeWarehouseInfo.Info.SlotCount);
	}

	if (Character->SyncMask.WarehouseInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.WarehouseInfo.Info, sizeof(struct _RTWarehouseInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.WarehouseInfo.Slots, sizeof(struct _RTItemSlot) * Character->Data.WarehouseInfo.Info.SlotCount);
	}

	if (Character->SyncMask.AnimaMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.AnimaMasteryInfo.Info, sizeof(struct _RTAnimaMasteryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AnimaMasteryInfo.PresetData, sizeof(struct _RTAnimaMasteryPresetData) * Character->Data.AnimaMasteryInfo.Info.PresetCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AnimaMasteryInfo.CategoryData, sizeof(struct _RTAnimaMasteryCategoryData) * Character->Data.AnimaMasteryInfo.Info.StorageCount);
	}

	if (Character->SyncMask.WarehouseInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.SettingsInfo, sizeof(struct _RTCharacterSettingsInfo));
	}

	if (Character->SyncMask.Info) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.Info, sizeof(struct _RTCharacterInfo));
	}

	if (Character->SyncMask.StyleInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.StyleInfo, sizeof(struct _RTCharacterStyleInfo));
	}

	if (Character->SyncMask.EquipmentInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.EquipmentInfo.Info, sizeof(struct _RTEquipmentInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.EquipmentInfo.EquipmentSlots, sizeof(struct _RTItemSlot) * Character->Data.EquipmentInfo.Info.EquipmentSlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.EquipmentInfo.InventorySlots, sizeof(struct _RTItemSlot) * Character->Data.EquipmentInfo.Info.InventorySlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.EquipmentInfo.LinkSlots, sizeof(struct _RTEquipmentLinkSlot) * Character->Data.EquipmentInfo.Info.LinkSlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.EquipmentInfo.LockSlots, sizeof(struct _RTEquipmentLockSlot) * Character->Data.EquipmentInfo.Info.LockSlotCount);
	}

	if (Character->SyncMask.InventoryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.InventoryInfo.Info, sizeof(struct _RTInventoryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.InventoryInfo.Slots, sizeof(struct _RTItemSlot) * Character->Data.InventoryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.SkillSlotInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.SkillSlotInfo.Info, sizeof(struct _RTSkillSlotInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.SkillSlotInfo.Slots, sizeof(struct _RTSkillSlot) * Character->Data.SkillSlotInfo.Info.SlotCount);
	}

	if (Character->SyncMask.QuickSlotInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.QuickSlotInfo.Info, sizeof(struct _RTQuickSlotInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.QuickSlotInfo.Slots, sizeof(struct _RTQuickSlot) * Character->Data.QuickSlotInfo.Info.SlotCount);
	}

	if (Character->SyncMask.AbilityInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.AbilityInfo.Info, sizeof(struct _RTAbilityInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AbilityInfo.EssenceAbilitySlots, sizeof(struct _RTEssenceAbilitySlot) * Character->Data.AbilityInfo.Info.EssenceAbilityCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AbilityInfo.BlendedAbilitySlots, sizeof(struct _RTBlendedAbilitySlot) * Character->Data.AbilityInfo.Info.BlendedAbilityCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AbilityInfo.KarmaAbilitySlots, sizeof(struct _RTKarmaAbilitySlot) * Character->Data.AbilityInfo.Info.KarmaAbilityCount);
	}

	if (Character->SyncMask.BlessingBeadInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.BlessingBeadInfo.Info, sizeof(struct _RTBlessingBeadInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.BlessingBeadInfo.Slots, sizeof(struct _RTBlessingBeadSlot) * Character->Data.BlessingBeadInfo.Info.SlotCount);
	}

	if (Character->SyncMask.PremiumServiceInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.PremiumServiceInfo.Info, sizeof(struct _RTPremiumServiceInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.PremiumServiceInfo.Slots, sizeof(struct _RTPremiumServiceSlot) * Character->Data.PremiumServiceInfo.Info.SlotCount);
	}

	if (Character->SyncMask.QuestInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.QuestInfo.Info, sizeof(struct _RTQuestInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.QuestInfo.Slots, sizeof(struct _RTQuestSlot) * Character->Data.QuestInfo.Info.SlotCount);
	}

	if (Character->SyncMask.QuestInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.DailyQuestInfo.Info, sizeof(struct _RTDailyQuestInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.DailyQuestInfo.Slots, sizeof(struct _RTDailyQuestSlot) * Character->Data.DailyQuestInfo.Info.SlotCount);
	}

	if (Character->SyncMask.MercenaryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.MercenaryInfo.Info, sizeof(struct _RTMercenaryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.MercenaryInfo.Slots, sizeof(struct _RTMercenarySlot) * Character->Data.MercenaryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.MercenaryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.AppearanceInfo.Info, sizeof(struct _RTAppearanceInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AppearanceInfo.EquipmentSlots, sizeof(struct _RTItemSlotAppearance) * Character->Data.AppearanceInfo.Info.EquipmentAppearanceCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AppearanceInfo.InventorySlots, sizeof(struct _RTItemSlotAppearance) * Character->Data.AppearanceInfo.Info.InventoryAppearanceCount);
	}

	if (Character->SyncMask.AchievementInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.AchievementInfo.Info, sizeof(struct _RTAchievementInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AchievementInfo.Slots, sizeof(struct _RTAchievementSlot) * Character->Data.AchievementInfo.Info.SlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AchievementInfo.RewardSlots, sizeof(struct _RTAchievementRewardSlot) * Character->Data.AchievementInfo.Info.RewardSlotCount);
	}

	if (Character->SyncMask.CraftInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.CraftInfo.Info, sizeof(struct _RTCraftInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.CraftInfo.Slots, sizeof(struct _RTCraftSlot) * Character->Data.CraftInfo.Info.SlotCount);
	}

	if (Character->SyncMask.RequestCraftInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.RequestCraftInfo.Info, sizeof(struct _RTRequestCraftInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.RequestCraftInfo.Slots, sizeof(struct _RTRequestCraftSlot) * Character->Data.RequestCraftInfo.Info.SlotCount);
	}

	if (Character->SyncMask.BuffInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.BuffInfo.Info, sizeof(struct _RTBuffInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.BuffInfo.Slots, sizeof(struct _RTBuffSlot) * Character->Data.BuffInfo.Info.BuffCount);
	}

	if (Character->SyncMask.VehicleInventoryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.VehicleInventoryInfo.Info, sizeof(struct _RTVehicleInventoryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.VehicleInventoryInfo.Slots, sizeof(struct _RTBuffSlot) * Character->Data.VehicleInventoryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.WarpServiceInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.WarpServiceInfo.Info, sizeof(struct _RTWarpServiceInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.WarpServiceInfo.Slots, sizeof(struct _RTWarpServiceSlot) * Character->Data.WarpServiceInfo.Info.SlotCount);
	}

	if (Character->SyncMask.OverlordMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.OverlordMasteryInfo.Info, sizeof(struct _RTOverlordMasteryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.OverlordMasteryInfo.Slots, sizeof(struct _RTOverlordMasterySlot) * Character->Data.OverlordMasteryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.HonorMedalInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.HonorMedalInfo.Info, sizeof(struct _RTHonorMedalInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.HonorMedalInfo.Slots, sizeof(struct _RTHonorMedalSlot) * Character->Data.HonorMedalInfo.Info.SlotCount);
	}

	if (Character->SyncMask.ForceWingInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.ForceWingInfo.Info, sizeof(struct _RTForceWingInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.ForceWingInfo.PresetSlots, sizeof(struct _RTForceWingPresetSlot) * Character->Data.ForceWingInfo.Info.PresetSlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.ForceWingInfo.TrainingSlots, sizeof(struct _RTForceWingTrainingSlot) * Character->Data.ForceWingInfo.Info.TrainingSlotCount);
	}

	if (Character->SyncMask.GiftboxInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.GiftboxInfo.Info, sizeof(struct _RTGiftBoxInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.GiftboxInfo.Slots, sizeof(struct _RTGiftBoxSlot) * Character->Data.GiftboxInfo.Info.SlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.GiftboxInfo.RewardSlots, sizeof(struct _RTGiftBoxRewardSlot) * Character->Data.GiftboxInfo.Info.SlotCount);
	}

	if (Character->SyncMask.TransformInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.TransformInfo.Info, sizeof(struct _RTTransformInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.TransformInfo.Slots, sizeof(struct _RTTransformSlot) * Character->Data.TransformInfo.Info.SlotCount);
	}

	if (Character->SyncMask.TranscendenceInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.TranscendenceInfo.Info, sizeof(struct _RTTranscendenceInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.TranscendenceInfo.Slots, sizeof(struct _RTTranscendenceSlot) * Character->Data.TranscendenceInfo.Info.SlotCount);
	}

	if (Character->SyncMask.StellarMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.StellarMasteryInfo.Info, sizeof(struct _RTStellarMasteryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.StellarMasteryInfo.Slots, sizeof(struct _RTStellarMasterySlot) * Character->Data.StellarMasteryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.MythMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.MythMasteryInfo.Info, sizeof(struct _RTMythMasteryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.MythMasteryInfo.Slots, sizeof(struct _RTMythMasterySlot) * Character->Data.MythMasteryInfo.Info.PropertySlotCount);
	}

	if (Character->SyncMask.NewbieSupportInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.NewbieSupportInfo.Info, sizeof(struct _RTNewbieSupportInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.NewbieSupportInfo.Slots, sizeof(struct _RTNewbieSupportSlot) * Character->Data.NewbieSupportInfo.Info.SlotCount);
	}

	if (Character->SyncMask.CostumeInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.CostumeInfo.Info, sizeof(struct _RTCostumeInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.CostumeInfo.Pages, sizeof(struct _RTCostumePage) * Character->Data.CostumeInfo.Info.PageCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.CostumeInfo.AppliedSlots, sizeof(struct _RTCostumePage) * Character->Data.CostumeInfo.Info.AppliedSlotCount);
	}

	if (Character->SyncMask.TemporaryInventoryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.TemporaryInventoryInfo.Info, sizeof(struct _RTInventoryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.TemporaryInventoryInfo.Slots, sizeof(struct _RTItemSlot) * Character->Data.TemporaryInventoryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.RecoveryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.RecoveryInfo.Info, sizeof(struct _RTRecoveryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.RecoveryInfo.Prices, sizeof(UInt64) * Character->Data.RecoveryInfo.Info.SlotCount);
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.RecoveryInfo.Slots, sizeof(struct _RTItemSlot) * Character->Data.RecoveryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.PresetInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.PresetInfo, sizeof(struct _RTCharacterPresetInfo));
	}

	if (Character->SyncMask.AuraMasteryInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.AuraMasteryInfo.Info, sizeof(struct _RTAuraMasteryInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.AuraMasteryInfo.Slots, sizeof(struct _RTAuraMasterySlot) * Character->Data.AuraMasteryInfo.Info.SlotCount);
	}

	if (Character->SyncMask.SecretShopInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.SecretShopInfo, sizeof(struct _RTCharacterSecretShopData));
	}

	if (Character->SyncMask.DamageBoosterInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.DamageBoosterInfo.Info, sizeof(struct _RTDamageBoosterInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.DamageBoosterInfo.Slots, sizeof(struct _RTDamageBoosterSlot) * Character->Data.DamageBoosterInfo.Info.SlotCount);
	}

	if (Character->SyncMask.ExplorationInfo) {
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, &Character->Data.ExplorationInfo.Info, sizeof(struct _RTExplorationInfo));
		IPCPacketBufferAppendCopy(Server->IPCSocket->PacketBuffer, Character->Data.ExplorationInfo.Slots, sizeof(struct _RTExplorationSlot) * Character->Data.ExplorationInfo.Info.SlotCount);
	}

	Character->SyncMask.RawValue = 0;

	IPCSocketUnicast(Server->IPCSocket, Request);
}

Void ServerSyncDB(
	ServerRef Server,
	ServerContextRef Context,
	Bool Force
) {
	Timestamp Timestamp = GetTimestampMs();

	SocketConnectionIteratorRef Iterator = SocketGetConnectionIterator(Context->ClientSocket);
	while (Iterator) {
		SocketConnectionRef Connection = SocketConnectionIteratorFetch(Context->ClientSocket, Iterator);
		// TODO: Connection can become null!!!

		Iterator = SocketConnectionIteratorNext(Context->ClientSocket, Iterator);

		ClientContextRef Client = (ClientContextRef)Connection->Userdata;
		if (Client->CharacterIndex < 1) continue;

		RTCharacterRef Character = RTWorldManagerGetCharacterByIndex(Context->Runtime->WorldManager, Client->CharacterIndex);
		assert(Character);

		Bool PerformSync = Force || (
			Character->SyncMask.RawValue && 
            (Timestamp - Character->SyncTimestamp) >= Context->Config.WorldSvr.DBSyncTimer
		);

		if (PerformSync) {
			ServerSyncCharacter(Server, Context, Client, Character);
		}
	}
}

IPC_PROCEDURE_BINDING(D2W, DBSYNC) {
	if (!Client || !Character) return;

	if (Packet->SyncMaskFailed.RawValue) {
		Character->SyncMask.RawValue |= Packet->SyncMaskFailed.RawValue;
	}
}
