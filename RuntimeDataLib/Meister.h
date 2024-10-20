RUNTIME_DATA_FILE_BEGIN(meister.enc)

RUNTIME_DATA_TYPE_BEGIN(MasterItem, "meister.meister_item")
	RUNTIME_DATA_PROPERTY(Int32, Grade, "grade")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(MasterItemOption, "meister_option")
		RUNTIME_DATA_PROPERTY(Int32, ID, "id")
		RUNTIME_DATA_PROPERTY(Int32, BoostCount, "boost_count")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(MasterItemOptionValue, "meister_force")
			RUNTIME_DATA_PROPERTY(Int32, ForceEffectIndex, "code")
			RUNTIME_DATA_PROPERTY(Int32, ForceValue, "value")
			RUNTIME_DATA_PROPERTY(Int32, ForceValueType, "value_type")
		RUNTIME_DATA_TYPE_END_CHILD(MasterItemOptionValue)
	RUNTIME_DATA_TYPE_END_CHILD(MasterItemOption)
RUNTIME_DATA_TYPE_END(MasterItem)
RUNTIME_DATA_TYPE_INDEX(MasterItem, Int32, Grade)
RUNTIME_DATA_TYPE_INDEX_CHILD(MasterItem, MasterItemOption, Int32, ID)

RUNTIME_DATA_FILE_END
