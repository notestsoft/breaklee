RUNTIME_DATA_FILE_BEGIN(Const.enc)

RUNTIME_DATA_TYPE_BEGIN(HonorLevelFormula, "Const.cabal_reputation.reputation")
    RUNTIME_DATA_PROPERTY(Int32, Rank, "class")
    RUNTIME_DATA_PROPERTY(Int64, MinPoint, "min_reputation")
    RUNTIME_DATA_PROPERTY(Int64, MaxPoint, "promote_reputation")
RUNTIME_DATA_TYPE_END(HonorLevelFormula)
RUNTIME_DATA_TYPE_INDEX(HonorLevelFormula, Int32, Rank)

RUNTIME_DATA_FILE_END
