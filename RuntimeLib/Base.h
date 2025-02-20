#pragma once

#include <CoreLib/CoreLib.h>
#include <RuntimeDataLib/RuntimeDataLib.h>
#include <math.h>

EXTERN_C_BEGIN

#pragma pack(push, 1)

typedef struct _RTAnimaMasteryInfo* RTAnimaMasteryInfoRef;
typedef struct _RTAnimaMasteryPresetData* RTAnimaMasteryPresetDataRef;
typedef struct _RTAnimaMasteryCategoryData* RTAnimaMasteryCategoryDataRef;
typedef struct _RTCharacterAnimaMasteryInfo* RTCharacterAnimaMasteryInfoRef;
typedef struct _RTBattleAttributes* RTBattleAttributesRef;
typedef struct _RTBattleStyleLevelFormulaData* RTBattleStyleLevelFormulaDataRef;
typedef struct _RTBattleStyleClassFormulaData* RTBattleStyleClassFormulaDataRef;
typedef struct _RTBattleStyleSlopeData* RTBattleStyleSlopeDataRef;
typedef struct _RTBattleStyleSlopeFormulaData* RTBattleStyleSlopeFormulaDataRef;
typedef struct _RTBattleStyleStatsFormulaData* RTBattleStyleStatsFormulaDataRef;
typedef struct _RTBattleStyleSkillRankData* RTBattleStyleSkillRankDataRef;
typedef struct _RTBattleStyleSkillFormulaData* RTBattleStyleSkillFormulaDataRef;
typedef struct _RTBattleStyleRankData* RTBattleStyleRankDataRef;
typedef struct _RTBattleStyleRankFormulaData* RTBattleStyleRankFormulaDataRef;
typedef struct _RTBuffSlot* RTBuffSlotRef;
typedef struct _RTBuffInfo* RTBuffInfoRef;
typedef struct _RTBattleResult* RTBattleResultRef;
typedef struct _RTCharacterInfo* RTCharacterInfoRef;
typedef struct _RTCharacterData* RTCharacterDataRef;
typedef struct _RTCharacterWarEntry* RTCharacterWarEntryRef;
typedef struct _RTDailyQuestSlot* RTDailyQuestSlotRef;
typedef struct _RTEquipmentLockSlot* RTEquipmentLockSlotRef;
typedef struct _RTCharacterEquipmentData* RTCharacterEquipmentDataRef;
typedef struct _RTCharacterInventoryInfo* RTCharacterInventoryInfoRef;
typedef struct _RTCharacterSkillSlotInfo* RTCharacterSkillSlotInfoRef;
typedef struct _RTCharacterQuickSlotInfo* RTCharacterQuickSlotInfoRef;
typedef struct _RTCharacterQuestFlagInfo* RTCharacterQuestFlagInfoRef;
typedef struct _RTCharacterDungeonQuestFlagInfo* RTCharacterDungeonQuestFlagInfoRef;
typedef struct _RTAccountCostumeSlot* RTAccountCostumeSlotRef;
typedef struct _RTCostumePage* RTCostumePageRef;
typedef struct _RTAppliedCostumeSlot* RTAppliedCostumeSlotRef;
typedef struct _RTEssenceAbilitySlot* RTEssenceAbilitySlotRef;
typedef struct _RTBlendedAbilitySlot* RTBlendedAbilitySlotRef;
typedef struct _RTKarmaAbilitySlot* RTKarmaAbilitySlotRef;
typedef struct _RTCharacterEssenceAbilityInfo* RTCharacterEssenceAbilityInfoRef;
typedef struct _RTCharacterQuestSlotInfo* RTCharacterQuestSlotInfoRef;
typedef struct _RTOverlordMasterySlot* RTOverlordMasterySlotRef;
typedef struct _RTCharacterOverlordMasteryInfo* RTCharacterOverlordMasteryInfoRef;
typedef struct _RTCollectionSlot* RTCollectionSlotRef;
typedef struct _RTCharacterCollectionInfo* RTCharacterCollectionInfoRef;
typedef struct _RTCharacter* RTCharacterRef;
typedef struct _RTDropItem* RTDropItemRef;
typedef struct _RTDropTable* RTDropTableRef;
typedef struct _RTDropResult* RTDropResultRef;
typedef struct _RTForceEffectFormula* RTForceEffectFormulaRef;
typedef struct _RTForceWingArrivalSkillSlot* RTForceWingArrivalSkillSlotRef;
typedef struct _RTForceWingPresetSlot* RTForceWingPresetSlotRef;
typedef struct _RTForceWingTrainingSlot* RTForceWingTrainingSlotRef;
typedef struct _RTCharacterForceWingInfo* RTCharacterForceWingInfoRef;
typedef struct _RTGiftBoxSlot* RTGiftBoxSlotRef;
typedef struct _RTGiftBoxRewardSlot* RTGiftBoxRewardSlotRef;
typedef struct _RTGiftBoxInfo* RTGiftBoxInfoRef;
typedef struct _RTCharacterGiftboxInfo* RTCharacterGiftboxInfoRef;
typedef struct _RTHonorMedalSlot* RTHonorMedalSlotRef;
typedef struct _RTCharacterHonorMedalInfo* RTCharacterHonorMedalInfoRef;
typedef struct _RTInstantWarManager* RTInstantWarManagerRef;
typedef struct _RTItemData* RTItemDataRef;
typedef struct _RTItemSlot* RTItemSlotRef;
typedef struct _RTItemSlotAppearance* RTItemSlotAppearanceRef;
typedef struct _RTGoldMeritMasterySlot* RTGoldMeritMasterySlotRef;
typedef struct _RTPlatinumMeritExtendedMemorizeSlot* RTPlatinumMeritExtendedMemorizeSlotRef;
typedef struct _RTPlatinumMeritUnlockedSlot* RTPlatinumMeritUnlockedSlotRef;
typedef struct _RTPlatinumMeritMasterySlot* RTPlatinumMeritMasterySlotRef;
typedef struct _RTPlatinumMeritSpecialMasterySlot* RTPlatinumMeritSpecialMasterySlotRef;
typedef struct _RTDiamondMeritExtendedMemorizeSlot* RTDiamondMeritExtendedMemorizeSlotRef;
typedef struct _RTDiamondMeritUnlockedSlot* RTDiamondMeritUnlockedSlotRef;
typedef struct _RTDiamondMeritMasterySlot* RTDiamondMeritMasterySlotRef;
typedef struct _RTDiamondMeritSpecialMasterySlot* RTDiamondMeritSpecialMasterySlotRef;
typedef struct _RTMobSpawnData* RTMobSpawnDataRef;
typedef struct _RTMobSkillData* RTMobSkillDataRef;
typedef struct _RTMobSpeciesData* RTMobSpeciesDataRef;
typedef struct _RTMobBuffSlot* RTMobBuffSlotRef;
typedef struct _RTMob* RTMobRef;
typedef struct _RTMobActionData* RTMobActionDataRef;
typedef struct _RTMobActionGroupData* RTMobActionGroupDataRef;
typedef struct _RTMobTriggerData* RTMobTriggerDataRef;
typedef struct _RTMobPatternSpawnData* RTMobPatternSpawnDataRef;
typedef struct _RTMobPatternData* RTMobPatternDataRef;
typedef struct _RTMobTriggerState* RTMobTriggerStateRef;
typedef struct _RTMobActionState* RTMobActionStateRef;
typedef struct _RTMobPattern* RTMobPatternRef;
typedef struct _RTMovement* RTMovementRef;
typedef struct _RTNewbieSupportSlot* RTNewbieSupportSlotRef;
typedef struct _RTCharacterNewbieSupportInfo* RTCharacterNewbieSupportInfoRef;
typedef struct _RTNotification* RTNotificationRef;
typedef struct _RTNotificationManager* RTNotificationManagerRef;
typedef struct _RTNpc* RTNpcRef;
typedef struct _RTOptionPoolManager* RTOptionPoolManagerRef;
typedef struct _RTPartyMemberInfo* RTPartyMemberInfoRef;
typedef struct _RTPartyMemberData* RTPartyMemberDataRef;
typedef struct _RTPartyInvitation* RTPartyInvitationRef;
typedef struct _RTParty* RTPartyRef;
typedef struct _RTPartyManager* RTPartyManagerRef;
typedef struct _RTPersonalShopSlot* RTPersonalShopSlotRef;
typedef struct _RTPersonalShopInfo* RTPersonalShopInfoRef;
typedef struct _RTCharacterPersonalShopInfo* RTCharacterPersonalShopInfoRef;
typedef struct _RTPosition* RTPositionRef;
typedef struct _RTPvPContext* RTPvPContextRef;
typedef struct _RTPvPMemberContext* RTPvPMemberContextRef;
typedef struct _RTPvPManager* RTPvPManagerRef;
typedef struct _RTQuestConditionData* RTQuestConditionDataRef;
typedef struct _RTQuestNpcData* RTQuestNpcDataRef;
typedef struct _RTQuestNpcSetData* RTQuestNpcSetDataRef;
typedef struct _RTQuestMissionData* RTQuestMissionDataRef;
typedef struct _RTQuestData* RTQuestDataRef;
typedef struct _RTQuestRewardItemData* RTQuestRewardItemDataRef;
typedef struct _RTQuestRewardItemSetData* RTQuestRewardItemSetDataRef;
typedef struct _RTQuestSlot* RTQuestSlotRef;
typedef struct _RTQuickSlot* RTQuickSlotRef;
typedef struct _RTCharacterRequestCraftInfo* RTCharacterRequestCraftInfoRef;
typedef struct _RTRuntime* RTRuntimeRef;
typedef struct _RTResearchSupportMaterialSlot* RTResearchSupportMaterialSlotRef;
typedef struct _RTResearchSupportMissionSlot* RTResearchSupportMissionSlotRef;
typedef struct _RTResearchSupportMissionBoard* RTResearchSupportMissionBoardRef;
typedef struct _RTRequestCraftSlot* RTRequestCraftSlotRef;
typedef struct _RTRequestCraftInventorySlot* RTRequestCraftInventorySlotRef;
typedef struct _RTCharacterResearchSupportInfo* RTCharacterResearchSupportInfoRef;
typedef struct _RTScript* RTScriptRef;
typedef struct _RTScriptManager* RTScriptManagerRef; 
typedef struct _RTCooldownSlot* RTCooldownSlotRef;
typedef struct _RTSkillSlot* RTSkillSlotRef;
typedef struct _RTStellarMasterySlot* RTStellarMasterySlotRef;
typedef struct _RTMythMasterySlot* RTMythMasterySlotRef;
typedef struct _RTTradeContext* RTTradeContextRef;
typedef struct _RTTradeMemberContext* RTTradeMemberContextRef;
typedef struct _RTTradeManager* RTTradeManagerRef;
typedef struct _RTTrainerSkillData* RTTrainerSkillDataRef;
typedef struct _RTTrainerData* RTTrainerDataRef;
typedef struct _RTSkillLevelData* RTSkillLevelDataRef;
typedef struct _RTSkillValueData* RTSkillValueDataRef;
typedef struct _RTCharacterSkillData* RTCharacterSkillDataRef;
typedef struct _RTWarp* RTWarpRef;
typedef struct _RTDungeonTriggerData* RTDungeonTriggerDataRef;
typedef struct _RTDungeonTriggerActionData* RTDungeonTriggerActionDataRef;
typedef struct _RTDungeonTimeControlData* RTDungeonTimeControlDataRef;
typedef struct _RTTimerData* RTTimerDataRef;
typedef struct _RTTimer* RTTimerRef;
typedef struct _RTDungeonTimerData* RTDungeonTimerDataRef;
typedef struct _RTImmuneData* RTImmuneDataRef;
typedef struct _RTDungeonImmuneControlData* RTDungeonImmuneControlDataRef;
typedef struct _RTDungeonGateControlData* RTDungeonGateControlDataRef;
typedef struct _RTDungeonData* RTDungeonDataRef;
typedef struct _RTMissionDungeonPatternPartData* RTMissionDungeonPatternPartDataRef;
typedef struct _RTCharacterWarehouseInfo* RTCharacterWarehouseInfoRef;
typedef struct _RTCharacterVehicleInventoryInfo* RTCharacterVehicleInventoryInfoRef;
typedef struct _RTWorldItem* RTWorldItemRef;
typedef struct _RTWorldData* RTWorldDataRef;
typedef struct _RTWorldContext* RTWorldContextRef;
typedef struct _RTWorldChunk* RTWorldChunkRef;
typedef struct _RTWorldManager* RTWorldManagerRef;

struct _RTPosition {
	Int32 X;
	Int32 Y;
};
typedef struct _RTPosition RTPosition;

union _RTShortDate {
    struct {
        UInt16 Year : 7;
        UInt16 Month : 4;
        UInt16 Day : 5;
    };

    UInt16 RawValue;
};
typedef union _RTShortDate RTShortDate;

#pragma pack(pop)

EXTERN_C_END
