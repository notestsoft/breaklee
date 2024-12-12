RUNTIME_DATA_FILE_BEGIN(Const.enc)

RUNTIME_DATA_TYPE_BEGIN(Cooltime, "Const.global_ctime.ctime")
	RUNTIME_DATA_PROPERTY(UInt16, Index, "cooltime_id")
	RUNTIME_DATA_PROPERTY(Int32, CheckSave, "checksave")
	RUNTIME_DATA_PROPERTY(Int32, Interval, "global_ctime")
	RUNTIME_DATA_PROPERTY(Int32, WarInterval, "global_mission_ctime")
	RUNTIME_DATA_PROPERTY_ARRAY(UInt16, LinkList, "link_id", 4, ',')
RUNTIME_DATA_TYPE_END(Cooltime)
RUNTIME_DATA_TYPE_INDEX(Cooltime, Int32, Index)

RUNTIME_DATA_FILE_END
