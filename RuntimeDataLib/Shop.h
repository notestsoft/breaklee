RUNTIME_DATA_FILE_BEGIN(ShopList.xml)

RUNTIME_DATA_TYPE_BEGIN(ShopIndex, "ShopList.Shop", 128)
	RUNTIME_DATA_PROPERTY(Int32, WorldIndex, "WorldIndex")
	RUNTIME_DATA_PROPERTY(Int32, NpcIndex, "NpcIndex")
	RUNTIME_DATA_PROPERTY(Int32, ShopIndex, "ShopIndex")
	RUNTIME_DATA_PROPERTY(Int32, IsCouponShop, "IsCouponShop")
RUNTIME_DATA_TYPE_END(ShopIndex)

RUNTIME_DATA_FILE_END

RUNTIME_DATA_FILE_BEGIN(ShopPool.xml)

RUNTIME_DATA_TYPE_BEGIN(ShopPool, "ShopPool.Shop", 64)
	RUNTIME_DATA_PROPERTY(Int32, ShopIndex, "ShopIndex")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(ShopItem, "Item", 128)
		RUNTIME_DATA_PROPERTY(Int32, TabIndex, "TabIndex")
		RUNTIME_DATA_PROPERTY(Int32, SlotIndex, "SlotIndex")
		RUNTIME_DATA_PROPERTY(UInt64, ItemID, "ItemID")
		RUNTIME_DATA_PROPERTY(UInt64, ItemOptions, "ItemOption")
		RUNTIME_DATA_PROPERTY(UInt32, ItemDuration, "ItemDuration")
		RUNTIME_DATA_PROPERTY(Int32, MinLevel, "MinLevel")
		RUNTIME_DATA_PROPERTY(Int32, MaxLevel, "MaxLevel")
		RUNTIME_DATA_PROPERTY(Int32, MinGuildLevel, "MinGuildLevel")
		RUNTIME_DATA_PROPERTY(Int32, MinHonorRank, "MinHonorRank")
		RUNTIME_DATA_PROPERTY(Int32, MaxHonorRank, "MaxHonorRank")
		RUNTIME_DATA_PROPERTY(Int32, IsPremiumOnly, "IsPremiumOnly")
		RUNTIME_DATA_PROPERTY(Int32, IsWinningOnly, "IsWinningOnly")
		RUNTIME_DATA_PROPERTY(Int32, DailyPurchaseLimit, "DailyPurchaseLimit")
		RUNTIME_DATA_PROPERTY(Int32, WeeklyPurchaseLimit, "WeeklyPurchaseLimit")
		RUNTIME_DATA_PROPERTY(Int64, PriceAlz, "PriceAlz")
		RUNTIME_DATA_PROPERTY(Int64, PriceWexp, "PriceWexp")
		RUNTIME_DATA_PROPERTY(Int64, PriceAP, "PriceAP")
		RUNTIME_DATA_PROPERTY(Int32, PriceDP, "PriceDP")
		RUNTIME_DATA_PROPERTY(Int32, PriceCash, "PriceCash")
		RUNTIME_DATA_PROPERTY(Int32, PriceGem, "PriceGem")
		RUNTIME_DATA_PROPERTY(Int32, ShopPricePoolIndex, "ShopPricePoolIndex")
	RUNTIME_DATA_TYPE_END_CHILD(ShopItem, 128)
RUNTIME_DATA_TYPE_END(ShopPool)
RUNTIME_DATA_TYPE_INDEX(ShopPool, Int32, ShopIndex)

RUNTIME_DATA_FILE_END

RUNTIME_DATA_FILE_BEGIN(ShopPricePool.xml)

RUNTIME_DATA_TYPE_BEGIN(ShopPricePool, "ShopPricePool.ItemPrice", 512)
	RUNTIME_DATA_PROPERTY(Int32, PoolIndex, "PoolIndex")
	RUNTIME_DATA_PROPERTY(UInt64, ItemID, "ItemID")
	RUNTIME_DATA_PROPERTY(UInt64, ItemOptions, "ItemOptions")
	RUNTIME_DATA_PROPERTY(Int32, ItemCount, "ItemCount")
RUNTIME_DATA_TYPE_END(ShopPricePool)

RUNTIME_DATA_FILE_END
