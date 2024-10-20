RUNTIME_DATA_FILE_BEGIN(DIR_PATH(Loot, MeritItemPool.xml))

RUNTIME_DATA_TYPE_BEGIN(MeritItemPool, "MeritItemPool.Pool")
	RUNTIME_DATA_PROPERTY(Int32, Type, "Type")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(MeritItemPoolGroup, "ItemGroup")
		RUNTIME_DATA_PROPERTY(Int32, EvaluationCount, "EvaluationCount")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(MeritItemPoolGroupItem, "Item")
			RUNTIME_DATA_PROPERTY_ARRAY(UInt64, ItemIDs, "ItemIDs", 3, ',')
			RUNTIME_DATA_PROPERTY(Int32, Rate, "Rate")
		RUNTIME_DATA_TYPE_END_CHILD(MeritItemPoolGroupItem)
	RUNTIME_DATA_TYPE_END_CHILD(MeritItemPoolGroup)
RUNTIME_DATA_TYPE_END(MeritItemPool)
RUNTIME_DATA_TYPE_INDEX(MeritItemPool, Int32, Type)
RUNTIME_DATA_TYPE_INDEX_CHILD(MeritItemPool, MeritItemPoolGroup, Int32, EvaluationCount)

RUNTIME_DATA_FILE_END
