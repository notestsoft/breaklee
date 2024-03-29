RUNTIME_DATA_FILE_BEGIN(ForceWing.enc)

RUNTIME_DATA_TYPE_BEGIN(ForceWingActivation, "ForceWing.ForceWing_ActiveLV.Wing_Min_Lv", 1)
	RUNTIME_DATA_PROPERTY(Int32, MinLevel, "Min_lv")
RUNTIME_DATA_TYPE_END(ForceWingActivation)
RUNTIME_DATA_TYPE_INDEX_SINGLE(ForceWingActivation)

RUNTIME_DATA_TYPE_BEGIN(ForceWingGradeLimit, "ForceWing.ForceWing_Limit_Grade.Wing_Limit_Grade", 1)
	RUNTIME_DATA_PROPERTY(Int32, MaxGrade, "Limit_Grade")
RUNTIME_DATA_TYPE_END(ForceWingGradeLimit)
RUNTIME_DATA_TYPE_INDEX_SINGLE(ForceWingGradeLimit)

RUNTIME_DATA_TYPE_BEGIN(ForceWingGradeInfo, "ForceWing.ForceWing_Grade.Wing_Info", 8)
	RUNTIME_DATA_PROPERTY(Int32, Grade, "Grade")
	RUNTIME_DATA_PROPERTY(Int32, MinLevel, "Min_lv")
	RUNTIME_DATA_PROPERTY(Int32, MaxLevel, "Max_lv")
	RUNTIME_DATA_PROPERTY(UInt64, MaterialItemID, "Material_Item")
	RUNTIME_DATA_PROPERTY(UInt64, WingItemID, "Wing_Id")
RUNTIME_DATA_TYPE_END(ForceWingGradeInfo)
RUNTIME_DATA_TYPE_INDEX(ForceWingGradeInfo, Int32, Grade)

RUNTIME_DATA_TYPE_BEGIN(ForceWingGrade, "ForceWing.ForceWing_Grow.Wing_Grade", 8)
	RUNTIME_DATA_PROPERTY(Int32, Grade, "grade")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(ForceWingGradeLevel, "Wing_grow", 128)
		RUNTIME_DATA_PROPERTY(Int32, Level, "Level")
		RUNTIME_DATA_PROPERTY(UInt64, Exp, "Exp")
		RUNTIME_DATA_PROPERTY(Int32, RequiredMaterialItemCount, "Item_need")
		RUNTIME_DATA_PROPERTY(UInt64, ExtraMaterialItemID, "Material_Item2")
		RUNTIME_DATA_PROPERTY(Int32, RequiredExtraMaterialItemCount, "Item2_need")
	RUNTIME_DATA_TYPE_END_CHILD(ForceWingGradeLevel, 128)
RUNTIME_DATA_TYPE_END(ForceWingGrade)
RUNTIME_DATA_TYPE_INDEX(ForceWingGrade, Int32, Grade)
RUNTIME_DATA_TYPE_INDEX_CHILD(ForceWingGrade, ForceWingGradeLevel, Int32, Level)

#define RUNTIME_DATA_MAX_FORCE_WING_UPGRADE_MATERIAL_COUNT 2

RUNTIME_DATA_TYPE_BEGIN(ForceWingUpgrade, "ForceWing.ForceWing_Upgrade.Wing_Upgrade", 8)
	RUNTIME_DATA_PROPERTY(Int32, Grade, "Grade")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(ForceWingUpgradeMaterial, "Material", RUNTIME_DATA_MAX_FORCE_WING_UPGRADE_MATERIAL_COUNT)
		RUNTIME_DATA_PROPERTY(Int32, Order, "Order")
		RUNTIME_DATA_PROPERTY(UInt64, MaterialItemID, "Item")
		RUNTIME_DATA_PROPERTY(Int32, RequiredMaterialItemCount, "Count")
	RUNTIME_DATA_TYPE_END_CHILD(ForceWingUpgradeMaterial, RUNTIME_DATA_MAX_FORCE_WING_UPGRADE_MATERIAL_COUNT)
RUNTIME_DATA_TYPE_END(ForceWingUpgrade)
RUNTIME_DATA_TYPE_INDEX(ForceWingUpgrade, Int32, Grade)

RUNTIME_DATA_FILE_END