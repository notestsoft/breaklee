RUNTIME_DATA_FILE_BEGIN(Event.xml)

RUNTIME_DATA_TYPE_BEGIN(Event, "events.event", 32)
	RUNTIME_DATA_PROPERTY(Int32, ID, "id")
	RUNTIME_DATA_PROPERTY(Int32, Type, "type")
	RUNTIME_DATA_PROPERTY(Int32, UseFlags, "use_flags")
	RUNTIME_DATA_PROPERTY(Int32, WorldIndex, "world_id")
	RUNTIME_DATA_PROPERTY(Int32, NpcIndex, "npc_id")
	RUNTIME_DATA_PROPERTY_ARRAY(Char, Description, "description", 260, '\0')

	RUNTIME_DATA_TYPE_BEGIN_CHILD(EventDescription, "event_description", 8)
		RUNTIME_DATA_PROPERTY_ARRAY(Char, Description, "content", 260, '\0')
	RUNTIME_DATA_TYPE_END_CHILD(EventDescription, 8)

	RUNTIME_DATA_TYPE_BEGIN_CHILD(EventShop, "event_shop", 4)
		RUNTIME_DATA_TYPE_BEGIN_CHILD(EventShopItem, "item", 128)
			RUNTIME_DATA_PROPERTY(Int32, SlotIndex, "slot_id")
			RUNTIME_DATA_PROPERTY(UInt64, ItemID, "item_id")
			RUNTIME_DATA_PROPERTY(UInt64, ItemOptions, "option")
			RUNTIME_DATA_PROPERTY(UInt8, ItemDurationID, "duration_id")
			RUNTIME_DATA_PROPERTY(UInt64, ItemPrice, "price")
		RUNTIME_DATA_TYPE_END_CHILD(EventShopItem, 128)
	RUNTIME_DATA_TYPE_END_CHILD(EventShop, 4)

	RUNTIME_DATA_TYPE_BEGIN_CHILD(EventItem, "event_item", 8)
		RUNTIME_DATA_PROPERTY(UInt64, ItemID, "item_id")
		RUNTIME_DATA_PROPERTY(UInt64, ItemOptions, "option")
		RUNTIME_DATA_PROPERTY(Int32, ExternalID, "external_id")
		RUNTIME_DATA_PROPERTY(UInt32, TextureItemID, "texture_item_id")
		RUNTIME_DATA_PROPERTY_ARRAY(Char, Title, "title", 260, '\0')
		RUNTIME_DATA_PROPERTY_ARRAY(Char, Description, "description", 260, '\0')
	RUNTIME_DATA_TYPE_END_CHILD(EventItem, 8)

RUNTIME_DATA_TYPE_END(Event)
RUNTIME_DATA_TYPE_INDEX(Event, Int32, ID)
RUNTIME_DATA_TYPE_INDEX_CHILD(EventShop, EventShopItem, Int32, SlotIndex)

RUNTIME_DATA_FILE_END
