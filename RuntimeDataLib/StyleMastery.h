RUNTIME_DATA_FILE_BEGIN(DIR_PATH(Character, StyleMastery.xml))

RUNTIME_DATA_TYPE_BEGIN(SpiritPointLimit, "StyleMastery.SpiritPointLimit.Level")
	RUNTIME_DATA_PROPERTY(Int32, Level, "Level")
	RUNTIME_DATA_PROPERTY(Int32, Value, "Value")
RUNTIME_DATA_TYPE_END(SpiritPointLimit)
RUNTIME_DATA_TYPE_INDEX(SpiritPointLimit, Int32, Level)

RUNTIME_DATA_TYPE_BEGIN(RageLimit, "StyleMastery.RagePointLimit.Style")
	RUNTIME_DATA_PROPERTY(Int32, Style, "Index")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(RageLimitLevel, "Level")
		RUNTIME_DATA_PROPERTY(Int32, Level, "Level")
		RUNTIME_DATA_PROPERTY(Int32, Value, "Value")
	RUNTIME_DATA_TYPE_END_CHILD(RageLimitLevel)
RUNTIME_DATA_TYPE_END(RageLimit)
RUNTIME_DATA_TYPE_INDEX(RageLimit, Int32, Style)
RUNTIME_DATA_TYPE_INDEX_CHILD(RageLimit, RageLimitLevel, Int32, Level)

RUNTIME_DATA_FILE_END
