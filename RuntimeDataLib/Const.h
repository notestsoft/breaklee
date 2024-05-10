RUNTIME_DATA_FILE_BEGIN(Const.enc)

RUNTIME_DATA_TYPE_BEGIN(HonorLevelFormula, "Const.cabal_reputation.reputation", 64)
    RUNTIME_DATA_PROPERTY(Int32, Rank, "class")
    RUNTIME_DATA_PROPERTY(Int64, MinPoint, "min_reputation")
    RUNTIME_DATA_PROPERTY(Int64, MaxPoint, "promote_reputation")
RUNTIME_DATA_TYPE_END(HonorLevelFormula)
RUNTIME_DATA_TYPE_INDEX(HonorLevelFormula, Int32, Rank)

RUNTIME_DATA_FILE_END

RUNTIME_DATA_FILE_BEGIN(Const.xml)

RUNTIME_DATA_TYPE_BEGIN(SpiritPointLimit, "constants.sp_limit.mastery_level", 32)
    RUNTIME_DATA_PROPERTY(Int32, Level, "level")
    RUNTIME_DATA_PROPERTY(Int32, Value, "value")
RUNTIME_DATA_TYPE_END(SpiritPointLimit)
RUNTIME_DATA_TYPE_INDEX(SpiritPointLimit, Int32, Level)

RUNTIME_DATA_TYPE_BEGIN(RageLimit, "constants.rage_limit.mastery_style", 16)
    RUNTIME_DATA_PROPERTY(Int32, Style, "style")
    RUNTIME_DATA_TYPE_BEGIN_CHILD(RageLimitLevel, "mastery_level", 32)
        RUNTIME_DATA_PROPERTY(Int32, Level, "level")
        RUNTIME_DATA_PROPERTY(Int32, Value, "value")
    RUNTIME_DATA_TYPE_END_CHILD(RageLimitLevel, 32)
RUNTIME_DATA_TYPE_END(RageLimit)
RUNTIME_DATA_TYPE_INDEX(RageLimit, Int32, Style)
RUNTIME_DATA_TYPE_INDEX_CHILD(RageLimit, RageLimitLevel, Int32, Level)

RUNTIME_DATA_FILE_END
