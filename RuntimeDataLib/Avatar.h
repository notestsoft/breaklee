RUNTIME_DATA_FILE_BEGIN(avatar.enc)

RUNTIME_DATA_TYPE_BEGIN(Avatar, "cabal_avatar.avatar")
	RUNTIME_DATA_PROPERTY(Int32, Type, "type")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(AvatarItem, "avatar_item")
		RUNTIME_DATA_PROPERTY(Int32, ID, "id")
		RUNTIME_DATA_PROPERTY(Int32, ArmorItemID, "armor_item_id")
		RUNTIME_DATA_PROPERTY(Int32, ClassType, "class_type")
		RUNTIME_DATA_PROPERTY(Int32, AstralID, "astral_id")
	RUNTIME_DATA_TYPE_END_CHILD(AvatarItem)
RUNTIME_DATA_TYPE_END(Avatar)

RUNTIME_DATA_TYPE_BEGIN(AvatarAstral, "cabal_avatar.avatar_astral.avatar_astral_fx")
	RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_PROPERTY(Int32, FxCodeOnBow, "fxcode_on_bow")
	RUNTIME_DATA_PROPERTY(Int32, FxCodeOnShield, "fxcode_on_shield")
	RUNTIME_DATA_PROPERTY(Int32, FxCodeKeepBow, "fxcode_keep_bow")
	RUNTIME_DATA_PROPERTY(Int32, FxCodeKeepShield, "fxcode_keep_shield")
	RUNTIME_DATA_PROPERTY(Int32, FxCodeOnGun, "fxcode_on_gun")
	RUNTIME_DATA_PROPERTY(Int32, FxCodeKeepGun, "fxcode_keep_gun")
RUNTIME_DATA_TYPE_END(AvatarAstral)

RUNTIME_DATA_TYPE_BEGIN(AvatarBike, "cabal_avatar.avatar_bike.avatar_bike_data")
	RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_PROPERTY(Int32, IdleAnimationCode, "idle_ani_code")
	RUNTIME_DATA_PROPERTY(Int32, PreAnimationSync, "pre_anim_sync")
	RUNTIME_DATA_PROPERTY(Int32, FxType, "fx_type")
RUNTIME_DATA_TYPE_END(AvatarBike)

RUNTIME_DATA_TYPE_BEGIN(AvatarForceWingCostume, "cabal_avatar.ForceWing_Costume.Wing_Costume")
	RUNTIME_DATA_PROPERTY(Int32, ID, "ID")
RUNTIME_DATA_TYPE_END(AvatarForceWingCostume)

RUNTIME_DATA_FILE_END