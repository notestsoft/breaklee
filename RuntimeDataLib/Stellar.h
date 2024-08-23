RUNTIME_DATA_FILE_BEGIN(stellar.enc)

RUNTIME_DATA_TYPE_BEGIN(StellarGroup, "stellar_ability.stellar_main.stellar_info", 1)
	RUNTIME_DATA_PROPERTY(UInt32, GroupID, "group_id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarLine, "detail", 5)
		RUNTIME_DATA_PROPERTY(UInt32, LineID, "line_no")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarLineGrade, "group", 5)
			RUNTIME_DATA_PROPERTY(UInt32, Grade, "grade")
			RUNTIME_DATA_PROPERTY(UInt32, ForceCodePer, "forcecode_per") // Reference to StellarForcePool.PoolID
			RUNTIME_DATA_PROPERTY(UInt32, ItemID, "Itemkind")
			RUNTIME_DATA_PROPERTY(UInt32, ItemCount, "itemcount")
			RUNTIME_DATA_PROPERTY(UInt32, MinGrade, "mingrade")
			RUNTIME_DATA_PROPERTY(UInt32, MaxGrade, "maxgrade")
		RUNTIME_DATA_TYPE_END_CHILD(StellarLineGrade, 5)
	RUNTIME_DATA_TYPE_END_CHILD(StellarLine, 5)
RUNTIME_DATA_TYPE_END(StellarGroup)
RUNTIME_DATA_TYPE_INDEX(StellarGroup, UInt32, GroupID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarGroup, StellarLine, UInt32, LineID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarLine, StellarLineGrade, UInt32, Grade)

RUNTIME_DATA_TYPE_BEGIN(StellarTransformationGroup, "stellar_ability.fixed_grade.fixed_info", 1)
	RUNTIME_DATA_PROPERTY(UInt32, GroupID, "group_id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarLineTransformation, "detail", 5)
		RUNTIME_DATA_PROPERTY(UInt32, LineID, "line_no")
		RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarLineTransformationGrade, "group", 5)
			RUNTIME_DATA_PROPERTY(UInt32, Grade, "grade")
			RUNTIME_DATA_PROPERTY(UInt32, ItemID, "Itemkind")
			RUNTIME_DATA_PROPERTY(UInt32, ItemCount, "itemcount")
		RUNTIME_DATA_TYPE_END_CHILD(StellarLineTransformationGrade, 5)
	RUNTIME_DATA_TYPE_END_CHILD(StellarLineTransformation, 5)
RUNTIME_DATA_TYPE_END(StellarTransformationGroup)
RUNTIME_DATA_TYPE_INDEX(StellarTransformationGroup, UInt32, GroupID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarTransformationGroup, StellarLineTransformation, UInt32, LineID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarLineTransformation, StellarLineTransformationGrade, UInt32, Grade)

RUNTIME_DATA_FILE_END


RUNTIME_DATA_FILE_BEGIN(Stellar.xml)

RUNTIME_DATA_TYPE_BEGIN(StellarForcePool, "Stellar.StellarForcePools.StellarForcePool", 25)
	RUNTIME_DATA_PROPERTY(Int32, PoolID, "id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarForceEffect, "StellarForceEffect", 12)
		RUNTIME_DATA_PROPERTY(UInt32, Chance, "chance")
		RUNTIME_DATA_PROPERTY(UInt32, ForceEffectID, "forceEffectId")
		RUNTIME_DATA_PROPERTY(UInt32, Value, "value")
		RUNTIME_DATA_PROPERTY(UInt32, ValueType, "valueType")
	RUNTIME_DATA_TYPE_END_CHILD(StellarForceEffect, 12)
RUNTIME_DATA_TYPE_END(StellarForcePool)
RUNTIME_DATA_TYPE_INDEX(StellarForcePool, Int32, PoolID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarForcePool, StellarForceEffect, UInt32, ForceEffectID)

RUNTIME_DATA_TYPE_BEGIN(StellarForceValuePool, "Stellar.StellarForceValuePools.StellarForceValuePool", 5)
	RUNTIME_DATA_PROPERTY(Int32, PoolID, "id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarForceValue, "StellarForceValue", 5)
		RUNTIME_DATA_PROPERTY(UInt32, Chance, "chance")
		RUNTIME_DATA_PROPERTY(UInt32, StellarForcePoolID, "stellarForcePoolId")
	RUNTIME_DATA_TYPE_END_CHILD(StellarForceValue, 5)
RUNTIME_DATA_TYPE_END(StellarForceValuePool)
RUNTIME_DATA_TYPE_INDEX(StellarForceValuePool, Int32, PoolID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarForceValuePool, StellarForceValue, UInt32, StellarForcePoolID)

RUNTIME_DATA_TYPE_BEGIN(StellarLinkPool, "Stellar.StellarLinkPools.StellarLinkPool", 5)
	RUNTIME_DATA_PROPERTY(UInt32, PoolID, "id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(StellarLink, "StellarLink", 5)
		RUNTIME_DATA_PROPERTY(UInt32, Grade, "grade")
		RUNTIME_DATA_PROPERTY(UInt32, Chance, "chance")
	RUNTIME_DATA_TYPE_END_CHILD(StellarLink, 5)
RUNTIME_DATA_TYPE_END(StellarLinkPool)
RUNTIME_DATA_TYPE_INDEX(StellarLinkPool, UInt32, PoolID)
RUNTIME_DATA_TYPE_INDEX_CHILD(StellarLinkPool, StellarLink, UInt32, Grade)

RUNTIME_DATA_FILE_END
