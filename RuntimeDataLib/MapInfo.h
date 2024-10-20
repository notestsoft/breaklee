RUNTIME_DATA_FILE_BEGIN(mapinfo.enc)

RUNTIME_DATA_TYPE_BEGIN(MapCode, "cabal.mapcode_guide.guide")
	RUNTIME_DATA_PROPERTY(Int32, EntryLevel, "entry_lv")
	RUNTIME_DATA_PROPERTY(Int32, WorldID, "world_id")
	RUNTIME_DATA_PROPERTY(Int32, WarpIndex, "warp_index")
RUNTIME_DATA_TYPE_END(MapCode)
RUNTIME_DATA_TYPE_INDEX(MapCode, Int32, EntryLevel)

RUNTIME_DATA_FILE_END