RUNTIME_DATA_FILE_BEGIN(DIR_PATH(War, War.xml))

RUNTIME_DATA_TYPE_BEGIN(WarMap, "InstantWar.war.war_map")
	RUNTIME_DATA_PROPERTY(Int32, Id, "warmap_id")
	RUNTIME_DATA_PROPERTY(Int32, WorldId, "world_id")
	RUNTIME_DATA_PROPERTY(Int32, MinLv, "minlv")
	RUNTIME_DATA_PROPERTY(Int32, MaxLv, "maxlv")
	RUNTIME_DATA_PROPERTY(Int32, WarType, "war_type")
	RUNTIME_DATA_PROPERTY(Int32, LobbyWorldId, "lobby_world_id")
	RUNTIME_DATA_PROPERTY(Int32, MaxUser, "max_user")
	RUNTIME_DATA_PROPERTY(Int32, LobbyTime, "lobby_time")
	RUNTIME_DATA_PROPERTY(Int32, BfTime, "bf_time")
	RUNTIME_DATA_PROPERTY(Int32, EntryExpenses, "entry_expenses")

RUNTIME_DATA_TYPE_END(WarMap)

RUNTIME_DATA_TYPE_INDEX(WarMap, Int32, Id)

RUNTIME_DATA_FILE_END
