RUNTIME_DATA_FILE_BEGIN(ChallengeMissionData.enc)

RUNTIME_DATA_TYPE_BEGIN(ChallengeMissionList, "challenge_mission.mission_list.mission")
	RUNTIME_DATA_PROPERTY(Int32, Index, "idx")
	RUNTIME_DATA_PROPERTY(Int32, MissionType, "goal_type")
	RUNTIME_DATA_PROPERTY(Int32, TargetCount, "target_count")
	RUNTIME_DATA_PROPERTY(Int32, Point, "point")
	RUNTIME_DATA_PROPERTY(Int32, MaxCount, "max_count")
RUNTIME_DATA_TYPE_END(ChallengeMissionList)

RUNTIME_DATA_TYPE_BEGIN(ChallengeMissionActiveList, "challenge_mission.active_mission_list.active_mission")
	RUNTIME_DATA_PROPERTY(Int32, Index, "idx")
RUNTIME_DATA_TYPE_END(ChallengeMissionActiveList)

RUNTIME_DATA_TYPE_BEGIN(ChallengeMissionRewardGradeList, "challenge_mission.reward_grade_list.reward_grade")
	RUNTIME_DATA_PROPERTY(Int32, Index, "idx")
	RUNTIME_DATA_PROPERTY(Int32, Condition, "condition")
RUNTIME_DATA_TYPE_END(ChallengeMissionRewardGradeList)

RUNTIME_DATA_TYPE_BEGIN(ChallengeMissionRewardSlotList, "challenge_mission.reward_slot_list.reward_slot")
	RUNTIME_DATA_PROPERTY(Int32, Index, "idx")
	RUNTIME_DATA_PROPERTY(Int32, RewardPoolIndex, "reward_pool")
RUNTIME_DATA_TYPE_END(ChallengeMissionRewardSlotList)

RUNTIME_DATA_TYPE_BEGIN(ChallengeMissionRewardPoolList, "challenge_mission.reward_pool_list.reward_pool")
	RUNTIME_DATA_PROPERTY(Int32, Index, "idx")
	RUNTIME_DATA_PROPERTY(Int32, Order, "order")
	RUNTIME_DATA_PROPERTY(UInt64, ItemID, "itemkindidx")
	RUNTIME_DATA_PROPERTY(UInt64, ItemOptions, "opt")
	RUNTIME_DATA_PROPERTY(Int32, DurationIndex, "duration_idx")
	RUNTIME_DATA_PROPERTY(Int32, Count, "count")
RUNTIME_DATA_TYPE_END(ChallengeMissionRewardPoolList)

RUNTIME_DATA_FILE_END