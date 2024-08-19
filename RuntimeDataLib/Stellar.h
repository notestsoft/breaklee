RUNTIME_DATA_FILE_BEGIN(stellar.enc)


RUNTIME_DATA_TYPE_BEGIN(StellarGroup, "stellar_ability.stellar_main.stellar_info", 1)
	RUNTIME_DATA_PROPERTY(Int32, GroupID, "group_id")

	RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarLine, "detail", 5)
		RUNTIME_DATA_PROPERTY(Int32, LineID, "line_no")
	
		RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarLineGrade, "group", 5)
			RUNTIME_DATA_PROPERTY(Int32, Grade, "grade")
			RUNTIME_DATA_PROPERTY(Int32, ForceCodePer, "forcecode_per") // WTF is this?
			RUNTIME_DATA_PROPERTY(Int32, ItemID, "Itemkind")
			RUNTIME_DATA_PROPERTY(Int32, ItemCount, "itemcount")
			RUNTIME_DATA_PROPERTY(Int32, MinGrade, "mingrade")
			RUNTIME_DATA_PROPERTY(Int32, MaxGrade, "maxgrade")
		RUNTIME_DATA_TYPE_END_CHILD(StellarLineGrade, 5)
	
	RUNTIME_DATA_TYPE_END_CHILD(StellarLine, 5)

RUNTIME_DATA_TYPE_END(StellarGroup)
RUNTIME_DATA_TYPE_INDEX(StellarGroup, Int32, GroupID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarGroup, StellarLine, Int32, LineID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarLine, StellarLineGrade, Int32, Grade)


RUNTIME_DATA_FILE_END