RUNTIME_DATA_FILE_BEGIN(DIR_PATH(Upgrade, EpicBoost.xml))

RUNTIME_DATA_TYPE_BEGIN(EpicBoostMain, "EpicBoost.EpicBoostMain.Item")
	RUNTIME_DATA_PROPERTY(Int32, ItemType, "ItemType")
	RUNTIME_DATA_PROPERTY(Int32, MinLevel, "MinLevel")
RUNTIME_DATA_TYPE_END(EpicBoostMain)
RUNTIME_DATA_TYPE_INDEX(EpicBoostMain, Int32, ItemType)

RUNTIME_DATA_TYPE_BEGIN(EpicBoostNormalRate, "EpicBoost.NormalRate.Grade")
	RUNTIME_DATA_PROPERTY(Int32, Grade, "Grade")
	RUNTIME_DATA_PROPERTY(Int32, ResultGrade, "ResultGrade")
	RUNTIME_DATA_PROPERTY(Int32, Rate, "Rate")
RUNTIME_DATA_TYPE_END(EpicBoostNormalRate)
RUNTIME_DATA_TYPE_INDEX(EpicBoostNormalRate, Int32, Grade)

RUNTIME_DATA_TYPE_BEGIN(EpicBoostMasterRate, "EpicBoost.MasterRate.Grade")
	RUNTIME_DATA_PROPERTY(Int32, BoostCount, "BoostCount")
	RUNTIME_DATA_PROPERTY(Int32, Rate, "Rate")
RUNTIME_DATA_TYPE_END(EpicBoostMasterRate)
RUNTIME_DATA_TYPE_INDEX(EpicBoostMasterRate, Int32, BoostCount)

RUNTIME_DATA_FILE_END