RUNTIME_DATA_FILE_BEGIN(belongitem.enc)

RUNTIME_DATA_TYPE_BEGIN(BelongItem, "belongitem.belong_item_info.belong_item")
	RUNTIME_DATA_PROPERTY(Int32, ItemID, "item_id")
	RUNTIME_DATA_PROPERTY(Int32, ItemOption, "option")
	RUNTIME_DATA_PROPERTY(Int32, PossessionType, "possession_type")
	RUNTIME_DATA_PROPERTY(Int32, SellType, "sell_type")
RUNTIME_DATA_TYPE_END(BelongItem)

RUNTIME_DATA_FILE_END