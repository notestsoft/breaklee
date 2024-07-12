RUNTIME_DATA_FILE_BEGIN(animamastery.enc)

RUNTIME_DATA_TYPE_BEGIN(AnimaMasteryList, "animamastery.animamastery_list.animamastery", 128)
	RUNTIME_DATA_PROPERTY(Int32, SectionIndex, "section_type")
	RUNTIME_DATA_PROPERTY(Int32, SlotIndex, "order")
	RUNTIME_DATA_PROPERTY(Int32, ForceEffectIndex, "force_id")
	RUNTIME_DATA_PROPERTY(Int32, ForceValueType, "value_type")
	RUNTIME_DATA_PROPERTY(Int32, ForceValue, "value")
RUNTIME_DATA_TYPE_END(AnimaMasteryList)

RUNTIME_DATA_TYPE_BEGIN(AnimaMasteryPrice, "animamastery.price.Price", 4)
	RUNTIME_DATA_PROPERTY(Int32, SectionIndex, "section_type")
	RUNTIME_DATA_PROPERTY(UInt64, OpenPrice, "open_alz")
	RUNTIME_DATA_PROPERTY(UInt64, SlotPrice, "slot_alz")
	RUNTIME_DATA_PROPERTY(UInt64, RequiredItemID, "need_item_id")
	RUNTIME_DATA_PROPERTY(Int32, RequiredItemCount, "need_value")
RUNTIME_DATA_TYPE_END(AnimaMasteryPrice)
RUNTIME_DATA_TYPE_INDEX(AnimaMasteryPrice, Int32, SectionIndex)

RUNTIME_DATA_FILE_END