#ifndef CONFIG_BEGIN
#define CONFIG_BEGIN(__NAMESPACE__)
#endif

#ifndef CONFIG_PARAMETER
#define CONFIG_PARAMETER(__TYPE__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_PARAMETER_ARRAY
#define CONFIG_PARAMETER_ARRAY(__TYPE__, __LENGTH__, __NAME__, __PATH__, __DEFAULT__)
#endif

#ifndef CONFIG_END
#define CONFIG_END(__NAMESPACE__)
#endif

CONFIG_BEGIN(WorldSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "WorldSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "WorldSvr.Port", 38121)
CONFIG_PARAMETER(Int32, MaxConnectionCount, "WorldSvr.MaxConnectionCount", 256)
CONFIG_PARAMETER(UInt8, WorldID, "WorldSvr.WorldID", 1)
CONFIG_PARAMETER(UInt32, WorldType, "WorldSvr.WorldType", 0x0400)
CONFIG_PARAMETER(UInt8, MaxSubpasswordFailureCount, "WorldSvr.MaxSubpasswordFailureCount", 5)
CONFIG_PARAMETER(UInt64, SubpasswordBanDuration, "WorldSvr.SubpasswordBanDuration", 0)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, RuntimeDataPath, "WorldSvr.RuntimeDataPath", Data)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, ServerDataPath, "WorldSvr.ServerDataPath", ServerData)
CONFIG_PARAMETER(UInt32, MinRollDiceValue, "WorldSvr.MinRollDiceValue", 0)
CONFIG_PARAMETER(UInt32, MaxRollDiceValue, "WorldSvr.MaxRollDiceValue", 999)
CONFIG_PARAMETER(UInt64, ExpMultiplier, "WorldSvr.ExpMultiplier", 500)
CONFIG_PARAMETER(UInt64, SkillExpMultiplier, "WorldSvr.SkillExpMultiplier", 500)
CONFIG_PARAMETER(UInt32, DebugCharacter, "WorldSvr.DebugCharacter", 1)
CONFIG_PARAMETER(Bool, DebugSetQuestFlags, "WorldSvr.DebugSetQuestFlags", 0)
CONFIG_PARAMETER(Bool, DBSyncTimerPriorityLow, "WorldSvr.DBSyncTimerPriorityLow", 1000)
CONFIG_PARAMETER(Bool, DBSyncTimerPriorityHigh, "WorldSvr.DBSyncTimerPriorityHigh", 1000)
CONFIG_END(WorldSvr)

CONFIG_BEGIN(Environment)
CONFIG_PARAMETER(UInt16, MaxLevel, "Environment.MaxLevel", 200)
CONFIG_PARAMETER(UInt8, DummyEnabled, "Environment.DummyEnabled", 1)
CONFIG_PARAMETER(UInt8, CashshopEnabled, "Environment.CashshopEnabled", 1)
CONFIG_PARAMETER(UInt8, NetCafePointEnabled, "Environment.NetCafePointEnabled", 0)
CONFIG_PARAMETER(UInt16, MinChatLevel, "Environment.MinChatLevel", 10)
CONFIG_PARAMETER(UInt16, MinShoutLevel, "Environment.MinShoutLevel", 40)
CONFIG_PARAMETER(UInt16, MinShoutSkillRank, "Environment.MinShoutSkillRank", 5)
CONFIG_PARAMETER(UInt64, MaxInventoryCurrency, "Environment.MaxInventoryCurrency", 999999999999)
CONFIG_PARAMETER(UInt64, MaxWarehouseCurrency, "Environment.MaxWarehouseCurrency", 999999999999)
CONFIG_PARAMETER(UInt64, MaxAuctionCurrency, "Environment.MaxAuctionCurrency", 999999999999)
CONFIG_PARAMETER(Int64, MaxHonorPoint, "Environment.MaxHonorPoint", 90000000000)
CONFIG_PARAMETER(Int64, MinHonorPoint, "Environment.MinHonorPoint", -2147483648)
CONFIG_END(Environment)

CONFIG_BEGIN(MasterSvr)
CONFIG_PARAMETER_ARRAY(Char, MAX_PATH, Host, "MasterSvr.Host", 127.0.0.1)
CONFIG_PARAMETER(UInt16, Port, "MasterSvr.Port", 38181)
CONFIG_END(MasterSvr)

CONFIG_BEGIN(NetLib)
CONFIG_PARAMETER(UInt16, ProtocolIdentifier, "NetLib.ProtocolIdentifier", 0xB7D9)
CONFIG_PARAMETER(UInt16, ProtocolVersion, "NetLib.ProtocolVersion", 0x0009)
CONFIG_PARAMETER(UInt16, ProtocolExtension, "NetLib.ProtocolExtension", 0x1111)
CONFIG_PARAMETER(Index, ReadBufferSize, "NetLib.ReadBufferSize", 0x1FFFF)
CONFIG_PARAMETER(Index, WriteBufferSize, "NetLib.WriteBufferSize", 0x7FFFF)
CONFIG_END(NetLib)

#undef CONFIG_BEGIN
#undef CONFIG_PARAMETER
#undef CONFIG_PARAMETER_ARRAY
#undef CONFIG_END
