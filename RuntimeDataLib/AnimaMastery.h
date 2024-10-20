RUNTIME_DATA_FILE_BEGIN(animamastery.enc)

RUNTIME_DATA_TYPE_BEGIN(AnimaMasteryValue, "animamastery.animamastery_list.animamastery")
	RUNTIME_DATA_PROPERTY(Int32, CategoryIndex, "section_type")
	RUNTIME_DATA_PROPERTY(Int32, ForceEffectOrder, "order")
	RUNTIME_DATA_PROPERTY(Int32, ForceEffectIndex, "force_id")
	RUNTIME_DATA_PROPERTY(Int32, ForceValueType, "value_type")
	RUNTIME_DATA_PROPERTY(Int32, ForceValue, "value")
RUNTIME_DATA_TYPE_END(AnimaMasteryValue)

RUNTIME_DATA_TYPE_BEGIN(AnimaMasteryPrice, "animamastery.price.Price")
	RUNTIME_DATA_PROPERTY(Int32, CategoryIndex, "section_type")
	RUNTIME_DATA_PROPERTY(UInt64, OpenPrice, "open_alz")
	RUNTIME_DATA_PROPERTY(UInt64, SlotPrice, "slot_alz")
	RUNTIME_DATA_PROPERTY(UInt64, RequiredItemID, "need_item_id")
	RUNTIME_DATA_PROPERTY(Int32, RequiredItemCount, "need_value")
RUNTIME_DATA_TYPE_END(AnimaMasteryPrice)
RUNTIME_DATA_TYPE_INDEX(AnimaMasteryPrice, Int32, CategoryIndex)

RUNTIME_DATA_FILE_END