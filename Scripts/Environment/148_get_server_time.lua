local CSC_GET_SERVER_TIME = 148

RegisterPacketLayout("c2s_get_server_time", {
})

RegisterPacketLayout("s2c_get_server_time", {
	u64("timestamp"),
    i16("timezone")
})

RegisterPacketLayout("s2c_nfy_error", {
    u16("error_command"),
    u16("error_sub_command"),
    u16("error_code"),
    u16("dead_type")
})

RegisterPacketHandler(CSC_GET_SERVER_TIME, "c2s_get_server_time", function (packet)
    local timestamp = os.time(os.date("!*t"))

	Unicast(CSC_GET_SERVER_TIME, "s2c_get_server_time", {
        timestamp = timestamp,
        timezone = 0 -- TODO: Add correct timezone calculation
    })
end)

