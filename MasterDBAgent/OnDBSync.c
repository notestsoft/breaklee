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

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncSettings",
				DB_INPUT_INT32(Packet->AccountID),
				DB_INPUT_INT32(Info->HotKeysDataLength),
				DB_INPUT_INT32(Info->OptionsDataLength),
				DB_INPUT_UINT32(Info->MacrosDataLength),
				DB_INPUT_DATA(Info->HotKeysData, Info->HotKeysDataLength),
				DB_INPUT_DATA(Info->OptionsData, Info->OptionsDataLength),
				DB_INPUT_DATA(Info->MacrosData, Info->MacrosDataLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.SettingsInfo = true;
			}
		}

		if (Packet->SyncMask.Info) {
			ReadMemory(struct _RTCharacterInfo, Info, 1);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncCharacter",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->PKState.RawValue),
				DB_INPUT_UINT8(Info->Level),
				DB_INPUT_UINT64(Info->Exp),
				DB_INPUT_INT64(Info->HonorPoint),
				DB_INPUT_INT64(Info->Wexp),
				DB_INPUT_INT64(Info->CurrentHP),
				DB_INPUT_INT64(Info->CurrentMP),
				DB_INPUT_INT64(Info->CurrentSP),
				DB_INPUT_INT64(Info->CurrentBP),
				DB_INPUT_INT64(Info->CurrentRage),
				DB_INPUT_INT64(Info->DP),
				DB_INPUT_UINT64(Info->DPDuration),
				DB_INPUT_UINT8(Info->SkillRank),
				DB_INPUT_UINT16(Info->SkillLevel),
				DB_INPUT_UINT64(Info->SkillExp),
				DB_INPUT_UINT16(Info->SkillPoint),
				DB_INPUT_UINT16(Info->Stat[RUNTIME_CHARACTER_STAT_STR]),
				DB_INPUT_UINT16(Info->Stat[RUNTIME_CHARACTER_STAT_DEX]),
				DB_INPUT_UINT16(Info->Stat[RUNTIME_CHARACTER_STAT_INT]),
				DB_INPUT_UINT16(Info->Stat[RUNTIME_CHARACTER_STAT_PNT]),
				DB_INPUT_UINT64(Info->Alz),
				DB_INPUT_UINT8(Info->WorldIndex),
				DB_INPUT_UINT16(Info->PositionX),
				DB_INPUT_UINT16(Info->PositionY),
				DB_INPUT_INT32(Info->DungeonIndex),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.Info = true;
			}
		}

		if (Packet->SyncMask.StyleInfo) {
			ReadMemory(struct _RTCharacterStyleInfo, Info, 1);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncCharacterStyle",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->Nation),
				DB_INPUT_UINT32(Info->WarpMask),
				DB_INPUT_UINT32(Info->MapsMask),
				DB_INPUT_UINT32(Info->Style.RawValue),
				DB_INPUT_UINT32(Info->LiveStyle.RawValue),
				DB_INPUT_UINT8(Info->ExtendedStyle.RawValue),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.StyleInfo = true;
			}
		}

		if (Packet->SyncMask.EquipmentInfo) {
			ReadMemory(struct _RTEquipmentInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->EquipmentSlotCount);
			ReadMemory(struct _RTItemSlot, InventorySlots, Info->InventorySlotCount);
			ReadMemory(struct _RTEquipmentLinkSlot, LinkSlots, Info->LinkSlotCount);
			ReadMemory(struct _RTEquipmentLockSlot, LockSlots, Info->LockSlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncEquipment",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->EquipmentSlotCount),
				DB_INPUT_UINT8(Info->InventorySlotCount),
				DB_INPUT_UINT8(Info->LinkSlotCount),
				DB_INPUT_UINT8(Info->LockSlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_INPUT_DATA(InventorySlots, InventorySlotsLength),
				DB_INPUT_DATA(LinkSlots, LinkSlotsLength),
				DB_INPUT_DATA(LockSlots, LockSlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.EquipmentInfo = true;
			}
		}

		if (Packet->SyncMask.InventoryInfo) {
			ReadMemory(struct _RTInventoryInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncInventory",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.InventoryInfo = true;
			}
		}

		if (Packet->SyncMask.SkillSlotInfo) {
			ReadMemory(struct _RTSkillSlotInfo, Info, 1);
			ReadMemory(struct _RTSkillSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncSkillSlot",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.SkillSlotInfo = true;
			}
		}

		if (Packet->SyncMask.QuickSlotInfo) {
			ReadMemory(struct _RTQuickSlotInfo, Info, 1);
			ReadMemory(struct _RTQuickSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncQuickSlot",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.QuickSlotInfo = true;
			}
		}

		if (Packet->SyncMask.AbilityInfo) {
			ReadMemory(struct _RTAbilityInfo, Info, 1);
			ReadMemory(struct _RTEssenceAbilitySlot, EssenceAbilitySlots, Info->EssenceAbilityCount);
			ReadMemory(struct _RTBlendedAbilitySlot, BlendedAbilitySlots, Info->BlendedAbilityCount);
			ReadMemory(struct _RTKarmaAbilitySlot, KarmaAbilitySlots, Info->KarmaAbilityCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncAbility",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->APTotal),
				DB_INPUT_UINT16(Info->AP),
				DB_INPUT_UINT32(Info->Axp),
				DB_INPUT_UINT8(Info->EssenceAbilityCount),
				DB_INPUT_UINT8(Info->ExtendedEssenceAbilityCount),
				DB_INPUT_UINT8(Info->BlendedAbilityCount),
				DB_INPUT_UINT8(Info->ExtendedBlendedAbilityCount),
				DB_INPUT_UINT8(Info->KarmaAbilityCount),
				DB_INPUT_UINT8(Info->ExtendedKarmaAbilityCount),
				DB_INPUT_DATA(EssenceAbilitySlots, EssenceAbilitySlotsLength),
				DB_INPUT_DATA(BlendedAbilitySlots, BlendedAbilitySlotsLength),
				DB_INPUT_DATA(KarmaAbilitySlots, KarmaAbilitySlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.AbilityInfo = true;
			}
		}

		if (Packet->SyncMask.BlessingBeadInfo) {
			ReadMemory(struct _RTBlessingBeadInfo, Info, 1);
			ReadMemory(struct _RTBlessingBeadSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncBlessingBead",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.BlessingBeadInfo = true;
			}
		}

		if (Packet->SyncMask.PremiumServiceInfo) {
			ReadMemory(struct _RTPremiumServiceInfo, Info, 1);
			ReadMemory(struct _RTPremiumServiceSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncPremiumService",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.PremiumServiceInfo = true;
			}
		}

		if (Packet->SyncMask.QuestInfo) {
			ReadMemory(struct _RTQuestInfo, Info, 1);
			ReadMemory(struct _RTQuestSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncQuest",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->SlotCount),
				DB_INPUT_DATA(Info->FinishedQuests, sizeof(Info->FinishedQuests)),
				DB_INPUT_DATA(Info->DeletedQuests, sizeof(Info->DeletedQuests)),
				DB_INPUT_DATA(Info->FinishedDungeons, sizeof(Info->FinishedDungeons)),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.QuestInfo = true;
			}
		}

		if (Packet->SyncMask.DailyQuestInfo) {
			ReadMemory(struct _RTDailyQuestInfo, Info, 1);
			ReadMemory(struct _RTDailyQuestSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncDailyQuest",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT32(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.DailyQuestInfo = true;
			}
		}

		if (Packet->SyncMask.MercenaryInfo) {
			ReadMemory(struct _RTMercenaryInfo, Info, 1);
			ReadMemory(struct _RTMercenarySlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncMercenary",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.MercenaryInfo = true;
			}
		}

		if (Packet->SyncMask.AppearanceInfo) {
			ReadMemory(struct _RTAppearanceInfo, Info, 1);
			ReadMemory(struct _RTItemSlotAppearance, EquipmentSlots, Info->EquipmentAppearanceCount);
			ReadMemory(struct _RTItemSlotAppearance, InventorySlots, Info->InventoryAppearanceCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncAppearance",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT8(Info->EquipmentAppearanceCount),
				DB_INPUT_INT16(Info->InventoryAppearanceCount),
				DB_INPUT_DATA(EquipmentSlots, EquipmentSlotsLength),
				DB_INPUT_DATA(InventorySlots, InventorySlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.AppearanceInfo = true;
			}
		}

		if (Packet->SyncMask.AchievementInfo) {
			ReadMemory(struct _RTAchievementInfo, Info, 1);
			ReadMemory(struct _RTAchievementSlot, Slots, Info->SlotCount);
			ReadMemory(struct _RTAchievementRewardSlot, RewardSlots, Info->RewardSlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncAchievement",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->AllAchievementScore),
				DB_INPUT_INT32(Info->NormalAchievementScore),
				DB_INPUT_INT32(Info->QuestAchievementScore),
				DB_INPUT_INT32(Info->DungeonAchievementScore),
				DB_INPUT_INT32(Info->ItemsAchievementScore),
				DB_INPUT_INT32(Info->PvpAchievementScore),
				DB_INPUT_INT32(Info->WarAchievementScore),
				DB_INPUT_INT32(Info->HuntingAchievementScore),
				DB_INPUT_INT32(Info->CraftAchievementScore),
				DB_INPUT_INT32(Info->CommunityAchievementScore),
				DB_INPUT_INT32(Info->SharedAchievementScore),
				DB_INPUT_INT32(Info->SpecialAchievementScore),
				DB_INPUT_INT32(Info->GeneralMemoirAchievementScore),
				DB_INPUT_INT32(Info->SharedMemoirAchievementScore),
				DB_INPUT_UINT16(Info->DisplayTitle),
				DB_INPUT_UINT16(Info->EventTitle),
				DB_INPUT_UINT16(Info->GuildTitle),
				DB_INPUT_UINT16(Info->WarTitle),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_INT32(Info->RewardSlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_INPUT_DATA(RewardSlots, RewardSlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.AchievementInfo = true;
			}
		}

		if (Packet->SyncMask.CraftInfo) {
			ReadMemory(struct _RTCraftInfo, Info, 1);
			ReadMemory(struct _RTCraftSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncCraft",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_INT32(Info->Energy),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.CraftInfo = true;
			}
		}

		if (Packet->SyncMask.RequestCraftInfo) {
			ReadMemory(struct _RTRequestCraftInfo, Info, 1);
			ReadMemory(struct _RTRequestCraftSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncRequestCraft",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->SlotCount),
				DB_INPUT_UINT16(Info->Exp),
				DB_INPUT_DATA(Info->RegisteredFlags, sizeof(Info->RegisteredFlags)),
				DB_INPUT_DATA(Info->FavoriteFlags, sizeof(Info->FavoriteFlags)),
				DB_INPUT_UINT16(Info->SortingOrder),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.RequestCraftInfo = true;
			}
		}

		if (Packet->SyncMask.BuffInfo) {
			ReadMemory(struct _RTBuffInfo, Info, 1);
			ReadMemory(struct _RTBuffSlot, Slots, Info->BuffCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncBuff",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->SkillCooldownCount),
				DB_INPUT_UINT16(Info->BuffCount),
				DB_INPUT_UINT32(Info->SpiritRaiseBuffCooldown),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.BuffInfo = true;
			}
		}

		if (Packet->SyncMask.VehicleInventoryInfo) {
			ReadMemory(struct _RTVehicleInventoryInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncVehicleInventory",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT16(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.VehicleInventoryInfo = true;
			}
		}

		if (Packet->SyncMask.WarpServiceInfo) {
			ReadMemory(struct _RTWarpServiceInfo, Info, 1);
			ReadMemory(struct _RTWarpServiceSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncWarpService",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.WarpServiceInfo = true;
			}
		}

		if (Packet->SyncMask.OverlordMasteryInfo) {
			ReadMemory(struct _RTOverlordMasteryInfo, Info, 1);
			ReadMemory(struct _RTOverlordMasterySlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncOverlordMastery",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT16(Info->Level),
				DB_INPUT_INT64(Info->Exp),
				DB_INPUT_INT16(Info->Point),
				DB_INPUT_INT8(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.OverlordMasteryInfo = true;
			}
		}

		if (Packet->SyncMask.HonorMedalInfo) {
			ReadMemory(struct _RTHonorMedalInfo, Info, 1);
			ReadMemory(struct _RTHonorMedalSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncHonorMedal",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->Grade),
				DB_INPUT_INT32(Info->Score),
				DB_INPUT_INT8(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.HonorMedalInfo = true;
			}
		}

		if (Packet->SyncMask.ForceWingInfo) {
			ReadMemory(struct _RTForceWingInfo, Info, 1);
			ReadMemory(struct _RTForceWingPresetSlot, PresetSlots, Info->PresetSlotCount);
			ReadMemory(struct _RTForceWingTrainingSlot, TrainingSlots, Info->TrainingSlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncForceWing",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->Grade),
				DB_INPUT_UINT8(Info->Level),
				DB_INPUT_INT64(Info->Exp),
				DB_INPUT_UINT8(Info->ActivePresetIndex),
				DB_INPUT_DATA(Info->PresetEnabled, sizeof(Info->PresetEnabled)),
				DB_INPUT_DATA(Info->PresetTrainingPointCount, sizeof(Info->PresetTrainingPointCount)),
				DB_INPUT_UINT8(Info->PresetSlotCount),
				DB_INPUT_UINT8(Info->TrainingSlotCount),
				DB_INPUT_DATA(Info->TrainingUnlockFlags, sizeof(Info->TrainingUnlockFlags)),
				DB_INPUT_DATA(Info->ArrivalSkillSlots, sizeof(Info->ArrivalSkillSlots)),
				DB_INPUT_DATA(&Info->ArrivalSkillRestoreSlot, sizeof(Info->ArrivalSkillRestoreSlot)),
				DB_INPUT_DATA(PresetSlots, PresetSlotsLength),
				DB_INPUT_DATA(TrainingSlots, TrainingSlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.ForceWingInfo = true;
			}
		}

		if (Packet->SyncMask.GiftboxInfo) {
			ReadMemory(struct _RTGiftBoxInfo, Info, 1);
			ReadMemory(struct _RTGiftBoxSlot, Slots, Info->SlotCount);
			ReadMemory(struct _RTGiftBoxRewardSlot, RewardSlots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncGiftbox",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT16(Info->SpecialPoints),
				DB_INPUT_UINT8(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_INPUT_DATA(RewardSlots, RewardSlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.GiftboxInfo = true;
			}
		}

		if (Packet->SyncMask.TransformInfo) {
			ReadMemory(struct _RTTransformInfo, Info, 1);
			ReadMemory(struct _RTTransformSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncTransform",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.TransformInfo = true;
			}
		}

		if (Packet->SyncMask.TranscendenceInfo) {
			ReadMemory(struct _RTTranscendenceInfo, Info, 1);
			ReadMemory(struct _RTTranscendenceSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncSkillTranscendence",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->Points),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.TransformInfo = true;
			}
		}

		if (Packet->SyncMask.StellarMasteryInfo) {
			ReadMemory(struct _RTStellarMasteryInfo, Info, 1);
			ReadMemory(struct _RTStellarMasterySlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncStellarMastery",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.StellarMasteryInfo = true;
			}
		}

		if (Packet->SyncMask.MythMasteryInfo) {
			ReadMemory(struct _RTMythMasteryInfo, Info, 1);
			ReadMemory(struct _RTMythMasterySlot, Slots, Info->PropertySlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncMythMastery",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->Rebirth),
				DB_INPUT_INT32(Info->HolyPower),
				DB_INPUT_INT32(Info->Level),
				DB_INPUT_UINT64(Info->Exp),
				DB_INPUT_INT32(Info->Points),
				DB_INPUT_INT32(Info->UnlockedPageCount),
				DB_INPUT_UINT8(Info->PropertySlotCount),
				DB_INPUT_INT32(Info->StigmaGrade),
				DB_INPUT_INT32(Info->StigmaExp),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.MythMasteryInfo = true;
			}
		}

		if (Packet->SyncMask.NewbieSupportInfo) {
			ReadMemory(struct _RTNewbieSupportInfo, Info, 1);
			ReadMemory(struct _RTNewbieSupportSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncNewbieSupport",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT64(Info->Timestamp),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.NewbieSupportInfo = true;
			}
		}

		if (Packet->SyncMask.CostumeInfo) {
			ReadMemory(struct _RTCostumeInfo, Info, 1);
			ReadMemory(struct _RTCostumePage, Pages, Info->PageCount);
			ReadMemory(struct _RTAppliedCostumeSlot, AppliedSlots, Info->AppliedSlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncCostume",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->PageCount),
				DB_INPUT_INT32(Info->AppliedSlotCount),
				DB_INPUT_INT32(Info->ActivePageIndex),
				DB_INPUT_DATA(Pages, PagesLength),
				DB_INPUT_DATA(AppliedSlots, AppliedSlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.CostumeInfo = true;
			}
		}

		if (Packet->SyncMask.TemporaryInventoryInfo) {
			ReadMemory(struct _RTInventoryInfo, Info, 1);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncTemporaryInventory",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT16(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.TemporaryInventoryInfo = true;
			}
		}

		if (Packet->SyncMask.RecoveryInfo) {
			ReadMemory(struct _RTRecoveryInfo, Info, 1);
			ReadMemory(UInt64, Prices, Info->SlotCount);
			ReadMemory(struct _RTItemSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncRecovery",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_DATA(Prices, PricesLength),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.RecoveryInfo = true;
			}
		}

		if (Packet->SyncMask.PresetInfo) {
			ReadMemory(struct _RTCharacterPresetInfo, Info, 1);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncRecovery",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_DATA(Info->Configurations, sizeof(Info->Configurations)),
				DB_INPUT_INT32(Info->ActiveEquipmentPresetIndex),
				DB_INPUT_INT32(Info->ActiveAnimaMasteryPresetIndex),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.PresetInfo = true;
			}
		}

		if (Packet->SyncMask.AuraMasteryInfo) {
			ReadMemory(struct _RTAuraMasteryInfo, Info, 1);
			ReadMemory(struct _RTAuraMasterySlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncAuraMastery",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT32(Info->Points),
				DB_INPUT_INT32(Info->AccumulatedTimeInMinutes),
				DB_INPUT_INT8(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.AuraMasteryInfo = true;
			}
		}

		if (Packet->SyncMask.SecretShopInfo) {
			ReadMemory(struct _RTCharacterSecretShopData, Info, 1);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncAuraMastery",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_UINT8(Info->RefreshCost),
				DB_INPUT_DATA(Info->Slots, sizeof(Info->Slots)),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.SecretShopInfo = true;
			}
		}

		if (Packet->SyncMask.DamageBoosterInfo) {
			ReadMemory(struct _RTDamageBoosterInfo, Info, 1);
			ReadMemory(struct _RTDamageBoosterSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncDamageBooster",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_DATA(Info->ItemID, sizeof(Info->ItemID)),
				DB_INPUT_INT8(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.DamageBoosterInfo = true;
			}
		}

		if (Packet->SyncMask.ExplorationInfo) {
			ReadMemory(struct _RTExplorationInfo, Info, 1);
			ReadMemory(struct _RTExplorationSlot, Slots, Info->SlotCount);

			if (!DatabaseCallProcedure(
				Context->Database,
				"SyncExploration",
				DB_INPUT_INT32(Packet->CharacterID),
				DB_INPUT_INT64(Info->EndDate),
				DB_INPUT_INT32(Info->Points),
				DB_INPUT_INT32(Info->Level),
				DB_INPUT_INT32(Info->SlotCount),
				DB_INPUT_DATA(Slots, SlotsLength),
				DB_PARAM_END
			)) {
				Response->SyncMaskFailed.ExplorationInfo = true;
			}
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
