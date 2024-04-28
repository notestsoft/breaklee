RUNTIME_DATA_FILE_BEGIN(destroy.enc)

RUNTIME_DATA_TYPE_BEGIN(DestroyItem, "destroy.list.destroy_list", 32768)
	RUNTIME_DATA_PROPERTY(UInt32, ItemIndex, "item_index")
	RUNTIME_DATA_PROPERTY(Int32, MinEnchantLevel, "min_enchant_level")
	RUNTIME_DATA_PROPERTY(Int32, MaxEnchantLevel, "max_enchant_level")
	RUNTIME_DATA_PROPERTY(Int32, Slot, "slot")
	RUNTIME_DATA_PROPERTY(Int32, Epic, "epic")
	RUNTIME_DATA_PROPERTY_ARRAY(Int32, EpicLevel, "epic_level", 3, ',')
	RUNTIME_DATA_PROPERTY_ARRAY(Int32, EpicOption, "epic_option", 12, ',')
	RUNTIME_DATA_PROPERTY(Int32, AccountBind, "account_bind")
	RUNTIME_DATA_PROPERTY(Int32, CharacterBind, "character_bind")
	RUNTIME_DATA_PROPERTY(Int32, PoolID, "pool_id")
	RUNTIME_DATA_PROPERTY(Int32, Alert, "alert")
RUNTIME_DATA_TYPE_END(DestroyItem)

RUNTIME_DATA_TYPE_BEGIN(DestroyItemPool, "destroy.pool.destroy_pool", 1024)
	RUNTIME_DATA_PROPERTY(Int32, PoolID, "pool_id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(DestroyItemPoolResult, "result", 4)
		RUNTIME_DATA_PROPERTY(UInt32, ItemKind, "item_kind")
		RUNTIME_DATA_PROPERTY(Int32, MinCount, "min_count")
		RUNTIME_DATA_PROPERTY(Int32, MaxCount, "max_count")
	RUNTIME_DATA_TYPE_END_CHILD(DestroyItemPoolResult, 4)
RUNTIME_DATA_TYPE_END(DestroyItemPool)
RUNTIME_DATA_TYPE_INDEX(DestroyItemPool, UInt32, PoolID)

RUNTIME_DATA_FILE_END

RUNTIME_DATA_FILE_BEGIN(DestroyOptionPool.xml)

RUNTIME_DATA_TYPE_BEGIN(DestroyItemOptionPool, "DestroyOptionPool.Pool", 32)
	RUNTIME_DATA_PROPERTY(UInt64, ItemID, "item_id")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(DestroyItemOptionPoolOption, "Option", 32)
		RUNTIME_DATA_PROPERTY(UInt64, Option, "option")
		RUNTIME_DATA_PROPERTY(Int32, Rate, "rate")
	RUNTIME_DATA_TYPE_END_CHILD(DestroyItemOptionPoolOption, 32)
RUNTIME_DATA_TYPE_END(DestroyItemOptionPool)
RUNTIME_DATA_TYPE_INDEX(DestroyItemOptionPool, UInt64, ItemID)

RUNTIME_DATA_FILE_END
