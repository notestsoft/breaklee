#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_CHARACTER) {
    IPC_D2W_DATA_GET_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_CHARACTER);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;
    Response->AccountID = Packet->AccountID;
    Response->CharacterID = Packet->CharacterID;
    Response->CharacterIndex = Packet->CharacterIndex;

    struct _RTItemSlot EquipmentSlots[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT] = { 0 };
    struct _RTItemSlot EquipmentInventorySlots[RUNTIME_CHARACTER_MAX_EQUIPMENT_PRESET_SLOT_COUNT] = { 0 };
    struct _RTEquipmentLinkSlot EquipmentLinkSlots[RUNTIME_CHARACTER_MAX_EQUIPMENT_PRESET_SLOT_COUNT] = { 0 };
    struct _RTEquipmentLockSlot EquipmentLockSlots[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT] = { 0 };
    struct _RTItemSlot InventorySlots[RUNTIME_INVENTORY_PAGE_SIZE * RUNTIME_INVENTORY_PAGE_COUNT] = { 0 };
    struct _RTItemSlot VehicleInventorySlots[RUNTIME_CHARACTER_MAX_VEHICLE_INVENTORY_SLOT_COUNT] = { 0 };
    struct _RTSkillSlot SkillSlots[RUNTIME_CHARACTER_MAX_SKILL_SLOT_COUNT] = { 0 };
    struct _RTQuickSlot QuickSlots[RUNTIME_CHARACTER_MAX_QUICK_SLOT_COUNT] = { 0 };
    struct _RTMercenarySlot MercenarySlots[RUNTIME_CHARACTER_MAX_MERCENARY_SLOT_COUNT] = { 0 };
    struct _RTEssenceAbilitySlot EssenceAbilitySlots[RUNTIME_CHARACTER_MAX_ESSENCE_ABILITY_SLOT_COUNT] = { 0 };
    struct _RTBlendedAbilitySlot BlendedAbilitySlots[RUNTIME_CHARACTER_MAX_BLENDED_ABILITY_SLOT_COUNT] = { 0 };
    struct _RTKarmaAbilitySlot KarmaAbilitySlots[RUNTIME_CHARACTER_MAX_KARMA_ABILITY_SLOT_COUNT] = { 0 };
    struct _RTBlessingBeadSlot BlessingBeadSlots[RUNTIME_CHARACTER_MAX_BLESSING_BEAD_SLOT_COUNT] = { 0 };
    struct _RTPremiumServiceSlot PremiumServiceSlots[RUNTIME_CHARACTER_MAX_PREMIUM_SERVICE_SLOT_COUNT] = { 0 };
    struct _RTQuestSlot QuestSlots[RUNTIME_CHARACTER_MAX_QUEST_SLOT_COUNT] = { 0 };
    struct _RTDailyQuestSlot DailyQuestSlots[RUNTIME_CHARACTER_MAX_DAILY_QUEST_SLOT_COUNT] = { 0 };
    struct _RTItemSlotAppearance EquipmentAppearanceSlots[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT] = { 0 };
    struct _RTItemSlotAppearance InventoryAppearanceSlots[RUNTIME_INVENTORY_TOTAL_SIZE] = { 0 };
    struct _RTAchievementSlot AchievementSlots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT] = { 0 };
    struct _RTAchievementRewardSlot AchievementRewardSlots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT] = { 0 };
    struct _RTCraftSlot CraftSlots[RUNTIME_CHARACTER_MAX_CRAFT_SLOT_COUNT] = { 0 };
    struct _RTRequestCraftSlot RequestCraftSlots[RUNTIME_CHARACTER_MAX_REQUEST_CRAFT_SLOT_COUNT] = { 0 };
    struct _RTBuffSlot BuffSlots[RUNTIME_CHARACTER_MAX_BUFF_SLOT_COUNT] = { 0 };
    struct _RTGoldMeritMasterySlot GoldMeritMasterySlots[RUNTIME_CHARACTER_MAX_GOLD_MERIT_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTPlatinumMeritExtendedMemorizeSlot PlatinumMeritExtendedMemorizeSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MEMORIZE_COUNT] = { 0 };
    struct _RTPlatinumMeritUnlockedSlot PlatinumMeritUnlockedSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTPlatinumMeritMasterySlot PlatinumMeritMasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTPlatinumMeritSpecialMasterySlot PlatinumMeritSpecialMasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_SPECIAL_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTDiamondMeritExtendedMemorizeSlot DiamondMeritExtendedMemorizeSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MEMORIZE_COUNT] = { 0 };
    struct _RTDiamondMeritUnlockedSlot DiamondMeritUnlockedSlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTDiamondMeritMasterySlot DiamondMeritMasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTDiamondMeritSpecialMasterySlot DiamondMeritSpecialMasterySlots[RUNTIME_CHARACTER_MAX_PLATINUM_MERIT_SPECIAL_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTAchievementExtendedRewardSlot AchievementExtendedRewardSlots[RUNTIME_CHARACTER_MAX_ACHIEVEMENT_SLOT_COUNT] = { 0 };
    struct _RTWarpServiceSlot WarpServiceSlots[RUNTIME_CHARACTER_MAX_WARP_SERVICE_SLOT_COUNT] = { 0 };
    struct _RTOverlordMasterySlot OverlordMasterySlots[RUNTIME_CHARACTER_MAX_OVERLORD_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTHonorMedalSlot HonorMedalSlots[RUNTIME_CHARACTER_MAX_HONOR_MEDAL_SLOT_COUNT] = { 0 };
    struct _RTForceWingPresetSlot ForceWingPresetSlots[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_SLOT_COUNT] = { 0 };
    struct _RTForceWingTrainingSlot ForceWingTrainingSlots[RUNTIME_CHARACTER_MAX_FORCE_WING_PRESET_SLOT_COUNT] = { 0 };
    struct _RTGiftBoxSlot GiftBoxSlots[RUNTIME_CHARACTER_MAX_GIFT_BOX_SLOT_COUNT] = { 0 };
    struct _RTGiftBoxRewardSlot GiftBoxRewardSlots[RUNTIME_CHARACTER_MAX_GIFT_BOX_SLOT_COUNT] = { 0 };
    struct _RTCollectionSlot CollectionSlots[RUNTIME_CHARACTER_MAX_COLLECTION_SLOT_COUNT] = { 0 };
    struct _RTTransformSlot TransformSlots[RUNTIME_CHARACTER_MAX_TRANSFORM_SLOT_COUNT] = { 0 };
    struct _RTAuraMasterySlot AuraMasterySlots[RUNTIME_CHARACTER_MAX_AURA_MASTERY_SLOT_COUNT] = { 0 };
    struct _RTTranscendenceSlot TranscendenceSlots[RUNTIME_CHARACTER_MAX_TRANSCENDENCE_SLOT_COUNT] = { 0 };
    struct _RTDamageBoosterSlot DamageBoosterSlots[RUNTIME_CHARACTER_MAX_DAMAGE_BOOSTER_SLOT_COUNT] = { 0 };
    struct _RTStellarMasterySlot StellarMasterySlots[RUNTIME_CHARACTER_MAX_STELLAR_SLOT_COUNT] = { 0 };
    struct _RTMythMasterySlot MythMasterySlots[RUNTIME_CHARACTER_MAX_MYTH_SLOT_COUNT] = { 0 };
    struct _RTNewbieSupportSlot NewbieSupportSlots[RUNTIME_CHARACTER_MAX_NEWBIE_SUPPORT_SLOT_COUNT] = { 0 };
    struct _RTEventPassMissionPage EventPassMissionPages[RUNTIME_CHARACTER_MAX_EVENT_PASS_MISSION_PAGE_COUNT] = { 0 };
    struct _RTEventPassMissionSlot EventPassMissionSlots[RUNTIME_CHARACTER_MAX_EVENT_PASS_MISSION_SLOT_COUNT] = { 0 };
    struct _RTEventPassRewardSlot EventPassRewardSlots[RUNTIME_CHARACTER_MAX_EVENT_PASS_REWARD_SLOT_COUNT] = { 0 };
    struct _RTAccountCostumeSlot AccountCostumeSlots[RUNTIME_CHARACTER_MAX_ACCOUNT_COSTUME_SLOT_COUNT] = { 0 };
    struct _RTCostumePage CostumePages[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_COUNT] = { 0 };
    struct _RTAppliedCostumeSlot AppliedCostumeSlots[RUNTIME_CHARACTER_MAX_COSTUME_PAGE_SLOT_COUNT] = { 0 };
    struct _RTExplorationSlot ExplorationSlots[RUNTIME_CHARACTER_MAX_EXPLORATION_SLOT_COUNT] = { 0 };
    struct _RTAnimaMasteryPresetData AnimaMasteryPresetData[RUNTIME_MAX_ANIMA_MASTERY_PRESET_COUNT] = { 0 };
    struct _RTAnimaMasteryCategoryData AnimaMasteryCategoryData[RUNTIME_MAX_ANIMA_MASTERY_STORAGE_COUNT * RUNTIME_MAX_ANIMA_MASTERY_CATEGORY_COUNT] = { 0 };

    DatabaseHandleRef Handle = DatabaseCallProcedureFetch(
        Context->Database,
        "GetCharacter",
        DB_INPUT_INT32(Packet->CharacterID),
        DB_PARAM_END
    );

    if (!DatabaseHandleReadNext(
        Handle,
        DB_TYPE_UINT8, &Response->Character.IsWarehousePasswordSet,
        DB_TYPE_UINT8, &Response->Character.IsInventoryPasswordSet,
        DB_TYPE_UINT8, &Response->Character.IsWarehouseLocked,
        DB_TYPE_UINT8, &Response->Character.IsInventoryLocked,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.WorldIndex,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.DungeonIndex,
        DB_TYPE_INT16, &Response->Character.CharacterInfo.PositionX,
        DB_TYPE_INT16, &Response->Character.CharacterInfo.PositionY,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.Exp,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.Alz,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.Wexp,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.Level,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.Stat[RUNTIME_CHARACTER_STAT_STR],
        DB_TYPE_INT32, &Response->Character.CharacterInfo.Stat[RUNTIME_CHARACTER_STAT_DEX],
        DB_TYPE_INT32, &Response->Character.CharacterInfo.Stat[RUNTIME_CHARACTER_STAT_INT],
        DB_TYPE_INT32, &Response->Character.CharacterInfo.Stat[RUNTIME_CHARACTER_STAT_PNT],
        DB_TYPE_INT32, &Response->Character.CharacterInfo.SkillRank,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.CurrentHP,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.CurrentMP,
        DB_TYPE_INT16, &Response->Character.CharacterInfo.CurrentSP,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.CurrentRage,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.CurrentBP,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.DPDuration,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.DP,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.SkillLevel,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.SkillExp,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.SkillPoint,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.RestExp,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.HonorPoint,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.DeathPenaltyExp,
        DB_TYPE_INT64, &Response->Character.CharacterInfo.DeathPenaltyHp,
        DB_TYPE_INT32, &Response->Character.CharacterInfo.DeathPenaltyMp,
        DB_TYPE_INT16, &Response->Character.CharacterInfo.PKState,
        DB_TYPE_UINT8, &Response->Character.CharacterStyleInfo.Nation,
        DB_TYPE_INT32, &Response->Character.CharacterStyleInfo.WarpMask,
        DB_TYPE_INT32, &Response->Character.CharacterStyleInfo.MapsMask,
        DB_TYPE_INT32, &Response->Character.CharacterStyleInfo.Style.RawValue,
        DB_TYPE_INT32, &Response->Character.CharacterStyleInfo.LiveStyle.RawValue,
        DB_TYPE_UINT8, &Response->Character.EquipmentInfo.EquipmentSlotCount,
        DB_TYPE_UINT8, &Response->Character.EquipmentInfo.InventorySlotCount,
        DB_TYPE_UINT8, &Response->Character.EquipmentInfo.LinkSlotCount,
        DB_TYPE_UINT8, &Response->Character.EquipmentInfo.LockSlotCount,
        DB_TYPE_INT16, &Response->Character.InventoryInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.VehicleInventoryInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.SkillSlotInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.QuickSlotInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.MercenaryInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.ItemPeriodCount,
        DB_TYPE_INT16, &Response->Character.AbilityInfo.APTotal,
        DB_TYPE_INT16, &Response->Character.AbilityInfo.AP,
        DB_TYPE_INT32, &Response->Character.AbilityInfo.Axp,
        DB_TYPE_UINT8, &Response->Character.AbilityInfo.EssenceAbilityCount,
        DB_TYPE_UINT8, &Response->Character.AbilityInfo.ExtendedEssenceAbilityCount,
        DB_TYPE_UINT8, &Response->Character.AbilityInfo.BlendedAbilityCount,
        DB_TYPE_UINT8, &Response->Character.AbilityInfo.ExtendedBlendedAbilityCount,
        DB_TYPE_UINT8, &Response->Character.AbilityInfo.KarmaAbilityCount,
        DB_TYPE_UINT8, &Response->Character.AbilityInfo.ExtendedKarmaAbilityCount,
        DB_TYPE_UINT8, &Response->Character.BlessingBeadInfo.SlotCount,
        DB_TYPE_UINT8, &Response->Character.PremiumServiceInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.QuestInfo.SlotCount,
        DB_TYPE_DATA, Response->Character.QuestInfo.FinishedQuests, sizeof(Response->Character.QuestInfo.FinishedQuests),
        DB_TYPE_DATA, Response->Character.QuestInfo.DeletedQuests, sizeof(Response->Character.QuestInfo.DeletedQuests),
        DB_TYPE_DATA, Response->Character.QuestInfo.FinishedDungeons, sizeof(Response->Character.QuestInfo.FinishedDungeons),
        DB_TYPE_INT32, &Response->Character.DailyQuestInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.HelpWindow,
        DB_TYPE_UINT8, &Response->Character.AppearanceInfo.EquipmentAppearanceCount,
        DB_TYPE_INT16, &Response->Character.AppearanceInfo.InventoryAppearanceCount,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.AllAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.NormalAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.QuestAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.DungeonAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.ItemsAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.PvpAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.WarAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.HuntingAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.CraftAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.CommunityAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.SharedAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.SpecialAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.GeneralMemoirAchievementScore,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.SharedMemoirAchievementScore,
        DB_TYPE_INT16, &Response->Character.AchievementInfo.DisplayTitle,
        DB_TYPE_INT16, &Response->Character.AchievementInfo.EventTitle,
        DB_TYPE_INT16, &Response->Character.AchievementInfo.GuildTitle,
        DB_TYPE_INT16, &Response->Character.AchievementInfo.WarTitle,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.AchievementInfo.RewardSlotCount,
        DB_TYPE_INT32, &Response->Character.CraftInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.CraftInfo.Energy,
        DB_TYPE_UINT8, &Response->Character.RequestCraftInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.RequestCraftInfo.Exp,
        DB_TYPE_DATA, Response->Character.RequestCraftInfo.RegisteredFlags, sizeof(Response->Character.RequestCraftInfo.RegisteredFlags),
        DB_TYPE_DATA, Response->Character.RequestCraftInfo.FavoriteFlags, sizeof(Response->Character.RequestCraftInfo.FavoriteFlags),
        DB_TYPE_INT16, &Response->Character.RequestCraftInfo.SortingOrder,
        DB_TYPE_UINT8, &Response->Character.BuffInfo.SkillCooldownCount,
        DB_TYPE_INT16, &Response->Character.BuffInfo.BuffCount,
        DB_TYPE_INT32, &Response->Character.BuffInfo.SpiritRaiseBuffCooldown,
        DB_TYPE_INT32, &Response->Character.UpgradeInfo.UpgradePoints,
        DB_TYPE_INT64, &Response->Character.UpgradeInfo.UpgradePointTimestamp,
        DB_TYPE_INT32, &Response->Character.GoldMeritMasteryInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.GoldMeritMasteryInfo.Exp,
        DB_TYPE_INT32, &Response->Character.GoldMeritMasteryInfo.Points,
        DB_TYPE_UINT8, &Response->Character.PlatinumMeritMasteryInfo.IsEnabled,
        DB_TYPE_INT32, &Response->Character.PlatinumMeritMasteryInfo.Exp,
        DB_TYPE_INT32, &Response->Character.PlatinumMeritMasteryInfo.Points,
        DB_TYPE_UINT8, &Response->Character.PlatinumMeritMasteryInfo.ActiveMemorizeIndex,
        DB_TYPE_INT32, &Response->Character.PlatinumMeritMasteryInfo.OpenSlotMasteryIndex,
        DB_TYPE_INT64, &Response->Character.PlatinumMeritMasteryInfo.OpenSlotUnlockTime,
        DB_TYPE_INT16, &Response->Character.PlatinumMeritMasteryInfo.ExtendedMemorizeCount,
        DB_TYPE_INT16, &Response->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount,
        DB_TYPE_INT16, &Response->Character.PlatinumMeritMasteryInfo.MasterySlotCount,
        DB_TYPE_INT16, &Response->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount,
        DB_TYPE_UINT8, &Response->Character.DiamondMeritMasteryInfo.IsEnabled,
        DB_TYPE_INT32, &Response->Character.DiamondMeritMasteryInfo.Exp,
        DB_TYPE_INT32, &Response->Character.DiamondMeritMasteryInfo.Points,
        DB_TYPE_UINT8, &Response->Character.DiamondMeritMasteryInfo.ActiveMemorizeIndex,
        DB_TYPE_INT32, &Response->Character.DiamondMeritMasteryInfo.OpenSlotMasteryIndex,
        DB_TYPE_INT64, &Response->Character.DiamondMeritMasteryInfo.OpenSlotUnlockTime,
        DB_TYPE_INT16, &Response->Character.DiamondMeritMasteryInfo.ExtendedMemorizeCount,
        DB_TYPE_INT16, &Response->Character.DiamondMeritMasteryInfo.UnlockedSlotCount,
        DB_TYPE_INT16, &Response->Character.DiamondMeritMasteryInfo.MasterySlotCount,
        DB_TYPE_INT16, &Response->Character.DiamondMeritMasteryInfo.SpecialMasterySlotCount,
        DB_TYPE_INT32, &Response->Character.DiamondMeritMasteryInfo.ExtendedMasterySlotCount,
        DB_TYPE_INT32, &Response->Character.AchievementExtendedInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.ForceGem,
        DB_TYPE_INT32, &Response->Character.WarpServiceInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.OverlordMasteryInfo.Level,
        DB_TYPE_INT64, &Response->Character.OverlordMasteryInfo.Exp,
        DB_TYPE_INT16, &Response->Character.OverlordMasteryInfo.Point,
        DB_TYPE_UINT8, &Response->Character.OverlordMasteryInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.HonorMedalInfo.Grade,
        DB_TYPE_INT32, &Response->Character.HonorMedalInfo.Score,
        DB_TYPE_UINT8, &Response->Character.HonorMedalInfo.SlotCount,
        DB_TYPE_UINT8, &Response->Character.ForceWingInfo.Grade,
        DB_TYPE_UINT8, &Response->Character.ForceWingInfo.Level,
        DB_TYPE_INT64, &Response->Character.ForceWingInfo.Exp,
        DB_TYPE_UINT8, &Response->Character.ForceWingInfo.ActivePresetIndex,
        DB_TYPE_DATA, Response->Character.ForceWingInfo.PresetEnabled, sizeof(Response->Character.ForceWingInfo.PresetEnabled),
        DB_TYPE_DATA, Response->Character.ForceWingInfo.PresetTrainingPointCount, sizeof(Response->Character.ForceWingInfo.PresetTrainingPointCount),
        DB_TYPE_UINT8, &Response->Character.ForceWingInfo.PresetSlotCount,
        DB_TYPE_UINT8, &Response->Character.ForceWingInfo.TrainingSlotCount,
        DB_TYPE_DATA, Response->Character.ForceWingInfo.TrainingUnlockFlags, sizeof(Response->Character.ForceWingInfo.TrainingUnlockFlags),
        DB_TYPE_DATA, Response->Character.ForceWingInfo.ArrivalSkillSlots, sizeof(Response->Character.ForceWingInfo.ArrivalSkillSlots),
        DB_TYPE_DATA, Response->Character.ForceWingInfo.ArrivalSkillRestoreSlot, sizeof(Response->Character.ForceWingInfo.ArrivalSkillRestoreSlot),
        DB_TYPE_INT16, &Response->Character.GiftBoxInfo.SpecialPoints,
        DB_TYPE_UINT8, &Response->Character.GiftBoxInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.CollectionInfo.SlotCount,
        DB_TYPE_INT16, &Response->Character.TransformInfo.SlotCount,
        DB_TYPE_UINT8, &Response->Character.SecretShopData.RefreshCost,
        DB_TYPE_INT64, &Response->Character.SecretShopData.ResetTimestamp,
        DB_TYPE_UINT8, &Response->Character.SecretShopData.RefreshCount,
        DB_TYPE_DATA, Response->Character.SecretShopData.Slots, sizeof(Response->Character.SecretShopData.Slots),
        DB_TYPE_INT32, &Response->Character.AuraMasteryInfo.Points,
        DB_TYPE_INT32, &Response->Character.AuraMasteryInfo.AccumulatedTimeInMinutes,
        DB_TYPE_UINT8, &Response->Character.AuraMasteryInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.TranscendenceInfo.Points,
        DB_TYPE_INT32, &Response->Character.TranscendenceInfo.SlotCount,
        DB_TYPE_DATA, Response->Character.DamageBoosterInfo.ItemID, sizeof(Response->Character.DamageBoosterInfo.ItemID),
        DB_TYPE_UINT8, &Response->Character.DamageBoosterInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.ResearchSupportInfo.Exp,
        DB_TYPE_INT32, &Response->Character.ResearchSupportInfo.DecodedCircuitCount,
        DB_TYPE_UINT8, &Response->Character.ResearchSupportInfo.ResetCount,
        DB_TYPE_INT64, &Response->Character.ResearchSupportInfo.SeasonStartDate,
        DB_TYPE_INT64, &Response->Character.ResearchSupportInfo.SeasonEndDate,
        DB_TYPE_DATA, Response->Character.ResearchSupportInfo.MaterialSlots, sizeof(Response->Character.ResearchSupportInfo.MaterialSlots),
        DB_TYPE_DATA, &Response->Character.ResearchSupportInfo.ActiveMissionBoard, sizeof(Response->Character.ResearchSupportInfo.ActiveMissionBoard),
        DB_TYPE_DATA, Response->Character.ResearchSupportInfo.MissionBoards, sizeof(Response->Character.ResearchSupportInfo.MissionBoards),
        DB_TYPE_UINT8, &Response->Character.StellarMasteryInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.MythMasteryInfo.Rebirth,
        DB_TYPE_INT32, &Response->Character.MythMasteryInfo.HolyPower,
        DB_TYPE_INT32, &Response->Character.MythMasteryInfo.Level,
        DB_TYPE_INT64, &Response->Character.MythMasteryInfo.Exp,
        DB_TYPE_INT32, &Response->Character.MythMasteryInfo.Points,
        DB_TYPE_INT32, &Response->Character.MythMasteryInfo.UnlockedPageCount,
        DB_TYPE_UINT8, &Response->Character.MythMasteryInfo.PropertySlotCount,
        DB_TYPE_INT32, &Response->Character.MythMasteryInfo.StigmaGrade,
        DB_TYPE_INT32, &Response->Character.MythMasteryInfo.StigmaExp,
        DB_TYPE_INT64, &Response->Character.NewbieSupportInfo.Timestamp,
        DB_TYPE_INT32, &Response->Character.NewbieSupportInfo.SlotCount,
        DB_TYPE_INT64, &Response->Character.EventPassInfo.StartDate,
        DB_TYPE_INT64, &Response->Character.EventPassInfo.EndDate,
        DB_TYPE_INT32, &Response->Character.EventPassInfo.MissionPageCount,
        DB_TYPE_INT32, &Response->Character.EventPassInfo.MissionSlotCount,
        DB_TYPE_INT32, &Response->Character.EventPassInfo.RewardSlotCount,
        DB_TYPE_INT32, &Response->Character.CostumeWarehouseInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.CostumeInfo.PageCount,
        DB_TYPE_INT32, &Response->Character.CostumeInfo.AppliedSlotCount,
        DB_TYPE_INT32, &Response->Character.CostumeInfo.ActivePageIndex,
        DB_TYPE_INT64, &Response->Character.ExplorationInfo.EndDate,
        DB_TYPE_INT32, &Response->Character.ExplorationInfo.Points,
        DB_TYPE_INT32, &Response->Character.ExplorationInfo.Level,
        DB_TYPE_INT32, &Response->Character.ExplorationInfo.SlotCount,
        DB_TYPE_INT32, &Response->Character.AnimaMasteryInfo.PresetCount,
        DB_TYPE_INT32, &Response->Character.AnimaMasteryInfo.StorageCount,
        DB_TYPE_INT32, &Response->Character.AnimaMasteryInfo.UnlockedCategoryFlags,
        DB_TYPE_DATA, Response->Character.PresetInfo.Configurations, sizeof(Response->Character.PresetInfo.Configurations),
        DB_TYPE_INT32, &Response->Character.PresetInfo.ActiveEquipmentPresetIndex,
        DB_TYPE_INT32, &Response->Character.PresetInfo.ActiveAnimaMasteryPresetIndex,
        DB_TYPE_INT32, &Response->SettingsInfo.HotKeysDataLength,
        DB_TYPE_INT32, &Response->SettingsInfo.OptionsDataLength,
        DB_TYPE_INT32, &Response->SettingsInfo.MacrosDataLength,
        DB_TYPE_UINT8, &Response->Character.NameLength,
        DB_TYPE_STRING, Response->Character.Name, sizeof(Response->Character.Name),
        DB_TYPE_DATA, EquipmentSlots, sizeof(EquipmentSlots),
        DB_TYPE_DATA, EquipmentInventorySlots, sizeof(EquipmentInventorySlots),
        DB_TYPE_DATA, EquipmentLinkSlots, sizeof(EquipmentLinkSlots),
        DB_TYPE_DATA, EquipmentLockSlots, sizeof(EquipmentLockSlots),
        DB_TYPE_DATA, InventorySlots, sizeof(InventorySlots),
        DB_TYPE_DATA, SkillSlots, sizeof(SkillSlots),
        DB_TYPE_DATA, QuickSlots, sizeof(QuickSlots),
        DB_TYPE_DATA, EssenceAbilitySlots, sizeof(EssenceAbilitySlots),
        DB_TYPE_DATA, BlendedAbilitySlots, sizeof(BlendedAbilitySlots),
        DB_TYPE_DATA, KarmaAbilitySlots, sizeof(KarmaAbilitySlots),
        DB_TYPE_DATA, BlessingBeadSlots, sizeof(BlessingBeadSlots),
        DB_TYPE_DATA, PremiumServiceSlots, sizeof(PremiumServiceSlots),
        DB_TYPE_DATA, QuestSlots, sizeof(QuestSlots),
        DB_TYPE_DATA, DailyQuestSlots, sizeof(DailyQuestSlots),
        DB_TYPE_DATA, MercenarySlots, sizeof(MercenarySlots),
        DB_TYPE_DATA, EquipmentAppearanceSlots, sizeof(EquipmentAppearanceSlots),
        DB_TYPE_DATA, InventoryAppearanceSlots, sizeof(InventoryAppearanceSlots),
        DB_TYPE_DATA, AchievementSlots, sizeof(AchievementSlots),
        DB_TYPE_DATA, AchievementRewardSlots, sizeof(AchievementRewardSlots),
        DB_TYPE_DATA, AchievementExtendedRewardSlots, sizeof(AchievementExtendedRewardSlots),
        DB_TYPE_DATA, BuffSlots, sizeof(BuffSlots),
        DB_TYPE_DATA, CraftSlots, sizeof(CraftSlots),
        DB_TYPE_DATA, RequestCraftSlots, sizeof(RequestCraftSlots),
        DB_TYPE_DATA, VehicleInventorySlots, sizeof(VehicleInventorySlots),
        DB_TYPE_DATA, GoldMeritMasterySlots, sizeof(GoldMeritMasterySlots),
        DB_TYPE_DATA, PlatinumMeritExtendedMemorizeSlots, sizeof(PlatinumMeritExtendedMemorizeSlots),
        DB_TYPE_DATA, PlatinumMeritUnlockedSlots, sizeof(PlatinumMeritUnlockedSlots),
        DB_TYPE_DATA, PlatinumMeritMasterySlots, sizeof(PlatinumMeritMasterySlots),
        DB_TYPE_DATA, PlatinumMeritSpecialMasterySlots, sizeof(PlatinumMeritSpecialMasterySlots),
        DB_TYPE_DATA, DiamondMeritExtendedMemorizeSlots, sizeof(DiamondMeritExtendedMemorizeSlots),
        DB_TYPE_DATA, DiamondMeritUnlockedSlots, sizeof(DiamondMeritUnlockedSlots),
        DB_TYPE_DATA, DiamondMeritMasterySlots, sizeof(DiamondMeritMasterySlots),
        DB_TYPE_DATA, DiamondMeritSpecialMasterySlots, sizeof(DiamondMeritSpecialMasterySlots),
        DB_TYPE_DATA, WarpServiceSlots, sizeof(WarpServiceSlots),
        DB_TYPE_DATA, OverlordMasterySlots, sizeof(OverlordMasterySlots),
        DB_TYPE_DATA, HonorMedalSlots, sizeof(HonorMedalSlots),
        DB_TYPE_DATA, ForceWingPresetSlots, sizeof(ForceWingPresetSlots),
        DB_TYPE_DATA, ForceWingTrainingSlots, sizeof(ForceWingTrainingSlots),
        DB_TYPE_DATA, GiftBoxSlots, sizeof(GiftBoxSlots),
        DB_TYPE_DATA, CollectionSlots, sizeof(CollectionSlots),
        DB_TYPE_DATA, TransformSlots, sizeof(TransformSlots),
        DB_TYPE_DATA, AuraMasterySlots, sizeof(AuraMasterySlots),
        DB_TYPE_DATA, TranscendenceSlots, sizeof(TranscendenceSlots),
        DB_TYPE_DATA, DamageBoosterSlots, sizeof(DamageBoosterSlots),
        DB_TYPE_DATA, StellarMasterySlots, sizeof(StellarMasterySlots),
        DB_TYPE_DATA, MythMasterySlots, sizeof(MythMasterySlots),
        DB_TYPE_DATA, NewbieSupportSlots, sizeof(NewbieSupportSlots),
        DB_TYPE_DATA, EventPassMissionPages, sizeof(EventPassMissionPages),
        DB_TYPE_DATA, EventPassMissionSlots, sizeof(EventPassMissionSlots),
        DB_TYPE_DATA, EventPassRewardSlots, sizeof(EventPassRewardSlots),
        DB_TYPE_DATA, AccountCostumeSlots, sizeof(AccountCostumeSlots),
        DB_TYPE_DATA, CostumePages, sizeof(CostumePages),
        DB_TYPE_DATA, AppliedCostumeSlots, sizeof(AppliedCostumeSlots),
        DB_TYPE_DATA, ExplorationSlots, sizeof(ExplorationSlots),
        DB_TYPE_DATA, AnimaMasteryPresetData, sizeof(AnimaMasteryPresetData),
        DB_TYPE_DATA, AnimaMasteryCategoryData, sizeof(AnimaMasteryCategoryData),
        DB_TYPE_DATA, Response->SettingsInfo.HotKeysData, sizeof(Response->SettingsInfo.HotKeysData),
        DB_TYPE_DATA, Response->SettingsInfo.OptionsData, sizeof(Response->SettingsInfo.OptionsData),
        DB_TYPE_DATA, Response->SettingsInfo.MacrosData, sizeof(Response->SettingsInfo.MacrosData),
        DB_PARAM_END
    )) {
        goto error;
    }
    DatabaseHandleFlush(Handle);

    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EquipmentSlots, sizeof(struct _RTItemSlot) * Response->Character.EquipmentInfo.EquipmentSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EquipmentInventorySlots, sizeof(struct _RTItemSlot) * Response->Character.EquipmentInfo.InventorySlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EquipmentLinkSlots, sizeof(struct _RTEquipmentLinkSlot) * Response->Character.EquipmentInfo.LinkSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EquipmentLockSlots, sizeof(struct _RTEquipmentLockSlot) * Response->Character.EquipmentInfo.LockSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, InventorySlots, sizeof(struct _RTItemSlot) * Response->Character.InventoryInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, VehicleInventorySlots, sizeof(struct _RTItemSlot) * Response->Character.VehicleInventoryInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, SkillSlots, sizeof(struct _RTSkillSlot) * Response->Character.SkillSlotInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, QuickSlots, sizeof(struct _RTQuickSlot) * Response->Character.QuickSlotInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, MercenarySlots, sizeof(struct _RTMercenarySlot) * Response->Character.MercenaryInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EssenceAbilitySlots, sizeof(struct _RTEssenceAbilitySlot) * Response->Character.AbilityInfo.EssenceAbilityCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, BlendedAbilitySlots, sizeof(struct _RTBlendedAbilitySlot) * Response->Character.AbilityInfo.BlendedAbilityCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, KarmaAbilitySlots, sizeof(struct _RTKarmaAbilitySlot) * Response->Character.AbilityInfo.KarmaAbilityCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, BlessingBeadSlots, sizeof(struct _RTBlessingBeadSlot) * Response->Character.BlessingBeadInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, PremiumServiceSlots, sizeof(struct _RTPremiumServiceSlot) * Response->Character.PremiumServiceInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, QuestSlots, sizeof(struct _RTQuestSlot) * Response->Character.QuestInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, DailyQuestSlots, sizeof(struct _RTDailyQuestSlot) * Response->Character.DailyQuestInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EquipmentAppearanceSlots, sizeof(struct _RTItemSlotAppearance) * Response->Character.AppearanceInfo.EquipmentAppearanceCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, InventoryAppearanceSlots, sizeof(struct _RTItemSlotAppearance) * Response->Character.AppearanceInfo.InventoryAppearanceCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, AchievementSlots, sizeof(struct _RTAchievementSlot) * Response->Character.AchievementInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, AchievementRewardSlots, sizeof(struct _RTAchievementRewardSlot) * Response->Character.AchievementInfo.RewardSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, CraftSlots, sizeof(struct _RTCraftSlot) * Response->Character.CraftInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, RequestCraftSlots, sizeof(struct _RTRequestCraftSlot) * Response->Character.RequestCraftInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, BuffSlots, sizeof(struct _RTBuffSlot) * Response->Character.BuffInfo.BuffCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, GoldMeritMasterySlots, sizeof(struct _RTGoldMeritMasterySlot) * Response->Character.GoldMeritMasteryInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, PlatinumMeritExtendedMemorizeSlots, sizeof(struct _RTPlatinumMeritExtendedMemorizeSlot) * Response->Character.PlatinumMeritMasteryInfo.ExtendedMemorizeCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, PlatinumMeritUnlockedSlots, sizeof(struct _RTPlatinumMeritUnlockedSlot) * Response->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, PlatinumMeritMasterySlots, sizeof(struct _RTPlatinumMeritMasterySlot) * Response->Character.PlatinumMeritMasteryInfo.MasterySlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, PlatinumMeritSpecialMasterySlots, sizeof(struct _RTPlatinumMeritSpecialMasterySlot) * Response->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, DiamondMeritExtendedMemorizeSlots, sizeof(struct _RTDiamondMeritExtendedMemorizeSlot) * Response->Character.PlatinumMeritMasteryInfo.ExtendedMemorizeCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, DiamondMeritUnlockedSlots, sizeof(struct _RTDiamondMeritUnlockedSlot) * Response->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, DiamondMeritMasterySlots, sizeof(struct _RTDiamondMeritMasterySlot) * Response->Character.PlatinumMeritMasteryInfo.MasterySlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, DiamondMeritSpecialMasterySlots, sizeof(struct _RTDiamondMeritSpecialMasterySlot) * Response->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, AchievementExtendedRewardSlots, sizeof(struct _RTAchievementExtendedRewardSlot) * Response->Character.AchievementExtendedInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, WarpServiceSlots, sizeof(struct _RTWarpServiceSlot) * Response->Character.WarpServiceInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, OverlordMasterySlots, sizeof(struct _RTOverlordMasterySlot) * Response->Character.OverlordMasteryInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, HonorMedalSlots, sizeof(struct _RTHonorMedalSlot) * Response->Character.HonorMedalInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, ForceWingPresetSlots, sizeof(struct _RTForceWingPresetSlot) * Response->Character.ForceWingInfo.PresetSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, ForceWingTrainingSlots, sizeof(struct _RTForceWingTrainingSlot) * Response->Character.ForceWingInfo.TrainingSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, GiftBoxSlots, sizeof(struct _RTGiftBoxSlot) * Response->Character.GiftBoxInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, GiftBoxRewardSlots, sizeof(struct _RTGiftBoxRewardSlot) * Response->Character.GiftBoxInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, CollectionSlots, sizeof(struct _RTCollectionSlot) * Response->Character.CollectionInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, TransformSlots, sizeof(struct _RTTransformSlot) * Response->Character.TransformInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, AuraMasterySlots, sizeof(struct _RTAuraMasterySlot) * Response->Character.AuraMasteryInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, TranscendenceSlots, sizeof(struct _RTTranscendenceSlot) * Response->Character.TranscendenceInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, DamageBoosterSlots, sizeof(struct _RTDamageBoosterSlot) * Response->Character.DamageBoosterInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, StellarMasterySlots, sizeof(struct _RTStellarMasterySlot) * Response->Character.StellarMasteryInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, MythMasterySlots, sizeof(struct _RTMythMasterySlot) * Response->Character.MythMasteryInfo.PropertySlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, NewbieSupportSlots, sizeof(struct _RTNewbieSupportSlot) * Response->Character.NewbieSupportInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EventPassMissionPages, sizeof(struct _RTEventPassMissionPage) * Response->Character.EventPassInfo.MissionPageCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EventPassMissionSlots, sizeof(struct _RTEventPassMissionSlot) * Response->Character.EventPassInfo.MissionSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, EventPassRewardSlots, sizeof(struct _RTEventPassRewardSlot) * Response->Character.EventPassInfo.RewardSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, AccountCostumeSlots, sizeof(struct _RTAccountCostumeSlot) * Response->Character.CostumeWarehouseInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, CostumePages, sizeof(struct _RTCostumePage) * Response->Character.CostumeInfo.PageCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, AppliedCostumeSlots, sizeof(struct _RTAppliedCostumeSlot) * Response->Character.CostumeInfo.AppliedSlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, ExplorationSlots, sizeof(struct _RTExplorationSlot) * Response->Character.ExplorationInfo.SlotCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, AnimaMasteryPresetData, sizeof(struct _RTAnimaMasteryPresetData) * Response->Character.AnimaMasteryInfo.PresetCount);
    IPCPacketBufferAppendCopy(Connection->PacketBuffer, AnimaMasteryCategoryData, sizeof(struct _RTAnimaMasteryCategoryData) * Response->Character.AnimaMasteryInfo.StorageCount);

    Response->Success = true;
    IPCSocketUnicast(Socket, Response);
    return;

error:
	Response->Success = false;
    IPCSocketUnicast(Socket, Response);
}
