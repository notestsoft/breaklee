#ifndef RUNTIME_DATA_ENCHANT
#define RUNTIME_DATA_ENCHANT

#define RUNTIME_DATA_UPGRADE_RATE_VALUE_COUNT 6

enum {
	RUNTIME_DATA_UPGRADE_TYPE_NORMAL = 0,
	RUNTIME_DATA_UPGRADE_TYPE_ENCHANT = 1,
	RUNTIME_DATA_UPGRADE_TYPE_SUPERIOR = 2,
	RUNTIME_DATA_UPGRADE_TYPE_PERFECT = 3,
};

#endif

RUNTIME_DATA_FILE_BEGIN(Enchant.enc)

RUNTIME_DATA_TYPE_BEGIN(UpgradeItemBasic, "enchant.item")
RUNTIME_DATA_PROPERTY_PRECONDITION(Int32, Extended, "extend", 0)
	RUNTIME_DATA_PROPERTY(RTDataItemType, Type, "type")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(UpgradeItemBasicGrade, "grade")
		RUNTIME_DATA_PROPERTY(Int32, MinGrade, "min_grade")
		RUNTIME_DATA_PROPERTY(Int32, MaxGrade, "max_grade")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(UpgradeItemBasicGradeValue, "enchant_value")
			RUNTIME_DATA_PROPERTY(Int32, ForceID, "force_id")
			RUNTIME_DATA_PROPERTY_ARRAY(Int32, ForceValueFormula, "force_value", 2, ',')
		RUNTIME_DATA_TYPE_END_CHILD(UpgradeItemBasicGradeValue)
	RUNTIME_DATA_TYPE_END_CHILD(UpgradeItemBasicGrade)
RUNTIME_DATA_TYPE_END(UpgradeItemBasic)
RUNTIME_DATA_TYPE_INDEX(UpgradeItemBasic, RTDataItemType, Type)

RUNTIME_DATA_TYPE_BEGIN(UpgradeItem, "enchant.item")
RUNTIME_DATA_PROPERTY_PRECONDITION(Int32, Extended, "extend", 1)
	RUNTIME_DATA_PROPERTY(RTDataItemType, Type, "type")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(UpgradeItemGrade, "grade")
		RUNTIME_DATA_PROPERTY(Int32, MinGrade, "min_grade")
		RUNTIME_DATA_PROPERTY(Int32, MaxGrade, "max_grade")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(UpgradeItemGradeValue, "enchant_value")
			RUNTIME_DATA_PROPERTY(Int32, ForceID, "force_id")
			RUNTIME_DATA_PROPERTY_ARRAY(Int32, ForceValueFormula, "force_value", 4, ',')
			RUNTIME_DATA_PROPERTY(Int32, ForceValueType, "value_type")
			RUNTIME_DATA_PROPERTY(Int32, ApplyLevel, "Apply_lv")
		RUNTIME_DATA_TYPE_END_CHILD(UpgradeItemGradeValue)
	RUNTIME_DATA_TYPE_END_CHILD(UpgradeItemGrade)
RUNTIME_DATA_TYPE_END(UpgradeItem)
RUNTIME_DATA_TYPE_INDEX(UpgradeItem, RTDataItemType, Type)

RUNTIME_DATA_TYPE_BEGIN(UpgradeLimit, "enchant.enchant_limit.limit_value")
	RUNTIME_DATA_PROPERTY(Int32, ItemGrade, "item_grade")
	RUNTIME_DATA_PROPERTY(Int32, MaxItemLevel, "limit_lv")
RUNTIME_DATA_TYPE_END(UpgradeLimit)
RUNTIME_DATA_TYPE_INDEX(UpgradeLimit, Int32, ItemGrade)

RUNTIME_DATA_TYPE_BEGIN(UpgradeCostCore, "enchant.enchant_coreneeds.coreneeds")
	RUNTIME_DATA_PROPERTY(Int32, ItemLevel, "lv")
	RUNTIME_DATA_PROPERTY(Int32, NormalCount, "upgrade")
	RUNTIME_DATA_PROPERTY(Int32, PerfectCount, "perfect")
	RUNTIME_DATA_PROPERTY(Int32, EnhancedCount, "enhanced")
	RUNTIME_DATA_PROPERTY(Int32, SuperiorCount, "superior")
RUNTIME_DATA_TYPE_END(UpgradeCostCore)
RUNTIME_DATA_TYPE_INDEX(UpgradeCostCore, Int32, ItemLevel)

RUNTIME_DATA_TYPE_BEGIN(UpgradeResult, "enchant.enchant_result.result")
	RUNTIME_DATA_PROPERTY(Int32, ItemLevel, "lv")
	RUNTIME_DATA_PROPERTY_ARRAY(Int32, Normal, "upgrade", 8, ',')
	RUNTIME_DATA_PROPERTY_ARRAY(Int32, Enhanced, "enhanced", 8, ',')
	RUNTIME_DATA_PROPERTY_ARRAY(Int32, Superior, "superior", 8, ',')
RUNTIME_DATA_TYPE_END(UpgradeResult)
RUNTIME_DATA_TYPE_INDEX(UpgradeResult, Int32, ItemLevel)

RUNTIME_DATA_TYPE_BEGIN(UpgradeCostSafeguard, "enchant.enchant_buffer.result")
	RUNTIME_DATA_PROPERTY(Int32, ItemLevel, "lv")
	RUNTIME_DATA_PROPERTY(Int32, NormalCount, "upgrade")
	RUNTIME_DATA_PROPERTY(Int32, EnhancedCount, "enhanced")
	RUNTIME_DATA_PROPERTY(Int32, SuperiorCount, "superior")
RUNTIME_DATA_TYPE_END(UpgradeCostSafeguard)
RUNTIME_DATA_TYPE_INDEX(UpgradeCostSafeguard, Int32, ItemLevel)

RUNTIME_DATA_FILE_END

RUNTIME_DATA_FILE_BEGIN(DIR_PATH(Upgrade, NormalUpgrade.xml))

RUNTIME_DATA_TYPE_BEGIN(UpgradeGrade, "enchant.enchant_grade.enchant_grade_value")
	RUNTIME_DATA_PROPERTY(Int32, ItemGrade, "item_grade")
	RUNTIME_DATA_PROPERTY(Int32, CostGrade, "cost_grade")
RUNTIME_DATA_TYPE_END(UpgradeGrade)
RUNTIME_DATA_TYPE_INDEX(UpgradeGrade, Int32, ItemGrade)

RUNTIME_DATA_TYPE_BEGIN(UpgradeGradeChange, "enchant.enchant_grade_change.enchant_grade_change_value")
	RUNTIME_DATA_PROPERTY(Int32, ItemType, "item_type")
	RUNTIME_DATA_PROPERTY(Int32, ItemGrade, "item_grade")
	RUNTIME_DATA_PROPERTY(Int32, CostGrade, "cost_grade")
RUNTIME_DATA_TYPE_END(UpgradeGradeChange)

RUNTIME_DATA_TYPE_BEGIN(UpgradeExtendGrade, "enchant.enchant_extend_grade.enchant_grade_value")
	RUNTIME_DATA_PROPERTY(Int32, ItemGrade, "item_grade")
	RUNTIME_DATA_PROPERTY(Int32, CostGrade, "cost_grade")
RUNTIME_DATA_TYPE_END(UpgradeExtendGrade)
RUNTIME_DATA_TYPE_INDEX(UpgradeExtendGrade, Int32, ItemGrade)

RUNTIME_DATA_TYPE_BEGIN(UpgradeRate, "enchant.enchant_rate.enchant_rate_type")
	RUNTIME_DATA_PROPERTY(Int32, Type, "type")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(UpgradeRateValue, "enchant_rate_value")
		RUNTIME_DATA_PROPERTY(Int32, Level, "level")
		RUNTIME_DATA_PROPERTY_ARRAY(Int32, Rates, "rates", RUNTIME_DATA_UPGRADE_RATE_VALUE_COUNT, ',')
	RUNTIME_DATA_TYPE_END_CHILD(UpgradeRateValue)
RUNTIME_DATA_TYPE_END(UpgradeRate)
RUNTIME_DATA_TYPE_INDEX(UpgradeRate, Int32, Type)
RUNTIME_DATA_TYPE_INDEX_CHILD(UpgradeRate, UpgradeRateValue, Int32, Level)

RUNTIME_DATA_FILE_END
