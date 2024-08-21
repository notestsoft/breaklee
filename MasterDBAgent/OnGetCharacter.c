#include "IPCProtocol.h"
#include "IPCProcedures.h"

IPC_PROCEDURE_BINDING(W2D, GET_CHARACTER) {
    IPC_D2W_DATA_GET_CHARACTER* Response = IPCPacketBufferInit(Connection->PacketBuffer, D2W, GET_CHARACTER);
    Response->Header.Source = Server->IPCSocket->NodeID;
    Response->Header.Target = Packet->Header.Source;
    Response->Header.TargetConnectionID = Packet->Header.SourceConnectionID;

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
        SQL_PARAM_INPUT, SQL_INTEGER, &Packet->CharacterID,
        SQL_END
    );

    if (DatabaseHandleReadNext(
        Handle,
        SQL_TINYINT, &Response->Character.IsWarehousePasswordSet,
        SQL_TINYINT, &Response->Character.IsInventoryPasswordSet,
        SQL_TINYINT, &Response->Character.IsWarehouseLocked,
        SQL_TINYINT, &Response->Character.IsInventoryLocked,
        SQL_INTEGER, &Response->Character.CharacterInfo.WorldIndex,
        SQL_INTEGER, &Response->Character.CharacterInfo.DungeonIndex,
        SQL_SMALLINT, &Response->Character.CharacterInfo.PositionX,
        SQL_SMALLINT, &Response->Character.CharacterInfo.PositionY,
        SQL_BIGINT, &Response->Character.CharacterInfo.Exp,
        SQL_BIGINT, &Response->Character.CharacterInfo.Alz,
        SQL_BIGINT, &Response->Character.CharacterInfo.Wexp,
        SQL_INTEGER, &Response->Character.CharacterInfo.Level,
        SQL_INTEGER, &Response->Character.CharacterInfo.Stat[RUNTIME_CHARACTER_STAT_STR],
        SQL_INTEGER, &Response->Character.CharacterInfo.Stat[RUNTIME_CHARACTER_STAT_DEX],
        SQL_INTEGER, &Response->Character.CharacterInfo.Stat[RUNTIME_CHARACTER_STAT_INT],
        SQL_INTEGER, &Response->Character.CharacterInfo.Stat[RUNTIME_CHARACTER_STAT_PNT],
        SQL_INTEGER, &Response->Character.CharacterInfo.SkillRank,
        SQL_BIGINT, &Response->Character.CharacterInfo.CurrentHP,
        SQL_INTEGER, &Response->Character.CharacterInfo.CurrentMP,
        SQL_SMALLINT, &Response->Character.CharacterInfo.CurrentSP,
        SQL_INTEGER, &Response->Character.CharacterInfo.CurrentRage,
        SQL_INTEGER, &Response->Character.CharacterInfo.CurrentBP,
        SQL_BIGINT, &Response->Character.CharacterInfo.DPDuration,
        SQL_INTEGER, &Response->Character.CharacterInfo.DP,
        SQL_INTEGER, &Response->Character.CharacterInfo.SkillLevel,
        SQL_INTEGER, &Response->Character.CharacterInfo.SkillExp,
        SQL_INTEGER, &Response->Character.CharacterInfo.SkillPoint,
        SQL_BIGINT, &Response->Character.CharacterInfo.RestExp,
        SQL_BIGINT, &Response->Character.CharacterInfo.HonorPoint,
        SQL_BIGINT, &Response->Character.CharacterInfo.DeathPenaltyExp,
        SQL_BIGINT, &Response->Character.CharacterInfo.DeathPenaltyHp,
        SQL_INTEGER, &Response->Character.CharacterInfo.DeathPenaltyMp,
        SQL_SMALLINT, &Response->Character.CharacterInfo.PKState,
        SQL_TINYINT, &Response->Character.CharacterStyleInfo.Nation,
        SQL_INTEGER, &Response->Character.CharacterStyleInfo.WarpMask,
        SQL_INTEGER, &Response->Character.CharacterStyleInfo.MapsMask,
        SQL_INTEGER, &Response->Character.CharacterStyleInfo.Style.RawValue,
        SQL_INTEGER, &Response->Character.CharacterStyleInfo.LiveStyle.RawValue,
        SQL_TINYINT, &Response->Character.EquipmentInfo.EquipmentSlotCount,
        SQL_TINYINT, &Response->Character.EquipmentInfo.InventorySlotCount,
        SQL_TINYINT, &Response->Character.EquipmentInfo.LinkSlotCount,
        SQL_TINYINT, &Response->Character.EquipmentInfo.LockSlotCount,
        SQL_SMALLINT, &Response->Character.InventoryInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.VehicleInventoryInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.SkillSlotInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.QuickSlotInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.MercenaryInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.ItemPeriodCount,
        SQL_SMALLINT, &Response->Character.AbilityInfo.APTotal,
        SQL_SMALLINT, &Response->Character.AbilityInfo.AP,
        SQL_INTEGER, &Response->Character.AbilityInfo.Axp,
        SQL_TINYINT, &Response->Character.AbilityInfo.EssenceAbilityCount,
        SQL_TINYINT, &Response->Character.AbilityInfo.ExtendedEssenceAbilityCount,
        SQL_TINYINT, &Response->Character.AbilityInfo.BlendedAbilityCount,
        SQL_TINYINT, &Response->Character.AbilityInfo.ExtendedBlendedAbilityCount,
        SQL_TINYINT, &Response->Character.AbilityInfo.KarmaAbilityCount,
        SQL_TINYINT, &Response->Character.AbilityInfo.ExtendedKarmaAbilityCount,
        SQL_TINYINT, &Response->Character.BlessingBeadInfo.SlotCount,
        SQL_TINYINT, &Response->Character.PremiumServiceInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.QuestInfo.SlotCount,
        SQL_VARBINARY, Response->Character.QuestInfo.FinishedQuests, sizeof(Response->Character.QuestInfo.FinishedQuests),
        SQL_VARBINARY, Response->Character.QuestInfo.DeletedQuests, sizeof(Response->Character.QuestInfo.DeletedQuests),
        SQL_VARBINARY, Response->Character.QuestInfo.FinishedDungeons, sizeof(Response->Character.QuestInfo.FinishedDungeons),
        SQL_INTEGER, &Response->Character.DailyQuestInfo.SlotCount,
        SQL_INTEGER, &Response->Character.HelpWindow,
        SQL_TINYINT, &Response->Character.AppearanceInfo.EquipmentAppearanceCount,
        SQL_SMALLINT, &Response->Character.AppearanceInfo.InventoryAppearanceCount,
        SQL_INTEGER, &Response->Character.AchievementInfo.AllAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.NormalAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.QuestAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.DungeonAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.ItemsAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.PvpAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.WarAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.HuntingAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.CraftAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.CommunityAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.SharedAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.SpecialAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.GeneralMemoirAchievementScore,
        SQL_INTEGER, &Response->Character.AchievementInfo.SharedMemoirAchievementScore,
        SQL_SMALLINT, &Response->Character.AchievementInfo.DisplayTitle,
        SQL_SMALLINT, &Response->Character.AchievementInfo.EventTitle,
        SQL_SMALLINT, &Response->Character.AchievementInfo.GuildTitle,
        SQL_SMALLINT, &Response->Character.AchievementInfo.WarTitle,
        SQL_INTEGER, &Response->Character.AchievementInfo.AchievementCount,
        SQL_INTEGER, &Response->Character.AchievementInfo.AchievementRewardCount,
        SQL_INTEGER, &Response->Character.CraftInfo.SlotCount,
        SQL_INTEGER, &Response->Character.CraftInfo.Energy,
        SQL_TINYINT, &Response->Character.RequestCraftInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.RequestCraftInfo.Exp,
        SQL_VARBINARY, Response->Character.RequestCraftInfo.RegisteredFlags, sizeof(Response->Character.RequestCraftInfo.RegisteredFlags),
        SQL_VARBINARY, Response->Character.RequestCraftInfo.FavoriteFlags, sizeof(Response->Character.RequestCraftInfo.FavoriteFlags),
        SQL_SMALLINT, &Response->Character.RequestCraftInfo.SortingOrder,
        SQL_TINYINT, &Response->Character.BuffInfo.SkillCooldownCount,
        SQL_SMALLINT, &Response->Character.BuffInfo.BuffCount,
        SQL_INTEGER, &Response->Character.BuffInfo.SpiritRaiseBuffCooldown,
        SQL_INTEGER, &Response->Character.UpgradeInfo.UpgradePoints,
        SQL_BIGINT, &Response->Character.UpgradeInfo.UpgradePointTimestamp,
        SQL_INTEGER, &Response->Character.GoldMeritMasteryInfo.SlotCount,
        SQL_INTEGER, &Response->Character.GoldMeritMasteryInfo.Exp,
        SQL_INTEGER, &Response->Character.GoldMeritMasteryInfo.Points,
        SQL_TINYINT, &Response->Character.PlatinumMeritMasteryInfo.IsEnabled,
        SQL_INTEGER, &Response->Character.PlatinumMeritMasteryInfo.Exp,
        SQL_INTEGER, &Response->Character.PlatinumMeritMasteryInfo.Points,
        SQL_TINYINT, &Response->Character.PlatinumMeritMasteryInfo.ActiveMemorizeIndex,
        SQL_INTEGER, &Response->Character.PlatinumMeritMasteryInfo.OpenSlotMasteryIndex,
        SQL_BIGINT, &Response->Character.PlatinumMeritMasteryInfo.OpenSlotUnlockTime,
        SQL_SMALLINT, &Response->Character.PlatinumMeritMasteryInfo.ExtendedMemorizeCount,
        SQL_SMALLINT, &Response->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount,
        SQL_SMALLINT, &Response->Character.PlatinumMeritMasteryInfo.MasterySlotCount,
        SQL_SMALLINT, &Response->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount,
        SQL_TINYINT, &Response->Character.DiamondMeritMasteryInfo.IsEnabled,
        SQL_INTEGER, &Response->Character.DiamondMeritMasteryInfo.Exp,
        SQL_INTEGER, &Response->Character.DiamondMeritMasteryInfo.Points,
        SQL_TINYINT, &Response->Character.DiamondMeritMasteryInfo.ActiveMemorizeIndex,
        SQL_INTEGER, &Response->Character.DiamondMeritMasteryInfo.OpenSlotMasteryIndex,
        SQL_BIGINT, &Response->Character.DiamondMeritMasteryInfo.OpenSlotUnlockTime,
        SQL_SMALLINT, &Response->Character.DiamondMeritMasteryInfo.ExtendedMemorizeCount,
        SQL_SMALLINT, &Response->Character.DiamondMeritMasteryInfo.UnlockedSlotCount,
        SQL_SMALLINT, &Response->Character.DiamondMeritMasteryInfo.MasterySlotCount,
        SQL_SMALLINT, &Response->Character.DiamondMeritMasteryInfo.SpecialMasterySlotCount,
        SQL_INTEGER, &Response->Character.DiamondMeritMasteryInfo.ExtendedMasterySlotCount,
        SQL_INTEGER, &Response->Character.AchievementExtendedInfo.SlotCount,
        SQL_INTEGER, &Response->Character.ForceGem,
        SQL_INTEGER, &Response->Character.WarpServiceInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.OverlordMasteryInfo.Level,
        SQL_BIGINT, &Response->Character.OverlordMasteryInfo.Exp,
        SQL_SMALLINT, &Response->Character.OverlordMasteryInfo.Point,
        SQL_TINYINT, &Response->Character.OverlordMasteryInfo.SlotCount,
        SQL_INTEGER, &Response->Character.HonorMedalInfo.Grade,
        SQL_INTEGER, &Response->Character.HonorMedalInfo.Score,
        SQL_TINYINT, &Response->Character.HonorMedalInfo.SlotCount,
        SQL_TINYINT, &Response->Character.ForceWingInfo.Grade,
        SQL_TINYINT, &Response->Character.ForceWingInfo.Level,
        SQL_BIGINT, &Response->Character.ForceWingInfo.Exp,
        SQL_TINYINT, &Response->Character.ForceWingInfo.ActivePresetIndex,
        SQL_VARBINARY, Response->Character.ForceWingInfo.PresetEnabled, sizeof(Response->Character.ForceWingInfo.PresetEnabled),
        SQL_VARBINARY, Response->Character.ForceWingInfo.PresetTrainingPointCount, sizeof(Response->Character.ForceWingInfo.PresetTrainingPointCount),
        SQL_TINYINT, &Response->Character.ForceWingInfo.PresetSlotCount,
        SQL_TINYINT, &Response->Character.ForceWingInfo.TrainingSlotCount,
        SQL_VARBINARY, Response->Character.ForceWingInfo.TrainingUnlockFlags, sizeof(Response->Character.ForceWingInfo.TrainingUnlockFlags),
        SQL_VARBINARY, Response->Character.ForceWingInfo.ArrivalSkillSlots, sizeof(Response->Character.ForceWingInfo.ArrivalSkillSlots),
        SQL_VARBINARY, Response->Character.ForceWingInfo.ArrivalSkillRestoreSlot, sizeof(Response->Character.ForceWingInfo.ArrivalSkillRestoreSlot),
        SQL_SMALLINT, &Response->Character.GiftBoxInfo.SpecialPoints,
        SQL_TINYINT, &Response->Character.GiftBoxInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.CollectionInfo.SlotCount,
        SQL_SMALLINT, &Response->Character.TransformInfo.SlotCount,
        SQL_TINYINT, &Response->Character.SecretShopData.RefreshCost,
        SQL_BIGINT, &Response->Character.SecretShopData.ResetTimestamp,
        SQL_TINYINT, &Response->Character.SecretShopData.RefreshCount,
        SQL_VARBINARY, Response->Character.SecretShopData.Slots, sizeof(Response->Character.SecretShopData.Slots),
        SQL_INTEGER, &Response->Character.AuraMasteryInfo.Points,
        SQL_INTEGER, &Response->Character.AuraMasteryInfo.AccumulatedTimeInMinutes,
        SQL_TINYINT, &Response->Character.AuraMasteryInfo.SlotCount,
        SQL_INTEGER, &Response->Character.TranscendenceInfo.Points,
        SQL_INTEGER, &Response->Character.TranscendenceInfo.SlotCount,
        SQL_VARBINARY, Response->Character.DamageBoosterInfo.ItemID, sizeof(Response->Character.DamageBoosterInfo.ItemID),
        SQL_TINYINT, &Response->Character.DamageBoosterInfo.SlotCount,
        SQL_INTEGER, &Response->Character.ResearchSupportInfo.Exp,
        SQL_INTEGER, &Response->Character.ResearchSupportInfo.DecodedCircuitCount,
        SQL_TINYINT, &Response->Character.ResearchSupportInfo.ResetCount,
        SQL_BIGINT, &Response->Character.ResearchSupportInfo.SeasonStartDate,
        SQL_BIGINT, &Response->Character.ResearchSupportInfo.SeasonEndDate,
        SQL_VARBINARY, Response->Character.ResearchSupportInfo.MaterialSlots, sizeof(Response->Character.ResearchSupportInfo.MaterialSlots),
        SQL_VARBINARY, &Response->Character.ResearchSupportInfo.ActiveMissionBoard, sizeof(Response->Character.ResearchSupportInfo.ActiveMissionBoard),
        SQL_VARBINARY, Response->Character.ResearchSupportInfo.MissionBoards, sizeof(Response->Character.ResearchSupportInfo.MissionBoards),
        SQL_TINYINT, &Response->Character.StellarMasteryInfo.SlotCount,
        SQL_INTEGER, &Response->Character.MythMasteryInfo.Rebirth,
        SQL_INTEGER, &Response->Character.MythMasteryInfo.HolyPower,
        SQL_INTEGER, &Response->Character.MythMasteryInfo.Level,
        SQL_BIGINT, &Response->Character.MythMasteryInfo.Exp,
        SQL_INTEGER, &Response->Character.MythMasteryInfo.Points,
        SQL_INTEGER, &Response->Character.MythMasteryInfo.UnlockedPageCount,
        SQL_TINYINT, &Response->Character.MythMasteryInfo.PropertySlotCount,
        SQL_INTEGER, &Response->Character.MythMasteryInfo.StigmaGrade,
        SQL_INTEGER, &Response->Character.MythMasteryInfo.StigmaExp,
        SQL_BIGINT, &Response->Character.NewbieSupportInfo.Timestamp,
        SQL_INTEGER, &Response->Character.NewbieSupportInfo.SlotCount,
        SQL_BIGINT, &Response->Character.EventPassInfo.StartDate,
        SQL_BIGINT, &Response->Character.EventPassInfo.EndDate,
        SQL_INTEGER, &Response->Character.EventPassInfo.MissionSlotCount,
        SQL_INTEGER, &Response->Character.EventPassInfo.RewardSlotCount,
        SQL_INTEGER, &Response->Character.CostumeWarehouseInfo.SlotCount,
        SQL_INTEGER, &Response->Character.CostumeInfo.PageCount,
        SQL_INTEGER, &Response->Character.CostumeInfo.AppliedSlotCount,
        SQL_INTEGER, &Response->Character.CostumeInfo.ActivePageIndex,
        SQL_BIGINT, &Response->Character.ExplorationInfo.EndDate,
        SQL_INTEGER, &Response->Character.ExplorationInfo.Points,
        SQL_INTEGER, &Response->Character.ExplorationInfo.Level,
        SQL_INTEGER, &Response->Character.ExplorationInfo.SlotCount,
        SQL_INTEGER, &Response->Character.AnimaMasteryInfo.PresetCount,
        SQL_INTEGER, &Response->Character.AnimaMasteryInfo.StorageCount,
        SQL_INTEGER, &Response->Character.AnimaMasteryInfo.UnlockedCategoryFlags,
        SQL_VARBINARY, Response->Character.PresetInfo.Configurations, sizeof(Response->Character.PresetInfo.Configurations),
        SQL_INTEGER, &Response->Character.PresetInfo.ActiveEquipmentPresetIndex,
        SQL_INTEGER, &Response->Character.PresetInfo.ActiveAnimaMasteryPresetIndex,
        SQL_TINYINT, &Response->Character.NameLength,
        SQL_VARCHAR, Response->Character.Name, sizeof(Response->Character.Name),
        SQL_VARBINARY, EquipmentSlots, sizeof(EquipmentSlots),
        SQL_VARBINARY, EquipmentInventorySlots, sizeof(EquipmentInventorySlots),
        SQL_VARBINARY, EquipmentLinkSlots, sizeof(EquipmentLinkSlots),
        SQL_VARBINARY, EquipmentLockSlots, sizeof(EquipmentLockSlots),
        SQL_VARBINARY, InventorySlots, sizeof(InventorySlots),
        SQL_VARBINARY, SkillSlots, sizeof(SkillSlots),
        SQL_VARBINARY, QuickSlots, sizeof(QuickSlots),
        SQL_VARBINARY, EssenceAbilitySlots, sizeof(EssenceAbilitySlots),
        SQL_VARBINARY, BlendedAbilitySlots, sizeof(BlendedAbilitySlots),
        SQL_VARBINARY, KarmaAbilitySlots, sizeof(KarmaAbilitySlots),
        SQL_VARBINARY, BlessingBeadSlots, sizeof(BlessingBeadSlots),
        SQL_VARBINARY, PremiumServiceSlots, sizeof(PremiumServiceSlots),
        SQL_VARBINARY, QuestSlots, sizeof(QuestSlots),
        SQL_VARBINARY, DailyQuestSlots, sizeof(DailyQuestSlots),
        SQL_VARBINARY, MercenarySlots, sizeof(MercenarySlots),
        SQL_VARBINARY, EquipmentAppearanceSlots, sizeof(EquipmentAppearanceSlots),
        SQL_VARBINARY, InventoryAppearanceSlots, sizeof(InventoryAppearanceSlots),
        SQL_VARBINARY, AchievementSlots, sizeof(AchievementSlots),
        SQL_VARBINARY, AchievementRewardSlots, sizeof(AchievementRewardSlots),
        SQL_VARBINARY, AchievementExtendedRewardSlots, sizeof(AchievementExtendedRewardSlots),
        SQL_VARBINARY, BuffSlots, sizeof(BuffSlots),
        SQL_VARBINARY, CraftSlots, sizeof(CraftSlots),
        SQL_VARBINARY, RequestCraftSlots, sizeof(RequestCraftSlots),
        SQL_VARBINARY, VehicleInventorySlots, sizeof(VehicleInventorySlots),
        SQL_VARBINARY, GoldMeritMasterySlots, sizeof(GoldMeritMasterySlots),
        SQL_VARBINARY, PlatinumMeritExtendedMemorizeSlots, sizeof(PlatinumMeritExtendedMemorizeSlots),
        SQL_VARBINARY, PlatinumMeritUnlockedSlots, sizeof(PlatinumMeritUnlockedSlots),
        SQL_VARBINARY, PlatinumMeritMasterySlots, sizeof(PlatinumMeritMasterySlots),
        SQL_VARBINARY, PlatinumMeritSpecialMasterySlots, sizeof(PlatinumMeritSpecialMasterySlots),
        SQL_VARBINARY, DiamondMeritExtendedMemorizeSlots, sizeof(DiamondMeritExtendedMemorizeSlots),
        SQL_VARBINARY, DiamondMeritUnlockedSlots, sizeof(DiamondMeritUnlockedSlots),
        SQL_VARBINARY, DiamondMeritMasterySlots, sizeof(DiamondMeritMasterySlots),
        SQL_VARBINARY, DiamondMeritSpecialMasterySlots, sizeof(DiamondMeritSpecialMasterySlots),
        SQL_VARBINARY, WarpServiceSlots, sizeof(WarpServiceSlots),
        SQL_VARBINARY, OverlordMasterySlots, sizeof(OverlordMasterySlots),
        SQL_VARBINARY, HonorMedalSlots, sizeof(HonorMedalSlots),
        SQL_VARBINARY, ForceWingPresetSlots, sizeof(ForceWingPresetSlots),
        SQL_VARBINARY, ForceWingTrainingSlots, sizeof(ForceWingTrainingSlots),
        SQL_VARBINARY, GiftBoxSlots, sizeof(GiftBoxSlots),
        SQL_VARBINARY, CollectionSlots, sizeof(CollectionSlots),
        SQL_VARBINARY, TransformSlots, sizeof(TransformSlots),
        SQL_VARBINARY, AuraMasterySlots, sizeof(AuraMasterySlots),
        SQL_VARBINARY, TranscendenceSlots, sizeof(TranscendenceSlots),
        SQL_VARBINARY, DamageBoosterSlots, sizeof(DamageBoosterSlots),
        SQL_VARBINARY, StellarMasterySlots, sizeof(StellarMasterySlots),
        SQL_VARBINARY, MythMasterySlots, sizeof(MythMasterySlots),
        SQL_VARBINARY, NewbieSupportSlots, sizeof(NewbieSupportSlots),
        SQL_VARBINARY, EventPassMissionSlots, sizeof(EventPassMissionSlots),
        SQL_VARBINARY, EventPassRewardSlots, sizeof(EventPassRewardSlots),
        SQL_VARBINARY, AccountCostumeSlots, sizeof(AccountCostumeSlots),
        SQL_VARBINARY, CostumePages, sizeof(CostumePages),
        SQL_VARBINARY, AppliedCostumeSlots, sizeof(AppliedCostumeSlots),
        SQL_VARBINARY, ExplorationSlots, sizeof(ExplorationSlots),
        SQL_VARBINARY, AnimaMasteryPresetData, sizeof(AnimaMasteryPresetData),
        SQL_VARBINARY, AnimaMasteryCategoryData, sizeof(AnimaMasteryCategoryData),
        SQL_END
    )) {
        DatabaseHandleFlush(Handle);
    }

    if (Response->Character.EquipmentInfo.EquipmentSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            EquipmentSlots,
            sizeof(struct _RTItemSlot) * Response->Character.EquipmentInfo.EquipmentSlotCount
        );
    }

    if (Response->Character.EquipmentInfo.InventorySlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            EquipmentInventorySlots,
            sizeof(struct _RTItemSlot) * Response->Character.EquipmentInfo.InventorySlotCount
        );
    }

    if (Response->Character.EquipmentInfo.LinkSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            EquipmentLinkSlots,
            sizeof(struct _RTEquipmentLinkSlot) * Response->Character.EquipmentInfo.LinkSlotCount
        );
    }

    if (Response->Character.EquipmentInfo.LockSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            EquipmentLockSlots,
            sizeof(struct _RTEquipmentLockSlot) * Response->Character.EquipmentInfo.LockSlotCount
        );
    }

    if (Response->Character.InventoryInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            InventorySlots,
            sizeof(struct _RTItemSlot) * Response->Character.InventoryInfo.SlotCount
        );
    }

    if (Response->Character.VehicleInventoryInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            VehicleInventorySlots,
            sizeof(struct _RTItemSlot) * Response->Character.VehicleInventoryInfo.SlotCount
        );
    }

    if (Response->Character.SkillSlotInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            SkillSlots,
            sizeof(struct _RTSkillSlot) * Response->Character.SkillSlotInfo.SlotCount
        );
    }

    if (Response->Character.QuickSlotInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            QuickSlots,
            sizeof(struct _RTQuickSlot) * Response->Character.QuickSlotInfo.SlotCount
        );
    }

    if (Response->Character.MercenaryInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            MercenarySlots,
            sizeof(struct _RTMercenarySlot) * Response->Character.MercenaryInfo.SlotCount
        );
    }

    if (Response->Character.AbilityInfo.EssenceAbilityCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            EssenceAbilitySlots,
            sizeof(struct _RTEssenceAbilitySlot) * Response->Character.AbilityInfo.EssenceAbilityCount
        );
    }

    if (Response->Character.AbilityInfo.BlendedAbilityCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            BlendedAbilitySlots,
            sizeof(struct _RTBlendedAbilitySlot) * Response->Character.AbilityInfo.BlendedAbilityCount
        );
    }

    if (Response->Character.AbilityInfo.KarmaAbilityCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            KarmaAbilitySlots,
            sizeof(struct _RTKarmaAbilitySlot) * Response->Character.AbilityInfo.KarmaAbilityCount
        );
    }

    if (Response->Character.BlessingBeadInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            BlessingBeadSlots,
            sizeof(struct _RTBlessingBeadSlot) * Response->Character.BlessingBeadInfo.SlotCount
        );
    }

    if (Response->Character.PremiumServiceInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            PremiumServiceSlots,
            sizeof(struct _RTPremiumServiceSlot) * Response->Character.PremiumServiceInfo.SlotCount
        );
    }

    if (Response->Character.QuestInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            QuestSlots,
            sizeof(struct _RTQuestSlot) * Response->Character.QuestInfo.SlotCount
        );
    }

    if (Response->Character.DailyQuestInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            DailyQuestSlots,
            sizeof(struct _RTDailyQuestSlot) * Response->Character.DailyQuestInfo.SlotCount
        );
    }

    if (Response->Character.AppearanceInfo.EquipmentAppearanceCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            EquipmentAppearanceSlots,
            sizeof(struct _RTItemSlotAppearance) * Response->Character.AppearanceInfo.EquipmentAppearanceCount
        );
    }

    if (Response->Character.AppearanceInfo.InventoryAppearanceCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            InventoryAppearanceSlots,
            sizeof(struct _RTItemSlotAppearance) * Response->Character.AppearanceInfo.InventoryAppearanceCount
        );
    }

    if (Response->Character.AchievementInfo.AchievementCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            AchievementSlots,
            sizeof(struct _RTAchievementSlot) * Response->Character.AchievementInfo.AchievementCount
        );
    }

    if (Response->Character.AchievementInfo.AchievementRewardCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            AchievementRewardSlots,
            sizeof(struct _RTAchievementRewardSlot) * Response->Character.AchievementInfo.AchievementRewardCount
        );
    }

    if (Response->Character.CraftInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            CraftSlots,
            sizeof(struct _RTCraftSlot) * Response->Character.CraftInfo.SlotCount
        );
    }

    if (Response->Character.RequestCraftInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            RequestCraftSlots,
            sizeof(struct _RTRequestCraftSlot) * Response->Character.RequestCraftInfo.SlotCount
        );
    }

    if (Response->Character.BuffInfo.BuffCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            BuffSlots,
            sizeof(struct _RTBuffSlot) * Response->Character.BuffInfo.BuffCount
        );
    }

    if (Response->Character.GoldMeritMasteryInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            GoldMeritMasterySlots,
            sizeof(struct _RTGoldMeritMasterySlot) * Response->Character.GoldMeritMasteryInfo.SlotCount
        );
    }

    if (Response->Character.PlatinumMeritMasteryInfo.ExtendedMemorizeCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            PlatinumMeritExtendedMemorizeSlots,
            sizeof(struct _RTPlatinumMeritExtendedMemorizeSlot) * Response->Character.PlatinumMeritMasteryInfo.ExtendedMemorizeCount
        );
    }

    if (Response->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            PlatinumMeritUnlockedSlots,
            sizeof(struct _RTPlatinumMeritUnlockedSlot) * Response->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount
        );
    }

    if (Response->Character.PlatinumMeritMasteryInfo.MasterySlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            PlatinumMeritMasterySlots,
            sizeof(struct _RTPlatinumMeritMasterySlot) * Response->Character.PlatinumMeritMasteryInfo.MasterySlotCount
        );
    }

    if (Response->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            PlatinumMeritSpecialMasterySlots,
            sizeof(struct _RTPlatinumMeritSpecialMasterySlot) * Response->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount
        );
    }

    if (Response->Character.DiamondMeritMasteryInfo.ExtendedMemorizeCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            DiamondMeritExtendedMemorizeSlots,
            sizeof(struct _RTDiamondMeritExtendedMemorizeSlot) * Response->Character.PlatinumMeritMasteryInfo.ExtendedMemorizeCount
        );
    }

    if (Response->Character.DiamondMeritMasteryInfo.UnlockedSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            DiamondMeritUnlockedSlots,
            sizeof(struct _RTDiamondMeritUnlockedSlot) * Response->Character.PlatinumMeritMasteryInfo.UnlockedSlotCount
        );
    }

    if (Response->Character.DiamondMeritMasteryInfo.MasterySlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            DiamondMeritMasterySlots,
            sizeof(struct _RTDiamondMeritMasterySlot) * Response->Character.PlatinumMeritMasteryInfo.MasterySlotCount
        );
    }

    if (Response->Character.DiamondMeritMasteryInfo.SpecialMasterySlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            DiamondMeritSpecialMasterySlots,
            sizeof(struct _RTDiamondMeritSpecialMasterySlot) * Response->Character.PlatinumMeritMasteryInfo.SpecialMasterySlotCount
        );
    }

    if (Response->Character.AchievementExtendedInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            AchievementExtendedRewardSlots,
            sizeof(struct _RTAchievementExtendedRewardSlot) * Response->Character.AchievementExtendedInfo.SlotCount
        );
    }

    if (Response->Character.WarpServiceInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            WarpServiceSlots,
            sizeof(struct _RTWarpServiceSlot) * Response->Character.WarpServiceInfo.SlotCount
        );
    }

    if (Response->Character.OverlordMasteryInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            OverlordMasterySlots,
            sizeof(struct _RTOverlordMasterySlot) * Response->Character.OverlordMasteryInfo.SlotCount
        );
    }

    if (Response->Character.HonorMedalInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            HonorMedalSlots,
            sizeof(struct _RTHonorMedalSlot) * Response->Character.HonorMedalInfo.SlotCount
        );
    }

    if (Response->Character.ForceWingInfo.PresetSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            ForceWingPresetSlots,
            sizeof(struct _RTForceWingPresetSlot) * Response->Character.ForceWingInfo.PresetSlotCount
        );
    }

    if (Response->Character.ForceWingInfo.TrainingSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            ForceWingTrainingSlots,
            sizeof(struct _RTForceWingTrainingSlot) * Response->Character.ForceWingInfo.TrainingSlotCount
        );
    }

    if (Response->Character.GiftBoxInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            GiftBoxSlots,
            sizeof(struct _RTGiftBoxSlot) * Response->Character.GiftBoxInfo.SlotCount
        );

        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            GiftBoxRewardSlots,
            sizeof(struct _RTGiftBoxRewardSlot) * Response->Character.GiftBoxInfo.SlotCount
        );
    }

    if (Response->Character.CollectionInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            CollectionSlots,
            sizeof(struct _RTCollectionSlot) * Response->Character.CollectionInfo.SlotCount
        );
    }

    if (Response->Character.TransformInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            TransformSlots,
            sizeof(struct _RTTransformSlot) * Response->Character.TransformInfo.SlotCount
        );
    }

    if (Response->Character.AuraMasteryInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            AuraMasterySlots,
            sizeof(struct _RTAuraMasterySlot) * Response->Character.AuraMasteryInfo.SlotCount
        );
    }

    if (Response->Character.TranscendenceInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            TranscendenceSlots,
            sizeof(struct _RTTranscendenceSlot) * Response->Character.TranscendenceInfo.SlotCount
        );
    }

    if (Response->Character.DamageBoosterInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            DamageBoosterSlots,
            sizeof(struct _RTDamageBoosterSlot) * Response->Character.DamageBoosterInfo.SlotCount
        );
    }

    if (Response->Character.StellarMasteryInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            StellarMasterySlots,
            sizeof(struct _RTStellarMasterySlot) * Response->Character.StellarMasteryInfo.SlotCount
        );
    }

    if (Response->Character.MythMasteryInfo.PropertySlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            MythMasterySlots,
            sizeof(struct _RTMythMasterySlot) * Response->Character.MythMasteryInfo.PropertySlotCount
        );
    }

    if (Response->Character.NewbieSupportInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            NewbieSupportSlots,
            sizeof(struct _RTNewbieSupportSlot) * Response->Character.NewbieSupportInfo.SlotCount
        );
    }

    if (Response->Character.EventPassInfo.MissionSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            EventPassMissionSlots,
            sizeof(struct _RTEventPassMissionSlot) * Response->Character.EventPassInfo.MissionSlotCount
        );
    }

    if (Response->Character.EventPassInfo.RewardSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            EventPassRewardSlots,
            sizeof(struct _RTEventPassRewardSlot) * Response->Character.EventPassInfo.RewardSlotCount
        );
    }

    if (Response->Character.CostumeWarehouseInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            AccountCostumeSlots,
            sizeof(struct _RTAccountCostumeSlot) * Response->Character.CostumeWarehouseInfo.SlotCount
        );
    }

    if (Response->Character.CostumeInfo.PageCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            CostumePages,
            sizeof(struct _RTCostumePage) * Response->Character.CostumeInfo.PageCount
        );
    }

    if (Response->Character.CostumeInfo.AppliedSlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            AppliedCostumeSlots,
            sizeof(struct _RTAppliedCostumeSlot) * Response->Character.CostumeInfo.AppliedSlotCount
        );
    }

    if (Response->Character.ExplorationInfo.SlotCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            ExplorationSlots,
            sizeof(struct _RTExplorationSlot) * Response->Character.ExplorationInfo.SlotCount
        );
    }

    if (Response->Character.AnimaMasteryInfo.PresetCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            AnimaMasteryPresetData,
            sizeof(struct _RTAnimaMasteryPresetData) * Response->Character.AnimaMasteryInfo.PresetCount
        );
    }

    if (Response->Character.AnimaMasteryInfo.StorageCount > 0) {
        IPCPacketBufferAppendCopy(
            Connection->PacketBuffer,
            AnimaMasteryCategoryData,
            sizeof(struct _RTAnimaMasteryCategoryData) * Response->Character.AnimaMasteryInfo.StorageCount
        );
    }

    IPCSocketUnicast(Socket, Response);
    return;

error:
	Response->Success = false;
    IPCSocketUnicast(Socket, Response);
}
