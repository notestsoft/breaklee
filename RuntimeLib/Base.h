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
typedef struct _RTFinalBattleAttributes* RTFinalBattleAttributesRef;
typedef struct _RTBattleResult* RTBattleResultRef;
typedef struct _RTCharacterInfo* RTCharacterInfoRef;
typedef struct _RTCharacterData* RTCharacterDataRef;
typedef struct _RTEquipmentLockSlot* RTEquipmentLockSlotRef;
typedef struct _RTCharacterEquipmentData* RTCharacterEquipmentDataRef;
typedef struct _RTCharacterInventoryInfo* RTCharacterInventoryInfoRef;
typedef struct _RTCharacterSkillSlotInfo* RTCharacterSkillSlotInfoRef;
typedef struct _RTCharacterQuickSlotInfo* RTCharacterQuickSlotInfoRef;
typedef struct _RTCharacterQuestFlagInfo* RTCharacterQuestFlagInfoRef;
typedef struct _RTCharacterDungeonQuestFlagInfo* RTCharacterDungeonQuestFlagInfoRef;
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
typedef struct _RTItemData* RTItemDataRef;
typedef struct _RTItemSlot* RTItemSlotRef;
typedef struct _RTItemSlotAppearance* RTItemSlotAppearanceRef;
typedef struct _RTMobSpawnData* RTMobSpawnDataRef;
typedef struct _RTMobSkillData* RTMobSkillDataRef;
typedef struct _RTMobSpeciesData* RTMobSpeciesDataRef;
typedef struct _RTMobScript* RTMobScriptRef;
typedef struct _RTMob* RTMobRef;
typedef struct _RTMovement* RTMovementRef;
typedef struct _RTNewbieSupportSlot* RTNewbieSupportSlotRef;
typedef struct _RTCharacterNewbieSupportInfo* RTCharacterNewbieSupportInfoRef;
typedef struct _RTNotification* RTNotificationRef;
typedef struct _RTNotificationManager* RTNotificationManagerRef;
typedef struct _RTNpc* RTNpcRef;
typedef struct _RTPartyMemberInfo* RTPartyMemberInfoRef;
typedef struct _RTPartyMemberData* RTPartyMemberDataRef;
typedef struct _RTPartySlot* RTPartySlotRef;
typedef struct _RTPartyInvitation* RTPartyInvitationRef;
typedef struct _RTParty* RTPartyRef;
typedef struct _RTPartyManager* RTPartyManagerRef;
typedef struct _RTPosition* RTPositionRef;
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
typedef struct _RTCharacterResearchSupportInfo* RTCharacterResearchSupportInfoRef;
typedef struct _RTScript* RTScriptRef;
typedef struct _RTScriptManager* RTScriptManagerRef;
typedef struct _RTSkillSlot* RTSkillSlotRef;
typedef struct _RTStellarMasterySlot* RTStellarMasterySlotRef;
typedef struct _RTTrainerSkillData* RTTrainerSkillDataRef;
typedef struct _RTTrainerData* RTTrainerDataRef;
typedef struct _RTSkillLevelData* RTSkillLevelDataRef;
typedef struct _RTSkillValueData* RTSkillValueDataRef;
typedef struct _RTCharacterSkillData* RTCharacterSkillDataRef;
typedef struct _RTWarp* RTWarpRef;
typedef struct _RTDungeonTriggerData* RTDungeonTriggerDataRef;
typedef struct _RTDungeonTriggerActionData* RTDungeonTriggerActionDataRef;
typedef struct _RTDungeonTimeControlData* RTDungeonTimeControlDataRef;
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

#pragma pack(pop)

EXTERN_C_END
