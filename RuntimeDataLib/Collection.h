#ifndef RUNTIME_DATA_COLLECTION_ITEM_TYPE_INDEX
#define RUNTIME_DATA_COLLECTION_ITEM_TYPE_INDEX

enum {
	RUNTIME_DATA_COLLECTION_ITEM_TYPE_INDEX_ITEM		= 1,
	RUNTIME_DATA_COLLECTION_ITEM_TYPE_INDEX_UPGRADE		= 2,
	RUNTIME_DATA_COLLECTION_ITEM_TYPE_INDEX_QUEST		= 4,
};

enum {
	RUNTIME_DATA_COLLECTION_REWARD_TYPE_CURRENCY	= 1,
	RUNTIME_DATA_COLLECTION_REWARD_TYPE_ITEM		= 2,
};

#endif

RUNTIME_DATA_FILE_BEGIN(Collection.enc)

RUNTIME_DATA_TYPE_BEGIN(CollectionQuest, "Collection.Collection_open.collection_quest")
	RUNTIME_DATA_PROPERTY(UInt32, QuestID, "quest_id")
RUNTIME_DATA_TYPE_END(CollectionQuest)
RUNTIME_DATA_TYPE_INDEX_SINGLE(CollectionQuest)

RUNTIME_DATA_TYPE_BEGIN(CollectionKind, "Collection.Collection_type.collection_kind")
	RUNTIME_DATA_PROPERTY(Int32, Type, "type")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(CollectionKindInfo, "collection_info")
		RUNTIME_DATA_PROPERTY(Int32, ID, "c_id")
		RUNTIME_DATA_PROPERTY(Int32, RewardID, "c_reward_id")
		RUNTIME_DATA_PROPERTY(Int32, Order, "order")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(CollectionKindInfoDetail, "collection_detail")
			RUNTIME_DATA_PROPERTY(Int32, MissionID, "mission_id")
			RUNTIME_DATA_PROPERTY(Int32, RewardType, "m_reward_type")
			RUNTIME_DATA_PROPERTY(Int32, RewardID, "m_reward_id")
		RUNTIME_DATA_TYPE_END_CHILD(CollectionKindInfoDetail)
	RUNTIME_DATA_TYPE_END_CHILD(CollectionKindInfo)
RUNTIME_DATA_TYPE_END(CollectionKind)
RUNTIME_DATA_TYPE_INDEX(CollectionKind, Int32, Type)

RUNTIME_DATA_TYPE_BEGIN(CollectionMission, "Collection.Collection_mission.collection_type")
	RUNTIME_DATA_PROPERTY(Int32, Type, "c_type")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(CollectionMissionInfo, "mission_info")
		RUNTIME_DATA_PROPERTY(Int32, ID, "c_id")
		RUNTIME_DATA_PROPERTY(Int32, MissionID, "mission_id")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(CollectionMissionDetail, "mission_detail")
			RUNTIME_DATA_PROPERTY(Int32, ID, "slot_id")
			RUNTIME_DATA_PROPERTY(UInt32, ItemType, "m_item_type")
			RUNTIME_DATA_PROPERTY(UInt32, ItemID, "m_item_id")
			RUNTIME_DATA_PROPERTY(UInt32, ItemCount, "m_item_need")
		RUNTIME_DATA_TYPE_END_CHILD(CollectionMissionDetail)
	RUNTIME_DATA_TYPE_END_CHILD(CollectionMissionInfo)
RUNTIME_DATA_TYPE_END(CollectionMission)
RUNTIME_DATA_TYPE_INDEX(CollectionMission, Int32, Type)

RUNTIME_DATA_TYPE_BEGIN(CollectionMissionItem, "Collection.Missionitem_type_index.item_info")
	RUNTIME_DATA_PROPERTY(UInt32, ItemID, "m_item_id")
	RUNTIME_DATA_PROPERTY(UInt32, ItemFlag, "item_flag")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(CollectionMissionItemDetail, "item_detail")
		RUNTIME_DATA_PROPERTY(UInt32, ItemIndex, "item_index")
		RUNTIME_DATA_PROPERTY(UInt32, ItemOption, "item_option")
		RUNTIME_DATA_PROPERTY(UInt32, ItemKind, "item_kind")
	RUNTIME_DATA_TYPE_END_CHILD(CollectionMissionItemDetail)
RUNTIME_DATA_TYPE_END(CollectionMissionItem)
RUNTIME_DATA_TYPE_INDEX(CollectionMissionItem, UInt32, ItemID)

RUNTIME_DATA_TYPE_BEGIN(CollectionMissionUpgrade, "Collection.Missionitem_type_upgrade.item_info")
	RUNTIME_DATA_PROPERTY(UInt32, ItemID, "m_item_id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(CollectionMissionUpgradeDetail, "item_detail")
		RUNTIME_DATA_PROPERTY(UInt32, ItemIndex, "item_index")
		RUNTIME_DATA_PROPERTY(UInt32, ItemGrade, "item_grade")
	RUNTIME_DATA_TYPE_END_CHILD(CollectionMissionUpgradeDetail)
RUNTIME_DATA_TYPE_END(CollectionMissionUpgrade)
RUNTIME_DATA_TYPE_INDEX(CollectionMissionUpgrade, UInt32, ItemID)

RUNTIME_DATA_TYPE_BEGIN(CollectionMissionQuest, "Collection.Missionitem_type_quest.item_info")
	RUNTIME_DATA_PROPERTY(UInt32, ItemID, "m_item_id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(CollectionMissionQuestDetail, "item_detail")
		RUNTIME_DATA_PROPERTY(UInt32, ItemIndex, "item_index")
		RUNTIME_DATA_PROPERTY(UInt32, QuestID, "quest_id")
	RUNTIME_DATA_TYPE_END_CHILD(CollectionMissionQuestDetail)
RUNTIME_DATA_TYPE_END(CollectionMissionQuest)
RUNTIME_DATA_TYPE_INDEX(CollectionMissionQuest, UInt32, ItemID)

RUNTIME_DATA_TYPE_BEGIN(CollectionRewardCurrency, "Collection.Reward_forcegem.reward_info")
	RUNTIME_DATA_PROPERTY(Int32, RewardID, "reward_id")
	RUNTIME_DATA_PROPERTY(Int32, Quantity, "zem_qty")
RUNTIME_DATA_TYPE_END(CollectionRewardCurrency)
RUNTIME_DATA_TYPE_INDEX(CollectionRewardCurrency, Int32, RewardID)

RUNTIME_DATA_TYPE_BEGIN(CollectionRewardItem, "Collection.Reward_item.reward_info")
	RUNTIME_DATA_PROPERTY(Int32, RewardID, "reward_id")
	RUNTIME_DATA_PROPERTY(UInt32, ItemKind, "item_kind")
	RUNTIME_DATA_PROPERTY(UInt32, ItemOption, "item_opt")
	RUNTIME_DATA_PROPERTY(UInt32, ItemDuration, "Duration")
RUNTIME_DATA_TYPE_END(CollectionRewardItem)
RUNTIME_DATA_TYPE_INDEX(CollectionRewardItem, Int32, RewardID)

RUNTIME_DATA_TYPE_BEGIN(CollectionRewardAttribute, "Collection.Collection_reward.reward_info")
	RUNTIME_DATA_PROPERTY(Int32, RewardID, "c_reward_id")
	RUNTIME_DATA_PROPERTY(Int32, RewardAbility, "reward_ability")
	RUNTIME_DATA_PROPERTY(Int32, ValueType, "value_type")
	RUNTIME_DATA_PROPERTY(Int32, AbilityValue1, "ability_value1")
	RUNTIME_DATA_PROPERTY(Int32, AbilityValue2, "ability_value2")
	RUNTIME_DATA_PROPERTY(Int32, AbilityValue3, "ability_value3")
RUNTIME_DATA_TYPE_END(CollectionRewardAttribute)
RUNTIME_DATA_TYPE_INDEX(CollectionRewardAttribute, Int32, RewardID)

RUNTIME_DATA_FILE_END
