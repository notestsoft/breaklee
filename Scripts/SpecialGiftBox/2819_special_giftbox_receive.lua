local CSC_SPECIAL_GIFTBOX_RECEIVE = 2819

RegisterPacketLayout("c2s_special_giftbox_receive_item", {
	u16("inventory_slot_index"),
	u16("stack_size"),
})

RegisterPacketLayout("c2s_special_giftbox_receive", {
	u8("index"),
    u16("count"),
    vec("items", "c2s_special_giftbox_receive_item", "count")
})

RegisterPacketLayout("s2c_special_giftbox_receive_item", {
	u64("item_id"),
	u64("item_options"),
    u16("inventory_slot_index"),
    u32("unknown")
})

RegisterPacketLayout("s2c_special_giftbox_receive_slot", {
    u8("index"),
    u64("elapsed_time"),
    u64("total_time"),
    u8("item_count"),
    vec("items", "s2c_special_giftbox_receive_item", "item_count")
})

RegisterPacketLayout("s2c_special_giftbox_receive", {
	u8("count"),
	vec("slots", "s2c_special_giftbox_receive_slot", "count")
})

RegisterPacketHandler(CSC_SPECIAL_GIFTBOX_RECEIVE, "c2s_special_giftbox_receive", function (packet)
	Unicast(CSC_SPECIAL_GIFTBOX_RECEIVE, "s2c_special_giftbox_receive", {
        slots = {
            { 
                index = packet.index,
                elapsed_time = 0,
                total_time = 3600,
                items = {
                    { 
                        item_id = 1, 
                        item_options = 0, 
                        inventory_slot_index = packet.items[1].inventory_slot_index, 
                        unknown = 0 
                    }
                }
             }
        }
    })
end)
