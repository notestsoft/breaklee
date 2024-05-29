local CSC_SPECIAL_GIFTBOX_ROLL = 2797

RegisterPacketLayout("c2s_special_giftbox_roll", {
	u8("index"),
})

RegisterPacketLayout("s2c_special_giftbox_roll_item", {
    u8("index"),
	u64("item_id"),
	u64("item_options"),
})

RegisterPacketLayout("s2c_special_giftbox_roll", {
	u8("count"),
	vec("items", "s2c_special_giftbox_roll_item", "count")
})

RegisterPacketHandler(CSC_SPECIAL_GIFTBOX_ROLL, "c2s_special_giftbox_roll", function (packet)
	Unicast(CSC_SPECIAL_GIFTBOX_ROLL, "s2c_special_giftbox_roll", {
        items = {
            { index = packet.index, item_id = 1, item_options = 0 }
        }
    })
end)
