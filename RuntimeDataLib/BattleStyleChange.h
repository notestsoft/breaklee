RUNTIME_DATA_FILE_BEGIN(BattleStyleChange.enc)

RUNTIME_DATA_TYPE_BEGIN(BattleStyleChangeLevelLimit, "BattleStyleChange.limitlevel.limit")
	RUNTIME_DATA_PROPERTY(Int32, Index, "Index")
	RUNTIME_DATA_PROPERTY(Int32, Minlevel, "Minlevel")
	RUNTIME_DATA_PROPERTY(Int32, Maxlevel, "Maxlevel")
	RUNTIME_DATA_PROPERTY(Int32, MaxOverlordLevel, "MaxOlv")
RUNTIME_DATA_TYPE_END(BattleStyleChangeLevelLimit)

RUNTIME_DATA_TYPE_BEGIN(BattleStyleChangeBattleStyle, "BattleStyleChange.battlestyle_list.battlestyle")
	RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_PROPERTY(Int32, ListOn, "list_on")
	RUNTIME_DATA_PROPERTY(Int32, Type, "type")
	RUNTIME_DATA_PROPERTY(Int32, Battle1, "battle1")
	RUNTIME_DATA_PROPERTY(Int32, Battle2, "battle2")
	RUNTIME_DATA_PROPERTY(Int32, Battle3, "battle3")
RUNTIME_DATA_TYPE_END(BattleStyleChangeBattleStyle)

RUNTIME_DATA_TYPE_BEGIN(BattleStyleChangePassiveAbility, "BattleStyleChange.passive_ability.passive.ablity")
	RUNTIME_DATA_PROPERTY(Int32, Index, "index")
	RUNTIME_DATA_PROPERTY(Int32, Type1, "type1")
	RUNTIME_DATA_PROPERTY(Int32, Type2, "type2")
RUNTIME_DATA_TYPE_END(BattleStyleChangePassiveAbility)

RUNTIME_DATA_TYPE_BEGIN(BattleStyleChangeKarmaAbility, "BattleStyleChange.passive_ability.karma_passive.ablity")
	RUNTIME_DATA_PROPERTY(Int32, Index, "index")
	RUNTIME_DATA_PROPERTY(Int32, Type1, "type1")
	RUNTIME_DATA_PROPERTY(Int32, Type2, "type2")
RUNTIME_DATA_TYPE_END(BattleStyleChangeKarmaAbility)

RUNTIME_DATA_TYPE_BEGIN(BattleStyleChangeRateAbility, "BattleStyleChange.rate_ability.rate.ablity")
	RUNTIME_DATA_PROPERTY(Int32, Index, "index")
	RUNTIME_DATA_PROPERTY(Int32, Type1, "type1")
	RUNTIME_DATA_PROPERTY(Int32, Type2, "type2")
RUNTIME_DATA_TYPE_END(BattleStyleChangeRateAbility)

RUNTIME_DATA_FILE_END