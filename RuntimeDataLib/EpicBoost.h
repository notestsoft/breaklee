RUNTIME_DATA_FILE_BEGIN(meister.enc)

RUNTIME_DATA_TYPE_BEGIN(MasterItem, "meister.meister_item", 32)
	RUNTIME_DATA_PROPERTY(Int32, Grade, "grade")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(MasterItemOption, "meister_option", 64)
		RUNTIME_DATA_PROPERTY(Int32, ID, "id")
		RUNTIME_DATA_PROPERTY(Int32, BoostCount, "boost_count")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(MasterItemOptionValue, "meister_force", 4)
			RUNTIME_DATA_PROPERTY(Int32, ForceEffectIndex, "code")
			RUNTIME_DATA_PROPERTY(Int32, ForceValue, "value")
			RUNTIME_DATA_PROPERTY(Int32, ForceValueType, "value_type")
		RUNTIME_DATA_TYPE_END_CHILD(MasterItemOptionValue, 4)
	RUNTIME_DATA_TYPE_END_CHILD(MasterItemOption, 64)
RUNTIME_DATA_TYPE_END(MasterItem)
RUNTIME_DATA_TYPE_INDEX(MasterItem, Int32, Grade)
RUNTIME_DATA_TYPE_INDEX_CHILD(MasterItem, MasterItemOption, Int32, ID)

RUNTIME_DATA_FILE_END

RUNTIME_DATA_FILE_BEGIN(EpicBoost.xml)

RUNTIME_DATA_TYPE_BEGIN(EpicBoostMain, "EpicBoost.EpicBoostMain.Item", 16)
	RUNTIME_DATA_PROPERTY(Int32, ItemType, "ItemType")
	RUNTIME_DATA_PROPERTY(Int32, MinLevel, "MinLevel")
RUNTIME_DATA_TYPE_END(EpicBoostMain)
RUNTIME_DATA_TYPE_INDEX(EpicBoostMain, Int32, ItemType)

RUNTIME_DATA_TYPE_BEGIN(EpicBoostNormalRate, "EpicBoost.NormalRate.Grade", 8)
	RUNTIME_DATA_PROPERTY(Int32, Grade, "Grade")
	RUNTIME_DATA_PROPERTY(Int32, ResultGrade, "ResultGrade")
	RUNTIME_DATA_PROPERTY(Int32, Rate, "Rate")
RUNTIME_DATA_TYPE_END(EpicBoostNormalRate)
RUNTIME_DATA_TYPE_INDEX(EpicBoostNormalRate, Int32, Grade)

RUNTIME_DATA_TYPE_BEGIN(EpicBoostMasterRate, "EpicBoost.MasterRate.Grade", 8)
	RUNTIME_DATA_PROPERTY(Int32, BoostCount, "BoostCount")
	RUNTIME_DATA_PROPERTY(Int32, Rate, "Rate")
RUNTIME_DATA_TYPE_END(EpicBoostMasterRate)
RUNTIME_DATA_TYPE_INDEX(EpicBoostMasterRate, Int32, BoostCount)

RUNTIME_DATA_FILE_END