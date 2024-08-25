#include "IPCProtocol.h"
#include "IPCProcedures.h"

#define ReadMemory(__TYPE__, __NAME__, __COUNT__)		\
__TYPE__* __NAME__ = (__TYPE__*)Memory;					\
Int32 __NAME__##Length = sizeof(__TYPE__) * __COUNT__;	\
Memory += __NAME__##Length

IPC_PROCEDURE_BINDING(W2D, DBSYNC) {
	IPC_D2W_DATA_DBSYNC* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, DBSYNC);
	Response->Header.Source = Server->IPCSocket->NodeID;
	Response->Header.Target = Packet->Header.Source;
	Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
	Response->AccountID = Packet->AccountID;
	Response->CharacterID = Packet->CharacterID;
	Response->SyncMaskFailed.RawValue = 0;

	UInt8* Memory = (UInt8*)&Packet->Data[0];

    if (Packet->IsTransaction) DatabaseBeginTransaction(Context->Database); 
    {
		if (Packet->SyncMask.AccountInfo) {
			ReadMemory(struct _RTCharacterAccountInfo, Info, 1);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncAccountInfo",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_INT32(Info->CharacterSlotID),
				DB_INPUT_INT64(Info->CharacterSlotOrder),
				DB_INPUT_INT32(Info->CharacterSlotOpenMask),
				DB_INPUT_INT32(Info->ForceGem),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.AccountInfo = true;
			}
		}

		if (Packet->SyncMask.GoldMeritMasteryInfo) {
			ReadMemory(struct _RTGoldMeritMasteryInfo, Info, 1);
			ReadMemory(struct _RTGoldMeritMasterySlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncGoldMeritMastery",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_INT32(Info->Exp),
				DB_INPUT_INT32(Info->Points),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.GoldMeritMasteryInfo = true;
			}
		}

		if (Packet->SyncMask.PlatinumMeritMasteryInfo) {
			ReadMemory(struct _RTPlatinumMeritMasteryInfo, Info, 1);
			ReadMemory(struct _RTPlatinumMeritExtendedMemorizeSlot, ExtendedMemorizeSlots, Info->ExtendedMemorizeCount);
			ReadMemory(struct _RTPlatinumMeritUnlockedSlot, UnlockedSlots, Info->UnlockedSlotCount);
			ReadMemory(struct _RTPlatinumMeritMasterySlot, MasterySlots, Info->MasterySlotCount);
			ReadMemory(struct _RTPlatinumMeritSpecialMasterySlot, SpecialMasterySlots, Info->SpecialMasterySlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncPlatinumMeritMastery",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_UINT8(Info->IsEnabled),
				DB_INPUT_INT32(Info->Exp),
				DB_INPUT_INT32(Info->Points),
				DB_INPUT_UINT8(Info->ActiveMemorizeIndex),
				DB_INPUT_INT32(Info->OpenSlotMasteryIndex),
				DB_INPUT_UINT64(Info->OpenSlotUnlockTime),
				DB_INPUT_INT16(Info->ExtendedMemorizeCount),
				DB_INPUT_INT16(Info->UnlockedSlotCount),
				DB_INPUT_INT16(Info->MasterySlotCount),
				DB_INPUT_INT16(Info->SpecialMasterySlotCount),
				DB_INPUT_DATA(ExtendedMemorizeSlots, ExtendedMemorizeSlotsLength),
				DB_INPUT_DATA(UnlockedSlots, UnlockedSlotsLength),
				DB_INPUT_DATA(MasterySlots, MasterySlotsLength),
				DB_INPUT_DATA(SpecialMasterySlots, SpecialMasterySlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.PlatinumMeritMasteryInfo = true;
			}
		}

		if (Packet->SyncMask.DiamondMeritMasteryInfo) {
			ReadMemory(struct _RTDiamondMeritMasteryInfo, Info, 1);
			ReadMemory(struct _RTDiamondMeritExtendedMemorizeSlot, ExtendedMemorizeSlots, Info->ExtendedMemorizeCount);
			ReadMemory(struct _RTDiamondMeritUnlockedSlot, UnlockedSlots, Info->UnlockedSlotCount);
			ReadMemory(struct _RTDiamondMeritMasterySlot, MasterySlots, Info->MasterySlotCount);
			ReadMemory(struct _RTDiamondMeritSpecialMasterySlot, SpecialMasterySlots, Info->SpecialMasterySlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncDiamondMeritMastery",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_UINT8(Info->IsEnabled),
				DB_INPUT_INT32(Info->Exp),
				DB_INPUT_INT32(Info->Points),
				DB_INPUT_UINT8(Info->ActiveMemorizeIndex),
				DB_INPUT_INT32(Info->OpenSlotMasteryIndex),
				DB_INPUT_UINT64(Info->OpenSlotUnlockTime),
				DB_INPUT_INT16(Info->ExtendedMemorizeCount),
				DB_INPUT_INT16(Info->UnlockedSlotCount),
				DB_INPUT_INT16(Info->MasterySlotCount),
				DB_INPUT_INT16(Info->SpecialMasterySlotCount),
				DB_INPUT_INT32(Info->ExtendedMasterySlotCount),
				DB_INPUT_DATA(ExtendedMemorizeSlots, ExtendedMemorizeSlotsLength),
				DB_INPUT_DATA(UnlockedSlots, UnlockedSlotsLength),
				DB_INPUT_DATA(MasterySlots, MasterySlotsLength),
				DB_INPUT_DATA(SpecialMasterySlots, SpecialMasterySlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.DiamondMeritMasteryInfo = true;
			}
		}

		if (Packet->SyncMask.CollectionInfo) {
			ReadMemory(struct _RTCollectionInfo, Info, 1);
			ReadMemory(struct _RTCollectionSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncCollection",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_INT16(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.CollectionInfo = true;
			}
		}

		if (Packet->SyncMask.ResearchSupportInfo) {
			ReadMemory(struct _RTCharacterResearchSupportInfo, Info, 1);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncResearchSupport",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_INT32(Info->Exp),
				DB_INPUT_INT32(Info->DecodedCircuitCount),
				DB_INPUT_UINT8(Info->ResetCount),
				DB_INPUT_UINT64(Info->SeasonStartDate),
				DB_INPUT_UINT64(Info->SeasonEndDate),
				DB_INPUT_DATA(Info->MaterialSlots, sizeof(struct _RTResearchSupportMaterialSlot) * RUNTIME_CHARACTER_MAX_RESEARCH_SUPPORT_MATERIAL_COUNT),
				DB_INPUT_DATA(&Info->ActiveMissionBoard, sizeof(struct _RTResearchSupportMissionBoard)),
				DB_INPUT_DATA(Info->MissionBoards, sizeof(struct _RTResearchSupportMissionBoard) * RUNTIME_CHARACTER_MAX_RESEARCH_SUPPORT_BOARD_COUNT),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.ResearchSupportInfo = true;
			}
		}

		if (Packet->SyncMask.EventPassInfo) {
			ReadMemory(struct _RTEventPassInfo, Info, 1);
			ReadMemory(struct _RTEventPassMissionPage, MissionPages, Info->MissionPageCount);
			ReadMemory(struct _RTEventPassMissionSlot, MissionSlots, Info->MissionSlotCount);
			ReadMemory(struct _RTEventPassRewardSlot, RewardSlots, Info->RewardSlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncEventPass",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_UINT64(Info->StartDate),
				DB_INPUT_UINT64(Info->EndDate),
				DB_INPUT_INT32(Info->MissionPageCount),
				DB_INPUT_INT32(Info->MissionSlotCount),
				DB_INPUT_INT32(Info->RewardSlotCount),
				DB_INPUT_DATA(MissionPages, MissionPagesLength),
				DB_INPUT_DATA(MissionSlots, MissionSlotsLength),
				DB_INPUT_DATA(RewardSlots, RewardSlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.EventPassInfo = true;
			}
		}

		if (Packet->SyncMask.CostumeWarehouseInfo) {
			ReadMemory(struct _RTCostumeWarehouseInfo, Info, 1);
			ReadMemory(struct _RTAccountCostumeSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncCostumeWarehouse",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.CostumeWarehouseInfo = true;
			}
		}

		if (Packet->SyncMask.WarehouseInfo) {
			ReadMemory(struct _RTWarehouseInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncWarehouse",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_INT16(Info->SlotCount),
				DB_INPUT_UINT64(Info->Currency),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.WarehouseInfo = true;
			}
		}

		if (Packet->SyncMask.AnimaMasteryInfo) {
			ReadMemory(struct _RTAnimaMasteryInfo, Info, 1);
			ReadMemory(struct _RTAnimaMasteryPresetData, PresetData, Info->PresetCount);
			ReadMemory(struct _RTAnimaMasteryCategoryData, CategoryData, Info->StorageCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncAnimaMastery",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_INT32(Info->PresetCount),
				DB_INPUT_INT32(Info->StorageCount),
				DB_INPUT_UINT32(Info->UnlockedCategoryFlags),
				DB_INPUT_DATA(PresetData, PresetDataLength),
				DB_INPUT_DATA(CategoryData, CategoryDataLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.AnimaMasteryInfo = true;
			}
		}

		if (Packet->SyncMask.SettingsInfo) {
			ReadMemory(struct _RTCharacterSettingsInfo, Info, 1);

		}

		if (Packet->SyncMask.Info) {
			ReadMemory(struct _RTCharacterInfo, Info, 1);

		}

		if (Packet->SyncMask.StyleInfo) {
			ReadMemory(struct _RTCharacterStyleInfo, Info, 1);

		}

		if (Packet->SyncMask.EquipmentInfo) {
			ReadMemory(struct _RTEquipmentInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->EquipmentSlotCount);
			ReadMemory(struct _RTItemSlot, InventorySlots, Info->InventorySlotCount);
			ReadMemory(struct _RTEquipmentLinkSlot, LinkSlots, Info->LinkSlotCount);
			ReadMemory(struct _RTEquipmentLockSlot, LockSlots, Info->LockSlotCount);

		}

		if (Packet->SyncMask.InventoryInfo) {
			ReadMemory(struct _RTInventoryInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.SkillSlotInfo) {
			ReadMemory(struct _RTSkillSlotInfo, Info, 1);
			ReadMemory(struct _RTSkillSlot, Slots, Info->SlotCount);
		
		}

		if (Packet->SyncMask.QuickSlotInfo) {
			ReadMemory(struct _RTQuickSlotInfo, Info, 1);
			ReadMemory(struct _RTQuickSlot, Slots, Info->SlotCount);
		
		}

		if (Packet->SyncMask.AbilityInfo) {
			ReadMemory(struct _RTAbilityInfo, Info, 1);
			ReadMemory(struct _RTEssenceAbilitySlot, EssenceAbilitySlots, Info->EssenceAbilityCount);
			ReadMemory(struct _RTBlendedAbilitySlot, BlendedAbilitySlots, Info->BlendedAbilityCount);
			ReadMemory(struct _RTKarmaAbilitySlot, KarmaAbilitySlots, Info->KarmaAbilityCount);

		}

		if (Packet->SyncMask.BlessingBeadInfo) {
			ReadMemory(struct _RTBlessingBeadInfo, Info, 1);
			ReadMemory(struct _RTBlessingBeadSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.PremiumServiceInfo) {
			ReadMemory(struct _RTPremiumServiceInfo, Info, 1);
			ReadMemory(struct _RTPremiumServiceSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.QuestInfo) {
			ReadMemory(struct _RTQuestInfo, Info, 1);
			ReadMemory(struct _RTQuestSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.DailyQuestInfo) {
			ReadMemory(struct _RTDailyQuestInfo, Info, 1);
			ReadMemory(struct _RTDailyQuestSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.MercenaryInfo) {
			ReadMemory(struct _RTMercenaryInfo, Info, 1);
			ReadMemory(struct _RTMercenarySlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.AppearanceInfo) {
			ReadMemory(struct _RTAppearanceInfo, Info, 1);
			ReadMemory(struct _RTItemSlotAppearance, EquipmentSlots, Info->EquipmentAppearanceCount);
			ReadMemory(struct _RTItemSlotAppearance, InventorySlots, Info->InventoryAppearanceCount);

		}

		if (Packet->SyncMask.AchievementInfo) {
			ReadMemory(struct _RTAchievementInfo, Info, 1);
			ReadMemory(struct _RTAchievementSlot, Slots, Info->SlotCount);
			ReadMemory(struct _RTAchievementRewardSlot, RewardSlots, Info->RewardSlotCount);

		}

		if (Packet->SyncMask.CraftInfo) {
			ReadMemory(struct _RTCraftInfo, Info, 1);
			ReadMemory(struct _RTCraftSlot, Slots, Info->SlotCount);
		
		}

		if (Packet->SyncMask.RequestCraftInfo) {
			ReadMemory(struct _RTRequestCraftInfo, Info, 1);
			ReadMemory(struct _RTRequestCraftSlot, Slots, Info->SlotCount);
		}

		if (Packet->SyncMask.BuffInfo) {
			ReadMemory(struct _RTBuffInfo, Info, 1);
			ReadMemory(struct _RTBuffSlot, Slots, Info->BuffCount);

		}

		if (Packet->SyncMask.VehicleInventoryInfo) {
			ReadMemory(struct _RTVehicleInventoryInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.WarpServiceInfo) {
			ReadMemory(struct _RTWarpServiceInfo, Info, 1);
			ReadMemory(struct _RTWarpServiceSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.OverlordMasteryInfo) {
			ReadMemory(struct _RTOverlordMasteryInfo, Info, 1);
			ReadMemory(struct _RTOverlordMasterySlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.HonorMedalInfo) {
			ReadMemory(struct _RTHonorMedalInfo, Info, 1);
			ReadMemory(struct _RTHonorMedalSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.ForceWingInfo) {
			ReadMemory(struct _RTForceWingInfo, Info, 1);
			ReadMemory(struct _RTForceWingPresetSlot, PresetSlots, Info->PresetSlotCount);
			ReadMemory(struct _RTForceWingTrainingSlot, TrainingSlots, Info->TrainingSlotCount);
		}

		if (Packet->SyncMask.GiftboxInfo) {
			ReadMemory(struct _RTGiftBoxInfo, Info, 1);
			ReadMemory(struct _RTGiftBoxSlot, Slots, Info->SlotCount);
			ReadMemory(struct _RTGiftBoxRewardSlot, RewardSlots, Info->SlotCount);

		}

		if (Packet->SyncMask.TransformInfo) {
			ReadMemory(struct _RTTransformInfo, Info, 1);
			ReadMemory(struct _RTTransformSlot, Slots, Info->SlotCount);
		}

		if (Packet->SyncMask.TranscendenceInfo) {
			ReadMemory(struct _RTTranscendenceInfo, Info, 1);
			ReadMemory(struct _RTTranscendenceSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.StellarMasteryInfo) {
			ReadMemory(struct _RTStellarMasteryInfo, Info, 1);
			ReadMemory(struct _RTStellarMasterySlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.MythMasteryInfo) {
			ReadMemory(struct _RTMythMasteryInfo, Info, 1);
			ReadMemory(struct _RTMythMasterySlot, Slots, Info->PropertySlotCount);

		}

		if (Packet->SyncMask.NewbieSupportInfo) {
			ReadMemory(struct _RTNewbieSupportInfo, Info, 1);
			ReadMemory(struct _RTNewbieSupportSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.CostumeInfo) {
			ReadMemory(struct _RTCostumeInfo, Info, 1);
			ReadMemory(struct _RTCostumePage, Pages, Info->PageCount);
			ReadMemory(struct _RTAppliedCostumeSlot, AppliedSlots, Info->AppliedSlotCount);

		}

		if (Packet->SyncMask.TemporaryInventoryInfo) {
			ReadMemory(struct _RTInventoryInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.RecoveryInfo) {
			ReadMemory(struct _RTRecoveryInfo, Info, 1);
			ReadMemory(UInt64, Prices, Info->SlotCount);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);
		}

		if (Packet->SyncMask.PresetInfo) {
			ReadMemory(struct _RTCharacterPresetInfo, Info, 1);
		}

		if (Packet->SyncMask.AuraMasteryInfo) {
			ReadMemory(struct _RTAuraMasteryInfo, Info, 1);
			ReadMemory(struct _RTAuraMasterySlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.SecretShopInfo) {
			ReadMemory(struct _RTCharacterSecretShopData, Info, 1);

		}

		if (Packet->SyncMask.DamageBoosterInfo) {
			ReadMemory(struct _RTDamageBoosterInfo, Info, 1);
			ReadMemory(struct _RTDamageBoosterSlot, Slots, Info->SlotCount);

		}

		if (Packet->SyncMask.ExplorationInfo) {
			ReadMemory(struct _RTExplorationInfo, Info, 1);
			ReadMemory(struct _RTExplorationSlot, Slots, Info->SlotCount);

		}
    }

    if (Packet->IsTransaction) {
        if (Response->SyncMaskFailed.RawValue) {
            Response->SyncMaskFailed = Packet->SyncMask;
            DatabaseRollbackTransaction(Context->Database);
        }
        else if (!DatabaseCommitTransaction(Context->Database)) {
            Response->SyncMaskFailed = Packet->SyncMask;
            DatabaseRollbackTransaction(Context->Database);
        }
    }

    IPCSocketUnicast(Socket, Response);
}

#undef ReadMemory
